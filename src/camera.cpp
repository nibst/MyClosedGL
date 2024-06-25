#include "camera.hpp"

Camera::Camera():
    position_c(DEFAULT_POS),view_vector(DEFAULT_VIEW),up_vector(DEFAULT_UP),
    phi(DEFAULT_PHI),theta(DEFAULT_THETA),nearplane(DEFAULT_NEAR),farplane(DEFAULT_FAR),
    horizontal_FOV(DEFAULT_FOV),vertical_FOV(DEFAULT_FOV),speed(DEFAULT_SPEED),look_at_point(DEFAULT_LOOK_AT_POINT),
    look_at_camera_distance(DEFAULT_LOOK_AT_CAMERA_DISTANCE){;}

Camera::Camera(glm::vec4 position_c, glm::vec4 view_vector, glm::vec4 up_vector):
    position_c(position_c),view_vector(view_vector),up_vector(up_vector),
    phi(DEFAULT_PHI),theta(DEFAULT_THETA),nearplane(DEFAULT_NEAR),farplane(DEFAULT_FAR),
    horizontal_FOV(DEFAULT_FOV),vertical_FOV(DEFAULT_FOV),speed(DEFAULT_SPEED),
    look_at_camera_distance(DEFAULT_LOOK_AT_CAMERA_DISTANCE){;}

glm::mat4 Camera::getViewMatrix(){
    return Matrix_Camera_View(this->position_c,this->view_vector,this->up_vector, DEFAULT_RIGHT_VECTOR);
}
glm::mat4 Camera::getProjectionMatrix(){
    return Matrix_Perspective(this->vertical_FOV,this->horizontal_FOV,this->nearplane,this->farplane);
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
    glm::vec4 n = -this->view_vector;
    glm::vec4 v = crossproduct(this->up_vector,n);
    n = n / norm(n);
    v = v / norm(v);
    glm::vec4 u = this->up_vector/norm(this->up_vector);
    glm::vec4 movement;
    switch (direction)
    {
    case Forward:
        movement = -n * this->speed * delta_t;
        break;
    case Backwards:
        movement = n * this->speed * delta_t;
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
void Camera::resetCamera(){
    this->position_c = DEFAULT_POS;
    this->look_at_camera_distance = DEFAULT_LOOK_AT_CAMERA_DISTANCE;
    this->phi = DEFAULT_PHI;
    this->theta = DEFAULT_THETA;
    this->reseted = true;
}
bool Camera::isResetedCamera(){
    return this->reseted;
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
void Camera::setLookAtCameraDistance(float look_at_distance){
    this->look_at_camera_distance = look_at_distance;
}
float Camera::getLookAtCameraDistance(){
    return this->look_at_camera_distance;
}
void Camera::setInitialPosition(glm::vec4 initial_position){
    this->initial_position = initial_position;
}
void Camera::rotate(float yaw, float pitch, float roll){
    this->right_vector = DEFAULT_RIGHT_VECTOR;
    this->up_vector = DEFAULT_UP;
    this->view_vector = DEFAULT_VIEW;
    rotatePitch(pitch);
    rotateRoll(roll);
    rotateYaw(yaw);
}

void Camera::rotateRoll(float alpha)
{
    glm::vec4 newU = this->right_vector * (float)cos(alpha) + this->up_vector * (float)sin(alpha);
    glm::vec4 newV = -this->right_vector * (float)sin(alpha) + this->up_vector * (float)cos(alpha);

    this->right_vector = newU;
    this->up_vector = newV;
}

void Camera::rotatePitch(float alpha)
{
    // Left hand coordinate system rotation
    glm::vec4 newV = this->up_vector * (float)cos(alpha) - this->view_vector * (float)sin(alpha);
    glm::vec4 newN = this->up_vector * (float)sin(alpha) + this->view_vector * (float)cos(alpha);

    this->up_vector = newV;
    this->view_vector = newN;
}

void Camera::rotateYaw(float alpha)
{
    // Left hand coordinate system rotation
    glm::vec4 newU = this->right_vector * (float)cos(alpha) - this->view_vector * (float)sin(alpha);
    glm::vec4 newN = this->right_vector * (float)sin(alpha) + this->view_vector * (float)cos(alpha);

    this->right_vector = newU;
    this->view_vector = newN;
}
