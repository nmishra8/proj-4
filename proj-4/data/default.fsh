/*! \file default.fsh
 *
 * \brief Coloring of the skeleton or wireframe. 
 *
 * \author Lamont Samuels
 */

/* CMSC23700 Project 3 Sample code
 *
 * COPYRIGHT (c) 2014 Lamont Samuels (http://www.cs.uchicago.edu/~lamonts)
 * All rights reserved.
 */

#version 410

uniform vec3 primColor;	// color of the primtive 

layout (location = 0) out vec4 fragColor;

void main (void)
{
  // the fragment color 
    fragColor = vec4(primColor,1.0); 
}
