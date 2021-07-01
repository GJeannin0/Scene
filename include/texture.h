#pragma once

#include <string>
#include <fstream>
#include <glad/glad.h>



namespace gl {

	class Texture {
	public:
		std::string path_;
		unsigned int type_; //0null ,1diffuse ,2specular ,3normal
		unsigned int id;
		Texture() {}
		Texture(const std::string& file_name, unsigned int type);
		void Bind(unsigned int i = 0);
		void UnBind();
	protected:
		void IsError(const char* file, int line);
	};

} // End namespace gl.
