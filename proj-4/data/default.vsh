/*! \file default.vsh
 *
 * \brief Modelling and rendering of the skeleton or wireframe. 
 *
 * \author Lamont Samuels
 */

/* CMSC23700 Project 3 Sample code
 *
 * COPYRIGHT (c) 2014 Lamont Samuels (http://www.cs.uchicago.edu/~lamonts)
 * All rights reserved.
 */

#version 410

uniform mat4 modelView;			// model-view transform
uniform mat4 projection;		// projection transform

layout (location = 0) in vec3 coord;	// vertex position

void main ()
{
  // pass vertex data through to fragment shader
    gl_Position = projection * modelView * vec4(coord,1);	// clip coordinates for vertex
} 