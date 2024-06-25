#include "model_object.hpp"
#include <glm/vec4.hpp>
#include <iostream>
#include <cmath>


std::string ModelObject::getName(){return this->name;}
size_t ModelObject::getFirstIndex(){return this->first_index;}
size_t ModelObject::getNumIndices(){return this->num_indices;}
GLenum ModelObject::getRenderingMode(){return this->rendering_mode;}
VAO ModelObject::getVAO(){return this->vao;}

ModelObject::ModelObject(){;}
ModelObject::ModelObject(std::string name,size_t first_index,size_t num_indices,
            GLenum rendering_mode,VAO vao,glm::vec3 bbox_min,glm::vec3 bbox_max,GLint id):
            name(name),first_index(first_index),num_indices(num_indices),rendering_mode(rendering_mode),
            vao(vao),id(id){;}

void ModelObject::setObjectID(GLint id){
    this->id = id;
}
GLint ModelObject::getID(){
    return this->id;
}
// Função que computa as normais de um Object, caso elas não tenham sido
// especificadas dentro do arquivo ".obj"
void ModelObject::computeNormals(Object* model)
{
    if ( !model->attrib.normals.empty() )
        return;

    // Primeiro computamos as normais para todos os TRIÂNGULOS.
    // Segundo, computamos as normais dos VÉRTICES através do método proposto
    // por Gouraud, onde a normal de cada vértice vai ser a média das normais de
    // todas as faces que compartilham este vértice.

    size_t num_vertices = model->attrib.vertices.size() / 3;

    std::vector<int> num_triangles_per_vertex(num_vertices, 0);
    std::vector<glm::vec4> vertex_normals(num_vertices, glm::vec4(0.0f,0.0f,0.0f,0.0f));

    for (size_t shape = 0; shape < model->shapes.size(); ++shape)
    {
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);

            glm::vec4  vertices[3];
            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];
                const float vx = model->attrib.vertices[3*idx.vertex_index + 0];
                const float vy = model->attrib.vertices[3*idx.vertex_index + 1];
                const float vz = model->attrib.vertices[3*idx.vertex_index + 2];
                vertices[vertex] = glm::vec4(vx,vy,vz,1.0);
            }

            const glm::vec4  a = vertices[0];
            const glm::vec4  b = vertices[1];
            const glm::vec4  c = vertices[2];

            const glm::vec4  n = crossproduct(b-a,c-a);

            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];
                num_triangles_per_vertex[idx.vertex_index] += 1;
                vertex_normals[idx.vertex_index] += n;
                model->shapes[shape].mesh.indices[3*triangle + vertex].normal_index = idx.vertex_index;
            }
        }
    }

    model->attrib.normals.resize( 3*num_vertices );

    for (size_t i = 0; i < vertex_normals.size(); ++i)
    {
        glm::vec4 n = vertex_normals[i] / (float)num_triangles_per_vertex[i];
        n /= norm(n);
        model->attrib.normals[3*i + 0] = n.x;
        model->attrib.normals[3*i + 1] = n.y;
        model->attrib.normals[3*i + 2] = n.z;
    }
}
// Constrói triângulos para futura renderização a partir de um Object.
void ModelObject::buildTriangles(Object* model)
{

    std::vector<GLuint> indices;
    std::vector<float>  model_coefficients;
    std::vector<float>  normal_coefficients;
    std::vector<float>  texture_coefficients;
    for (size_t shape = 0; shape < model->shapes.size(); ++shape)
    {
        size_t first_index = indices.size();
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();
        const float minval = std::numeric_limits<float>::min();
        const float maxval = std::numeric_limits<float>::max();

        glm::vec3 bbox_min = glm::vec3(maxval,maxval,maxval);
        glm::vec3 bbox_max = glm::vec3(minval,minval,minval);

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);

            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];

                indices.push_back(first_index + 3*triangle + vertex);
          
                const float vx = model->attrib.vertices[3*idx.vertex_index + 0];
                const float vy = model->attrib.vertices[3*idx.vertex_index + 1];
                const float vz = model->attrib.vertices[3*idx.vertex_index + 2];
                //printf("tri %d vert %d = (%.2f, %.2f, %.2f)\n", (int)triangle, (int)vertex, vx, vy, vz);
                model_coefficients.push_back( vx ); // X
                model_coefficients.push_back( vy ); // Y
                model_coefficients.push_back( vz ); // Z
                model_coefficients.push_back( 1.0f ); // W

                bbox_min.x = std::min(bbox_min.x, vx);
                bbox_min.y = std::min(bbox_min.y, vy);
                bbox_min.z = std::min(bbox_min.z, vz);
                bbox_max.x = std::max(bbox_max.x, vx);
                bbox_max.y = std::max(bbox_max.y, vy);
                bbox_max.z = std::max(bbox_max.z, vz);

                // Inspecionando o código da tinyobjloader, o aluno Bernardo
                // Sulzbach (2017/1) apontou que a maneira correta de testar se
                // existem normais e coordenadas de textura no Object é
                // comparando se o índice retornado é -1. Fazemos isso abaixo.

                if ( idx.normal_index != -1 )
                {
                    const float nx = model->attrib.normals[3*idx.normal_index + 0];
                    const float ny = model->attrib.normals[3*idx.normal_index + 1];
                    const float nz = model->attrib.normals[3*idx.normal_index + 2];
                    normal_coefficients.push_back( nx ); // X
                    normal_coefficients.push_back( ny ); // Y
                    normal_coefficients.push_back( nz ); // Z
                    normal_coefficients.push_back( 0.0f ); // W
                }

                if ( idx.texcoord_index != -1 )
                {
                    const float u = model->attrib.texcoords[2*idx.texcoord_index + 0];
                    const float v = model->attrib.texcoords[2*idx.texcoord_index + 1];
                    texture_coefficients.push_back( u );
                    texture_coefficients.push_back( v );
                }
            }
        }
        this->vao.loadToVAO(model_coefficients,texture_coefficients,normal_coefficients,indices);
        size_t last_index = indices.size() - 1;
        this->name           = model->shapes[shape].name;
        this->first_index    = first_index; // Primeiro índice
        this->num_indices    = last_index - first_index + 1; // Número de indices
        this->rendering_mode = GL_TRIANGLES;       // Índices correspondem ao tipo de rasterização GL_TRIANGLES.
        this->bbox_max = bbox_max; //
        this->bbox_min = bbox_min;
    }
}


