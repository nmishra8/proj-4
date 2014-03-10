/*! \file view.c
 *
 * \brief This file defines the viewer operations.
 *
 * \author John Reppy
 */

/* CMSC23700 Project 2 sample code (Winter 2014)
 *
 * COPYRIGHT (c) 2014 John Reppy (http://www.cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#include "cs237.hxx"
#include "view.hxx"
#include "plane.hxx"
#include <sstream>

/* default motion parameters */
#define DEFAULT_X_ROT	0.0f
#define DEFAULT_Y_ROT	0.0f
#define DEFAULT_Z_ROT	0.0f
#define DEFAULT_SCALE	0.35f

/* clip planes in world coordinates */
#define NEAR_Z		0.2f
#define FAR_Z		100.0f
#define FOVY		65.0f	/* field-of-view in Y-dimension */

/* The path to the shaders; this is usually set from the compiler command-line.
 * but it defaults to a path relative to the build directory.
 */
#ifndef DATA_DIR
#  define DATA_DIR "../data"
#endif

//! the directional light's direction in world coordinates
static cs237::vec4f DirLight(1.0, 1.0, 0.5, 0.0);

/* InitView:
 *
 * Program initialization.
 */
void View::Init (int argc, const char **argv)
{
    this->wid = 800;
    this->ht = 800;
    this->isVis = GL_TRUE;

  /* initialize the motion parameters */
    this->xRot = DEFAULT_X_ROT;
    this->yRot = DEFAULT_Y_ROT;
    this->zRot = DEFAULT_Z_ROT;
    this->xLastIncr = 0;
    this->yLastIncr = 0;
    this->fInertia = cs237::vec2f(-0.5f, 0.0f);
    this->fScale = DEFAULT_SCALE;
    this->xLast = -1;
    this->yLast = -1;
    this->rotate = true;
    this->mouseTracking = false;

  /* initialize the camera */
    this->camPos	= cs237::vec3f(0.0, 15.0, 40.0);
    this->camTarget	= cs237::vec3f(0.0, 15.0, 0.0);
    this->camUp		= cs237::vec3f(0.0, 1.0, 0.0);

  /* initialize the rendering mode */ 
    this->renderMode = SKELETON; 

  /* initialize the animation clock */
    this->lastDraw	= glfwGetTime();;
    this->lastFrame = 0; 
    this->lastT = 0.0; 

    this->shouldExit	= false;
    this->needsRedraw	= true;

    this->guard = nullptr;  // cannot initialize until after GL context is created!
    this->floor = new Plane {
	    cs237::color3ub{ 128, 204, 128 },		// color
	    cs237::vec3f{  0.0f,  1.0f,  0.0f },	// norm                     
	    cs237::vec3f{  40.0f, 0.0f, -40.0f },	// corners
	    cs237::vec3f{ -40.0f, 0.0f, -40.0f },
	    cs237::vec3f{ -40.0f, 0.0f,  40.0f },
	    cs237::vec3f{  40.0f, 0.0f,  40.0f }
	};

}

/* Reset:
 *
 * Reset view parameters
 */
void View::Reset ()
{
    this->xRot    	= DEFAULT_X_ROT;
    this->yRot    	= DEFAULT_Y_ROT;
    this->zRot    	= DEFAULT_Z_ROT;
    this->xLastIncr	= 0;
    this->yLastIncr	= 0;
    this->fInertia	= cs237::vec2f(-0.5f, 0.0f);
    this->fScale	= DEFAULT_SCALE;
}

/* BindFrameBuffer:
 *
 * initialize the framebuffer back to the screen. 
 */
void View::BindFrameBuffer()
{ 
    int fwid, fht; 
    CS237_CHECK (glBindFramebuffer (GL_FRAMEBUFFER, 0) );
    CS237_CHECK (glfwGetFramebufferSize (this->window, &fwid, &fht) );
    CS237_CHECK (glViewport (0, 0, fwid, fht) );

}

//! \brief initialze the guard data; this must be done after the OpenGL context has been created
void View::InitGuard ()
{
    //if (this->guard == nullptr)
	//this->guard = new Guard();

}

/* InitProjMatrix:
 *
 * initialize the projection matrix based on the view state.
 */
void View::InitProjMatrix ()
{
    this->projectionMat = cs237::perspective (
	FOVY,
	(float)this->wid / (float)this->ht,
	NEAR_Z ,
	FAR_Z);

}

/* InitModelViewMatrix:
 *
 * initialize the model-view matrix based on the view state.
 */
void View::InitModelViewMatrix ()
{

    this->viewMat = cs237::lookAt (
	this->camPos,
	this->camTarget,
	this->camUp);

    this->modelViewMat = this->viewMat
	* cs237::rotateX(this->xRot)
	* cs237::rotateY(this->yRot)
	* cs237::rotateZ(this->zRot)
	* cs237::scale(this->fScale);

}

/* InitRenderer:
 */
void View::InitRenderer ()
{
  /***** This is placeholder code for initializing the renderers; you will need to update it *****/
    //this->skeletonRender.Init (std::string(DATA_DIR "/default"));
    this->floorRender.Init (std::string(DATA_DIR "/default"));
}

/* Render:
 *
 * render the current state of the animation
 */
void View::Render ()
{
    glEnable (GL_CULL_FACE);
    glCullFace (GL_BACK);
    glEnable (GL_DEPTH_TEST);
    glDepthMask (GL_TRUE);

  /* clear the screen */
    glClearColor (0.2f, 0.2f, 0.4f, 1.0f);	// clear the surface
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    InitModelViewMatrix ();
    this->RenderFloor();  // placeholder

}

void View::RenderFloor ()
{
    this->floorRender.Enable();

    cs237::setUniform (this->floorRender.projLoc, this->projectionMat);
    cs237::setUniform (this->floorRender.mvLoc, this->modelViewMat);

    this->floor->Draw (this, this->floorRender);

}
