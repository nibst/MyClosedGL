#include "primitives.hpp"
#include "color_buffer.hpp"
#include <string.h>
#include <cmath>
#include <iostream>


ColorBuffer::ColorBuffer(int newWidth, int newHeight){
    this->pixels = new ColorPixel[newWidth * newHeight];
    memset(this->pixels, 0.0f, sizeof(ColorPixel) * newWidth * newHeight);
    this->width = newWidth;
    this->height = newHeight;
}
ColorBuffer::~ColorBuffer(){
    delete[] this->pixels;
}
void ColorBuffer::resize(int newWidth, int newHeight){
    delete[] this->pixels;
    this->pixels = new ColorPixel[newWidth * newHeight];
    memset(this->pixels, 0, sizeof(float) * newWidth * newHeight);
    width = newWidth;
    height = newHeight;
}
void ColorBuffer::draw(glm::vec3 position, glm::vec4 color){
    if (position.x >= 0 && position.x < width && position.y >= 0 && position.y < height){
        (this->pixels + (int)std::floor(position.y) * width  + (int)std::floor(position.x))->r = std::min(1.0f, std::max(color.x, 0.0f));
        (this->pixels + (int)std::floor(position.y) * width  + (int)std::floor(position.x))->g = std::min(1.0f, std::max(color.y, 0.0f));
        (this->pixels + (int)std::floor(position.y) * width  + (int)std::floor(position.x))->b = std::min(1.0f, std::max(color.z, 0.0f));
        (this->pixels + (int)std::floor(position.y) * width  + (int)std::floor(position.x))->a = std::min(1.0f, std::max(color.w, 0.0f));
    }
}
void ColorBuffer::clear(){
    delete[] this->pixels;
    this->pixels = new ColorPixel[width * height];
    memset(this->pixels, 0.0f, sizeof(ColorPixel) * width * height);
}