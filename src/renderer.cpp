#include "renderer.hpp"
#include <fstream>
#include <sstream>
#include <GL3/gl3w.h>
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional
/*
// Razão de proporção da janela (largura/altura). Veja função FramebufferSizeCallback().
float g_ScreenRatio = 1.0f;

// Variáveis que definem a câmera em coordenadas esféricas, controladas pelo
// usuário através do mouse (veja função CursorPosCallback()). A posição
// efetiva da câmera é calculada dentro da função main(), dentro do loop de
// renderização.
float g_CameraTheta = 0.0f; // Ângulo no plano ZX em relação ao eixo Z
float g_CameraPhi = 0.0f;   // Ângulo em relação ao eixo Y
float g_CameraDistance = 3.5f; // Distância da câmera para a origem
*/

Renderer::Renderer(){
    // Carregamento de todas funções definidas por OpenGL 3.3, utilizando a
    // biblioteca GLAD.
    LoadShadersFromFiles();
    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);


    glFrontFace(GL_CW);
    this->frontFace = GL_CW;
}
void Renderer::setCullingMode(CullingModes mode) {
    switch (mode)
    {
    case NoCulling:
        glDisable(GL_CULL_FACE);
        break;
    case BackfaceCulling:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        break;
    case FrontFaceCulling:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        break;
    default:
        break;
    }

}
void Renderer::setLightingMode(LightingModes lighting_mode)
{
    switch (lighting_mode)
    {
    case NoShader:
        glUniform1i(vertex_shader_type_uniform, NoShader);
        glUniform1i(fragment_shader_type_uniform, NoShader);
        break;
    case Gouraud_AD:
        glUniform1i(vertex_shader_type_uniform, Gouraud_AD);
        glUniform1i(fragment_shader_type_uniform, NoShader);
        break;
    case Gouraud_ADS:
        glUniform1i(vertex_shader_type_uniform, Gouraud_ADS);
        glUniform1i(fragment_shader_type_uniform, NoShader);
        break;
    case Phong:
        glUniform1i(vertex_shader_type_uniform, NoShader);
        glUniform1i(fragment_shader_type_uniform, Phong);
        break;
    default:
        break;
    }
}

void Renderer::setGLFrontFace(int clock_rotation){
    this->frontFace = clock_rotation;
    glFrontFace(clock_rotation);
}
void Renderer::swapGLFrontFace(){
    if (this->frontFace == GL_CCW){
        this->setGLFrontFace(GL_CW);
    }
    else{
        this->setGLFrontFace(GL_CCW);
    }
}
void Renderer::setupMatrices(ModelObject model_object, Camera camera){
    this->model = glm::mat4(1.0f);
    this->view = camera.getViewMatrix();
    this->projection = camera.getProjectionMatrix();
}
void Renderer::render(ModelObject object, Camera camera, float screen_width, float screen_height){
    if (close2GL_active){
        setupMatrices(object, camera);
        glm::mat4 model_view_projection= this->projection*this->view*this->model;
        this->processTrianglesClosed2GL(object,model_view_projection,screen_width,screen_height);
    }
    glBindVertexArray(object.getVAO().getID());

    glUniformMatrix4fv(model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
    //glUniform1i(g_object_id_uniform, object.getID());
    // Pedimos para a GPU rasterizar os vértices dos eixos XYZ
    // apontados pelo VAO como linhas. Veja a definição de
    // g_VirtualScene[""] dentro da função BuildTrianglesAndAddToVirtualScene(), e veja
    // a documentação da função glDrawElements() em
    // http://docs.gl/gl3/glDrawElements.
    glDrawElements(
        object.getRenderingMode(),
        object.getNumIndices(),
        GL_UNSIGNED_INT,
        (void*)(object.getFirstIndex() * sizeof(GLuint))
    );

    // "Desligamos" o VAO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);
}
void Renderer::LoadShadersFromFiles()
{

    GLuint vertex_shader_id = LoadShader_Vertex("./shader_vertex.glsl");
    GLuint fragment_shader_id = LoadShader_Fragment("./shader_fragment.glsl");

    // Deletamos o programa de GPU anterior, caso ele exista.
    if ( gpuProgramID != 0 )
        glDeleteProgram(gpuProgramID);

    // Criamos um programa de GPU utilizando os shaders carregados acima.
    gpuProgramID = CreateGpuProgram(vertex_shader_id, fragment_shader_id);

    // Buscamos o endereço das variáveis definidas dentro do Vertex Shader.
    // Utilizaremos estas variáveis para enviar dados para a placa de vídeo
    // (GPU)! Veja arquivo "shader_vertex.glsl" e "shader_fragment.glsl".
    diffuse_color_uniform           = glGetUniformLocation(gpuProgramID, "diffuse_color");
    diffuse_intensity_uniform       = glGetUniformLocation(gpuProgramID, "diffuse_intensity");
    ambient_color_uniform           = glGetUniformLocation(gpuProgramID, "ambient_color");
    ambient_intensity_uniform       = glGetUniformLocation(gpuProgramID, "ambient_intensity");
    specular_color_uniform          = glGetUniformLocation(gpuProgramID, "specular_color");
    specular_intensity_uniform      = glGetUniformLocation(gpuProgramID, "specular_intensity");
    shine_coefficient_uniform       = glGetUniformLocation(gpuProgramID, "shine_coefficient");
    model_uniform      = glGetUniformLocation(gpuProgramID, "model"); // Variável da matriz "model"
    view_uniform       = glGetUniformLocation(gpuProgramID, "view"); // Variável da matriz "view" em shader_vertex.glsl
    projection_uniform = glGetUniformLocation(gpuProgramID, "projection"); // Variável da matriz "projection" em shader_vertex.glsl
    object_id_uniform  = glGetUniformLocation(gpuProgramID, "object_id"); // Variável "object_id" em shader_fragment.glsl

    vertex_shader_type_uniform   = glGetUniformLocation(gpuProgramID, "vertex_shader_type");
    fragment_shader_type_uniform = glGetUniformLocation(gpuProgramID, "fragment_shader_type");
    
    close2GL_uniform  = glGetUniformLocation(gpuProgramID, "close2GL_active");
}

// Carrega um Vertex Shader de um arquivo GLSL. Veja definição de LoadShader() abaixo.
GLuint Renderer::LoadShader_Vertex(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos vértices.
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, vertex_shader_id);

    // Retorna o ID gerado acima
    return vertex_shader_id;
}

