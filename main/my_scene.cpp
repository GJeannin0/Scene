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

#include <random>


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
		Model rockModel_;
		Shader shader_;
		Shader skyShader_;
		Shader instanceShader_;

		unsigned int amount = 21;

		float offset = 2.5f;
		float time_ = 0.0f;

		unsigned int VBO_;
		unsigned int VAO_[2];
		unsigned int EBO_;
		unsigned int vertex_shader_;
		unsigned int fragment_shader_;
		unsigned int program_;

		unsigned int cubemapTexture_;
		unsigned int skyboxVAO_;

		unsigned int instanceVBO_ = 0;
		const size_t offsetInstances = 4;

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
		//Init skybox

		float skyboxVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};

		unsigned int skyboxVBO;
		glGenVertexArrays(1, &skyboxVAO_);
		glGenBuffers(1, &skyboxVBO);
		glBindVertexArray(skyboxVAO_);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		std::string path = "..\\";

		std::vector<std::string> faces
		{
			path + "data/textures/skybox/right.jpg",
			path + "data/textures/skybox/left.jpg",
			path + "data/textures/skybox/top.jpg",
			path + "data/textures/skybox/bottom.jpg",
			path + "data/textures/skybox/front.jpg",
			path + "data/textures/skybox/back.jpg"
		};
		Texture skybox;
		cubemapTexture_ = skybox.LoadCubemap(faces);

		glEnable(GL_DEPTH_TEST);
		camera_ = std::make_unique<Camera>(glm::vec3(amount * 5, 10.0f, 10.0f));
		model.LoadModel("data/meshes/nanosuit.obj");

		std::string ifs_verts(
			path + "data/shaders/model/model.vert");
		std::string ifs_frags(
			path + "data/shaders/model/model.frag");

		shader_ = Shader(ifs_verts, ifs_frags);
		shader_.Use();

		ifs_verts = (
			path + "data/shaders/cubemap/cubemap.vert");
		ifs_frags = (
			path + "data/shaders/cubemap/cubemap.frag");

		skyShader_ = Shader(ifs_verts, ifs_frags);
		skyShader_.Use();

		ifs_verts = (
			path + "data/shaders/instancing/instancing.vert");
		ifs_frags = (
			path + "data/shaders/instancing/instancing.frag");

		instanceShader_ = Shader(ifs_verts, ifs_frags);
		instanceShader_.Use();
	}

	void MyScene::Update(seconds dt)
	{
		std::vector<glm::mat4> modelMatrices_;
		for (unsigned int i = 0; i < amount; i++)
		{
			for (unsigned int j = 0; j < amount; j++)
			{
				glm::mat4 model = glm::mat4(1.0f);

				// 1. translation: displace on axis
				model = glm::translate(model, glm::vec3(i * 10, 0.0f, j * (-20.0f)));
				//model = glm::rotate(model, time_, glm::vec3(0.f, 1.f, 0.f));
				// 4. now add to list of matrices
				modelMatrices_.push_back(model);
			}
		}

		time_ += dt.count();
		glClearColor(0.5f, 0.4f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float moveSpeed = 5.0f;
		camera_ = std::make_unique<Camera>(glm::vec3(amount * 5, 10.0f, 20.0f - time_ * moveSpeed));
		view_ = camera_->GetViewMatrix();

		projection_ = glm::perspective(
			glm::radians(45.0f),
			4.0f / 3.0f,
			0.1f,
			100.f);

		model_ = glm::rotate(glm::mat4(1.0f), time_, glm::vec3(0.f, 1.f, 0.f));
		glm::vec3 offset(0.0, 15.0, 0.0);

		//inv_model_ = glm::transpose(glm::inverse(model_));
		instanceShader_.Use();
		//shader_.SetMat4("model", model_);
		//shader_.SetMat4("view", view_);
		instanceShader_.SetMat4("cameraMatrix", projection_ * view_);
		//shader_.SetMat4("invModel", inv_model_);
		instanceShader_.SetVec3("viewPos", camera_->position);
		//shader_.SetVec3("lightPosition", camera_->position);

		model.DrawModel(instanceShader_, modelMatrices_, offsetInstances);

		// Draw skybox
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyShader_.Use();
		view_ = glm::mat4(glm::mat3(camera_->GetViewMatrix())); // remove translation from the view matrix
		skyShader_.SetMat4("view", view_);
		skyShader_.SetMat4("projection", projection_);
		// skybox cube
		glBindVertexArray(skyboxVAO_);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture_);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
	}

	void MyScene::Destroy() {}

	void MyScene::OnEvent(SDL_Event& event)
	{
		if ((event.type == SDL_KEYDOWN) &&
			(event.key.keysym.sym == SDLK_ESCAPE))
		{
			exit(0);
		}
	}

	void MyScene::DrawImGui() {}

} // End namespace gl.

int main(int argc, char** argv)
{
	gl::MyScene program;
	gl::Engine engine(program);
	engine.Run();
	return EXIT_SUCCESS;
}