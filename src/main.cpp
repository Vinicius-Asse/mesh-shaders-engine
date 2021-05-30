#include <core/main.hpp>

    SDL_Window      *window;
    SDL_GLContext    context;

struct ParsedShader
{
    std::string VertexShader;
    std::string FragmentShader;
};
 
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
        { -0.5f,  0.5f, 0, 0, 1.0, 1.0 },   // Upper Left
        {  0.5f,  0.5f, 0, 1.0, 0, 1.0 },   // Upper Right
        { -0.5f, -0.5f, 0, 1.0, 1.0, 0 },   // Botton Left
        {  0.5f, -0.5f, 0, 1.0, 0.0, 0 }    // Botton Right
    };

    GLint i[6] = {
        0, 1, 2,
        1, 2, 3
    };

    Mesh quad(i, v, 4);

    //Setting Up Shaders
    ParsedShader shaders = parseShader("resources/shaders/base.shader");
    unsigned int shader = createShader(shaders.VertexShader, shaders.FragmentShader);
    glUseProgram(shader);
    
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

ParsedShader parseShader(const std::string& filePath) {
    std::ifstream stream(filePath);

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            } else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        } else {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str() };
}

unsigned int compileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int lenght;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
        char* message = (char*)alloca(sizeof(char) * lenght);
        glGetShaderInfoLog(id, lenght, &lenght, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << " shader: " << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}