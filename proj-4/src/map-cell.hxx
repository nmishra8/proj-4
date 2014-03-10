/*! \file map-cell.hxx
 *
 * \author John Reppy
 */

/* CMSC23700 Final project sample code
 *
 * COPYRIGHT (c) 2014 John Reppy (http://cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#ifndef _MAP_CELL_HXX_
#define _MAP_CELL_HXX_

#include "map.hxx"
#include "qtree-util.hxx"

class Tile;

class Cell {
  public:

  //! Cell constructor
  //! \param map[in]  the map containing this cell
  //! \param r[in]    this cell's row in the grid of cells
  //! \param c[in]    this cell's column in the grid of cells
  //! \param stem[in] the stem or path prefix used to load this cell's data
    Cell (Map *map, uint32_t r, uint32_t c, std::string const &stem);

    ~Cell ();

  //! load the cell data from the "hf.cell" file
    void load ();

  //! returns true if cell data has been loaded
    bool isLoaded () const { return (this->_tiles != nullptr); }

  //! the row of this cell in the grid of cells in the map
    int row () const { return this->_row; }
  //! the column of this cell in the grid of cells in the map
    int col () const { return this->_col; }
  //! the number of levels of detail supported by this cell
    int depth () const { return this->_nLODs; }
  //! the width of this cell in hScale units.  The number of vertices across will be width()+1
    int width () const { return this->_map->cellWidth(); }

  // return the path of a data file  for this cell
  // \param file[in] the name of the file
  // \return the path to the file for this cell.
    std::string datafile (std::string const &file);

  // get a particular tile; we assume that the cell data has been loaded
    Tile &tile (int id);

  // constants
    static const uint32_t MAGIC = 0x63656C6C;  // 'cell'
    static const uint32_t MIN_NUM_LODS = 1;
    static const uint32_t MAX_NUM_LODS = 8;

  private:
    Map		*_map;		//!< the map containing this cell
    uint32_t	_row, _col;	//!< the row and column of this cell in its map
    std::string _stem;		//!< prefix of pathnames for access cell data files
    uint32_t	_nLODs;		//!< number of levels of detail in this cell's representation
    uint32_t	_nTiles;	//!< the number of tiles
    Tile	*_tiles;	//!< the complete quadtree of tiles

    Tile *loadTile (int id);

};

//! packed vertex representation
struct Vertex {
    int16_t	_x;		//!< x coordinate relative to Cell's NW corner (in hScale units)
    int16_t	_y;		//!< y coordinate relative to Cell's base elevation (in vScale units)
    int16_t	_z;		//!< z coordinate relative to Cell's NW corner (in hScale units)
    int16_t	_morphDelta;	//!< y morph target relative to _y (in vScale units)
};

//! LOD mesh chunk
struct Chunk {
    float	_maxError;	//<! maximum geometric error (in meters) for this chunk
    int16_t	_minY;		//<! minimum Y value of the vertices in this chunk
    int16_t	_maxY;		//<! maximum Y value of the vertices in this chunk
    uint32_t	_nVertices;	//!< number of vertices in chunk; should be < 2^16
    uint32_t	_nIndices;	//!< number of indices in chunk
    Vertex	*_vertices;	//!< vertex array; each verte is packed into 64-bits
    uint16_t	*_indices;	//!< index array

    size_t vSize() const { return this->_nVertices * sizeof(Vertex); }
    size_t iSize() const { return this->_nIndices * sizeof(uint16_t); }
};

//! A tile is a node in the LOD quadtree.  It contains the mesh data for the corresponding
//! LOD chunk, and can also be used to attach other useful information (such as the chunk's
//! bounding box).
class Tile {
  public:

    Tile();
    ~Tile();

  //! the row of this tile's NW vertex in its cell
    uint32_t nwRow () const { return this->_row; }
  //! the column of this tile's NW vertex in its cell
    uint32_t nwCol () const { return this->_col; }
  //! the width of this cell in hScale units.  The number of vertices across will be width()+1
    uint32_t width () const { return this->_cell->width() >> this->_lod; }
  //! the level of detail of this tile (0 is coarsest)
    int lod () const { return this->_lod; }

  //! read-only access to mesh data for this tile
    Chunk const & chunk() const { return this->_chunk; }

  //! the tile's bounding box in world coordinates
    cs237::AABBd const & bbox () const { return this->_bbox; }

  //! return the i'th child of this tile (nullptr if the tile is a leaf)
    Tile *child (int i) const;

  private:
    Cell	*_cell;		//!< the cell that contains this tile
    uint32_t	_id;		//!< the ID of this tile, which is also its index in the quadtree array
    uint32_t	_row;		//<! the row of this tile's NW vertex in its cell
    uint32_t	_col;		//<! the column of this tile's NW vertex in its cell
    int32_t	_lod;		//!< the level of detail of this tile (0 == coarsest)
    Chunk	_chunk;		//<! mesh data for this tile
    cs237::AABBd _bbox;		//<! the tile's bounding box in world coordinates; note that we use
				//!  double precision here so that we can support large worlds	

  //! allocate memory for the chunk
    void _allocChunk (uint32_t nv, uint32_t ni);

    friend class Cell;
};

/***** Inline functions *****/

inline std::string Cell::datafile (std::string const &file)
{
    return this->_stem + file;
}

inline Tile &Cell::tile (int id)
{
    assert (this->isLoaded());
    assert ((0 <= id) && (id < this->_nTiles));

    return this->_tiles[id];
}

inline Tile *Tile::child (int i) const
{
    assert ((0 <= i) && (i < 4));
    if (this->_lod+1 < this->_cell->depth())
	return &(this->_cell->tile(QTree::nwChild(this->_id) + i));
    else
	return nullptr;
}


#endif //! _MAP_CELL_HXX_
