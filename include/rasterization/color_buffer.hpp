#include "primitives.hpp"
#include <glm/vec4.hpp>
class ColorBuffer{
    private:
        int    width;
        int    height;
        ColorPixel *pixels;
    public:
        ColorBuffer(int newWidth, int newHeight);
        ~ColorBuffer();
        void resize(int newWidth, int newHeight);
        void draw(glm::vec3 position, glm::vec4 color);
        void clear();


};