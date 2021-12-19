#include "window.h"

#define GLFW_INCLUDE_NONE
#include "../../external/GLFW/glfw3.h"
#include "../../external/GLAD/glad.h"

// --- vars ----
GLFWwindow* window = NULL;


// intis glfw & glad, also creates the window
// returns: <stddef.h> return_code
bool create_window(const int width, const int height, const char* title, bool maximize)
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

	// open a window and create its opengl context
	window = glfwCreateWindow(width, height, title, NULL, NULL);
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
	if (maximize)
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

void* get_window()
{
	return window;
}

void get_window_size(int* w, int* h)
{
	glfwGetWindowSize(window, w, h);
	// printf("window: w: %d, h: %d\n", *w, *h);
}



