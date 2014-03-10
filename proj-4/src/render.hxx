/*! \file render.hxx
 *
 * \author John Reppy & Lamont Samuels
 */

/* CMSC23700 Project 3 sample code (Winter 2014)
 *
 * COPYRIGHT (c) 2014 John Reppy (http://www.cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#ifndef _RENDER_HXX_
#define _RENDER_HXX_

//! the different types of textures 
enum {
    COLOR_TEX = 0,	//!< index of color texture
    SPEC_TEX = 1,	//!< index of specular-map texture
    NORM_TEX = 2,	//!< index of normal-map texture
    MAX_NUM_TEXS = 3	//!< max number of textures (not all textures are present for all skins)
};

//! The different mesh modes
enum RenderMode {
    SKELETON = 0,	//!< render the skeleton only
    WIREFRAME,		//!< draw the wire frame
    MODEL,        	//!< draw the model without shadows
    SHADOWS,      	//!< draw the model with shadows
};

//! the information needed to render a mesh
struct MeshInfo {
    GLuint		vaoId;		//!< vertex-array-object ID for this mesh
    GLuint		vbufId;		//!< vertex-buffer ID for the vertex data
    GLuint		nbufId;		//!< buffer ID for the normal-vector data
    GLuint		tcbufId;	//!< buffer ID for the texture coordinate data
    GLuint		ebufId;		//!< buffer ID for the index array
    GLenum		prim;		//!< the primitive type for rendering the mesh
					//!< (e.g., GL_TRIANGLES, GL_TRIANGLE_FAN, etc.)
    cs237::texture2D	*texs[MAX_NUM_TEXS]; //!< Textures; nullptr for unused slots
    int			nIndices;	//!< the number of vertex indices

  //! create a MeshInfo object by initializing its buffer Ids.  The buffer data is
  //! loaded separately.
    MeshInfo (GLenum p);

  //! initialize the basic vertex data for the mesh (vertices and normals)
    void LoadVertexData (int nVerts, cs237::vec3f *verts, cs237::vec3f *norms);

  //! initialize the textures for the mesh
    void LoadTextures (cs237::texture2D *texs[MAX_NUM_TEXS]);

  //! updates the vertex data buffers for the mesh
    void UpdateVertexData (int nVerts, cs237::vec3f *verts, cs237::vec3f *norms); 

  //! initialize the texture-coordinate data buffer for the mesh
    void LoadTexCoords (int nCoords, cs237::vec2f *tcoords);

  //! initialize the element array for the mesh
    void LoadIndices (int n, const unsigned short *indices);

};

struct Renderer {
    cs237::ShaderProgram *shader;	//!< the shader program
					// Uniform variable locations
    int			mvLoc;		//!< the model-view matrix uniform
    int			projLoc;	//!< the projection matrix uniform
    int			colorLoc;	//!< the color uniform
    int         textureLoc; //!< the texture uniform
    int         shadowLoc; //!< the texture uniform
    int         depthLoc; //!< the texture uniform
    Renderer () : shader(nullptr) { }

  //! initialize a renderer from the given shader program
    void Init (std::string const & shaderPrefix);

  //! enable the renderer
    void Enable ();

  //! render a mesh using this renderer
    void Render (const MeshInfo *mesh);

  //! render a mesh (by the vertices array) using this renderer
    void RenderArray(const MeshInfo *mesh,int arraySize); 

};

#endif // !_RENDER_HXX_