// Carrega um Fragment Shader de um arquivo GLSL . Veja definição de LoadShader() abaixo.
GLuint Renderer::LoadShader_Fragment(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos fragmentos.
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, fragment_shader_id);

    // Retorna o ID gerado acima
    return fragment_shader_id;
}

// Função auxilar, utilizada pelas duas funções acima. Carrega código de GPU de
// um arquivo GLSL e faz sua compilação.
void Renderer::LoadShader(const char* filename, GLuint shader_id)
{
    // Lemos o arquivo de texto indicado pela variável "filename"
    // e colocamos seu conteúdo em memória, apontado pela variável
    // "shader_string".GL_CW);
    std::ifstream file;
    try {
        file.exceptions(std::ifstream::failbit);
        file.open(filename);
    } catch ( std::exception& e ) {
        fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }
    std::stringstream shader;
    shader << file.rdbuf();
    std::string str = shader.str();
    const GLchar* shader_string = str.c_str();
    const GLint   shader_string_length = static_cast<GLint>( str.length() );

    // Define o código do shader GLSL, contido na string "shader_string"
    glShaderSource(shader_id, 1, &shader_string, &shader_string_length);

    // Compila o código do shader GLSL (em tempo de execução)
    glCompileShader(shader_id);

    // Verificamos se ocorreu algum erro ou "warning" durante a compilação
    GLint compiled_ok;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    // Alocamos memória para guardar o log de compilação.
    // A chamada "new" em C++ é equivalente ao "malloc()" do C.
    GLchar* log = new GLchar[log_length];
    glGetShaderInfoLog(shader_id, log_length, &log_length, log);

    // Imprime no terminal qualquer erro ou "warning" de compilação
    if ( log_length != 0 )
    {
        std::string  output;

        if ( !compiled_ok )
        {
            output += "ERROR: OpenGL compilation of \"";
            output += filename;
            output += "\" failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else
        {
            output += "WARNING: OpenGL compilation of \"";
            output += filename;
            output += "\".\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }

        fprintf(stderr, "%s", output.c_str());
    }

    // A chamada "delete" em C++ é equivalente ao "free()" do C
    delete [] log;
}
GLuint Renderer::CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id){
     // Criamos um identificador (ID) para este programa de GPU
    GLuint program_id = glCreateProgram();

    // Definição dos dois shaders GLSL que devem ser executados pelo programa
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);

    // Linkagem dos shaders acima ao programa
    glLinkProgram(program_id);

    // Verificamos se ocorreu algum erro durante a linkagem
    GLint linked_ok = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

    // Imprime no terminal qualquer erro de linkagem
    if ( linked_ok == GL_FALSE )
    {
        GLint log_length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

        // Alocamos memória para guardar o log de compilação.
        // A chamada "new" em C++ é equivalente ao "malloc()" do C.
        GLchar* log = new GLchar[log_length];

        glGetProgramInfoLog(program_id, log_length, &log_length, log);

        std::string output;

        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "\n== End of link log\n";

        // A chamada "delete" em C++ é equivalente ao "free()" do C
        delete [] log;

        fprintf(stderr, "%s", output.c_str());
    }

    // Os "Shader Objects" podem ser marcados para deleção após serem linkados 
    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    // Retornamos o ID gerado acima
    return program_id;
}
void Renderer::setRenderMode(GLint renderMode)
{
    switch (renderMode)
    {
    case 0://solid/fill
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    case 1://wireframe
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case 2://points
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;
    default:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    }
}
void Renderer::setEngine(Engines engine){
    switch (engine)
    {
    case OpenGL:
        glUniform1i(close2GL_uniform, false);
        this->close2GL_active = false;
        break;
    case Close2GL:
        glUniform1i(close2GL_uniform, true);
        this->close2GL_active = true;

        break;
    default:
        glUniform1i(close2GL_uniform, false);
        this->close2GL_active = false;

        break;
    }
}
GLint Renderer::getFrontFaceOrientation(){
    return this->frontFace;
}
bool Renderer::isClosed2GLActive(){
    return this->close2GL_active;
}

