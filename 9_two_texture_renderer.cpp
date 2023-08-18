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
#include "src/vendor/imgui/imgui.h"
#include "src/vendor/imgui/imgui_impl_glfw.h"
#include "src/vendor/imgui/imgui_impl_opengl3.h"
#include "src/vertexarray.h"
#include "src/vertexbuffer.h"
#include "src/vertexbufferlayout.h"
#include "vector"

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
        // 数据 索引
		float size_x=100.0;
		float size_y=100.0;
		float init_x=-50.0;
		float init_y=-50.0;
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

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        vertexarray va;
        vertexbuffer vb(positions, sizeof(float) * 4 * 4);
        vertexbufferlayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);

        va.AddBuffer(vb, layout);
        indexbuffer ib(indices, 6);

        glm::mat4 proj =
            glm::ortho(0.0, 960.0, 0.0, 540.0, -1.0, 1.0);  //投影矩阵
        glm::mat4 view =
            glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0));  //视图矩阵
        
        shader mshader("res/shader/basic.shader");
        mshader.Bind();

        texture mtexture("res/shader/1.png");
        mtexture.Bind();
        mshader.SetUniform1i("u_Texture", 0);

        va.Unbind();
        mshader.Unbind();
        vb.unbind();
        ib.unbind();

        renderer mrenderer;

        // gui上下文
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui::StyleColorsDark();
        // 需要指定GLSL版本, 也就是shader中的version
        const char* glsl_version = "#version 330";
        ImGui_ImplOpenGL3_Init(glsl_version);

		 glm::vec3 translationA(200, 200, 0);
		 glm::vec3 translationB(400, 200, 0);
		//  std::vector<glm::vec3> transArray{translationA,translationB,glm::vec3(500,500,0)};
		 std::vector<glm::vec3> transArray{translationA,translationB};

        //动态颜色
        float r = 0.1, increment = 0.05;

        // loop
        while (!glfwWindowShouldClose(window)) {
            // 清除buffer 并设置背景颜色
            mrenderer.Clear();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // 进行绘制
            mshader.Bind();

			for(int i=0;i<transArray.size();i++)
			{
				glm::mat4 modele =
            	glm::translate(glm::mat4(1.0),transArray[i]);  //模型矩阵
       			glm::mat4 mvp = proj * view * modele;
	        	mshader.SetUniformMat4("u_MVP", mvp);
            	mrenderer.Draw(va, ib, mshader);

				{
					ImGui::SliderFloat3(
						"Translation:"+(char)('0' + i),
						&transArray[i].x,
						0.0f,
						960.0f);  // Edit 1 float using a slider from 0.0f to 1.0f

            	}
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                            -500.0f / ImGui::GetIO().Framerate,
                            ImGui::GetIO().Framerate);
			}


			// {
			// 	glm::mat4 modele =
            // 	glm::translate(glm::mat4(1.0),translationA);  //模型矩阵
       		// 	glm::mat4 mvp = proj * view * modele;
	        // 	mshader.SetUniformMat4f("u_MVP", mvp);
            // 	mrenderer.Draw(va, ib, mshader);
			// }

			// {
			// 	glm::mat4 modele =
            // 	glm::translate(glm::mat4(1.0),translationB);  //模型矩阵
       		// 	glm::mat4 mvp = proj * view * modele;
	        // 	mshader.SetUniformMat4f("u_MVP", mvp);
            // 	mrenderer.Draw(va, ib, mshader);
			// }

            // {
            //     ImGui::SliderFloat3(
            //         "Translation A",
            //         &translationA.x,
            //         0.0f,
            //         960.0f);  // Edit 1 float using a slider from 0.0f to 1.0f

			// 	ImGui::SliderFloat3(
            //         "Translation B",
            //         &translationB.x,
            //         0.0f,
            //         960.0f);  // Edit 1 float using a slider from 0.0f to 1.0f

            //     ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
            //                 -500.0f / ImGui::GetIO().Framerate,
            //                 ImGui::GetIO().Framerate);
            // }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // 交换buffer，进行显示
            glfwSwapBuffers(window);
            // 分发事件
            glfwPollEvents();
        }
    }
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    //终止窗口
    glfwTerminate();
    return 0;
}