/*! \file main.c
 *
 * \brief this file contains the main function and the GLFW callbacks.
 *
 * \author John Reppy
 */

/* CMSC23700 Project 3 sample code (Winter 2014)
 *
 * COPYRIGHT (c) 2014 John Reppy (http://www.cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#include "cs237.hxx"

#include "view.hxx"
#include "map.hxx"
#include "map-cell.hxx"
#include <unistd.h>


#define RENDER_TIMESTEP		0.016	// ~60Hz

//
// Rotation defines
//
#define INERTIA_THRESHOLD       1.0f
#define INERTIA_FACTOR          0.5f
#define SCALE_INCREMENT         0.01f
#define MIN_SCALE		0.25f
#define MAX_SCALE		0.75f

/* the viewer state */
View	*View = nullptr;

/*! \brief Set the viewport to the current size of the framebufffer.
 *         We use the framebuffer size instead of the window size, because it
 *         is different on Apple's retina displays.
 */
static void SetViewport (GLFWwindow *win)
{
    int fbWid, fbHt;
    glfwGetFramebufferSize (win, &fbWid, &fbHt);
    glViewport(0, 0 , fbWid, fbHt);

}

/***** Callback functions *****/

static void Error (int err, const char *msg)
{
    std::cerr << "[GLFW ERROR " << err << "] " << msg << "\n" << std::endl;
}

/*! \brief Run the simulation and then redraw the animation.
 */
void Display (GLFWwindow *win)
{
  /* avoid drawing when we are invisible */
    if (! View->isVis)
	return;

    View->Render ();
    glfwSwapBuffers (win);

    View->needsRedraw = false;

} /* end of Display */

/*! \brief Window resize callback.
 *  \param wid the new width of the window.
 *  \param ht the new height of the window. 
 */
void Reshape (GLFWwindow *win, int wid, int ht)
{
    View->wid = wid;
    View->ht = ht;

  /* set the viewport */
    SetViewport (win);

  /* recompute the projection matrix */
    View->InitProjMatrix ();

} /* end of Reshape */

/*! \brief Keyboard-event callback.
 *  \param win the window receiving the event
 *  \param key The keyboard code of the key
 *  \param scancode The system-specific scancode of the key.
 *  \param action GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT.
 *  \param mods the state of the keyboard modifier keys.
 */
void Key (GLFWwindow *win, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
	return;

    switch (key) {
      case GLFW_KEY_ESCAPE:
      case GLFW_KEY_Q:
	if (mods == 0)
	    glfwSetWindowShouldClose (win, true);
	break;
      case GLFW_KEY_M:
	if (mods == 0) {
	    switch (View->renderMode) {
	      case SHADOWS: View->renderMode = SKELETON; break;
	      case SKELETON: View->renderMode = WIREFRAME; break;
	      case WIREFRAME: View->renderMode = MODEL; break;
	      case MODEL: View->renderMode = SHADOWS; break;
	    }
	}
	break;
      case GLFW_KEY_SPACE:
	if (mods == 0) {
	    View->rotate = !View->rotate;
	    if (! View->rotate) {
		View->fInertiaOld = View->fInertia;
	    }
	    else {
		View->fInertia = View->fInertiaOld;
	      // To prevent confusion, force some rotation
		if ((View->fInertia.x == 0.0) && (View->fInertia.y == 0.0))
		    View->fInertia.x = -0.5;
	    }
	}
	break;
      case GLFW_KEY_EQUAL:
	if (mods == GLFW_MOD_SHIFT) {
	  // shift+'=' is '+'
	    View->fScale += SCALE_INCREMENT;
	    if (View->fScale > MAX_SCALE) View->fScale = MAX_SCALE;
	}
	break;
      case GLFW_KEY_KP_ADD:  // keypad '+'
	if (mods == 0) {
	    View->fScale += SCALE_INCREMENT;
	    if (View->fScale > MAX_SCALE) View->fScale = MAX_SCALE;
	}
	break;
      case GLFW_KEY_MINUS:
	if (mods == 0) {
	    View->fScale -= SCALE_INCREMENT;
	    if (View->fScale < MIN_SCALE) View->fScale = MIN_SCALE;
	}
	break;
      case GLFW_KEY_KP_SUBTRACT:  // keypad '-'
	if (mods == 0) {
	    View->fScale -= SCALE_INCREMENT;
	    if (View->fScale < MIN_SCALE) View->fScale = MIN_SCALE;
	}
	break;
      case GLFW_KEY_HOME:
	if (mods == 0)
	    View->Reset ();
        break;
      case GLFW_KEY_LEFT:
	if (mods == 0)
	    View->zRot--;
        break;
      case GLFW_KEY_RIGHT:
	if (mods == 0)
	    View->zRot++;
        break;
      case GLFW_KEY_UP:
	if (mods == 0)
	    View->xRot--;
        break;
      case GLFW_KEY_DOWN:
	if (mods == 0)
	    View->xRot++;
      break;
      default: // ignore all other keys
	return;
    }
    View->needsRedraw = true;

} /* Key */

