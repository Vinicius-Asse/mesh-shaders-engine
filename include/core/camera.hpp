#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/perpendicular.hpp>


class Camera {
private:
    glm::vec3 position;

    glm::vec3 up          = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);

    glm::vec2 inputAxis   = glm::vec3(0.0f, 0.0f, 0.0f);

    float moveSpeed, fov;
    bool running = false;
    bool mouseEnabled = false;

public:
    static Camera* MainCamera;

    SDL_Window* window;
    glm::vec3 ligthDir    = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 ligthColor  = glm::vec3(1.0f, 1.0f, 1.0f);
    
    bool fixedLight;

    Camera(glm::vec3 position, float fov, SDL_Window *window);
    //~Camera();

    void setActive();
    void update();
    void handleInputs(SDL_Event e);

    glm::mat4 getViewProjectionMatrix();
    glm::mat4 getMVPMatrix(glm::mat4 model);
};