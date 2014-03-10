/*! \file guard.hxx
 *
 * \author John Reppy & Lamont Samuels
 *
 *  \brief The representation of a plane 
 */

/* CMSC23700 Project 3 sample code (Winter 2014)  
 *
 * COPYRIGHT (c) 2014 John Reppy (http://www.cs.uchicago.edu/~jhr) &
 *                    Lamont Samuels (http://www.cs.uchicago.edu/~lamonts)
 * All rights reserved.
 */

#ifndef _PLANE_HXX_
#define _PLANE_HXX_

#include "cs237.hxx"
#include "render.hxx"
#include "view.hxx"

/*! \brief the representation of a Plane */
struct Plane {
    cs237::color3ub	color;		//!< the plane's color
    cs237::vec3f	norm;		//!< the plane's normal vector
    cs237::vec3f	corner[4];	//!< the corners of the plane
    MeshInfo		*mesh;		//!< the mesh info for renderering the Plane
    bool		initialized; 	//!< true if the plane is initalized 	

    Plane ()
	: mesh(nullptr), initialized(false)
    {}

    Plane (cs237::color3ub c, cs237::vec3f n, cs237::vec3f v1, cs237::vec3f v2, cs237::vec3f v3, cs237::vec3f v4)
	: color(c), norm(n), corner{v1,v2,v3,v4}, mesh(nullptr), initialized(false)
    {}

  //! \brief initialize the mesh for the plane.
    void InitPlane (); 

  //! \brief draw a plane
  //! \param view the view that is rendering the plane 
  //! \param render the render that will render the plane 
    void Draw (View *view, Renderer &render); 

};

#endif /* !_PLANE_HXX_ */
