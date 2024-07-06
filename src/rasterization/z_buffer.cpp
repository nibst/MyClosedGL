#include "primitives.hpp"
#include "z_buffer.hpp"
#include <string.h>
#include <cmath>
#include <iostream>


ZBuffer::ZBuffer(int newWidth, int newHeight){
    this->pixels = new float[newWidth * newHeight];
    memset(this->pixels, 0.0f, sizeof(ColorPixel) * newWidth * newHeight);
    this->width = newWidth;
    this->height = newHeight;
}
ZBuffer::~ZBuffer(){
    delete[] this->pixels;
}
void ZBuffer::resize(int newWidth, int newHeight){
    delete[] this->pixels;
    this->pixels = new float[newWidth * newHeight];
    memset(this->pixels, 0, sizeof(float) * newWidth * newHeight);
    width = newWidth;
    height = newHeight;
}
void ZBuffer::putAt(glm::vec3 position, float depth){
    if (position.x >= 0 && position.x < width && position.y >= 0 && position.y < height){
        *(this->pixels + (int)std::floor(position.y) * width  + (int)std::floor(position.x)) = depth;
    }
}
float ZBuffer::at(float x, float y){
    return *(this->pixels + (int)std::floor(y) * width  + (int)std::floor(x));
}
void ZBuffer::clear(){
    delete[] this->pixels;
    this->pixels = new float[width * height];
    memset(this->pixels, 0.0f, sizeof(float) * width * height);
}