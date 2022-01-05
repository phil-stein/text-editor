#ifndef GAME_H
#define GAME_H

#include <core/core.h>

void app_init();
void app_update();

void app_quit();    // implementation in main.c

void app_set_cwd(const char* _cwd);
void app_cat_cwd(const char* cat);
const char* app_get_cwd();
void app_new_file();
void app_load_file(const char* path);
void app_fill_out(const char* str);
#endif
