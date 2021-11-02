#include <core/main.hpp>

    SDL_Window      *window;
    SDL_GLContext    context;
    bool             running;
    const char*      title;
 
int main(int argc, char** argv) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) finishError("Nao foi possivel inicializar o SDL");

    setupWindow("Marching Cubes");
    mainLoop();

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void setupWindow(const char *title){
    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    context = SDL_GL_CreateContext(window);
    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

    if (!window || !context) finishError("Nao foi possivel inicializar a Janela");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    SDL_GL_SetSwapInterval(0);
}

void mainLoop() {
    SDL_Event e;

    Parameters *param = new Parameters();

    Camera camera = Camera(
        glm::vec3(0.0f, 0.0f, 20.5f),
        60.0f, window
    );

    Compute program(param);

    program.start();

    running = true;

    //GAME LOOP
    while(running) {

        // Framerate Control 
        TimeDeltaTime = timeControl();

        bool changedMesh = false;

        // INPUT HANDLER
        while(SDL_PollEvent(&e) != 0){
            switch(e.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_WINDOWEVENT:
                    if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                        int width, height;
                        SDL_GetWindowSize(window, &width, &height);
                        glViewport(0, 0, width, height);
                    }
                    break;
                case SDL_KEYDOWN:
                    switch(e.key.keysym.sym) {
                        case SDLK_LEFT:  param->noiseDisplacement.x += 0.5f;         changedMesh = true; break;
                        case SDLK_RIGHT: param->noiseDisplacement.x -= 0.5f;         changedMesh = true; break;
                        case SDLK_UP:    param->noiseDisplacement.z += 0.5f;         changedMesh = true; break;
                        case SDLK_DOWN:  param->noiseDisplacement.z -= 0.5f;         changedMesh = true; break;
                        case SDLK_i:     param->noiseScale += 0.5f;                  changedMesh = true; break;
                        case SDLK_o:     param->noiseScale -= 0.5f;                  changedMesh = true; break;
                        case SDLK_q:     param->surfaceLevel -= 0.1f;                changedMesh = true; break;
                        case SDLK_e:     param->surfaceLevel += 0.1f;                changedMesh = true; break;
                        case SDLK_t:     param->pointDencity += 0.5f;                changedMesh = true; break;
                        case SDLK_y:     param->pointDencity -= 0.5f;                changedMesh = true; break;
                        case SDLK_k:     param->smooth       = !param->smooth;       changedMesh = true; break;
                        case SDLK_j:     param->linearInterp = !param->linearInterp; changedMesh = true; break;
                        case SDLK_g:     param->useGPU       = !param->useGPU;       changedMesh = true; break;
                    }
                    break;
                default:
                    break;
            }

            camera.handleInputs(e);
        }

        if (changedMesh) program.start();

        // UPDATE
        {
            camera.update();
            program.update();
        }

        // DRAW
        {
            //BEGIN DRAW: Clear Screen
            glClearColor(0.07f, 0.13f, 0.17f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            program.draw();

            //END DRAW: Swap Front Buffer and Back Buffer
            SDL_GL_SwapWindow(window);
        }
    }
}

void finishError(std::string err_msg) {
    std::cout << "Ocorreu um problema durante a execucao do programa: " << err_msg << std::endl;
    fflush(stdout);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", err_msg.c_str(), NULL);
    SDL_Quit();
}

double timeControl(){
	static int frames = 0;
	static unsigned int terminoFrame = 0, inicioFrame = 0, timerFrame = 0;
	
	frames ++;
	inicioFrame = SDL_GetTicks();
	double deltaTime = (double)(inicioFrame - terminoFrame) / 100;
	
	if (SDL_TICKS_PASSED(inicioFrame, timerFrame + 1000)){
        std::string title = "Matching Cubes (" + std::to_string(frames) + " FPS)";
        SDL_SetWindowTitle(window, title.c_str());

		frames = 0;
		timerFrame = inicioFrame;
	}
	
	terminoFrame = inicioFrame;
	
	if (deltaTime < 1000 / 400) {
		SDL_Delay(1000 / 400 - deltaTime);
	}

    return deltaTime;
}

void GLAPIENTRY MessageCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam )
{
    if (type == GL_DEBUG_TYPE_ERROR) {
        finishError(message);
    } else {
        //std::cout << "[WARNING] " << message << std::endl;
    }
}