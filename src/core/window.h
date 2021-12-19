#ifndef WINDOW_H
#define WINDOW_H

#include "core.h"


// ---- func decls ----
bool create_window(const int width, const int height, const char* title, bool maximize);
void error_callback(int error, const char* description);
void resize_callback(void* window, int width, int height);

void* get_window();

void get_window_size(int* w, int* h);

#endif
