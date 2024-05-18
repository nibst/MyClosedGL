#pragma once
#include "matrices.h"
#include "options.hpp"
#define DEFAULT_POS glm::vec4(2.0f,3.0f,2.0f,1.0f)
#define DEFAULT_VIEW glm::vec4(0.0f,0.0f,3.5f,0.0f)
#define DEFAULT_PHI 1.0f
#define DEFAULT_THETA 0.0f
#define DEFAULT_NEAR -0.1f
#define DEFAULT_FAR -150.f
#define DEFAULT_RATIO 1.0f
#define DEFAULT_UP glm::vec4(0.0f,1.0f,0.0f,0.0f)
#define DEFAULT_SPEED 14.5f
#define DEFAULT_FOV 3.141592/3.0f
#define DEFAULT_LOOK_AT_POINT glm::vec4(0.0f,0.0f,1.0f,0.0f)

class Camera{
    public:
        Camera();
        Camera(glm::vec4 position_c, glm::vec4 view_vector, glm::vec4 up_vector);
        glm::mat4 getViewMatrix();
        glm::mat4 getProjectionMatrix();
        glm::vec4 getCenterPosition();
        glm::vec4 getViewVector();
        glm::vec4 getLookAtPoint();
        glm::vec4 getUpVector();
        
        virtual void rotate(float dphi,float dtheta);
        void move(MovementOptions direction, float delta_time);

        float getHorizontalFOV();
        float getVerticalFOV();
    
        void setViewVector(glm::vec4 vector);
        void setLookAtPoint(glm::vec4 look_at_point);
        void setHorizontalFOV(float horizontal_FOV); 
        void setCenterPosition(glm::vec4 position);
        void setVerticalFOV(float vertical_FOV);
        virtual void calculateZoom(double yoffset);

        float speed;
        float phi;
        float theta;
        float nearplane;  // Posição do "near plane"
        float farplane; // Posição do "far plane"

    private:

        float horizontal_FOV;
        float vertical_FOV;
        glm::vec4 view_vector;  // Vetor "view", sentido para onde a câmera está virada
        glm::vec4 up_vector;   // Vetor "up" fixado para apontar para o "céu" (eito Y global)
        glm::vec4 pitch;
        glm::vec4 yaw;
        glm::vec4 roll;
        glm::vec4 look_at_point;
        glm::vec4 position_c ;  // Ponto "c", centro da câmera

};