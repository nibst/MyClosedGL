cmake_minimum_required (VERSION 2.6)

project (CMP143)

LINK_DIRECTORIES( ${CMAKE_SOURCE_DIR}/lib )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Second, for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin)
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/lib )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/lib )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

set (CMAKE_DEBUG_POSTFIX "_d")

find_package(OpenGL REQUIRED)

if(WIN32)
set(COMMON_LIBS ${OPENGL_LIBRARIES} optimized glfw debug glfw)
elseif (UNIX)
set(COMMON_LIBS ${OPENGL_LIBRARIES} glfw ${GLFW_LIBRARIES} X11)
else()
set(COMMON_LIBS)
endif()
set(COMMON_LIBS ${COMMON_LIBS} ${EXTRA_LIBS})

set(RUN_DIR ${PROJECT_SOURCE_DIR}/bin)

add_executable(CMP143 src/main.cpp
 src/model_object.cpp 
 src/matrices.cpp
 src/VAO.cpp
 src/renderer.cpp
 src/tiny_obj_loader.cpp
 src/camera.cpp
 src/options_manager.cpp
 lib/imgui/imgui_demo.cpp
 lib/imgui/imgui_draw.cpp
 lib/imgui/imgui_tables.cpp
 lib/imgui/imgui_widgets.cpp
 lib/imgui/imgui.cpp
 lib/imgui/imgui_impl_glfw.cpp
 lib/imgui/imgui_impl_opengl3.cpp
 lib/FileBrowser/ImGuiFileBrowser.cpp
 lib/libglfw3.a 
 lib/gl3w.c shader_vertex.glsl shader_fragment.glsl
 )
set_property(TARGET CMP143 PROPERTY DEBUG_POSTFIX _d)
target_link_libraries(CMP143 ${COMMON_LIBS})

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LINUX -DIMGUI_IMPL_OPENGL_LOADER_GL3W")
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

include_directories( include )
include_directories(lib/glfw/include)
include_directories(lib/imgui)
include_directories(lib/FileBrowser)

option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_DOCS OFF)
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/glfw)
