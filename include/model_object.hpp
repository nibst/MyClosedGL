#pragma once
#include <glm/vec3.hpp>
#include <string>
#include <vector>
// Headers da biblioteca para carregar modelos obj
#include <tiny_obj_loader.h>
#include <stb_image.h>
#include "matrices.h"
#include "VAO.hpp"
#include <stdexcept>
// Estrutura que representa um modelo geométrico carregado a partir de um
// arquivo ".obj". Veja https://en.wikipedia.org/wiki/Wavefront_.obj_file .
struct Object
{
    tinyobj::attrib_t                 attrib;
    std::vector<tinyobj::shape_t>     shapes;
    std::vector<tinyobj::material_t>  materials;

    // Este construtor lê o modelo de um arquivo utilizando a biblioteca tinyobjloader.
    // Veja: https://github.com/syoyo/tinyobjloader
    Object(const char* filename, const char* basepath = NULL, bool triangulate = true)
    {
        printf("Carregando objetos do arquivo \"%s\"...\n", filename);

        // Se basepath == NULL, então setamos basepath como o dirname do
        // filename, para que os arquivos MTL sejam corretamente carregados caso
        // estejam no mesmo diretório dos arquivos OBJ.
        std::string fullpath(filename);
        std::string dirname;
        if (basepath == NULL)
        {
            auto i = fullpath.find_last_of("/");
            if (i != std::string::npos)
            {
                dirname = fullpath.substr(0, i+1);
                basepath = dirname.c_str();
            }
        }

        std::string err;
        std::string warn;

        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, basepath, triangulate);

        if (!err.empty())
            fprintf(stderr, "\n%s\n", err.c_str());

        if (!ret)
            throw std::runtime_error("Erro ao carregar modelo.");

        for (size_t shape = 0; shape < shapes.size(); ++shape)
        {
            if (shapes[shape].name.empty())
            {
                fprintf(stderr,
                        "*********************************************\n"
                        "Erro: Objeto sem nome dentro do arquivo '%s'.\n"
                        "Veja https://www.inf.ufrgs.br/~eslgastal/fcg-faq-etc.html#Modelos-3D-no-formato-OBJ .\n"
                        "*********************************************\n",
                    filename);
                throw std::runtime_error("Objeto sem nome.");
            }
            printf("- Objeto '%s'\n", shapes[shape].name.c_str());
        }

        printf("OK.\n");
    }
};
struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec3 color;
};

struct Triangle {
    Vertex vertices[3];
    glm::vec3 face_normal; // Face normal
};

struct Material {
    glm::vec3 ambient_color;
    glm::vec3 diffuse_color;
    glm::vec3 specular_color;
    float shine;
};

class ModelObject{

    public:
        ModelObject();
        ModelObject(std::string name,size_t first_index,size_t num_indices,
                    GLenum rendering_mode,VAO vao,glm::vec3 bbox_min,glm::vec3 bbox_max,GLint id = 0);
        void setObjectID(GLint id);
        std::string getName();
        size_t getFirstIndex();
        size_t getNumIndices();
        GLenum getRenderingMode();
        VAO getVAO();
        GLint getID();
        
        void loadFromOBJFileName(const char* filename);
        void loadFromInFileName(const char *fileName);
        void setupVao();
        void processTrianglesClosed2GL(glm::mat4 modelViewProj, GLint frontFaceOrientation,float g_ScreenWidth, float g_ScreenHeight);
        void setCenterPosition(glm::vec4 center_position);
        glm::vec4 getCenterPosition();
        void setModelCoefficients(std::vector<float> model_coefficients);
        void setIndices(std::vector<GLuint> indices);

        int num_triangles;
        int num_materials;
        std::vector<Material> materials;
        std::vector<Triangle> triangles;
        std::string  name;        // Nome do objeto
        size_t       first_index; // Índice do primeiro vértice dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
        size_t       num_indices; // Número de índices do objeto dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
        GLenum       rendering_mode; // Modo de rasterização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
        VAO          vao; // ID do VAO onde estão armazenados os atributos do modelo
        glm::vec3    bbox_min; // Axis-Aligned Bounding Box do objeto
        glm::vec3    bbox_max;
        GLint id;
        
    private:
        glm::vec4 center_position;
        std::vector<GLuint> indices;
        std::vector<float>  model_coefficients;
        std::vector<float>  normal_coefficients;
        std::vector<float>  texture_coefficients;
        void buildTriangles();
        void buildTriangles(Object* obj);
        void computeNormals(Object* obj);

    };

