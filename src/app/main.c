#include "app.h"
#include <global/global.h>
#include <core/window.h>
#include <core/time.h>
#include <core/renderer.h>
#include <core/input.h>
#include <text/text_inc.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GLAD/glad.h>



// ---- vars ----
static GLFWwindow* window;

f32 t_last_frame = 0.0f;
f32 delta_t = 0.0f;

int main(void)
{
  #if __STDC_VERSION__ >=  201710L
  PF("-- using c18 --\n");
  #elif __STDC_VERSION__ >= 201112L
  PF("-- using c11 --\n");
  #elif __STDC_VERSION__ >= 199901L
  PF("-- using c99 --\n");
  #else
  PF("-- using c89/c90 --\n");
  #endif

  // 1920, 1080
	if (!window_create(1000, 800, "text-edit 2000 mega laktosefrei", WINDOW_MIN))
	{
		ERR("window creation failed\n");
		return 1;
	}
	window = window_get();
  // ---- state ----
  glClearColor(40.0f / 256.0f, 44.0f / 256.0f, 52.0f / 256.0f, 1.0f);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);

  // ---- init ----
	// renderer_init();
  // P("-> renderer_init() finished");
	input_init();
  // P("-> input_init() finished");
	app_init();
  //  P("-> app_init() finished");

  int w, h;
	while (!glfwWindowShouldClose(window))
	{
		// glfwRequestWindowAttention(window);
		glfwPollEvents();
		
		// ---- time ----
		delta_t = get_total_t() - t_last_frame;
		t_last_frame = get_total_t();

		// ---- update ----
		
    // @TODO: pass window size and delta t into update funcs
    window_get_size(&w, &h);
	  glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT);

    // renderer_update();
		app_update(delta_t);
    text_draw_update();
		input_update();

		glfwSwapBuffers(window);
	}
  text_cleanup();

	return 0;
}

f32 get_delta_t()
{
	return delta_t;
}

f32 get_total_t()
{
	return (f32)glfwGetTime();
}

void app_quit()
{
  glfwSetWindowShouldClose(window, true);
}

