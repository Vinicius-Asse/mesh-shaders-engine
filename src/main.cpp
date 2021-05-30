#include <core/main.hpp>

    SDL_Window      *window;
    SDL_GLContext    context;
 
int main(int argc, char** argv) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        finishError("Nao foi possivel inicializar o SDL");

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

    if (!window || !context)
        finishError("Nao foi possivel inicializar a Janela");
}

void mainLoop() {
    bool isRunning = true;
    SDL_Event e;

    Vertex v[4] = {
        { -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f },   // Upper Left
        {  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f },   // Upper Right
        { -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f },   // Botton Left
        {  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f }    // Botton Right
    };

    GLint i[6] = {
        0, 1, 2,
        1, 2, 3
    };

    Shader basicShader("resources/shaders/base.shader");
    basicShader.enable();

    Mesh quad(i, 6, v, 4);
    
    //Game Loop
    while(isRunning){
        //Handle events on queue
        while( SDL_PollEvent(&e) != 0 ){
            switch (e.type){

            case SDL_QUIT:
                isRunning = false;
                break;
            
            default:
                break;
            }
        }

        //Clear screen
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //Draw Elements
        quad.draw();

        // Exchange frame (?) buffer (see Vulkan tutorial for what this is)
        SDL_GL_SwapWindow(window);

        //Wait to next frame
    }
}

void finishError(std::string err_msg) {
    std::cout << "Ocorreu um problema durante a execucao do programa: " << err_msg << std::endl;
    fflush(stdout);
    //SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", err_msg.c_str(), NULL);
    SDL_Quit();
}