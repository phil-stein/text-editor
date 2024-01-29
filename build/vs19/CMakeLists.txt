cmake_minimum_required(VERSION 3.20.1)

set(NAME "text_editor")

project(${NAME}
      VERSION 0.1
      DESCRIPTION "text editor")
      # LANGUAGES CXX)

set(ROOT_DIR_STEP "../..")
set(ROOT_DIR      "${CMAKE_SOURCE_DIR}/${ROOT_DIR_STEP}")

# puts .exe in _bin, but also dll's
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ROOT_DIR}/bin)
# # set the output dir for libraries 
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ROOT_DIR}/_libs/win64)   # static lib's
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ROOT_DIR}/_libs/win64)   # dll's, should prob. be _bin
# # set_target_properties(${LIB_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${ROOT_DIR}/_libs/win64)
# # set_target_properties(${LIB_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${ROOT_DIR}/_libs/win64)

add_executable(${NAME}
              ${ROOT_DIR}/src/app/main.c
              ${ROOT_DIR}/src/app/app.c
              ${ROOT_DIR}/src/app/app.h
              ${ROOT_DIR}/src/app/cmd.c
              ${ROOT_DIR}/src/app/cmd.h
              ${ROOT_DIR}/src/app/ui.c
              ${ROOT_DIR}/src/app/ui.h
              ${ROOT_DIR}/src/core/file_io.c
              ${ROOT_DIR}/src/core/file_io.h
              ${ROOT_DIR}/src/core/glad.c
              ${ROOT_DIR}/src/core/input.c
              ${ROOT_DIR}/src/core/input.h
              ${ROOT_DIR}/src/core/renderer.c
              ${ROOT_DIR}/src/core/renderer.h
              ${ROOT_DIR}/src/core/shader.c
              ${ROOT_DIR}/src/core/shader.h
              ${ROOT_DIR}/src/core/texture.c
              ${ROOT_DIR}/src/core/texture.h
              ${ROOT_DIR}/src/core/time.h
              ${ROOT_DIR}/src/core/window.c
              ${ROOT_DIR}/src/core/window.h
              )
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -std=gnu99")

target_include_directories(${NAME} PRIVATE ${ROOT_DIR}/src)
target_include_directories(${NAME} PRIVATE ${ROOT_DIR}/external)
target_include_directories(${NAME} PRIVATE ${ROOT_DIR}/external/FREETYPE)
target_compile_features(   ${NAME} PRIVATE cxx_std_11)
# target_compile_options(    ${NAME} PRIVATE -Wall -DGLOBAL_DEBUG -DASSETM_NO_ZIP -DDEBUG_TIMER -DDEBUG_OPENGL -DDEBUG_DRAW -DASSET_PATH=\"/Workspace/C/bovengine/_assets/\")
target_compile_options(    ${NAME} PRIVATE -Wall -DGLOBAL_DEBUG) 

if(${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
  target_compile_options(${NAME} PRIVATE) 
else() # if(${CMAKE_GENERATOR} STREQUAL "Visual Studio 2016 19")
  target_compile_options(${NAME} PRIVATE) 
endif()
# # -- link libs --
# # set _build/... folder based on generator, i.e. mingw-gcc or vs19
# if(${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
#   add_subdirectory(${ROOT_DIR}/_build/bovengine ${ROOT_DIR}/_build/bovengine)
# else() # if(${CMAKE_GENERATOR} STREQUAL "Visual Studio 2016 19")
#   add_subdirectory(${ROOT_DIR}/_build/bovengine ${ROOT_DIR}/_build/bovengine_vs19)
# endif()
# target_link_libraries(${NAME} bovengine)
# # target_compile_definitions(bovengine PRIVATE EDITOR)
# 
# # @NOTE: the next two lines make it compile flecs and include it as static .a / .lib
# #        directly linking to the .a/.lib makes compile times faster
# #        linking to libflecs_static.a doesnt work
# # add_subdirectory(${ROOT_DIR}/_external/flecs ${ROOT_DIR}/_build/flecs)
# # Target_link_libraries(${NAME} flecs::flecs_static)
# if(${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
#   # target_link_libraries(${NAME} ${ROOT_DIR}/_libs/win64/libflecs_static.a)
#   target_link_libraries(${NAME} ${ROOT_DIR}/_libs/win64/libflecs.dll.a)
# else() # if(${CMAKE_GENERATOR} STREQUAL "Visual Studio 2016 19")
#   # target_link_libraries(${NAME} ${ROOT_DIR}/_libs/win64/flecs_static.lib)
#   add_subdirectory(${ROOT_DIR}/_external/flecs ${ROOT_DIR}/_build/flecs)
#   Target_link_libraries(${NAME} flecs::flecs_static)
# endif()
# 
# # --- trying to link .a library ---
# # u .a library for mingw-gcc and .lib for vs19 based on generator, i.e. mingw-gcc or vs19
# # message("!!! generator: '${CMAKE_GENERATOR}'")
if(${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
  target_link_libraries(${NAME} ${ROOT_DIR}/libs/libglfw3.a)
  target_link_libraries(${NAME} ${ROOT_DIR}/libs/libfreetype.a)
  # message("!!! -> used .a")
else() # if(${CMAKE_GENERATOR} STREQUAL "Visual Studio 2016 19")
  target_link_libraries(${NAME} ${ROOT_DIR}/libs/glfw3.lib)
  # message("!!! -> used .lib")
endif()


