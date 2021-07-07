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
        float time_ = 0.0f;
    	
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
        glm::mat4 inv_model_ = glm::mat4(1.0f);


    	
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
        shader_.Use();

    }

    void MyScene::Update(seconds dt)
    {
        time_ += dt.count();
        glClearColor(0.3f, 0.3f, 0.1f, 1.0f);       
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);       

        view_ = camera_->GetViewMatrix();

		projection_ = glm::perspective(
			glm::radians(45.0f), 
			4.0f / 3.0f, 
			0.1f, 
			100.f);
    	
        model_ = glm::rotate(glm::mat4(1.0f), time_, glm::vec3(0.f, 1.f, 0.f));
    	
        shader_.SetMat4("model", model_);
        shader_.SetMat4("view", view_);
        shader_.SetMat4("projection", projection_);
        shader_.SetMat4("invModel", inv_model_);
        shader_.SetVec3("cameraPosition", camera_->position);
    	
        model.DrawModel(shader_);
    }

    void MyScene::Destroy(){}

    void MyScene::OnEvent(SDL_Event& event)
    {
        if ((event.type == SDL_KEYDOWN) &&
            (event.key.keysym.sym == SDLK_ESCAPE))
        {
            exit(0);
        }
    }

    void MyScene::DrawImGui(){}

} // End namespace gl.

int main(int argc, char** argv)
{
    gl::MyScene program;
    gl::Engine engine(program);
    engine.Run();
    return EXIT_SUCCESS;
}