void ModelObject::loadFromOBJFileName(const char* filename){
    Object model(filename);
    this->computeNormals(&model);
    this->buildTriangles(&model);
}
void ModelObject::loadFromInFileName(const char *fileName)
{   
    
    FILE *fp = fopen(fileName, "r");
    if (fp == NULL) {
        std::cerr << "ERROR: Unable to open TriObj [" << fileName << "]!" << std::endl;
        exit(1);
    }
    char name[1024];
	fscanf(fp, "Object name = %s\n", name);
    this->name = std::string(name);
    fscanf(fp, "# triangles = %d\n", &(this->num_triangles)); // read # of triangles
    fscanf(fp, "Material count = %d\n", &(this->num_materials)); // read material count

    this->materials.resize(this->num_materials);
    for (int i = 0; i < this->num_materials; i++) {
        Material &mat = this->materials[i];
        fscanf(fp, "ambient color %f %f %f\n", &mat.ambient_color.x, &mat.ambient_color.y, &mat.ambient_color.z);
        fscanf(fp, "diffuse color %f %f %f\n", &mat.diffuse_color.x, &mat.diffuse_color.y, &mat.diffuse_color.z);
        fscanf(fp, "specular color %f %f %f\n", &mat.specular_color.x, &mat.specular_color.y, &mat.specular_color.z);
        fscanf(fp, "material shine %f\n", &mat.shine);
    }
    char ch;
    fscanf(fp, "%c", &ch);
    while (ch != '\n') // skip documentation line
        fscanf(fp, "%c", &ch);

    this->triangles.resize(this->num_triangles);
    for (int i = 0; i < this->num_triangles; i++) // read triangles
    {
        int color_index[3];
        Triangle &tri = this->triangles[i];//everything i write on tri will be on triangles
        fscanf(fp, "v0 %f %f %f %f %f %f %d\n", &tri.vertices[0].pos.x, &tri.vertices[0].pos.y, &tri.vertices[0].pos.z,
               &tri.vertices[0].normal.x, &tri.vertices[0].normal.y, &tri.vertices[0].normal.z, &color_index[0]);
        fscanf(fp, "v1 %f %f %f %f %f %f %d\n", &tri.vertices[1].pos.x, &tri.vertices[1].pos.y, &tri.vertices[1].pos.z,
               &tri.vertices[1].normal.x, &tri.vertices[1].normal.y, &tri.vertices[1].normal.z, &color_index[1]);
        fscanf(fp, "v2 %f %f %f %f %f %f %d\n", &tri.vertices[2].pos.x, &tri.vertices[2].pos.y, &tri.vertices[2].pos.z,
               &tri.vertices[2].normal.x, &tri.vertices[2].normal.y, &tri.vertices[2].normal.z, &color_index[2]);
        fscanf(fp, "face normal %f %f %f\n", &tri.face_normal.x, &tri.face_normal.y, &tri.face_normal.z);

        tri.vertices[0].color[0] = (unsigned char)(int)(255*(this->materials[color_index[0]].diffuse_color.x));
        tri.vertices[0].color[1] = (unsigned char)(int)(255*(this->materials[color_index[0]].diffuse_color.y));
        tri.vertices[0].color[2] = (unsigned char)(int)(255*(this->materials[color_index[0]].diffuse_color.z));

        tri.vertices[1].color[0] = (unsigned char)(int)(255*(this->materials[color_index[1]].diffuse_color.x));
        tri.vertices[1].color[1] = (unsigned char)(int)(255*(this->materials[color_index[1]].diffuse_color.y));
        tri.vertices[1].color[2] = (unsigned char)(int)(255*(this->materials[color_index[1]].diffuse_color.z));

        tri.vertices[2].color[0] = (unsigned char)(int)(255*(this->materials[color_index[2]].diffuse_color.x));
        tri.vertices[2].color[1] = (unsigned char)(int)(255*(this->materials[color_index[2]].diffuse_color.y));
        tri.vertices[2].color[2] = (unsigned char)(int)(255*(this->materials[color_index[2]].diffuse_color.z));
    
    }
    fclose(fp);
    this->buildTriangles();

}

 
void ModelObject::buildTriangles(){
     
    std::vector<GLuint> indices;
    std::vector<float>  model_coefficients;
    std::vector<float>  normal_coefficients;
    std::vector<float>  texture_coefficients;
    size_t first_index = indices.size();
    size_t num_triangles = this->num_triangles;
    const float minval = std::numeric_limits<float>::min();
    const float maxval = std::numeric_limits<float>::max();

    glm::vec3 bbox_min = glm::vec3(maxval,maxval,maxval);
    glm::vec3 bbox_max = glm::vec3(minval,minval,minval);

    for (size_t triangle = 0; triangle < num_triangles; ++triangle)
    {
        for (size_t vertex = 0; vertex < 3; ++vertex)
        {

            indices.push_back(first_index + 3*triangle + vertex);
        
            const float vx = this->triangles[triangle].vertices[vertex].pos.x;
            const float vy = this->triangles[triangle].vertices[vertex].pos.y;
            const float vz = this->triangles[triangle].vertices[vertex].pos.z;
            //printf("tri %d vert %d = (%.2f, %.2f, %.2f)\n", (int)triangle, (int)vertex, vx, vy, vz);
            model_coefficients.push_back( vx ); // X
            model_coefficients.push_back( vy ); // Y
            model_coefficients.push_back( vz ); // Z
            model_coefficients.push_back( 1.0f ); // W

            bbox_min.x = std::min(bbox_min.x, vx);
            bbox_min.y = std::min(bbox_min.y, vy);
            bbox_min.z = std::min(bbox_min.z, vz);
            bbox_max.x = std::max(bbox_max.x, vx);
            bbox_max.y = std::max(bbox_max.y, vy);
            bbox_max.z = std::max(bbox_max.z, vz);

            const float nx = this->triangles[triangle].vertices[vertex].normal.x;
            const float ny = this->triangles[triangle].vertices[vertex].normal.y;
            const float nz = this->triangles[triangle].vertices[vertex].normal.z;
            normal_coefficients.push_back( nx ); // X
            normal_coefficients.push_back( ny ); // Y
            normal_coefficients.push_back( nz ); // Z
            normal_coefficients.push_back( 0.0f ); // W
        }
    }
    this->model_coefficients = model_coefficients;
    this->normal_coefficients = normal_coefficients;
    this->indices = indices;
    this->texture_coefficients = texture_coefficients;
    this->setupVao();
    size_t last_index = indices.size() - 1;
    this->first_index    = first_index; // Primeiro índice
    this->num_indices    = last_index - first_index + 1; // Número de indices
    this->rendering_mode = GL_TRIANGLES;       // Índices correspondem ao tipo de rasterização GL_TRIANGLES.
    this->bbox_max = bbox_max; //
    this->bbox_min = bbox_min;

}
void ModelObject::setupVao(){
    this->vao.loadToVAO(model_coefficients,texture_coefficients,normal_coefficients,indices);
}
void ModelObject::processTrianglesClosed2GL(glm::mat4 modelViewProj, GLint frontFaceOrientation,float g_ScreenWidth, float g_ScreenHeight){
    int clipped_vertices = 0;
    std::vector<GLfloat> close2gl_coefficients;
    for (Triangle tri: this->triangles) {
        bool clip_vertices = false;
        // three triangles vertices
        glm::vec4 coords1 = glm::vec4(tri.vertices[0].pos,1.0f);
        glm::vec4 coords2 = glm::vec4(tri.vertices[1].pos,1.0f);
        glm::vec4 coords3 = glm::vec4(tri.vertices[2].pos,1.0f);                   
        coords1 = modelViewProj * coords1;
        coords2 = modelViewProj * coords2;
        coords3 = modelViewProj * coords3;
        
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
                glm::mat4 viewport = Matrix_Viewport(0.0f, (float)g_ScreenWidth, (float)g_ScreenHeight, 0.0f);
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
                
                if (frontFaceOrientation == GL_CW) { // clockwise
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
        int num_vertices = this->num_triangles*3;
        num_vertices -= clipped_vertices;
        GLuint indices[num_vertices];
        for (int i = 0; i < num_vertices; i++) {
            indices[i] = i;
        }
        this->model_coefficients = close2gl_coefficients;
        this->setupVao();
    }
}
void ModelObject::setCenterPosition(glm::vec4 center_position){
    this->center_position = center_position;
}
glm::vec4 ModelObject::getCenterPosition(){
    return this->center_position;
}
void ModelObject::setModelCoefficients(std::vector<float> model_coefficients){
    this->model_coefficients = model_coefficients;
}
void ModelObject::setIndices(std::vector<GLuint> indices){
    this->indices = indices;
}
void ModelObject::setNumIndices(size_t num_indices){
    this->num_indices = num_indices;
}