#include <glm/vec3.hpp>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec3 color;
};

struct Triangle {
    Vertex vertices[3];
    glm::vec3 face_normal; 
};

struct Material {
    glm::vec3 ambient_color;
    glm::vec3 diffuse_color;
    glm::vec3 specular_color;
    float shine;
};

struct ColorPixel{
    float r,g,b,a;
};