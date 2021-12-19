#include "app.h"
#include "../core/core.h"
#include "../core/window.h"
#include "../core/time.h"
#include "../core/renderer.h"
#include "../core/input.h"
#include "../text/text_inc.h"

#define GLFW_INCLUDE_NONE
#include "../../external/GLFW/glfw3.h"
#include "../../external/GLAD/glad.h"



// ---- vars ----
GLFWwindow* window;

f32 t_last_frame = 0.0f;
f32 delta_t = 0.0f;

int main(void)
{
	// 1920, 1080
	if (!create_window(1000, 800, "text-edit 2000 mega laktosefrei", false))
	{
		ERR("window creation failed\n");
		return 1;
	}
	window = get_window();

	// ---- init ----
  text_init(
"assets/fonts/\
JetBrains Mono NL Regular \
Nerd Font Complete Mono Windows Compatible.ttf", 
      4);
  text_draw_init();
	renderer_init();
	input_init();
	app_init();

	while (!glfwWindowShouldClose(window))
	{
		// glfwRequestWindowAttention(window);
		glfwPollEvents();
		
		// ---- time ----
		delta_t = get_total_t() - t_last_frame;
		t_last_frame = get_total_t();

		// ---- update ----
		renderer_update();
		app_update();
    text_draw_update();
		input_update();

		glfwSwapBuffers(window);
	}

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

