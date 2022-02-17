#include <core/main.hpp>

    SDL_Window      *window;
    SDL_GLContext    context;
    const char*      title;
    bool             running       = true;

    int              SCREEN_WIDTH  = 1280; 
    int              SCREEN_HEIGHT = 600;
    int              framerate;
    int              MAX_FPS       = 1000;
 
int main(int argc, char** argv) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) finishError("Nao foi possível inicializar o SDL");

    setupWindow("Marching Cubes");

    std::srand(static_cast<unsigned int>(std::time(nullptr))); 

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init("#version 450");

    mainLoop(io);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void setupWindow(const char *title){

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    
    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );

    context = SDL_GL_CreateContext(window);
    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

    if (!window || !context) finishError("Nao foi possível inicializar a Janela");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    printf("Versao OpenGL: %s\n", glGetString(GL_VERSION));

    SDL_GL_SetSwapInterval(0);
}

void mainLoop(ImGuiIO& io) {
    SDL_Event e;

    Parameters *param = new Parameters();

    Camera camera = Camera(glm::vec3(0.0f, 0.0f, 20.0f), 60.0f, window);

    Shader* baseShader = new Shader("resources/shaders/base.glsl", ShaderType::VERTEX_SHADER);

    Point* points = createPoints(param);

    Program* cpuProgram = new MarchingCubesCPUImpl(param, baseShader, points);
    Program* computeProgram = new MarchingCubesComputeImpl(param, baseShader, points);
    Program* meshProgram = new MarchingCubesMeshImpl(param, baseShader, points);

    Program* program = getProgram(param, cpuProgram, computeProgram, meshProgram);

    //GAME LOOP
    while(running) {
        Uint32 startFrame = SDL_GetTicks();

        // Inicializa frame ImGui
        setupImGuiFrame(io, e);

        // INPUT HANDLER
        while(SDL_PollEvent(&e) != 0){
            switch(e.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                default:
                    break;
            }

            // Captura eventos para movimentação da camera pelo mundo
            if (!io.WantCaptureMouse) {
                camera.handleInputs(e);
            }
        }

        // UPDATE
        {
            // Atualiza a localização dos Pontos 
            updatePoints(points, param);

            // Atualiza a movimentação da camera no mundo
            camera.update();

            // Atualiza a geometria
            program->update();
        }

        // DRAW
        {
            //BEGIN DRAW: Clear Screen
            clearScreen(0.0f, 0.0f, 0.0f, 0.0f);

            program->draw();

            Implementation currImpl = param->impl;

            drawImGuiElements(program, io, param, camera);

            // Imprime a troca na implementação do programa
            if (currImpl != param->impl) {
                program = getProgram(param, cpuProgram, computeProgram, meshProgram);
            }

            //END DRAW: Swap Front Buffer and Back Buffer
            SDL_GL_SwapWindow(window);
        }
    
        // Framerate Control 
        TimeDeltaTime = timeControl(&startFrame);
    }

    delete cpuProgram;
    delete computeProgram;
    delete meshProgram;
    delete param;

    free(points);
}

Point* createPoints(Parameters *param) {
    Point* points = (Point*) malloc(sizeof(Point) * param->pointsCount);

    int minX = -param->worldBounds.x / 2.0f;
    int maxX =  param->worldBounds.x / 2.0f;
    
    int minY = -param->worldBounds.y / 2.0f;
    int maxY =  param->worldBounds.y / 2.0f;

    int minZ = -param->worldBounds.z / 2.0f;
    int maxZ =  param->worldBounds.z / 2.0f;

    for (int i = 0; i < param->pointsCount; i++) {
        glm::vec3 position = glm::ballRand(param->worldBounds.x / 2);
        points[i] = {
            position.x, position.y, position.z, // Position
            0.0f,                               // Value (não utilizado)
            0.0f, 0.0f, 0.0f,                   // Velocity
        };
    }

    LOG("Quantidade de esferas: " << (sizeof(Point)/sizeof(points)));

    return points;
}

void updatePoints(Point* points, Parameters* param) {
    for (int i = 0; i < param->pointsCount; i++) {
        Point p = points[i];

        glm::vec3 velocity = glm::vec3(p.vx, p.vy, p.vz);
        glm::vec3 position = glm::vec3(p.x, p.y, p.z);

        float dist = glm::length2(position);

        glm::vec3 newVelocity = velocity + glm::normalize(position) * glm::min(dist, 0.01f);
        glm::vec3 newPosition = position - newVelocity * (float)TimeDeltaTime * param->simulationSpeed;

        // Colisão com as bordas
        if (newPosition.x >  param->worldBounds.x / 2) newPosition.x =  param->worldBounds.x / 2;
        if (newPosition.x < -param->worldBounds.x / 2) newPosition.x = -param->worldBounds.x / 2;

        if (newPosition.y >  param->worldBounds.y / 2) newPosition.y =  param->worldBounds.y / 2;
        if (newPosition.y < -param->worldBounds.y / 2) newPosition.y = -param->worldBounds.y / 2;

        if (newPosition.z >  param->worldBounds.z / 2) newPosition.z =  param->worldBounds.z / 2;
        if (newPosition.z < -param->worldBounds.z / 2) newPosition.z = -param->worldBounds.z / 2;

        p.x = newPosition.x;
        p.y = newPosition.y;
        p.z = newPosition.z;

        p.vx = newVelocity.x;
        p.vy = newVelocity.y;
        p.vz = newVelocity.z;

        points[i] = p;
    }
}

