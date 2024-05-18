#include "options_manager.hpp"

OptionsManager::OptionsManager(GLFWwindow *window, Renderer *renderer, Camera *camera)
{
    this->model_loaded = false;
    this->camera = camera;
    this->renderer = renderer;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsClassic();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 400");
}

OptionsManager::~OptionsManager()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void OptionsManager::RenderWelcomeWindow()
{
    ImGui::Begin("Welcome", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    if (ImGui::Button("Select Model ..."))
        ImGui::OpenPopup("Open File");
    if (fileDialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ".in"))
        LoadInputFile(fileDialog.selected_path.c_str());
    ImGui::End();
}

void OptionsManager::RenderPropertyWindow()
{
    ImGui::Begin("Properties", NULL, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::RadioButton("OpenGL", &properties.engine, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Close2GL", &properties.engine, 1);
    ImGui::Spacing();

    if (ImGui::Button("Select Model..."))
        ImGui::OpenPopup("Open File");
    if (fileDialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ".in"))
        LoadInputFile(fileDialog.selected_path.c_str());

    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Camera"))
    {
        ImGui::Text("Rotation");
        ImGui::SliderFloat("Pitch", (float *)&properties.rotationPitch, -2.0f * M_PI, 2.0f * M_PI);
        ImGui::SliderFloat("Roll", (float *)&properties.rotationRoll, -2.0f * M_PI, 2.0f * M_PI);
        ImGui::SliderFloat("Yaw", (float *)&properties.rotationYaw, -2.0f * M_PI, 2.0f * M_PI);

        ImGui::Text("Movement");
        ImGui::Text("  ");
        ImGui::SameLine();
        if (ImGui::Button("^"))
        {
            properties.shouldMove = true;
            properties.movementDirection = Forward;
        }
        ImGui::SameLine();
        ImGui::Text("  ");
        ImGui::SameLine();
        if (ImGui::Button("Up"))
        {
            properties.shouldMove = true;
            properties.movementDirection = Up;
        }
        if (ImGui::Button("<"))
        {
            properties.shouldMove = true;
            properties.movementDirection = Left;
        }
        ImGui::SameLine();
        ImGui::Text("  ");
        ImGui::SameLine();
        if (ImGui::Button(">"))
        {
            properties.shouldMove = true;
            properties.movementDirection = Right;
        }

        ImGui::Text("  ");
        ImGui::SameLine();
        if (ImGui::Button("v"))
        {
            properties.shouldMove = true;
            properties.movementDirection = Backwards;
        }
        ImGui::SameLine();
        ImGui::Text("  ");
        ImGui::SameLine();
        if (ImGui::Button("Down"))
        {
            properties.shouldMove = true;
            properties.movementDirection = Down;
        }

        ImGui::SliderFloat("Speed", (float *)&properties.speed, 0.1f, 1000.0f);
        ImGui::Checkbox("Look at Model", &properties.keepLookingAtModel);

        ImGui::Text("Reset Position");
        if (ImGui::Button("Reset"))
            properties.resetCamera = true;

        ImGui::Spacing();
    }

    if (ImGui::CollapsingHeader("Rendering"))
    {
        ImGui::Text("Rendering Mode");
        ImGui::RadioButton("Solid", &properties.renderMode, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Wireframe", &properties.renderMode, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Points", &properties.renderMode, 2);

        ImGui::Text("Normal Orientation");
        ImGui::RadioButton("Clockwise", &properties.orientation, GL_CW);
        ImGui::SameLine();
        ImGui::RadioButton("Counter-Clockwise", &properties.orientation, GL_CCW);

        ImGui::Text("Culling");
        ImGui::RadioButton("None", &properties.cullingMode, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Backface", &properties.cullingMode, -1);
        ImGui::SameLine();
        ImGui::RadioButton("Frontface", &properties.cullingMode, 1);

        ImGui::Spacing();
    }

    if (ImGui::CollapsingHeader("Projection"))
    {
        ImGui::Text("Horizontal Field of View");
        ImGui::SliderFloat("HFoV", (float *)&properties.horizontalFieldOfView, -M_PI, M_PI); 
        ImGui::Spacing();
        ImGui::Text("Vertical Field of View");
        ImGui::SliderFloat("VFoV", (float *)&properties.verticalFieldOfView, -M_PI, M_PI);
        ImGui::Spacing();

        ImGui::Text("Planes");
        ImGui::SliderFloat("Near", (float *)&properties.nearPlane, 0.1f, 100.0f);
        ImGui::SliderFloat("Far", (float *)&properties.farPlane, 0.1f, 10000.0f);

        ImGui::Spacing();
    }

    if (ImGui::CollapsingHeader("Lighting"))
    {
        ImGui::Checkbox("Enable light", &properties.lightOn);
        ImGui::Spacing();

        ImGui::Text("Diffuse Color");
        ImGui::ColorEdit3("##Diffuse Color Edit", (float *)&properties.modelDiffuseColor);
        ImGui::SameLine();
        ImGui::SliderFloat("Intensity ##DIFFUSE", (float *)&properties.diffuseIntensity, 0.0f, 1.0f);
        ImGui::Spacing();

        ImGui::Text("Ambient Color");
        ImGui::ColorEdit3("##Ambient Color Edit", (float *)&properties.modelAmbientColor);
        ImGui::SameLine();
        ImGui::SliderFloat("Intensity ##AMBIENT", (float *)&properties.ambientIntensity, 0.0f, 1.0f);
        ImGui::Spacing();

        ImGui::Text("Specular Color");
        ImGui::ColorEdit3("##Specular Color Edit", (float *)&properties.modelSpecularColor);
        ImGui::SameLine();
        ImGui::SliderFloat("Intensity ##SPECULAR", (float *)&properties.specularIntensity, 0.0f, 1.0f);
        ImGui::Spacing();

        ImGui::Text("Shine Coefficient");
        ImGui::SliderFloat("##Shine Coefficient", (float *)&properties.modelShineCoefficient, 0.0, 100.0);

        ImGui::Text("Lighting Mode");
        ImGui::RadioButton("Flat", &properties.lightingMode, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Gouraud AD", &properties.lightingMode, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Gouraud ADS", &properties.lightingMode, 2);
        ImGui::SameLine();
        ImGui::RadioButton("Phong", &properties.lightingMode, 3);
        ImGui::Spacing();

        ImGui::Text("Background color");
        ImGui::ColorEdit3("##Background Color Edit", (float *)&properties.backgroundColor);
        ImGui::Spacing();
    }
    /*
    if (managedScene->models[0].textured && ImGui::CollapsingHeader("Texture"))
    {
        ImGui::Checkbox("Enable Textures", &properties.textureOn);
        ImGui::Spacing();

        if (properties.textureOn)
        {
            if (ImGui::Button("Select Texture..."))
                ImGui::OpenPopup("Select Texture");
            if (fileDialog.showFileDialog("Select Texture", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ".jpg"))
                LoadTextureFile(fileDialog.selected_path.c_str());
            ImGui::Spacing();

            std::string textureName = managedScene->models[0].texture != nullptr ? managedScene->models[0].texture->getName() : "None";
            ImGui::Text("Current texture:");
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            ImGui::Text(textureName.c_str());
            ImGui::PopStyleColor();
            ImGui::Spacing();

            ImGui::Text("Resampling mode");
            ImGui::RadioButton("Nearest Neighbor", &properties.resamplingMode, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Bilinear", &properties.resamplingMode, 1);
            ImGui::SameLine();
            ImGui::RadioButton("Trilinear", &properties.resamplingMode, 2);
            ImGui::Spacing();
        }
    }
    */

    ImGui::Spacing();
    ImGui::End();
}

void OptionsManager::RenderWindow()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (!model_loaded)
        RenderWelcomeWindow();
    else
        RenderPropertyWindow();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void OptionsManager::ApplyPropertiesToScene(float delta_time)
{
    
    if (properties.shouldMove)
    {
        camera->move(properties.movementDirection, delta_time);

        if (properties.keepLookingAtModel)
            if(model_loaded){
                camera->setLookAtPoint(model_object.getCenterPosition());
                camera->setViewVector(camera->getLookAtPoint() - camera->getCenterPosition());
            }

        properties.shouldMove = false;
    }

    if (properties.resetCamera)
    {
       // managedScene->ResetCamera();

        properties.rotationPitch = 0.0f;
        properties.rotationRoll = 0.0f;
        properties.rotationYaw = 0.0f;

        properties.resetCamera = false;
    }

    if (!properties.keepLookingAtModel)
        ;//managedScene->camera.Rotate(properties.rotationPitch, properties.rotationRoll, properties.rotationYaw);
    else
        ;//managedScene->camera.LookAtFramedObject();
    camera->nearplane = -properties.nearPlane;
    camera->farplane = -properties.farPlane;
    if(properties.keepLookingAtModel)
    ;
    if(model_loaded){
        model_object.materials[0].diffuse_color = properties.modelDiffuseColor;
        model_object.materials[0].ambient_color = properties.modelAmbientColor;
        model_object.materials[0].specular_color = properties.modelSpecularColor;
        model_object.materials[0].shine = properties.modelShineCoefficient;
        glUniform3fv(renderer->diffuse_color_uniform, 1, glm::value_ptr(model_object.materials[0].diffuse_color));
        glUniform1f(renderer->diffuse_intensity_uniform, properties.diffuseIntensity);
        glUniform3fv(renderer->ambient_color_uniform, 1, glm::value_ptr(model_object.materials[0].ambient_color));
        glUniform1f(renderer->ambient_intensity_uniform, properties.ambientIntensity);
        glUniform3fv(renderer->specular_color_uniform, 1, glm::value_ptr(model_object.materials[0].specular_color));
        glUniform1f(renderer->specular_intensity_uniform, properties.specularIntensity);
        glUniform1f(renderer->shine_coefficient_uniform, model_object.materials[0].shine);
    }
}

void OptionsManager::ApplyPropertiesToRenderingEngine(glm::mat4 model_matrix,float g_ScreenWidth, float g_ScreenHeight)
{   

    renderer->setGLFrontFace(properties.orientation);
    renderer->setRenderMode(properties.renderMode);
    renderer->setCullingMode(static_cast<CullingModes>(properties.cullingMode));
    renderer->setEngine(static_cast<Engines>(properties.engine));
    renderer->setLightingMode(static_cast<LightingModes>(properties.lightingMode));
    if(properties.engine == Close2GL){
        glm::mat4 modelViewProj = camera->getProjectionMatrix() * camera->getViewMatrix() * model_matrix;
        this->model_object.processTrianglesClosed2GL(modelViewProj,renderer->getFrontFaceOrientation(), g_ScreenWidth, g_ScreenHeight);
    }
}

void OptionsManager::LoadInputFile(const char *fileName)
{   
    this->model_object.loadFromInFileName(fileName);
    properties.modelDiffuseColor = model_object.materials[0].diffuse_color;
    properties.modelAmbientColor =  model_object.materials[0].ambient_color;
    properties.modelSpecularColor =  model_object.materials[0].specular_color;
    properties.modelShineCoefficient =  model_object.materials[0].shine;
    model_loaded = true;
    std::cout << "Loaded model \"" << model_object.getName() << "\" from \"" << fileName << "\"" << std::endl;

}


void OptionsManager::ApplyProperties(float delta_time,glm::mat4 model_matrix,float g_ScreenWidth, float g_ScreenHeight)
{
    RenderWindow();
    ApplyPropertiesToScene(delta_time);
    ApplyPropertiesToRenderingEngine(model_matrix,g_ScreenWidth, g_ScreenHeight);
}

bool OptionsManager::isModelLoaded(){
    return this->model_loaded;
}

ModelObject OptionsManager::getModelObject(){
    return this->model_object;
}