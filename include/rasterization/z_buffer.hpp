#include "primitives.hpp"

class ZBuffer{
    private:
        int    width;
        int    height;
        float *pixels;

    public:
        ZBuffer();
        ZBuffer(int width, int height);
        ~ZBuffer();
        void resize(int newWidth, int newHeight);
        void putAt(glm::vec3 position, float depth);
        //get the depth at x,y position of z buffer
        float at(float x, float y);
        void clear();
};