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

    glEnable(GL_DEPTH_TEST);
}

void mainLoop() {
    bool isRunning = true;
    SDL_Event e;

    Vertex v[8] = {
        { -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  1.0f },   // FRONT Upper Left
        {  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  1.0f },   // FRONT Upper Right
        { -0.5f, -0.5f,  0.5f,  1.0f,  1.0f,  0.0f },   // FRONT Botton Left
        {  0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f },   // FRONT Botton Right

        { -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  1.0f },   // BACK Upper Left
        {  0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  1.0f },   // BACK Upper Right
        { -0.5f, -0.5f, -0.5f,  1.0f,  1.0f,  0.0f },   // BACK Botton Left
        {  0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f }    // BACK Botton Right
    };

    GLint i[36] = {
        //FRONT FACE
        0, 1, 2,
        1, 2, 3,
        //BACK FACE
        4, 5, 6,
        5, 6, 7,
        //UPPER FACE
        0, 1, 4,
        4, 1, 5,
        //BOTTON FACE
        2, 3, 6,
        6, 3, 7,
        //RIGHT FACE
        1, 3, 7,
        1, 7, 5,
        //LEFT FACE
        0, 2, 6,
        0, 6, 4
    };

    Shader basicShader("resources/shaders/base.shader");

    Mesh quad(i, 36, v, 8, &basicShader);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);

    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));
    proj = glm::perspective(glm::radians(60.0f), (float)(SCREEN_WIDTH/SCREEN_HEIGHT), 0.3f, 200.0f);

    float angle = 0.0f;

    unsigned int tm1 = 0, tm2 = 0, delta = 0;

    //Game Loop
    while(isRunning) {
        tm1 = SDL_GetTicks();
        delta = tm1 - tm2;

        if (delta > 1000/120.0) {
            //Handle events on queue
            while(SDL_PollEvent(&e) != 0){
                switch(e.type) {
                case SDL_QUIT:
                    isRunning = false;
                    break;
                
                default:
                    break;
                }
            }

            //Clear screen
            glClearColor(0.07f, 0.13f, 0.17f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            model = glm::rotate(model, glm::radians(1.0f), glm::vec3(0.5f, 0.5f, 0.0f));

            basicShader.enable();

            int modelLoc = glGetUniformLocation(basicShader.uId, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            int viewLoc = glGetUniformLocation(basicShader.uId, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

            int projLoc = glGetUniformLocation(basicShader.uId, "proj");
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

            if (angle++ > 360) angle = 0;

            //Draw Elements
            quad.draw();

            // Exchange frame (?) buffer (see Vulkan tutorial for what this is)
            SDL_GL_SwapWindow(window);

            tm2 = tm1;
        }
    }
}

void finishError(std::string err_msg) {
    std::cout << "Ocorreu um problema durante a execucao do programa: " << err_msg << std::endl;
    fflush(stdout);
    //SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", err_msg.c_str(), NULL);
    SDL_Quit();
}

double getDeltaTime(unsigned int startTime) {
    
}