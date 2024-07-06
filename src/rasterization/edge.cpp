#include "edge.hpp"

Edge::Edge()
{}

Edge::Edge(Vertex start, Vertex end)
{
    this->start = start;
    this->end = end;

    this->current_position_along_edge = start.pos;

    float dx = end.pos.x - start.pos.x;
    float dy = end.pos.y - start.pos.y;
    float dz = end.pos.z - start.pos.z;
    if(dx ==0.0f){//actually this will never happen i think, but just in case...
        this->z_increment_per_x = 0.0f;//this doesnt have any logic behing, just put a 0 in it
    }
    else this->z_increment_per_x = dz/dx;

    if (dy == 0.0f) // Horizontal edge aligned to scanline
    {
        this->x_increment_per_y = dx;//the edge is aligned to the scanline, so per y (scanline) we move to next active edge
        this->z_increment_per_y = 0.0f;
    }
    else{
        this->x_increment_per_y = dx / dy;
        this->z_increment_per_y = dz / dy;
    }
}
void Edge::increment(){
    this->current_position_along_edge.x += x_increment_per_y;
    this->current_position_along_edge.z += z_increment_per_y;
}
void Edge::setCurrentY(float y){
    this->current_position_along_edge.y = y;
}
