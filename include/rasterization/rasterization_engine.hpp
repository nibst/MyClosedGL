#include "primitives.hpp"
#include "color_buffer.hpp"
#include "z_buffer.hpp"
#include "edge.hpp"
//WORKS WITH VIEWPORT COORDINATES
class RasterizationEngine{
    private:
        ColorBuffer* color_buffer;
        ZBuffer* z_buffer;
        Edge active_edges[2];
        void drawVertexToBuffers(Vertex vertex);
        void clearBuffers();
        void sortVerticesByTopmost(Vertex *vertices);
        Vertex interpolateLinearlyOverEdge(Edge edge, glm::vec3 current_pos_in_edge);
        void drawOnScanline();
    public:
        void setBuffers(ColorBuffer *color_buffer, ZBuffer *z_buffer);
        void drawTriangleToBuffers(Triangle triangle);
};