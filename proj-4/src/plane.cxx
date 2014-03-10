/*! \file plane.c
 *
 * \brief Modelling and rendering of the a plane.
 *
 * \author John Reppy & Lamont Samuels
 */

/* CMSC23700 Project 3 sample code (Winter 2014)  
 *
 * COPYRIGHT (c) 2014 John Reppy (http://www.cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#include "cs237.hxx"
#include "plane.hxx"
#include "view.hxx"
#include "render.hxx"

// InitPlane:
//
// Initialize the mesh info for the plane.
//
void Plane::InitPlane()
{
    this->mesh = new MeshInfo (GL_TRIANGLE_FAN);
 // initialize the vertex data
    cs237::vec3f norms[4] = { this->norm, this->norm, this->norm, this->norm };
    this->mesh->LoadVertexData (4, this->corner, norms);

  // initialize the indices
    unsigned short indices[4] = {0, 1, 2, 3};
    this->mesh->LoadIndices (4, indices);
}

/* Draw:
 *
 */
void Plane::Draw (View *view, Renderer &render)
{
    if (!initialized) {
	InitPlane();
	initialized = true; 
    }
    cs237::setUniform (render.colorLoc, this->color);
    render.Render (this->mesh);
}
