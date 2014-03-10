/*! \file map-cell.cxx
 *
 * \author John Reppy
 *
 * Map cells.
 */

/* CMSC23700 Final project sample code
 *
 * COPYRIGHT (c) 2014 John Reppy (http://cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#include "cs237.hxx"
#include "map.hxx"
#include "map-cell.hxx"
#include "qtree-util.hxx"
#include <fstream>
#include <vector>

// A cell file has the following layout on disk.  All data is in little-endian layout.
//
//	uint32_t magic;		// Magic number; should be 0x63656C6C ('cell')
//	uint32_t compressed;	// true if the chunks are compressed
//	uint32_t size;		// cell width (will be width+1 vertices wide)
//	uint32_t nLODs;
//	uint64_t toc[N];	// file offsets of chunks
//
// Each chunk has the layout
//
//	float maxError;		// maximum geometric error for this chunk
//	uint32_t nVerts;	// number of vertices
//	uint32_t nIndices;	// number of indices
//	int16_t minY;		// minimum active elevation value in this chunk
//	int16_t maxY;		// maximum active elevation value in this chunk
//	Vertex verts[nVerts];
//	uint16_t indices[nIndices];
//
// Each Vertex is represented by four 16-bit signed integers.

template <typename T>
inline T readVal (std::ifstream &inS)
{
    T v;
    
    if (inS.read(reinterpret_cast<char *>(&v), sizeof(T)).fail()) {
#ifndef NDEBUG
	std::cerr << "Cell::load: error reading file" << std::endl;
#endif
	exit (1);
    }
    return v;
}

inline int16_t readI16 (std::ifstream &inS) { return readVal<int16_t>(inS); }
inline uint32_t readUI32 (std::ifstream &inS) { return readVal<uint32_t>(inS); }
inline float readF32 (std::ifstream &inS) { return readVal<float>(inS); }
inline uint64_t readUI64 (std::ifstream &inS) { return readVal<uint64_t>(inS); }


/***** class Cell member functions *****/

Cell::Cell (Map *map, uint32_t r, uint32_t c, std::string const &stem)
    : _map(map), _row(r), _col(c), _stem(stem), _nLODs(0), _nTiles(0), _tiles(nullptr)
{
}

Cell::~Cell () { }