void finishError(std::string err_msg) {
    std::cout << "Ocorreu um problema durante a execução do programa: " << err_msg << std::endl;
    fflush(stdout);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", err_msg.c_str(), NULL);
    SDL_Quit();
}

double timeControl(Uint32* startFrame){
	static int frames = 0;
	static Uint32 now = 0, timerFrame = 0;
	
	frames ++;
	now = SDL_GetTicks();
	double deltaTime = (double)(now - *startFrame) / 1000;
	
	if (SDL_TICKS_PASSED(*startFrame, timerFrame + 1000)){
        std::string title = "Matching Cubes (" + std::to_string(frames) + " FPS)";
        framerate = frames;
        SDL_SetWindowTitle(window, title.c_str());

		frames = 0;
		timerFrame = *startFrame;
	}

	if (deltaTime < 1000/MAX_FPS) {
		SDL_Delay(1000/MAX_FPS - deltaTime);
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

void setupImGuiFrame(ImGuiIO& io, SDL_Event e) {
    // Ignoring Inputs if mouse is hovering an ImGUI Element
    ImGui_ImplSDL2_ProcessEvent(&e);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void clearScreen(float r, float g, float b, float a) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void drawImGuiElements(Program* program, ImGuiIO& io, Parameters* param, Camera camera) {
    static bool firstDraw = true;
    static GLint maxTaskQnt, maxVertices, maxPrimitives;
    static bool supportMeshShader = false;
    
    if (firstDraw) {
        LOG("Buscando por informações do sistema... ");

        supportMeshShader = SDL_GL_ExtensionSupported("GL_NV_mesh_shader");
        if (supportMeshShader) {
            glGetIntegerv(GL_MAX_DRAW_MESH_TASKS_COUNT_NV, &maxTaskQnt);
            glGetIntegerv(GL_MAX_MESH_OUTPUT_VERTICES_NV, &maxVertices);
            glGetIntegerv(GL_MAX_MESH_OUTPUT_PRIMITIVES_NV, &maxPrimitives);
        }
        firstDraw = false;
    }

    bool useCPU        = param->impl == Implementation::CPU;
    bool useCompute    = param->impl == Implementation::COMPUTE_SHADER;
    bool useMeshShader = param->impl == Implementation::MESH_SHADER;

    float lightDirection[3] = { param->lightDirection.x, param->lightDirection.x, param->lightDirection.x };
    float worldBounds[3]    = { param->worldBounds.x, param->worldBounds.y, param->worldBounds.z };

    int resolutionMultiplier = param->surfaceResolution / 8.0f;

    ImGui::Begin("Informações");
    {
        std::string supportMeshShaderStr = supportMeshShader ? "SIM" : "NÃO";
        ImGui::Text(("Suporte à Mesh Shaders      :" + supportMeshShaderStr).c_str());

        if (supportMeshShader) {
            ImGui::Text(("Qnt. Max. Task Draw         : " + std::to_string(maxTaskQnt)).c_str());
            ImGui::Text(("Qnt. Max. Output Vertices   : " + std::to_string(maxVertices)).c_str());
            ImGui::Text(("Qnt. Max. Output Primitivos : " + std::to_string(maxPrimitives)).c_str());
        }

        ImGui::Separator();

        ImGui::Text(("Quadros por Segundo      : " + std::to_string(framerate)).c_str());
        ImGui::Text(("Quantidade de Triangulos : " + program->meshInfo["trizCount"]).c_str());
        ImGui::Text(("Quantidade de Vertices   : " + program->meshInfo["vertexCount"]).c_str());
        ImGui::Text(("Quantidade de Indices    : " + program->meshInfo["indexCount"]).c_str());
        ImGui::Text(("Tempo de Geração (ms)    : " + program->meshInfo["timeGeneratingMesh"]).c_str());
    }
    ImGui::End();

    ImGui::Begin("Meta Balls");
    {
        ImGui::Text("Quantidade de Esferas");
        ImGui::SliderInt("##pointsCount", &param->pointsCount, 0, 10);

        ImGui::Text("Força de atração");
        ImGui::DragFloat("##gravityForce", &param->gravityForce, 0.001f, 0, 1, "%.2f", 1.0f);

        ImGui::Text("Velocidade de simulação");
        ImGui::DragFloat("##simSpeed", &param->simulationSpeed, 0.001f, 0, 2.5, "%.2f", 1.0f);
    }
    ImGui::End();

    ImGui::Begin("Configurações");
    {
        ImGui::Text("Implementação");

        if (ImGui::Checkbox("CPU", &useCPU)) {
            param->impl = Implementation::CPU;
        }            
        if (ImGui::Checkbox("Compute Shader", &useCompute)) {
            param->impl = Implementation::COMPUTE_SHADER;
        }
        ImGui::BeginDisabled(!supportMeshShader);{
            if (ImGui::Checkbox("Mesh Shader", &useMeshShader)) {
                param->impl = Implementation::MESH_SHADER;
            }
        }
        ImGui::EndDisabled();

        ImGui::Separator();

        if (ImGui::CollapsingHeader("Tela")) {
            ImGui::Checkbox("Tela cheia", &param->fullscreen);
        }

        ImGui::Separator();

        if (ImGui::CollapsingHeader("Mundo")) {
            ImGui::Checkbox("Iluminação fixa", &param->fixedLight);
            
            if (param->fixedLight) {
                ImGui::Text("Direção da iluminação");
                ImGui::SliderFloat3("##Direção da iluminação", lightDirection, -90, 90, "%.3f", 1.0f);
            }

            ImGui::Text("Bordas da Simulação");
            ImGui::DragFloat3("##WorldBounds", worldBounds, .01f, 0.0f, 100.0f, "%.3f", 1.0f);

            ImGui::SameLine();
            ImGui::Checkbox("1:1:1", &param->lockAspectRatio);
        }

        ImGui::Separator();

        if (ImGui::CollapsingHeader("Geometria")) {
            ImGui::Checkbox("Suavizar Malha", &param->smooth);
            ImGui::Checkbox("Interpolação Linear", &param->linearInterp);

            ImGui::Text("Intensidade da Interpolação");
            ImGui::DragFloat("##Inten. da Interpolação", &param->smoothIntersect, 0.1f, 0.0001f, 10.0f, "%.3f");

            ImGui::Text("Resolução da Malha");
            ImGui::SliderInt("##resolution", &resolutionMultiplier, 1, 20, "%d");

            ImGui::Text("Nível da Superfície");
            ImGui::SliderFloat("##surfLevel", &param->surfaceLevel, -1.0, 1.0, "%.3f");
        }
    }
    ImGui::End();

    if (param->lockAspectRatio) {
        if (param->worldBounds.x != worldBounds[0]) {
            worldBounds[2] = worldBounds[1] = worldBounds[0];
        }
        else if (param->worldBounds.y != worldBounds[1]) {
            worldBounds[2] = worldBounds[0] = worldBounds[1];
        }
        else {
            worldBounds[0] = worldBounds[1] = worldBounds[2];
        }
    }
    
    param->surfaceResolution = 8 * resolutionMultiplier;
    param->worldBounds.x = worldBounds[0];
    param->worldBounds.y = worldBounds[1];
    param->worldBounds.z = worldBounds[2];

    if (!param->fixedLight) {
        camera.lightDir.x = lightDirection[0];
        camera.lightDir.y = lightDirection[1];
        camera.lightDir.z = lightDirection[2];
    }

    if (param->fullscreen) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    } else {
        SDL_SetWindowFullscreen(window, 0);
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void queryHardwareInfo(GLint* maxTaskQnt, GLint* maxVertices, GLint* maxPrimitives) {
    glGetIntegerv(GL_MAX_DRAW_MESH_TASKS_COUNT_NV, maxTaskQnt);
    glGetIntegerv(GL_MAX_MESH_OUTPUT_VERTICES_NV, maxVertices);
    glGetIntegerv(GL_MAX_MESH_OUTPUT_PRIMITIVES_NV, maxPrimitives);
}

Program* getProgram(Parameters* param, Program* cpuProgram, Program* computeProgram, Program* meshProgram) {
    Program * program = nullptr;

    switch (param->impl) {
        case Implementation::CPU: {
            LOG("Implementacao atual: CPU");
            program = cpuProgram;
            break;
        }
        case Implementation::COMPUTE_SHADER: {
            LOG("Implementacao atual: COMPUTE_SHADER");
            program = computeProgram;
            break;
        }
        case Implementation::MESH_SHADER: {
            LOG("Implementacao atual: MESH_SHADER");
            program = meshProgram;
            break;
        }
    default:
        LOG("Implementacao inválida");
        break;
    }

    program->start();

    return program;
}