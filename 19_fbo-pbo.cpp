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

void SaveImage();
void SaveImage2(cv::Mat output);

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

	glViewport(0, 0, 1280, 720);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    {
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

		cv::Mat image=cv::imread("res/shader/4.jpg");
		cv::flip(image,image,0);
		cv::cvtColor(image,image,cv::COLOR_BGR2RGB);

		unsigned int image_texture;
		glGenTextures(1, &image_texture);
		glBindTexture(GL_TEXTURE_2D,image_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glActiveTexture(0);

		int image_texture_location=glGetUniformLocation(image_shader,"u_Texture");
	    glUniform1i(image_texture_location,0);


		// //FBO
		// // create fbo texture
		cv::Mat input=cv::imread("res/shader/5.jpg");
		// cv::flip(input,input,0);
		cv::cvtColor(input,input,cv::COLOR_BGR2RGB);

		unsigned int gl_fbo_id, gl_fbo_texture_id;
		glGenTextures(1, &gl_fbo_texture_id);
		glBindTexture(GL_TEXTURE_2D, gl_fbo_texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D,
								0,
								GL_RGB,
								1280,
								720,
								0,
								GL_RGB,
								GL_UNSIGNED_BYTE,
								NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		// // create fbo
		glGenFramebuffers(1, &gl_fbo_id);
		glBindFramebuffer(GL_FRAMEBUFFER, gl_fbo_id);
		glFramebufferTexture2D(GL_FRAMEBUFFER,
											GL_COLOR_ATTACHMENT0,
											GL_TEXTURE_2D,
											gl_fbo_texture_id,
											0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
			GL_FRAMEBUFFER_COMPLETE) {
			printf("FBO INIT FAIL");
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//PBO
		// using pbo to accelerate opengl pipeline
		int data_index = 0;
		int current_index, next_index;
		unsigned int upload_pbo_ids[2];
		unsigned int download_pbo_ids[2];
		float width_previous_frame = 1280;
		float height_previous_frame = 720;
		float view_width = 1280;
    	float view_height = 720;
		// using 2 upload pbo and 2 download pbo to accelerate opengl pipeline
		// create 2 upload pbo
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenBuffers(2, upload_pbo_ids);

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, upload_pbo_ids[0]);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, upload_pbo_ids[1]);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		// create 2 download pbo
		glGenBuffers(2, download_pbo_ids);

		glBindBuffer(GL_PIXEL_PACK_BUFFER, download_pbo_ids[0]);
		glBufferData(GL_PIXEL_PACK_BUFFER,
								view_width * view_height * 3,
								0,
								GL_STREAM_DRAW);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

		glBindBuffer(GL_PIXEL_PACK_BUFFER, download_pbo_ids[1]);
		glBufferData(GL_PIXEL_PACK_BUFFER,
								view_width * view_height * 3,
								0,
								GL_STREAM_DRAW);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);


		while (!glfwWindowShouldClose(window)) {
			// FBO帧缓冲  离屏渲染
			glBindFramebuffer(GL_FRAMEBUFFER, gl_fbo_id);
			glClearColor(0.25, 0.75, 0.25, 1.0);
			glClear(GL_COLOR_BUFFER_BIT);
			glUseProgram(image_shader);
			glBindVertexArray(image_vao);
			glBindBuffer(GL_ARRAY_BUFFER, image_vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,image_ebo);
			glBindTexture(GL_TEXTURE_2D,image_texture);

			current_index = (data_index) % 2;
   			next_index = ((data_index) + 1) % 2;

			data_index++;


			width_previous_frame = input.cols;
			height_previous_frame = input.rows;
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER,upload_pbo_ids[next_index]);
			glBufferData(
				GL_PIXEL_UNPACK_BUFFER,
				width_previous_frame * height_previous_frame * 3,
				0,
				GL_STREAM_DRAW);
			GLubyte *bufPtr1 = (GLubyte *)glMapBufferRange(
				GL_PIXEL_UNPACK_BUFFER,
				0,
				width_previous_frame * height_previous_frame * 3,
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
			if (bufPtr1) {
				memcpy(bufPtr1,
					input.ptr(),
					static_cast<size_t>(width_previous_frame *
										height_previous_frame * 3));
				glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
			}
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);


			// render using previous texture from pbo
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER,upload_pbo_ids[current_index]);
			glTexImage2D(GL_TEXTURE_2D,
									0,
									GL_RGB,
									width_previous_frame,
									height_previous_frame,
									0,
									GL_RGB,
									GL_UNSIGNED_BYTE,
									0);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
			// render result to pbo
			glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,(const void *)0);
			SaveImage();

			// get render result from pbo (previous previous texture)
			cv::Mat img_process;
			glBindBuffer(GL_PIXEL_PACK_BUFFER,download_pbo_ids[current_index]);
			glReadPixels(0,
						 0,
						 view_width,
						 view_height,
						 GL_BGR,
						 GL_UNSIGNED_BYTE,
						 nullptr);

			glBindBuffer(GL_PIXEL_PACK_BUFFER,download_pbo_ids[next_index]);
			GLubyte *bufPtr = static_cast<GLubyte *>(
				glMapBufferRange(GL_PIXEL_PACK_BUFFER,
											0,
											view_width * view_height * 3,
											GL_MAP_READ_BIT));
			if (bufPtr) {
				img_process =cv::Mat(view_height, view_width, CV_8UC3, bufPtr);
				SaveImage2(img_process);
				glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
			}
			glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);


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
		imwrite(image_path, flipped);
		std::cout<<flipped.cols<<"          "<<flipped.rows<<std::endl;
		// cv::imshow("opencv window", img);
		mtx.unlock();
	}
}

std::mutex mtx2; 
char image_path2[200];
int num2=0;
void SaveImage2(cv::Mat output)
{
	{
		mtx.lock();
		sprintf(image_path2, "out_opengl2/%d.jpg", ++num2);
		imwrite(image_path2, output);
		std::cout<<output.cols<<"          "<<output.rows<<std::endl;
		// cv::imshow("opencv window", img);
		mtx.unlock();
	}
}
