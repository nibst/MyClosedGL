#pragma once
#include "matrices.h"
#include "options.hpp"
#define DEFAULT_LOOK_AT_CAMERA_DISTANCE 3.5f

#define DEFAULT_POS glm::vec4(0.0f,0.0f,DEFAULT_LOOK_AT_CAMERA_DISTANCE,1.0f)
#define DEFAULT_PHI 0.0f
#define DEFAULT_THETA 0.0f
#define DEFAULT_NEAR -0.1f
#define DEFAULT_FAR -150.f
#define DEFAULT_RATIO 1.0f

#define DEFAULT_RIGHT_VECTOR glm::vec4(1.0f,0.0f,0.0f,0.0f) // u vector
#define DEFAULT_UP glm::vec4(0.0f,1.0f,0.0f,0.0f) // v vector
#define DEFAULT_VIEW glm::vec4(0.0f,0.0f,-1.0f,0.0f) // n vector

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
        
        void move(MovementOptions direction, float delta_time);
        void resetCamera();

        float getHorizontalFOV();
        float getVerticalFOV();
    
        void setViewVector(glm::vec4 vector);
        void setLookAtPoint(glm::vec4 look_at_point);
        void setHorizontalFOV(float horizontal_FOV); 
        void setCenterPosition(glm::vec4 position);
        void setVerticalFOV(float vertical_FOV);
        void setLookAtCameraDistance(float look_at_distance);
        void setInitialPosition(glm::vec4 initial_position);
        bool isResetedCamera();
        float getLookAtCameraDistance();
        void rotate(float yaw, float pitch, float roll);
        void lookAtObject(glm::vec4 object_center);

        float speed;
        float phi;
        float theta;
        float nearplane;  // Posição do "near plane"
        float farplane; // Posição do "far plane"
        bool reseted;

    private:
        void rotateYaw(float alpha);
        void rotatePitch(float alpha);
        void rotateRoll(float alpha);
        float look_at_camera_distance;
        float horizontal_FOV;
        float vertical_FOV;
        //u,v,n coordinates system
        glm::vec4 view_vector;  // Vetor "view", sentido para onde a câmera está virada (n)
        glm::vec4 up_vector;    // Vetor "up"       (v)
        glm::vec4 right_vector;  // Vetor "right"    (u)
        float pitch;
        float yaw;
        float roll;

        glm::vec4 look_at_point;
        glm::vec4 position_c ;  // Ponto "c", centro da câmera
        glm::vec4 initial_position;

};