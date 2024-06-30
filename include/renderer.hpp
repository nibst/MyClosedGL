#pragma once
#include "model_object.hpp"
#include "options.hpp"
#include "camera.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

class Renderer{
    public:
        Renderer();
        void render(ModelObject object, Camera camera, float screen_width, float screen_height, glm::mat4 model);
        void setGLFrontFace(int clock_rotation);
        void swapGLFrontFace();
        void setRenderMode(GLint renderMode);
        void setCullingMode(CullingModes mode);
        void setEngine(Engines engine);
        void setLightingMode(LightingModes lighting_mode);
        void setBackGroundColor(glm::vec4 color);
        GLint getFrontFaceOrientation();
        void setupMatrices(Camera camera, glm::mat4 model);
        bool isClosed2GLActive();
        ModelObject processTrianglesClosed2GL(ModelObject model_object, glm::mat4 model_view_proj,float screen_width, float screen_height);
        GLuint gpuProgramID = 0;
        GLint model_uniform;
        GLint view_uniform;
        GLint projection_uniform;
        GLint object_id_uniform;
        GLint diffuse_color_uniform;    
        GLint diffuse_intensity_uniform;      
        GLint ambient_color_uniform;       
        GLint ambient_intensity_uniform;
        GLint specular_color_uniform;       
        GLint specular_intensity_uniform;      
        GLint shine_coefficient_uniform;          
        GLint vertex_shader_type_uniform;
        GLint fragment_shader_type_uniform;
        GLint close2GL_uniform;
    private:
        glm::mat4 model, view, projection, viewport,model_view_proj;
        bool close2GL_active;
        GLint frontFace;
        GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id);
        void LoadShadersFromFiles();
        GLuint LoadShader_Vertex(const char* filename);
        GLuint LoadShader_Fragment(const char* filename);
        void LoadShader(const char* filename, GLuint shader_id);
        // Variáveis que definem um programa de GPU (shaders). Veja função LoadShadersFromFiles().


};