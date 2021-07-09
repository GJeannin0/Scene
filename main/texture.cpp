//assimp

#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>

#include "stb_image.h"



namespace gl
{
	Texture::Texture(const std::string& file_name, unsigned int type)
	{
		type_ = type;
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(
			file_name.c_str(),
			&width,
			&height,
			&nrChannels,
			0);
		assert(data);
		glGenTextures(1, &id);
		IsError(__FILE__, __LINE__);
		glBindTexture(GL_TEXTURE_2D, id);
		IsError(__FILE__, __LINE__);
		switch (nrChannels)
		{
		case 3:
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data);
			IsError(__FILE__, __LINE__);
			break;
		case 4:
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGBA,
				width,
				height,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				data);
			IsError(__FILE__, __LINE__);
			break;
		case 1:
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				width,
				height,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				data);
			IsError(__FILE__, __LINE__);
			break;
		default:
			throw std::runtime_error(
				"Unknown number of colors: " + std::to_string(nrChannels));
		}
		stbi_image_free(data);
		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_WRAP_S,
			GL_MIRRORED_REPEAT);
		IsError(__FILE__, __LINE__);
		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_WRAP_T,
			GL_MIRRORED_REPEAT);
		IsError(__FILE__, __LINE__);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		IsError(__FILE__, __LINE__);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		IsError(__FILE__, __LINE__);
		glGenerateMipmap(GL_TEXTURE_2D);
		IsError(__FILE__, __LINE__);
		glBindTexture(GL_TEXTURE_2D, 0);
		IsError(__FILE__, __LINE__);
	}
	void Texture::Bind(unsigned int i)
	{
		glActiveTexture(GL_TEXTURE0 + i);		
		glBindTexture(GL_TEXTURE_2D, id);		
	}

	void Texture::UnBind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	unsigned int Texture::LoadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
	
	void Texture::IsError(const char* file, int line) {
		auto error_code = glGetError();
		if (error_code != GL_NO_ERROR)
		{
			throw std::runtime_error(
				std::to_string(error_code) +
				" in file: " + file +
				" at line: " + std::to_string(line));
		}
	}
}