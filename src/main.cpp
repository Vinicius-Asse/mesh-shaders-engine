#include <core/main.hpp>

    SDL_Window      *window;
    SDL_GLContext    context;
 
int main(int argc, char** argv) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) finishError("Nao foi possivel inicializar o SDL");

    setupWindow("Game Window");
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

    SDL_GL_SetSwapInterval(0);
}

void mainLoop() {
    SDL_Event e;
    bool isRunning = true;

    Shader basicShader("resources/shaders/base.shader");

    Camera camera(
        glm::vec3(0.0f, 1.0f, 5.0f),
        45.0f
    );

    Cube cube = Cube::getInstance(
        glm::vec3(0.0f, 0.0f, 2.5f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        &basicShader);

    Sphere sphere = Sphere::getInstance(
        glm::vec3(0.0f, 1.25f, 2.5f),
        glm::vec3(1.5f, 1.5f, 1.5f),
        32, 16,
        &basicShader);

    //GAME LOOP
    while(isRunning) {

        // Framerate Control 
        TimeDeltaTime = timeControl();

        // INPUT HANDLER
        while(SDL_PollEvent(&e) != 0){
            switch(e.type) {
            case SDL_QUIT:
                isRunning = false;
                break;
            default:
                break;
            }

            camera.handleInputs(e, window);
        }

        // UPDATE
        {
            camera.update(window);
            //cube.rotate(glm::vec3(0.5f, 0.5f, 0.0f));
            //sphere.rotate(glm::vec3(0.5f, 0.5f, 0.0f));
        }


        // DRAW
        {
            //BEGIN DRAW: Clear Screen
            glClearColor(0.07f, 0.13f, 0.17f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            cube.draw();
            sphere.draw();

            //END DRAW: Swap Front Buffer and Back Buffer
            SDL_GL_SwapWindow(window);
        }
    }
}

void finishError(std::string err_msg) {
    std::cout << "Ocorreu um problema durante a execucao do programa: " << err_msg << std::endl;
    fflush(stdout);
    //SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", err_msg.c_str(), NULL);
    SDL_Quit();
}

double timeControl(){
	static int frames = 0;
	static unsigned int terminoFrame = 0, inicioFrame = 0, timerFrame = 0;
	
	frames ++;
	inicioFrame = SDL_GetTicks();
	double deltaTime = (double)(inicioFrame - terminoFrame) / 100;
	
	if (SDL_TICKS_PASSED(inicioFrame, timerFrame + 1000)){
		if (0) printf ("%d FPS\n", frames);
		frames = 0;
		timerFrame = inicioFrame;
	}
	
	terminoFrame = inicioFrame;
	
	if (deltaTime < 1000 / 400) {
		SDL_Delay(1000 / 400 - deltaTime);
	}

    return deltaTime;
}