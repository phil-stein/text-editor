#ifndef GAME_H
#define GAME_H

#include <global/global.h>
#include <text/text_inc.h>


typedef struct app_data_t
{
  bool show_files_ui;
}app_data_t;
#define APP_DATA_INIT()     \
{                           \
  .show_files_ui = false,   \
}

app_data_t* app_data_get(); // @TODO: move to app_data.c

void app_init();
void app_update(float dt);

void app_quit();    // implementation in main.c


void app_set_cwd(const char* _cwd);
void app_cat_cwd(const char* cat);
const char* app_get_cwd();

void app_new_file();
void app_load_file(const char* path);
void app_save_open_file();
void app_save_open_file_as(const char* path);

font_t* app_get_main_font();
void app_resize_fonts(int size);

void app_fill_out(const char* str);
#endif
