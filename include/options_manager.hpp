#ifndef PROPERTY_MANAGER_H
#define PROPERTY_MANAGER_H

#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../lib/FileBrowser/ImGuiFileBrowser.h"

//#include "Texture.hpp"
#include "options.hpp"
//#include "Scene.hpp"
//#include "RenderingManager.hpp"

#include <iostream>
#include "model_object.hpp"
#include "renderer.hpp"
#include "camera.hpp"

class OptionsManager
{
private:
    imgui_addons::ImGuiFileBrowser fileDialog;
    bool model_loaded;

    Options properties;
    ModelObject model_object;
    Renderer* renderer;
    Camera *camera;
    void RenderWelcomeWindow();
    void RenderPropertyWindow();
    void RenderWindow();

    void ApplyPropertiesToScene(float delta_time);
    void ApplyPropertiesToRenderingEngine(glm::mat4 model_matrix,float g_ScreenWidth, float g_ScreenHeight);

public:
    OptionsManager(GLFWwindow *, Renderer*, Camera* );
    ~OptionsManager();
    bool isModelLoaded();
    ModelObject getModelObject();
    void LoadInputFile(const char *fileName);
    void ApplyProperties(float delta_time,glm::mat4 model_matrix,float g_ScreenWidth, float g_ScreenHeight);
};

#endif
