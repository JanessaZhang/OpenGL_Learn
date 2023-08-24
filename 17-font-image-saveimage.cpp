#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "src/indexbuffer.h"
#include "src/renderer.h"
#include "src/shader.h"
#include "src/texture.h"
#include "src/vendor/glm/glm.hpp"
#include "src/vendor/glm/gtc/matrix_transform.hpp"
#include "src/vertexarray.h"
#include "src/vertexbuffer.h"
#include "src/vertexbufferlayout.h"
#include <map>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <mutex>

#include <ft2build.h>
#include FT_FREETYPE_H

void RenderText(shader &ashader, std::string text, float x, float y, float scale, glm::vec3 color);
void RenderImage(shader &mshader,renderer &mrenderer,vertexarray &va,indexbuffer &ib,Texture &mtexture);
void SaveImage();

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
unsigned int VAO, VBO;

int main() {
    // glfw init
    if (!glfwInit()) {
	return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //创建window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "opengl window", NULL, NULL);
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
		//**************************************************************font*****************************************************************
		// compile and setup the shader
		// ----------------------------
		shader ashader("res/shader/font.shader");
		glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(1280), 0.0f, static_cast<float>(720));
		ashader.Bind();
		glUniformMatrix4fv(glGetUniformLocation(ashader.m_RendererID, "projection"), 1, GL_FALSE, &projection[0][0]);

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
		std::string font_name = "res/fonts/OCRAEXT.TTF";
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

		// configure VAO/VBO for texture quads
		// -----------------------------------
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);


		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //************************************************************************image**************************************************************
		float size_x=640.0;
		float size_y=480.0;
		float init_x=0.0;
		float init_y=0.0;
		float positions[] = {init_x,
			init_y,
			0.0,
			0.0,
			init_x+size_x,
			init_y,
			1.0,
			0.0,
			init_x+size_x,
			init_y+size_y,
			1.0,
			1.0,
			init_x,
			init_y+size_y,
			0.0,
			1.0};

		unsigned int indices[] = {0, 1, 2, 2, 3, 0};
		vertexarray va;
		vertexbuffer vb(positions, sizeof(float) * 4 * 4);
		vertexbufferlayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);

		va.AddBuffer(vb, layout);
		indexbuffer ib(indices, 6);

		glm::mat4 proj =
		glm::ortho(0.0, 1280.0, 0.0, 720.0, -1.0, 1.0);  //投影矩阵
		glm::mat4 view =
		glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0));  //视图矩阵

		shader mshader("res/shader/basic.shader");
		mshader.Bind();

		Texture mtexture("res/shader/5.jpg");
		mtexture.Bind();
		mshader.SetUniform1i("u_Texture", 0);

		renderer mrenderer;
			
		glm::vec3 translation(0, 0, 0);

		glm::mat4 modele =
		glm::translate(glm::mat4(1.0),translation);  //模型矩阵
		glm::mat4 mvp = proj * view * modele;

		mshader.Bind();
		mshader.SetUniformMat4("u_MVP", mvp);

        //************************************************************************point**************************************************************
		// float point_size_x=100.0;
		// float point_size_y=100.0;
		// float point_init_x=800.0;
		// float point_init_y=800.0;
		// float point_positions[] = {point_init_x,
		// 					point_init_y,
		// 					0.0,
		// 					0.0,
		// 					point_init_x+point_size_x,
		// 					point_init_y,
		// 					1.0,
		// 					0.0,
		// 					point_init_x+point_size_x,
		// 					point_init_y+point_size_y,
		// 					1.0,
		// 					1.0,
		// 					point_init_x,
		// 					point_init_y+point_size_y,
		// 					0.0,
		// 					1.0};

		// unsigned int point_indices[] = {0, 1, 2, 2, 3, 0};


		// vertexarray point_va;
		// vertexbuffer point_vb(point_positions, sizeof(float) * 4 * 4);
		// vertexbufferlayout point_layout;
		// point_layout.Push<float>(2);
		// point_layout.Push<float>(2);
		// point_va.AddBuffer(point_vb, point_layout);
		// indexbuffer point_ib(indices, 6);
		// shader point_mshader("res/shader/point.shader");
		// point_mshader.Bind();

		// glm::vec4 vec4_v(1.0, 0.3, 0.5, 1.0);
    	// point_mshader.SetUniform4f("u_color", vec4_v);
		// renderer point_mrenderer;
		




        //************************************************************************opencv**************************************************************

		// //保存图片
		// std::mutex mtx; 
		// char image_path[200];
		// int num=0;

		// loop
		while (!glfwWindowShouldClose(window)) {
				mrenderer.Clear();
				RenderImage(mshader,mrenderer,va,ib,mtexture);
				RenderText(ashader, "resolution: 1280*720", 10.f, 700.0f, 0.5f, glm::vec3(0.5, 0.8f, 0.2f));
				RenderText(ashader, "task_name : opengl_display_image", 10.0f, 680.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

				// mrenderer.Draw(point_va, point_ib, point_mshader,"points");
				// SaveImage();

				// cv::Mat img(720, 1280, CV_8UC3);
				// glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);
				// // glPixelStorei(GL_PACK_ROW_LENGTH, img.step/img.elemSize()); // 这句不加好像也没问题？
				// glReadPixels(0, 0, img.cols, img.rows, GL_BGR, GL_UNSIGNED_BYTE, img.data);
				// cv::Mat flipped;
				// cv::flip(img, flipped, 0);
				// {
				// 	mtx.lock();
				// 	sprintf(image_path, "/home/shasha/Janessa/vscode/OpenGl/Opengl_zss/out_opengl/%d.jpg", ++num);
				// 	imwrite(image_path, flipped);
				// 	// cv::imshow("opencv window", img);
				// 	mtx.unlock();
				// }

				// 交换buffer，进行显示
				glfwSwapBuffers(window);
				// 分发事件
				glfwPollEvents();
		}
    }	  
    //终止窗口
    glfwTerminate();
    return 0;
}

// render line of text
// -------------------
void RenderText(shader &ashader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state	
    ashader.Bind();
    glUniform3f(glGetUniformLocation(ashader.m_RendererID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

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
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// render line of image
// -------------------

void RenderImage(shader &mshader,renderer &mrenderer,vertexarray &va,indexbuffer &ib,Texture &mtexture)
{
	// 进行绘制
    mtexture.Bind();
	mshader.SetUniform1i("u_Texture", 0);
	mrenderer.Draw(va, ib, mshader);
}


//保存图片
std::mutex mtx; 
char image_path[200];
int num=0;
void SaveImage()
{
	cv::Mat img(720, 1280, CV_8UC3);
	glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);
	// glPixelStorei(GL_PACK_ROW_LENGTH, img.step/img.elemSize()); // 这句不加好像也没问题？
	glReadPixels(0, 0, img.cols, img.rows, GL_BGR, GL_UNSIGNED_BYTE, img.data);
	cv::Mat flipped;
	cv::flip(img, flipped, 0);
	{
		mtx.lock();
		sprintf(image_path, "out_opengl/%d.jpg", ++num);
		// imwrite(image_path, flipped);
		std::cout<<flipped.cols<<"          "<<flipped.rows<<std::endl;
		// cv::imshow("opencv window", img);
		mtx.unlock();
	}
}