/*! \brief The mouse button callback function
 */
void MouseButton (GLFWwindow *win, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_1) {
	View->mouseTracking = (action == GLFW_PRESS);
	if (View->mouseTracking) {
	  // initialize inertial parameters
	    View->fInertia = cs237::vec2f(0.0, 0.0);
	    double xPos, yPos;
	    glfwGetCursorPos (win, &xPos, &yPos);
	    View->xLast = static_cast<int>(xPos);
	    View->yLast = static_cast<int>(yPos);
	}
	else {
	    if (View->xLastIncr > INERTIA_THRESHOLD)
		View->fInertia.x = (View->xLastIncr - INERTIA_THRESHOLD)*INERTIA_FACTOR;
	    else if (-View->xLastIncr > INERTIA_THRESHOLD) 
		View->fInertia.x = (View->xLastIncr + INERTIA_THRESHOLD)*INERTIA_FACTOR;
	    if (View->yLastIncr > INERTIA_THRESHOLD) 
		View->fInertia.y = (View->yLastIncr - INERTIA_THRESHOLD)*INERTIA_FACTOR;
	    else if (-View->yLastIncr > INERTIA_THRESHOLD) 
		View->fInertia.y = (View->yLastIncr + INERTIA_THRESHOLD)*INERTIA_FACTOR;
	}
	View->xLastIncr = 0;
	View->yLastIncr = 0;
    }

}

/*! \brief The mouse motion callback function
 */
void MouseMotion (GLFWwindow *win, double xPos, double yPos)
{
    if (View->mouseTracking) {
	int x = static_cast<int>(xPos);
	int y = static_cast<int>(yPos);

	View->xLastIncr = x - View->xLast;
	View->yLastIncr = y - View->yLast;
	if ((glfwGetKey(win, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	|| (glfwGetKey(win, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)) {
	  // control key is pressed
	    if (View->xLast != -1) {
		View->yRot += View->xLastIncr;
	    }
	}
	else if (View->xLast != -1) {
	    View->zRot += View->xLastIncr;
	    View->xRot += View->yLastIncr;
	}

	View->xLast = x;
	View->yLast = y;
    }

}

/*! \brief Visibility-change callback: sets the visibility state of the view.
 *  \param state the current state of the window; GL_TRUE for iconified, GL_FALSE otherwise.
 */
void Visible (GLFWwindow *win, int state)
{
    View->isVis = (state == GL_FALSE);

} /* end of Visible */

/*! \brief the main function for the program
 *  \param argc the count of arguments in \a argv
 *  \param argv the array of command-line arguments.
 */
int main(int argc, const char **argv)
{
    Map map;
    
    // get the mapfile
    if (argc < 2) {
        std::cerr << "usage: proj-4 <map-dir>" << std::endl;
        return 1;
    }
    std::string mapDir(argv[1]);
    if (! map.loadMap (mapDir)) {
        return 1;
    }
    
    // get a cell
    Cell *cell = map.cell(0, 0);
    cell->load();
    glfwSetErrorCallback (Error);

    glfwInit ();

  // Check the GLFW version
    {
	int major;
	glfwGetVersion (&major, NULL, NULL);
	if (major < 3) {
	    std::cerr << "CS237 requires GLFW 3.0 or later\n" << std::endl;
	    exit (EXIT_FAILURE);
	}
    }

    View = new struct View();
    View->Init (argc, argv);

    glfwWindowHint (GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(View->wid, View->ht, "CMSC 23700: Project 3", NULL, NULL);
    if (window == nullptr)
	exit (EXIT_FAILURE);

    glfwMakeContextCurrent (window);

    SetViewport (window);

  // Check the OpenGL version
    {
	GLint major, minor;
	glGetIntegerv (GL_MAJOR_VERSION, &major);
	glGetIntegerv (GL_MINOR_VERSION, &minor);
	if ((major < 3) || ((major == 3) && (minor < 2))) {
	    std::cerr << "CS237 requires OpenGL 3.2 or later; found " << major << "." << minor << "\n" << std::endl;
	    exit (EXIT_FAILURE);
	}
    }

  // NOTE: the shaders must be initialized after a GL context is in place.
    View->window = window; 
    View->InitRenderer ();
    View->InitProjMatrix ();
    View->InitModelViewMatrix ();

  // set up callbacks
    glfwSetWindowRefreshCallback (window, Display);
    glfwSetWindowSizeCallback (window, Reshape);
    glfwSetWindowIconifyCallback (window, Visible);
    glfwSetKeyCallback (window, Key);
    glfwSetMouseButtonCallback (window, MouseButton);
    glfwSetCursorPosCallback (window, MouseMotion);

    while (! glfwWindowShouldClose(window)) {
	double now = glfwGetTime();

	if (View->needsRedraw || (now - View->lastDraw >= RENDER_TIMESTEP)) {
	    View->lastDraw = now;
	    Display (window);
	}

	usleep (1000);	// sleep for 1mS

	glfwPollEvents ();
    }

    glfwTerminate ();

    return EXIT_SUCCESS;
    
} /* end of main */