// load the cell data
void Cell::load ()
{
    if (this->isLoaded())
	return;

    std::string file = this->_stem + "/hf.cell";
    std::ifstream inS(file, std::ifstream::in | std::ifstream::binary);
    if (inS.fail()) {
#ifndef NDEBUG
	std::cerr << "Cell::load: unable to open \"" << file << "\"" << std::endl;
#endif
	exit (1);
    }

  // get header info
    uint32_t magic = readUI32(inS);
    bool compressed = (readUI32(inS) != 0);
    uint32_t size = readUI32(inS);
    uint32_t nLODs = readUI32(inS);
    if ((magic != Cell::MAGIC)
    || (this->_map->_cellSize != size)
    || (nLODs < Cell::MIN_NUM_LODS) || (Cell::MAX_NUM_LODS < nLODs)) {
#ifndef NDEBUG
	std::cerr << "Cell::load: bogus file header" << std::endl;
#endif
	exit (1);
    }

    if (compressed) {
	std::cerr << "Cell::load: compressed files are not supported yet" << std::endl;
	exit (1);
    }

    uint32_t qtreeSize = QTree::fullSize(nLODs);
    std::vector<std::streamoff> toc(qtreeSize);
    for (int i = 0;  i < qtreeSize;  i++) {
	toc[i] = static_cast<std::streamoff>(readUI64(inS));
    }

  // allocate and load the tiles.  Note that tiles are numbered in a breadth-first
  // order in the LOD quadtree.
    this->_nLODs = nLODs;
    this->_nTiles = qtreeSize;
    this->_tiles = new Tile[qtreeSize];

  // initialize the root tile
    this->_tiles[0]._cell = this;
    this->_tiles[0]._id = 0;
    this->_tiles[0]._row = 0;
    this->_tiles[0]._col = 0;
    this->_tiles[0]._lod = 0;

  // initialize the rest of the tiles in the tree (but not their chunks)
    struct { int dr, dc; } offset[4] = {
	    { 0, 0 }, // NW
	    { 0, 1 }, // NE
	    { 1, 1 }, // SE
	    { 1, 0 }, // SW
	};
    uint32_t id = 1;
    for (int level = 1;  level < nLODs;  level++) {
	int nQuadsAcross = 1 << (level-1);		// number of 2x2 groups across at this level of detail
        for (int r = 0;  r < nQuadsAcross;  r++) {
	    for (int c = 0;  c < nQuadsAcross;  c++) {
		int quadWid = (this->width() >> (level-1));  // width of a 2x2 group
		int nwRow = r * quadWid;
		int nwCol = c * quadWid;
		for (int i = 0;  i < 4;  i++, id++) {
		    this->_tiles[id]._cell = this;
		    this->_tiles[id]._id = id;
		    this->_tiles[id]._row = nwRow + offset[i].dr * (quadWid >> 1);
		    this->_tiles[id]._col = nwCol + offset[i].dc * (quadWid >> 1);
		    this->_tiles[id]._lod = level;
//std::clog << "tile[" << id << "] row = " << this->_tiles[id]._row << ", col = " << this->_tiles[id]._col
//<< ", lod = " << this->_tiles[id]._lod << std::endl;
		}
	    }
	}
    }
    assert (id == qtreeSize);

  // load the tile mesh data
    for (id = 0;  id < qtreeSize;  id++) {
	Chunk *cp = &(this->_tiles[id]._chunk);
      // find the beginning of the chunk in the input file
	inS.seekg(toc[id]);
      // read the chunk's header
	cp->_maxError = readF32(inS);
	uint32_t nVerts = readUI32(inS);
	uint32_t nIndices = readUI32(inS);
	cp->_minY = readI16(inS);
	cp->_maxY = readI16(inS);
      // allocate space for the chunk data
	this->_tiles[id]._allocChunk(nVerts, nIndices);
      // read the vertex data
	if (inS.read(reinterpret_cast<char *>(cp->_vertices), cp->vSize()).fail()) {
	    std::cerr << "Cell::load: error reading vertex data for tile " << id << std::endl;
	    exit (1);
	}
      // read the index array
	if (inS.read(reinterpret_cast<char *>(cp->_indices), cp->iSize()).fail()) {
	    std::cerr << "Cell::load: error reading index data for tile " << id << std::endl;
	    exit (1);
	}
      // compute the tile's bounding box.  We use double precision here, so that we can
      // support large worlds.
	cs237::vec3d nwCorner = this->_map->nwCellCorner(this->_row, this->_col);
	nwCorner.y = static_cast<double>(
	    this->_map->baseElevation() + this->_map->vScale() * static_cast<float>(cp->_minY));
	cs237::vec3d seCorner = nwCorner + this->_map->cellSize();
	seCorner.y = static_cast<double>(
	    this->_map->baseElevation() + this->_map->vScale() * static_cast<float>(cp->_maxY));
	this->_tiles[id]._bbox = cs237::AABBd(nwCorner, seCorner);
    }

}

/***** class Tile member functions *****/

Tile::Tile ()
{
    this->_chunk._nVertices = 0;
    this->_chunk._nIndices = 0;
    this->_chunk._vertices = nullptr;
    this->_chunk._indices = nullptr;
}

Tile::~Tile ()
{
    delete this->_chunk._vertices;
    delete this->_chunk._indices;
}

void Tile::_allocChunk (uint32_t nv, uint32_t ni)
{
    this->_chunk._nVertices = nv;
    this->_chunk._nIndices = ni;
    this->_chunk._vertices = new Vertex[nv];
    this->_chunk._indices = new uint16_t[ni];
}
