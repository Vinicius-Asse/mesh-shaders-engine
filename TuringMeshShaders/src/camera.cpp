#include <core/camera.hpp>

Camera* Camera::MainCamera = nullptr;

Camera::Camera(glm::vec3 _position, float _fov, SDL_Window *_window) {
    position = _position;
    fov = _fov;
    moveSpeed = 25.0f;
    window = _window;
    fixedLight = false;

    if (MainCamera == nullptr) MainCamera = this;
}

glm::mat4 Camera::getViewProjectionMatrix(){
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(
        position,
        position + orientation,
        up
    );

    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    glm::mat4 projection = glm::perspective(
        glm::radians(fov), 
        (float) (width/height),
        0.3f,
        200.0f
    );

    return projection * view;
}

glm::mat4 Camera::getMVPMatrix(glm::mat4 _model){
    return getViewProjectionMatrix() * _model;
}

void Camera::handleInputs(SDL_Event e) {

    int screenWidth, screenHeight;
    SDL_GetWindowSize(window, &screenWidth, &screenHeight);

    switch (e.type) {
        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_LEFT) {
                SDL_ShowCursor(0);
                SDL_WarpMouseInWindow(window, screenWidth / 2, screenHeight / 2);
                mouseEnabled = true;
            }
            break;
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

void Camera::update(double deltaTime) {
    if (mouseEnabled) {
        float mouseSense = 4500.0f; //TODO: Parametrizar sensibilidade

        //TODO: Organizar movimentacao da camera
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        int screenWidth, screenHeight;
        SDL_GetWindowSize(window, &screenWidth, &screenHeight);

        float deltaX = deltaTime * mouseSense * (float)(mouseY - screenHeight / 2) / screenHeight;
        float deltaY = deltaTime * mouseSense * (float)(mouseX - screenWidth / 2) / screenWidth;

        glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-deltaX), glm::normalize(glm::cross(orientation, up)));

        if (!((glm::angle(newOrientation, up) <= glm::radians(5.0f)) || (glm::angle(newOrientation, -up) <= glm::radians(5.0f)))) {
            orientation = newOrientation;
        }

        orientation = glm::rotate(orientation, glm::radians(-deltaY), up);

        glm::vec3 perpendicular = glm::cross(-orientation, up);
        glm::vec3 normalizedMov = glm::normalize((perpendicular * -inputAxis.x) + ( orientation  * -inputAxis.y));

        if (glm::length(inputAxis) != 0) {
            position += normalizedMov * ((moveSpeed * (float) deltaTime) * (running? 2.5f : 1.0f));
        }


        SDL_WarpMouseInWindow(window, screenWidth / 2, screenHeight / 2);
    }
    
    if (!fixedLight) {
        lightDir = orientation;
    }
}

void Camera::setActive() {
    MainCamera = this;
}