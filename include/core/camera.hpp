#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 up;


public:
    glm::mat4 view;
    glm::mat4 projection;

    Camera(glm::vec3 position, glm::vec3 rotation, float fov);
    //~Camera();

    void translate(glm::vec3 position);
    void rotate(glm::vec3 rotation);

    glm::mat4 getViewProjectionMatrix();
    glm::mat4 getMVPMatrix(glm::mat4 model);
};