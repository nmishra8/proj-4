/*! \file view.hxx
 *
 * \brief type definitions for tracking the view state.
 *
 * \author John Reppy & Lamont Samuels
 */

/* CMSC23700 Project 3 sample code (Winter 2014)
 *
 * COPYRIGHT (c) 2014 John Reppy (http://www.cs.uchicago.edu/~jhr) & Lamont Samuels (http://www.cs.uchicago.edu/~lamonts)
 * All rights reserved.
 */

#ifndef _VIEW_HXX_
#define _VIEW_HXX_

#include "cs237.hxx"
#include "render.hxx"
//#include "depth-buffer.hxx"

/*! \brief The current state of the view */
struct View {
    GLFWwindow		*window;	//!< the main window 

    bool		shouldExit;
    bool		needsRedraw;
  
  // Camera state
    cs237::vec3f	camPos;		//!< camera position in world space
    cs237::vec3f	camTarget;	//!< camera target in world space
    cs237::vec3f	camUp;		//!< camera up vector in world space

  // Motion state
    float		xRot;		//!< x-axis rotation
    float		yRot;		//!< y-axis rotation
    float		zRot;		//!< z-axis rotation
    cs237::vec2f	fInertia;
    cs237::vec2f	fInertiaOld;
    float		fScale;		//!< for zooming
    int			xLast, yLast;
    int			xLastIncr, yLastIncr; //!< used to track mouse coords
    bool		rotate;		//!< when true, the model rotates
    bool		mouseTracking;	//!< true when tracking mouse to control view

  // animation state
    int			lastFrame;	//!< index of last frame
    int			nextFrame;	//!< index of last frame
    double		lastDraw;	//!< time of last animation frame 
    double		lastT;		//!< last interpolation factor 
  
  // view info 
    cs237::mat4f	viewMat;	//!< the current view matrix
    cs237::mat4f	modelViewMat;	//!< the current model-view matrix
    cs237::mat4f	projectionMat;	//!< the current projection matrix

    int			wid, ht;	//!< window dimensions
    bool		isVis;		//!< true, when the window is visible

  // rendering state
    RenderMode		renderMode;	//!< what to render 
    Renderer		floorRender;	//!< the floor renderer
      // the models
    struct Plane	*floor;		//!< the floor that the guard stands on
    class Guard		*guard;		//!< the guard object

  //! Initialize
    void Init (int argc, const char **argv);

  //! \brief Reset the motion parameters
    void Reset ();

  //! \brief bind the framebuffer back to the screen. 
   void BindFrameBuffer(); 

  //! \brief initialze the guard data; this must be done after the OpenGL context has been created
    void InitGuard ();

  //! \brief initialize the renderer by loading and compiling the shader.
    void InitRenderer ();

  //! \brief initialize the projection matrix based on the view state.
    void InitProjMatrix ();

  //! \brief initialize the model-view matrix based on the view state.
    void InitModelViewMatrix ();

  /***** Rendering routines *****/

  //! \brief render the current state of the animation by computing the model's
  //!        pose at the current time and then rendering it using the current
  //!        render mode.
    void Render ();

  //! \brief render the floor
    void RenderFloor ();
    void RenderGuard ();


};

#endif /* !_VIEW_HXX_ */
