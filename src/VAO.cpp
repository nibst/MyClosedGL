#include "VAO.hpp"
#include <GL3/gl3w.h>
#include <cstdlib>

VAO::VAO(){
    glGenVertexArrays(1, &id);
}

void VAO::loadToVAO(std::vector<float>  model_coefficients,std::vector<float> texture_coefficients,std::vector<float> normal_coefficients,std::vector<GLuint> indices){
    
    this->bind();
    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"

    this->bindAttrib(location,number_of_dimensions,model_coefficients);

    if ( !normal_coefficients.empty() )
    {
        location = 1; // "(location = 1)" em "shader_vertex.glsl"
        number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
        this->bindAttrib(location,number_of_dimensions,normal_coefficients);
    }

    if ( !texture_coefficients.empty() )
    {
        location = 2; // "(location = 1)" em "shader_vertex.glsl"
        number_of_dimensions = 2; // vec2 em "shader_vertex.glsl"
        this->bindAttrib(location,number_of_dimensions,texture_coefficients);
    }
    this->bindIndices(indices);

}
void VAO::bindAttrib(GLuint location, GLint dimensions,std::vector<float> attribData){
    GLuint VBO_id;
    glGenBuffers(1, &VBO_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_id);
    glBufferData(GL_ARRAY_BUFFER, attribData.size() * sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, attribData.size() * sizeof(float), attribData.data());
    glVertexAttribPointer(location, dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


}
void VAO::bindIndices(std::vector<GLuint> indices){
    GLuint indices_id;
    glGenBuffers(1, &indices_id);
    // "Ligamos" o buffer. Note que o tipo agora é GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), indices.data());
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // XXX Errado!
}
void VAO::bind(){
    glGenVertexArrays(1, &id);

    glBindVertexArray(id);
}
void VAO::unbind(){
    // "Desligamos" o VAO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);
}

GLuint VAO::getID(){
    return id;
}