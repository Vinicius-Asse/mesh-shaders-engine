#include <core/camera.hpp>

Camera* Camera::MainCamera = nullptr;

Camera::Camera(glm::vec3 _position, float _fov) {
    position = _position;
    fov = _fov;
    moveSpeed = 1.0f;

    if (MainCamera == nullptr) MainCamera = this;
}

glm::mat4 Camera::getViewProjectionMatrix(){
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(
        position,
        position + orientation,
        up
    );

    glm::mat4 projection = glm::perspective(
        glm::radians(fov), 
        (float) (640/480),
        0.3f,
        200.0f
    );

    return projection * view;
}

glm::mat4 Camera::getMVPMatrix(glm::mat4 _model){
    return getViewProjectionMatrix() * _model;
}

void Camera::handleInputs(SDL_Event e, SDL_Window *window) {
    switch (e.type) {
        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_LEFT) {
                SDL_ShowCursor(0);
                SDL_WarpMouseInWindow(window, 640 / 2, 480 / 2);
                mouseEnabled = true;
            }
        case SDL_KEYDOWN:
            switch(e.key.keysym.sym) {
                case SDLK_a: inputAxis.x = -1; break;
                case SDLK_d: inputAxis.x =  1; break;
                case SDLK_w: inputAxis.y = -1; break;
                case SDLK_s: inputAxis.y =  1; break;
                case SDLK_LSHIFT: running = true; break;
                default: break;
            }
            break;
        case SDL_KEYUP:
            switch(e.key.keysym.sym) {
                case SDLK_a: if (inputAxis.x < 0) inputAxis.x = 0; break;
                case SDLK_d: if (inputAxis.x > 0) inputAxis.x = 0; break;
                case SDLK_w: if (inputAxis.y < 0) inputAxis.y = 0; break;
                case SDLK_s: if (inputAxis.y > 0) inputAxis.y = 0; break;
                case SDLK_LSHIFT: running = false; break;
                case SDLK_ESCAPE: mouseEnabled = false; SDL_ShowCursor(1); break;
                default: break;
            }
            break;
    default:
        break;
    }
}

void Camera::update(SDL_Window *window) {
    if (mouseEnabled) {
        float mouseSense = 350.0f; //TODO: Parametrizar sensibilidade

        //TODO: Organizar movimentacao da camera
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        float deltaX = TimeDeltaTime * mouseSense * (float)(mouseY - 480 / 2) / 480;
        float deltaY = TimeDeltaTime * mouseSense * (float)(mouseX - 640 / 2) / 640;

        glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-deltaX), glm::normalize(glm::cross(orientation, up)));

        if (!((glm::angle(newOrientation, up) <= glm::radians(5.0f)) || (glm::angle(newOrientation, -up) <= glm::radians(5.0f)))) {
            orientation = newOrientation;
        }

        orientation = glm::rotate(orientation, glm::radians(-deltaY), up);

        glm::vec3 perpendicular = glm::cross(-orientation, up);
        glm::vec3 normalizedMov = glm::normalize((perpendicular * -inputAxis.x) + ( orientation  * -inputAxis.y));

        if (glm::length(inputAxis) != 0) {
            position += normalizedMov * ((moveSpeed * (float) TimeDeltaTime) * (running? 2.5f : 1.0f));
        }

        SDL_WarpMouseInWindow(window, 640 / 2, 480 / 2);
    }
}

void Camera::setActive() {
    MainCamera = this;
}