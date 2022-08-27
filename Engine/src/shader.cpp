#include<core/shader.hpp>

Shader::Shader(const std::string& shaderPath, ShaderType _type) {
    //TRACE("*Creating Shader. Type: " << ToString(_type) << ", Path: " << shaderPath);
    type = _type;
    composedShader = parseShader(shaderPath);
    uId = createProgram(composedShader);
    //TRACE("*Shader Created. UID: " << std::to_string(uId) << ", Type: " << ToString(_type));
}

Shader::~Shader() {
    glDeleteProgram(uId);
    //TRACE("~Shader Deleted. UID: " << std::to_string(uId) << ", Type: " << ToString(type));
}

void Shader::enable() {
    glUseProgram(uId);
}

void Shader::disable() {
    glUseProgram(0);
}

ComposedShader Shader::parseShader(const std::string& filePath) {
    std::ifstream stream(filePath);

    enum class SubType {
        NONE=-1, VERTEX=0, FRAGMENT=1, COMPUTE=2, MESH=3
    };

    std::string line;
    std::stringstream ss[4];
    SubType lineType = SubType::NONE;
    while (getline(stream, line)) {
        // Segrega arquivo por '#! ... shader'
        if (line.find("#!") != std::string::npos) {
            if (line.find("vertex shader") != std::string::npos) {
                lineType = SubType::VERTEX;
            } else if (line.find("fragment shader") != std::string::npos) {
                lineType = SubType::FRAGMENT;
            } else if (line.find("compute shader") != std::string::npos) {
                lineType = SubType::COMPUTE;
            } else if (line.find("mesh shader") != std::string::npos) {
                lineType = SubType::MESH;
            }
        } else {
            // Substitui '#include "caminho/include"' pelo conteudo de caminho/include
            std::regex includeRegex("#include\\s+\"(.*)\"");
            if (std::regex_search(line, includeRegex)) {
                std::smatch sm;
                std::string includePath = "";
                if (std::regex_match(line, sm, includeRegex)) {
                    includePath = "resources/shaders/"+sm[1].str();
                }
                std::filesystem::path dir(filePath);
                std::filesystem::path file(includePath);
                std::filesystem::path fullPath = filePath / file;
                std::cout << "including file: " << includePath << "\n";
                ss[(int)lineType] << readFile(includePath) << "\n";
            }else {
                ss[(int)lineType] << line << '\n';
            }
        }
    }

    return { ss[0].str(), ss[1].str(), ss[2].str(), ss[3].str() };
}

GLuint Shader::createProgram(ComposedShader composedShader) {
    GLuint program = glCreateProgram();

    switch(type) {
        case ShaderType::VERTEX_SHADER: {
            GLuint vs = compileShader(GL_VERTEX_SHADER, composedShader.VertexShader);
            GLuint fs = compileShader(GL_FRAGMENT_SHADER, composedShader.FragmentShader);

            glAttachShader(program, vs);
            glAttachShader(program, fs);
            glLinkProgram(program);

            validateProgram(program);

            glDeleteShader(vs);
            glDeleteShader(fs);

            break;
        }
        case ShaderType::COMPUTE_SHADER: {
            GLuint cs = compileShader(GL_COMPUTE_SHADER, composedShader.ComputeShader);

            glAttachShader(program, cs);
            glLinkProgram(program);

            validateProgram(program);

            glDeleteShader(cs);

            break;
        }
        case ShaderType::MESH_SHADER: {
            //glProgramParameteri(program, GL_PROGRAM_SEPARABLE, GL_TRUE);

            GLuint ms = compileShader(GL_MESH_SHADER_NV, composedShader.MeshShader);
            GLuint fs = compileShader(GL_FRAGMENT_SHADER, composedShader.FragmentShader);

            glAttachShader(program, ms);
            glAttachShader(program, fs);

            glLinkProgram(program);

            validateProgram(program);

            glDetachShader(program, ms);
            glDetachShader(program, fs);
            glDeleteShader(ms);
            glDeleteShader(fs);

            //GLuint pipeline;
            //glGenProgramPipelines(1, &pipeline);
            //glUseProgramStages(pipeline, GL_MESH_SHADER_BIT_NV | GL_FRAGMENT_SHADER_BIT, program);
        }
    }

    return program;
}

GLuint Shader::compileShader(GLenum _type, const std::string& source) {
    GLuint id = glCreateShader(_type);

    const char* shaderType;
    switch (_type)
    {
    case GL_VERTEX_SHADER:
        shaderType = "Vertex Shader";
        break;
    case GL_FRAGMENT_SHADER:
        shaderType = "Fragment Shader";
        break;
    case GL_COMPUTE_SHADER:
        shaderType = "Compute Shader";
        break;
    case GL_MESH_SHADER_NV:
        shaderType = "Mesh Shader";
        break;
    default:
        shaderType = "Invalid Type";
        break;
    }

    if (id == GL_FALSE || !glIsShader(id)) {
        std::cout << "Failed to create shader " << shaderType << std::endl;
        return 0;
    }

    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    GLint result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result != GL_TRUE) {
        GLsizei length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        GLchar* message = (GLchar*)malloc(sizeof(GLchar) * length);
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << shaderType << ": " << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    //TRACE("#Success in '" << shaderType << "' compilation.");

    return id;
}

void Shader::validateProgram(GLint program) {
    GLint status;
    //glValidateProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (GL_FALSE == status)
    {
        std::cout << "Failed to link shader program!" << std::endl;
        GLint logLen;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
        if (logLen > 0)
        {
            GLsizei written;
            std::vector<GLchar> infoLog(logLen);
            glGetProgramInfoLog(program, logLen, &written, &infoLog[0]);
            std::cout << "Program log: " << std::endl << infoLog.data() << std::endl;

            glDeleteProgram(program);
            
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", infoLog.data(), NULL);
        }
        else {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "No linking error message", NULL);
        }
        SDL_Quit();
    }

    //TRACE("#Success in Shader validation.");
}

std::string Shader::readFile(std::string& path) {
    std::ifstream stream(path);

    std::string line;
    std::stringstream ss;
    while (getline(stream, line)) {
        ss << line << '\n';
    }

    return ss.str();
}