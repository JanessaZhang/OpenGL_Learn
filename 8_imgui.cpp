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
		float size_x=400.0;
		float size_y=300.0;
		float init_x=100.0;
		float init_y=100.0;
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
        mshader.SetUniform4f("u_color", 0.2, 0.3, 0.5, 1.0);

        texture mtexture("res/shader/5.jpg");
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

		glm::vec3 translation(50, 50, 0);

        //动态颜色
        float r = 0.1, increment = 0.05;

        // loop
        while (!glfwWindowShouldClose(window)) {
            // 清除buffer 并设置背景颜色
            mrenderer.Clear();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

			glm::mat4 modele =
            glm::translate(glm::mat4(1.0),translation);  //模型矩阵
       		glm::mat4 mvp = proj * view * modele;

            // 进行绘制
            mshader.Bind();
            mshader.SetUniform4f("u_color", r, 0.3, 0.5, 1.0);
            if (r < 0.0)
                increment = 0.05;
            if (r > 1.0)
                increment = -0.05;
            r += increment;

	        mshader.SetUniformMat4("u_MVP", mvp);

            {
                ImGui::SliderFloat3(
                    "Translation",
                    &translation.x,  //不用单独设置y和z，会按照内存布局去找
                    0.0f,
                    960.0f);  // Edit 1 float using a slider from 0.0f to 1.0f

                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                            1000.0f / ImGui::GetIO().Framerate,
                            ImGui::GetIO().Framerate);
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            //有索引缓冲区
            mrenderer.Draw(va, ib, mshader);

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