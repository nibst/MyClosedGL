#include "primitives.hpp"

class Edge{
    private:

    public:
        Vertex start,end;
        float x_increment_per_y,z_increment_per_y; //how much should increment when going to next scanline
        float z_increment_per_x;
        glm::vec3 current_position_along_edge;
        Edge();
        Edge(Vertex start, Vertex end);
        void increment();
        void setCurrentY(float y);
};

