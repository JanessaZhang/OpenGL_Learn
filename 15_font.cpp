#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include "src/indexbuffer.h"
#include "src/renderer.h"
#include "src/shader.h"
#include "src/texture.h"
#include "src/vendor/glm/glm.hpp"
#include "src/vendor/glm/gtc/matrix_transform.hpp"
#include "src/vendor/glm/gtc/type_ptr.hpp"
#include "src/vendor/imgui/imgui.h"
#include "src/vendor/imgui/imgui_impl_glfw.h"
#include "src/vendor/imgui/imgui_impl_opengl3.h"
#include "src/vertexarray.h"
#include "src/vertexbuffer.h"
#include "src/vertexbufferlayout.h"

#include <ft2build.h>
#include FT_FREETYPE_H

void RenderText(renderer &mrenderer,shader &mshader, vertexarray &va, vertexbuffer &vb,std::string text, float x, float y, float scale, glm::vec3 color);

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
// unsigned int VAO, VBO;
// unsigned int VBO;

int main() {
    // glfw init
    if (!glfwInit()) {
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //创建window
    GLFWwindow* window = glfwCreateWindow(960, 540, "first window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    //指定上下文
    glfwMakeContextCurrent(window);
    //设置频率
    glfwSwapInterval(1);

    // glad init
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// compile and setup the shader
		// ----------------------------
		shader ashader("res/shader/font.shader");
		glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(960), 0.0f, static_cast<float>(540));
		ashader.Bind();
		glUniformMatrix4fv(glGetUniformLocation(ashader.m_RendererID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// FreeType
		// --------
		FT_Library ft;
		// All functions return a value different than 0 whenever an error occurred
		if (FT_Init_FreeType(&ft))
		{
			std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
			return -1;
		}

		// find path to font
		std::string font_name = "/home/shasha/Janessa/vscode/OpenGl/Opengl_zss/resources/fonts/Antonio-Bold.ttf";
		if (font_name.empty())
		{
			std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
			return -1;
		}
		
		// load font as face
		FT_Face face;
		if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
			return -1;
		}
		else {
			// set size to load glyphs as
			FT_Set_Pixel_Sizes(face, 0, 48);

			// disable byte-alignment restriction
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			// load first 128 characters of ASCII set
			for (unsigned char c = 0; c < 128; c++)
			{
				// Load character glyph 
				if (FT_Load_Char(face, c, FT_LOAD_RENDER))
				{
					std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
					continue;
				}
				// generate texture




				unsigned int texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RED,
					face->glyph->bitmap.width,
					face->glyph->bitmap.rows,
					0,
					GL_RED,
					GL_UNSIGNED_BYTE,
					face->glyph->bitmap.buffer
				);
				// set texture options
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				// now store character for later use
				Character character = {
					texture,
					glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					static_cast<unsigned int>(face->glyph->advance.x)
				};
				Characters.insert(std::pair<char, Character>(c, character));
			}
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		// destroy FreeType once we're finished
		FT_Done_Face(face);
		FT_Done_FreeType(ft);


		vertexarray va;
        vertexbuffer vb(NULL, sizeof(float) * 6 * 4);
		va.Bind();
		vertexbufferlayout layout;
        layout.Push<float>(4);
		va.AddBuffer(vb, layout);
		va.Unbind();
		vb.unbind();

		renderer mrenderer;

		// render loop
		// -----------
		while (!glfwWindowShouldClose(window))
		{
			// render
			// ------
			mrenderer.Clear();

			RenderText(mrenderer,ashader, va,vb,"This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
			// RenderText(ashader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
			RenderText(mrenderer,ashader, va,vb,"(C) LearnOpenGL.com", 100.0f, 100.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
		
			// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
			// -------------------------------------------------------------------------------
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		glfwTerminate();
		return 0;
}
}

// render line of text
// -------------------
void RenderText(renderer &mrenderer,shader &mshader, vertexarray &va, vertexbuffer &vb,std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state	
    mshader.Bind();
	mshader.SetUniform3f("textColor",color);
    glActiveTexture(GL_TEXTURE0);
	va.Bind();

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory

		vb.bind();
		vb.buffer_data(vertices,sizeof(vertices));
		vb.unbind();

        // render quad
        mrenderer.Draw(0,6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }

	va.Unbind();
    glBindTexture(GL_TEXTURE_2D, 0);
}
