#ifndef GAME_H
#define GAME_H

#include <core/core.h>
#include <text/text_inc.h>

void app_init();
void app_update(float dt);

void app_quit();    // implementation in main.c

void app_set_cwd(const char* _cwd);
void app_cat_cwd(const char* cat);
const char* app_get_cwd();

void app_new_file();
void app_load_file(const char* path, font_t* font);
void app_save_open_file();
void app_save_open_file_as(const char* path);

void app_fill_out(const char* str);
#endif
