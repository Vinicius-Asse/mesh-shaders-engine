#include <core/camera.hpp>

Camera::Camera(glm::vec3 _position, glm::vec3 _rotation, float _fov) {
    position = _position;
    rotation = _rotation;

    up = glm::vec3(0.0f, 1.0f, 0.0f);

    view       = glm::mat4(1.0f);
    projection = glm::mat4(1.0f);

    view = glm::translate(view, _position);
    view = glm::lookAt(
        position,
        rotation,
        up
    );

    projection = glm::perspective(
        glm::radians(_fov), 
        (float) (640/480),
        0.3f,
        200.0f
    );


}

glm::mat4 Camera::getViewProjectionMatrix(){
    return projection * view;
}

glm::mat4 Camera::getMVPMatrix(glm::mat4 _model){
    return getViewProjectionMatrix() * _model;
}

void Camera::rotate(glm::vec3 _rotation){
    view = glm::rotate(view, glm::radians(1.0f), _rotation);
}