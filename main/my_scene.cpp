#include <SDL_main.h>
#include <glad/glad.h>
#include <array>
#include <string>
#include <iostream>
#include <fstream>

#include "engine.h"
#include "shader.h"
#include "../build/model.h"
#include "camera.h"


namespace gl {

    class MyScene : public Program
    {
    public:        
        void Init() override;
        void Update(seconds dt) override;
        void Destroy() override;
        void OnEvent(SDL_Event& event) override;
        void DrawImGui() override;

    protected:    	
        Model model;
        Shader shader_;
    	
        unsigned int VBO_;
        unsigned int VAO_[2];
        unsigned int EBO_;
        unsigned int vertex_shader_;
        unsigned int fragment_shader_;
        unsigned int program_;

        std::unique_ptr<Camera> camera_ = nullptr;
    	
        glm::mat4 model_ = glm::mat4(1.0f);
        glm::mat4 view_ = glm::mat4(1.0f);
        glm::mat4 projection_ = glm::mat4(1.0f);



    	
        void IsError(const std::string& file, int line);
    };

    void MyScene::IsError(const std::string& file, int line)
    {
        auto error_code = glGetError();
        if (error_code != GL_NO_ERROR)
        {
            std::cerr
                << error_code
                << " in file: " << file
                << " at line: " << line
                << "\n";
        }
    }

    void MyScene::Init()
    {
        glEnable(GL_DEPTH_TEST);
        camera_ = std::make_unique<Camera>(glm::vec3(.0f, 8.0f, 20.0f));
        model.LoadModel("data/meshes/nanosuit.obj");

        std::string path = "..\\";

        std::string ifs_verts(
            path + "data/shaders/model/model.vert");
        std::string ifs_frags(
            path + "data/shaders/model/model.frag");

        shader_ = Shader(ifs_verts, ifs_frags);



    	
        std::ifstream ifs_vert(
            path + "data\\shaders\\model\\model.vert");
        std::ifstream ifs_frag(
            path + "data\\shaders\\model\\model.frag");
    	
        if (!ifs_vert.is_open())
        {
            throw std::runtime_error("Could not open vertex file.");
        }
        if (!ifs_frag.is_open())
        {
            throw std::runtime_error("Could not open fragment file.");
        }

        std::string vertex_source{ std::istreambuf_iterator<char>(ifs_vert), {} };
        std::string fragment_source{ std::istreambuf_iterator<char>(ifs_frag), {} };

        // Vertex shader.
        vertex_shader_ = glCreateShader(GL_VERTEX_SHADER);       
        const char* ptr1 = vertex_source.c_str();
        glShaderSource(vertex_shader_, 1, &ptr1, 0);
        glCompileShader(vertex_shader_);
        GLint status = 0;
        glGetShaderiv(vertex_shader_, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            GLint infoLogLength;
            glGetShaderiv(vertex_shader_, GL_INFO_LOG_LENGTH, &infoLogLength);
            GLchar* infoLog = new GLchar[infoLogLength];
            glGetShaderInfoLog(vertex_shader_, infoLogLength, NULL, infoLog);
            std::cerr << "VS> could not compile: " << infoLog << "\n";
            delete[] infoLog;
            exit(0);
        }

        // Fragment shader.
        fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
        
        const char* ptr2 = fragment_source.c_str();
        glShaderSource(fragment_shader_, 1, &ptr2, 0);
        
        glCompileShader(fragment_shader_);
        glGetShaderiv(vertex_shader_, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            GLint infoLogLength;
            glGetShaderiv(vertex_shader_, GL_INFO_LOG_LENGTH, &infoLogLength);
            GLchar* infoLog = new GLchar[infoLogLength];
            glGetShaderInfoLog(vertex_shader_, infoLogLength, NULL, infoLog);
            std::cerr << "FS> could not compile: " << infoLog << "\n";
            delete[] infoLog;
            exit(0);
        }

        // Program.
        program_ = glCreateProgram();
        
        glAttachShader(program_, vertex_shader_);
        
        glAttachShader(program_, fragment_shader_);
        
        
        glLinkProgram(program_);
        
        assert(program_ != 0);
    }

    void MyScene::Update(seconds dt)
    {
        glClearColor(0.8f, 0.6f, 0.1f, 1.0f);       
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);       
        glUseProgram(program_);

        view_ = camera_->GetViewMatrix();

		projection_ = glm::perspective(
			glm::radians(45.0f), 
			4.0f / 3.0f, 
			0.1f, 
			100.f); 	
        shader_.SetMat4("model", model_);
        shader_.SetMat4("view", view_);
        shader_.SetMat4("projection", projection_);

    	
        model.DrawModel(shader_);
    }

    void MyScene::Destroy()
    {
        glDeleteProgram(program_);
        glDeleteShader(vertex_shader_);
        glDeleteShader(fragment_shader_);
    }

    void MyScene::OnEvent(SDL_Event& event)
    {
        if ((event.type == SDL_KEYDOWN) &&
            (event.key.keysym.sym == SDLK_ESCAPE))
        {
            exit(0);
        }
    }

    void MyScene::DrawImGui()
    {
    }

} // End namespace gl.

int main(int argc, char** argv)
{
    gl::MyScene program;
    gl::Engine engine(program);
    engine.Run();
    return EXIT_SUCCESS;
}
