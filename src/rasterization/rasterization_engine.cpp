
#include "rasterization_engine.hpp"
#include "matrices.h"
#include <iostream>
void RasterizationEngine::clearBuffers(){
    this->color_buffer->clear();
    this->z_buffer->clear();
}
void RasterizationEngine::setBuffers(ColorBuffer *color_buffer, ZBuffer *z_buffer){
    this->color_buffer = color_buffer;
    this->z_buffer = z_buffer;
}
void RasterizationEngine::drawVertexToBuffers(Vertex vertex){
    //only draw if depth is closer than the depth pointed by z buffer
    if(vertex.pos.z < this->z_buffer->at(vertex.pos.x,vertex.pos.y))
        this->color_buffer->draw(vertex.pos, glm::vec4(vertex.color, 1.0f));
        this->z_buffer->putAt(vertex.pos,vertex.pos.z);
}
void RasterizationEngine::sortVerticesByTopmost(Vertex *vertices){
    //remember that in screen coordinates the top left is 0,0 and going down gets higher Y values
    Vertex temp;
    if (vertices[0].pos.y > vertices[1].pos.y)
    {
        temp = vertices[0];
        vertices[0] = vertices[1];
        vertices[1] = temp;
    }
    if (vertices[0].pos.y > vertices[2].pos.y)
    {
        temp = vertices[0];
        vertices[0] = vertices[2];
        vertices[2] = temp;
    }
    if (vertices[1].pos.y > vertices[2].pos.y)
    {
        temp = vertices[1];
        vertices[1] = vertices[2];
        vertices[2] = temp;
    }
}
void RasterizationEngine::drawOnScanline(){
    //1. get interpolated points of active edges (left and right points)
    //2. make an "edge" (scanline) between these interpolated points
    //3. draw the points of this new edge (scanline) by interpolation of the edge points
    //  (each point has a delta_x of 1 between them, cause it is in screen coordinates)
    //4. profit

    float current_scanline_y = this->active_edges[0].current_position_along_edge.y;
    //1.
    Vertex left,right;
    if (this->active_edges[0].current_position_along_edge.x < this->active_edges[1].current_position_along_edge.x){
        left = interpolateLinearlyOverEdge(this->active_edges[0],this->active_edges[0].current_position_along_edge);
        right = interpolateLinearlyOverEdge(this->active_edges[1],this->active_edges[1].current_position_along_edge);  
    }
    else{
        right = interpolateLinearlyOverEdge(this->active_edges[0],this->active_edges[0].current_position_along_edge);
        left = interpolateLinearlyOverEdge(this->active_edges[1],this->active_edges[1].current_position_along_edge);  
    }
    //left.pos.y = std::floor(current_scanline_y);
    //right.pos.y = std::floor(current_scanline_y);
    //2 and 3
    if (left.pos.x != right.pos.x)
    {
        drawVertexToBuffers(left);
        drawVertexToBuffers(right);

        Edge scanline_edge = Edge(left, right);

        for (float currentX = left.pos.x, currentZ = left.pos.z;
             currentX <= std::floor(right.pos.x);
             currentX++, currentZ += scanline_edge.z_increment_per_x)
        {
            Vertex point = interpolateLinearlyOverEdge(scanline_edge, scanline_edge.current_position_along_edge);
            drawVertexToBuffers(point);
        }
    }
    //4 yeah

}
Vertex RasterizationEngine::interpolateLinearlyOverEdge(Edge edge, glm::vec3 current_pos_in_edge){
    glm::vec4 edge_vector= glm::vec4((edge.end.pos - edge.start.pos),0.0f);
    glm::vec4 u_vector = glm::vec4(current_pos_in_edge, 1.0f) - glm::vec4(edge.start.pos,1.0);
    //u_vector is basically a vector pointing directly at the point in the edge that we want to interpolate
    //if we do a projection of u_vector over edge_vector we get exactly the vector from edge_start to the point U (of u_vector)
    //in a projection we scale the vector to be projected on (edgeVector):
    //proj = ((u . edge)/(edge . edge)) * edge;
    //this ((u . edge)/(edge . edge)) is the scale factor, or in this case call it alpha 
    float alpha = dotproduct(u_vector,edge_vector) /
                  dotproduct(edge_vector,edge_vector);

    alpha = std::max(0.0f, std::min(alpha, 1.0f));
    Vertex interpolated;
    interpolated.pos = edge.start.pos + alpha * (edge.end.pos - edge.start.pos);
    interpolated.normal = edge.start.normal + alpha * (edge.end.normal  - edge.start.normal);
    interpolated.color = edge.start.color + alpha * (edge.end.color - edge.start.color);    
    return interpolated;

}
void RasterizationEngine::drawTriangleToBuffers(Triangle triangle)
{   

    sortVerticesByTopmost(triangle.vertices);
    for (int i = 0; i < 2; i++){
        this->active_edges[i] = Edge(triangle.vertices[0], triangle.vertices[i + 1]);
    }
    //draw color of vertices and zbuffer
    for (int i = 0; i < 3; i++){
        drawVertexToBuffers(triangle.vertices[i]);
    }
    float currentY = triangle.vertices[0].pos.y;
    float maxY = triangle.vertices[2].pos.y;
    //go along scanline and draw edges
    while (currentY <= maxY + 1)
    {
        active_edges[0].setCurrentY(currentY);
        active_edges[1].setCurrentY(currentY);
        drawOnScanline();
        //change active edge if needed
        if (std::floor(active_edges[0].end.pos.y) <= std::floor(currentY + 1))
            active_edges[0] = Edge(triangle.vertices[1], triangle.vertices[2]);

        if (std::floor(active_edges[1].end.pos.y) <= std::floor(currentY + 1))
            active_edges[1] = Edge(triangle.vertices[1], triangle.vertices[2]);

        active_edges[0].increment();
        active_edges[1].increment();
        currentY++;
    }
}
