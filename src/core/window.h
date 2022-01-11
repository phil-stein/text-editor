#ifndef WINDOW_H
#define WINDOW_H

#include <global/global.h>

typedef enum window_type { WINDOW_MIN, WINDOW_MAX, WINDOW_FULL } window_type;

// ---- func decls ----
bool window_create(const int width, const int height, const char* title, window_type type);
void error_callback(int error, const char* description);
void resize_callback(void* window, int width, int height);

void* window_get();
void  window_get_size(int* w, int* h);
window_type window_get_mode();
void  window_get_monitor_size_cm(float* w, float* h);
void  window_get_monitor_dpi(float* w, float* h);

void window_set_mode(window_type type);
void window_set_title(const char* title);

#endif
