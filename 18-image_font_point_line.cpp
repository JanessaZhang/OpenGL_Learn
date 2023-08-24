#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "src/shader.h"

#include <map>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <mutex>

#include <ft2build.h>
#include FT_FREETYPE_H

// struct shader_program_source
// {
// 	std::string vertex_source;
// 	std::string fragment_source;
// };


static shader_program_source parse_shader(const std::string &file_path)
{
	enum class ShaderType
	{
		NONE=-1,VERTEX=0,FRAGMENT=1
	};

	std::ifstream stream(file_path);
	std::string line;
	std::stringstream ss[2];
	ShaderType shader_type=ShaderType::NONE;

	while (getline(stream,line))
	{
		if(line.find("#shader")!=std::string::npos)
		{
			if(line.find("vertex")!=std::string::npos)
			// set vertex mode
			shader_type=ShaderType::VERTEX;
			else if(line.find("fragment")!=std::string::npos)
			// set fragment mode
			shader_type=ShaderType::FRAGMENT;

		}
		else{
			ss[(int)shader_type]<<line<<"\n";
		}
	}
	
	return {ss[0].str(),ss[1].str()};
}

// 编译shader
static unsigned int compile_shader(unsigned int type,
                                   const std::string& source) {
    // 根据类型创建shader
    unsigned int id = glCreateShader(type);
    // 获得shader源码
    const char* src = source.c_str();
    // 绑定源码到shader上
    glShaderSource(id, 1, &src, nullptr);
    // 编译shader
    glCompileShader(id);

    // TODO：error handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile "
                  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "!"
                  << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

// 创建shader
// shader的源码传进去，OPenGl编译源码，然后链接
static unsigned int create_shader(const std::string& vertex_shader,
                                  const std::string& fragment_shader) {
    // 创建缓冲区program
    unsigned int program = glCreateProgram();
    // 编译 获得两个编译好的shader
    unsigned int vs = compile_shader(GL_VERTEX_SHADER, vertex_shader);
    unsigned int fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);
    // 链接 把shader 绑定到缓冲区program  然后链接program
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void RenderText(shader &ashader, std::string text, float x, float y, float scale, glm::vec3 color);

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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    {
		//**************************************************************font*****************************************************************
		shader ashader("res/shader/font.shader");
		ashader.Bind();

		FT_Library ft;
		if (FT_Init_FreeType(&ft))
		{
			std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
			return -1;
		}

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
			FT_Set_Pixel_Sizes(face, 0, 28);

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


		

        //************************************************************************image**************************************************************
		float image_positions[] = {-1.0,-1.0,0.0,0.0,
		                    	    1.0,-1.0,1.0,0.0,
								    1.0, 1.0,1.0,1.0,
							       -1.0,1.0,0.0,1.0};

		unsigned int image_indices[] = {0, 1, 2, 2, 3, 0};

		unsigned int image_vao;
		glGenVertexArrays(1, &image_vao);
		glBindVertexArray(image_vao);

		unsigned int image_vbo;
		glGenBuffers(1, &image_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, image_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4*4, image_positions, GL_STATIC_DRAW);

		// 顶点属性
		// 启用顶点属性 指定顶点的布局
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void *)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void *)(2 * sizeof(float)));

		//创建索引缓冲区
		unsigned int image_ebo;
		glGenBuffers(1, &image_ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, image_ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * 6, image_indices, GL_STATIC_DRAW);

		shader_program_source image_source=parse_shader("res/shader/image.shader");
		unsigned int image_shader = create_shader(image_source.vertex_source, image_source.fragment_source);
		glUseProgram(image_shader);

		cv::Mat image=cv::imread("res/shader/5.jpg");
		cv::flip(image,image,0);
		cv::cvtColor(image,image,cv::COLOR_BGR2RGB);
		unsigned int image_texture;
		glGenTextures(1, &image_texture);
		glBindTexture(GL_TEXTURE_2D,image_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB8,
                 image.cols,
                 image.rows,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 image.ptr());
		glActiveTexture(0);

		int image_texture_location=glGetUniformLocation(image_shader,"u_Texture");
	    glUniform1i(image_texture_location,0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
		glUseProgram(0);
		glBindTexture(GL_TEXTURE_2D, 0);

        //************************************************************************line point**************************************************************
		float point_positions[] = {-0.5,-0.5,1.0,0.0,0.0,1.0,
		                            0.5,-0.5,0.0,1.0,0.0,1.0,
							 	    0.5, 0.5,1.0,0.0,0.0,1.0,
							  	   -0.5, 0.5,0.0,0.0,1.0,1.0};

		unsigned int point_vao;
		glGenVertexArrays(1, &point_vao);
		glBindVertexArray(point_vao);

		unsigned int point_vbo;
		glGenBuffers(1, &point_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, point_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6*4, point_positions, GL_STATIC_DRAW);

		// 顶点属性
		// 启用顶点属性 指定顶点的布局
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *)(2 * sizeof(float)));

		shader_program_source point_source=parse_shader("res/shader/point.shader");
		unsigned int point_shader = create_shader(point_source.vertex_source, point_source.fragment_source);
		glUseProgram(point_shader);
		glPointSize(20.0);
		glLineWidth(10.0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
		glUseProgram(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		
        //************************************************************************opencv**************************************************************

		// //保存图片
		// std::mutex mtx; 
		// char image_path[200];
		// int num=0;

		// loop
		float r=0.1,increment=0.05;
		while (!glfwWindowShouldClose(window)) {
			glClear(GL_COLOR_BUFFER_BIT);
        	glClearColor(0.25, 0.75, 0.25, 1.0);
			glUseProgram(image_shader);
			glBindVertexArray(image_vao);
			glBindBuffer(GL_ARRAY_BUFFER, image_vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,image_ebo);
			glBindTexture(GL_TEXTURE_2D,image_texture);
			glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,nullptr);

			glUseProgram(point_shader);
			glBindVertexArray(point_vao);
			glBindBuffer(GL_ARRAY_BUFFER, point_vbo);
			glDrawArrays(GL_POINTS, 0, 4);
			glDrawArrays(GL_LINE_LOOP, 0, 4);

			RenderText(ashader, "resolution: 1280*720", 10.f, 700.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
			// RenderText(ashader, "task_name : opengl_display_image", 10.0f, 680.0f, 1.0f, glm::vec3(0.3, 0.7f, 0.9f));

			SaveImage();

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

		std::cout<<"ch.Bearing.x: "<<ch.Bearing.x <<"  ch.Bearing.y :"<<ch.Bearing.y <<" ch.Size.x:"<<ch.Size.x<<"  ch.Size.y:"<<ch.Size.y<<std::endl;

		float xpos_start=(xpos-640)/640;
		float ypos_start=(ypos-360)/360;
		float xpos_end=(xpos + w-640)/640;
		float ypos_end=(ypos + h-360)/360;

		// update VBO for each character
		float vertices[6][4] = {
			{ xpos_start,     ypos_end,   0.0f, 0.0f },	    
			{ xpos_start,     ypos_start,       0.0f, 1.0f },
			{ xpos_end, ypos_start,       1.0f, 1.0f },

			{ xpos_start,     ypos_end,   0.0f, 0.0f },
			{ xpos_end, ypos_start,       1.0f, 1.0f },
			{ xpos_end, ypos_end,   1.0f, 0.0f }	   
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
