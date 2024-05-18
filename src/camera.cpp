#include "camera.hpp"

Camera::Camera():
    position_c(DEFAULT_POS),view_vector(DEFAULT_VIEW),up_vector(DEFAULT_UP),
    phi(DEFAULT_PHI),theta(DEFAULT_THETA),nearplane(DEFAULT_NEAR),farplane(DEFAULT_FAR),
    horizontal_FOV(DEFAULT_FOV),vertical_FOV(DEFAULT_FOV),speed(DEFAULT_SPEED),look_at_point(DEFAULT_LOOK_AT_POINT){;}

Camera::Camera(glm::vec4 position_c, glm::vec4 view_vector, glm::vec4 up_vector):
    position_c(position_c),view_vector(view_vector),up_vector(up_vector),
    phi(DEFAULT_PHI),theta(DEFAULT_THETA),nearplane(DEFAULT_NEAR),farplane(DEFAULT_FAR),
    horizontal_FOV(DEFAULT_FOV),vertical_FOV(DEFAULT_FOV),speed(DEFAULT_SPEED){;}

glm::mat4 Camera::getViewMatrix(){
    return Matrix_Camera_View(this->position_c,this->view_vector,this->up_vector);
}
glm::mat4 Camera::getProjectionMatrix(){
    return Matrix_Projection(this->vertical_FOV,this->horizontal_FOV,this->nearplane,this->farplane);
}
void Camera::setCenterPosition(glm::vec4 position){
    this->position_c = position;
}
glm::vec4 Camera::getCenterPosition(){
    return this->position_c;
}
void Camera::rotate(float dphi,float dtheta){
    this->phi = dphi;
    this->theta = dtheta;
    float y = sin(dphi);
    float z = cos(dphi)*cos(dtheta); //from polar to cartesian coordinates
    float x = cos(dphi)*sin(dtheta);
    this->view_vector = glm::vec4(x,-y,z,0.0f);  // Vetor "view", sentido para onde a câmera está virada
}
glm::vec4 Camera::getViewVector(){
    return this->view_vector;
}

void Camera::move(MovementOptions direction, float delta_t)
{    
    glm::vec4 w = -this->view_vector;
    glm::vec4 v = crossproduct(this->up_vector,w);
    w = w / norm(w);
    v = v / norm(v);
    glm::vec4 u = this->up_vector/norm(this->up_vector);
    glm::vec4 movement;
    switch (direction)
    {
    case Forward:
        movement = -w * this->speed * delta_t;
        break;
    case Backwards:
        movement = w * this->speed * delta_t;
        break;
    case Up:
        movement = u * this->speed * delta_t;
        break;
    case Down:
        movement = -u * this->speed * delta_t;
        break;
    case Right:
        movement = v * this->speed * delta_t;
        break;
    case Left:
        movement = -v * this->speed * delta_t;
        break;
    case NoMovement:
    default:
        movement = glm::vec4(0.0f);
        break;
    }

    this->position_c = this->position_c + movement;
}
void Camera::setViewVector(glm::vec4 view_vector){
    this->view_vector = view_vector;
}
void Camera::setLookAtPoint(glm::vec4 look_at_point){
    this->look_at_point = look_at_point;
}
glm::vec4 Camera::getLookAtPoint(){
    return this->look_at_point;
}
glm::vec4 Camera::getUpVector(){
    return this->up_vector;
}

float Camera::getHorizontalFOV(){
    return this->horizontal_FOV;
}
void Camera::setHorizontalFOV(float horizontal_FOV){
    this->horizontal_FOV = horizontal_FOV;
}

float Camera::getVerticalFOV(){
    return this->vertical_FOV;
}
void Camera::setVerticalFOV(float vertical_FOV){
    this->vertical_FOV = vertical_FOV;
}
void Camera::calculateZoom(double yoffset){;}