void Renderer::processTrianglesClosed2GL(ModelObject model_object, glm::mat4 model_view_proj,float screen_width, float screen_height){
    int clipped_vertices = 0;
    std::vector<GLfloat> close2gl_coefficients;
    for (Triangle tri: model_object.triangles) {
        bool clip_vertices = false;
        // three triangles vertices
        glm::vec4 coords1 = glm::vec4(tri.vertices[0].pos,1.0f);
        glm::vec4 coords2 = glm::vec4(tri.vertices[1].pos,1.0f);
        glm::vec4 coords3 = glm::vec4(tri.vertices[2].pos,1.0f);                   
        coords1 = model_view_proj * coords1;
        coords2 = model_view_proj * coords2;
        coords3 = model_view_proj * coords3;
        
        // clip if w <=  0
        if (coords1.w <= 0 || coords2.w <= 0 || coords3.w <= 0) {
            clipped_vertices += 3;
        } else {
            // division by w
            coords1.x /= coords1.w;
            coords1.y /= coords1.w;
            coords1.z /= coords1.w;
            coords1.w /= coords1.w;
            coords2.x /= coords2.w;
            coords2.y /= coords2.w;
            coords2.z /= coords2.w;
            coords2.w /= coords2.w;
            coords3.x /= coords3.w;
            coords3.y /= coords3.w;
            coords3.z /= coords3.w;
            coords3.w /= coords3.w;
            // clip if z outside (-1, 1)
            if ( coords1.z < -1 || coords1.z > 1 ||
                    coords2.z < -1 || coords2.z > 1 ||
                    coords3.z < -1 || coords3.z > 1 ) {
                clipped_vertices += 3;
            } else {
                
                // calculate screen coordinates for backface culling
                glm::mat4 viewport = Matrix_Viewport(0.0f, (float)screen_width, (float)screen_height, 0.0f);
                glm::vec4 coords1sc = viewport * coords1;
                glm::vec4 coords2sc = viewport * coords2;
                glm::vec4 coords3sc = viewport * coords3;
        
                // backface culling
                float area = 0;
                float sum  = 0;
                sum += (coords1sc.x*coords2sc.y - coords2sc.x*coords1sc.y);
                sum += (coords2sc.x*coords3sc.y - coords3sc.x*coords2sc.y);
                sum += (coords3sc.x*coords1sc.y - coords1sc.x*coords3sc.y);
                area = 0.5f * sum;
                
                if (this->frontFace == GL_CW) { // clockwise
                    if (area < 0) {
                        // cull
                        clipped_vertices += 3;
                    } else {
                        close2gl_coefficients.push_back(coords1.x);
                        close2gl_coefficients.push_back(coords1.y);
                        close2gl_coefficients.push_back(coords1.z);
                        close2gl_coefficients.push_back(coords1.w);
                        close2gl_coefficients.push_back(coords2.x);
                        close2gl_coefficients.push_back(coords2.y);
                        close2gl_coefficients.push_back(coords2.z);
                        close2gl_coefficients.push_back(coords2.w);
                        close2gl_coefficients.push_back(coords3.x);
                        close2gl_coefficients.push_back(coords3.y);
                        close2gl_coefficients.push_back(coords3.z);
                        close2gl_coefficients.push_back(coords3.w);
                    }
                } else { // counterclockwise
                    if (area > 0) {
                        // cull
                        clipped_vertices += 3;
                    } else {
                        close2gl_coefficients.push_back(coords1.x);
                        close2gl_coefficients.push_back(coords1.y);
                        close2gl_coefficients.push_back(coords1.z);
                        close2gl_coefficients.push_back(coords1.w);
                        close2gl_coefficients.push_back(coords2.x);
                        close2gl_coefficients.push_back(coords2.y);
                        close2gl_coefficients.push_back(coords2.z);
                        close2gl_coefficients.push_back(coords2.w);
                        close2gl_coefficients.push_back(coords3.x);
                        close2gl_coefficients.push_back(coords3.y);
                        close2gl_coefficients.push_back(coords3.z);
                        close2gl_coefficients.push_back(coords3.w);
                    }
                }
            }
        }
        int num_vertices = model_object.num_triangles*3;
        num_vertices -= clipped_vertices;
        std::vector<GLuint> indices;
        for (int i = 0; i < num_vertices; i++) {
            indices.push_back(i);
        }
        model_object.setModelCoefficients(close2gl_coefficients);
        model_object.setIndices(indices);
        model_object.setupVao();
    }
}
