#include "window.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GLAD/glad.h>

// --- vars ----
GLFWmonitor* monitor = NULL;
GLFWwindow* window   = NULL;
int monitor_w = 0;
int monitor_h = 0;
window_type win_type;

// intis glfw & glad, also creates the window
// returns: <stddef.h> return_code
bool window_create(const int width, const int height, const char* title, window_type type)
{

	// enable error logging for glfw
	glfwSetErrorCallback(error_callback);
	
  // Initialise GLFW
	if (glfwInit() == GLFW_FALSE)
	{
		fprintf(stderr, "Failed to initialize GLFW !!!\n");
		return false;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);
  monitor_w = mode->width;
  monitor_h = mode->height;

  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

  // open a window and create its opengl context
	if (type == WINDOW_FULL)
  {
      window = glfwCreateWindow(mode->width, mode->height, title, monitor, NULL);
  }
  else
  {
      window = glfwCreateWindow(width, height, title, NULL, NULL);
  }
  win_type = type;

	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return false;
	}

	// make the window's context current
	glfwMakeContextCurrent(window);

	// initialize glad, load the opengl bindings
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "Failed to initialize GLAD");
		return false;
	}

	// tell opengl the size of our window
	glViewport(0, 0, width, height);

	// maximize window
	if (type == WINDOW_MAX)
	{
		glfwMaximizeWindow(window);
	}

	// set the resize callback
	glfwSetFramebufferSizeCallback(window, (GLFWframebuffersizefun)resize_callback);

	glfwSetWindowAttrib(window, GLFW_FOCUS_ON_SHOW, true);
	// glfwSetWindowAttrib(window, GLFW_AUTO_ICONIFY, true);
	glfwRequestWindowAttention(window);

	return true; // all good :)
}

// glfw error func
void error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW-Error: %s\n", description);
}

// window resize callback
// resizes the "glViewport" according to the resized window
// window is type GLFWwindow*
void resize_callback(void* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void* window_get()
{
	return window;
}

void window_get_size(int* w, int* h)
{
	glfwGetWindowSize(window, w, h);
}

window_type window_get_mode()
{
  return win_type;
}

void window_get_monitor_size_cm(float* w, float* h)
{
  int w_mm, h_mm; // millimeter
  glfwGetMonitorPhysicalSize(monitor, &w_mm, &h_mm);
  *w = (float)w_mm * 0.1f;
  *h = (float)h_mm * 0.1f;
}

void window_get_monitor_dpi(float* w, float* h)
{
  window_get_monitor_size_cm(w, h);
  *w *= 0.39370079; // cm to inch
  *h *= 0.39370079; // cm to inch
  // int w_px, h_px;
	// glfwGetWindowSize(window, &w_px, &h_px);
  *w = monitor_w / *w; // pixels per inch
  *h = monitor_h / *h; // pixels per inch
}

void window_set_mode(window_type type)
{
  if (type == WINDOW_MIN)
  {
    glfwRestoreWindow(window);
  }
  else if (type == WINDOW_MAX)
  { 
		glfwMaximizeWindow(window);
  }
  else if (type == WINDOW_FULL)
  {
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
  }
  win_type = type;
}

void window_set_title(const char* title)
{
  glfwSetWindowTitle(window, title);
}

