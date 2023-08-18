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
#include "src/tests/TestClearColor.h"
#include "src/tests/TestTexture2D.h"

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

        renderer mrenderer;

        // gui上下文
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui::StyleColorsDark();
        // 需要指定GLSL版本, 也就是shader中的version
        const char* glsl_version = "#version 330";
        ImGui_ImplOpenGL3_Init(glsl_version);

		test::Test* currentTest=nullptr;
		test::TestMenu* testmenu=new test::TestMenu(currentTest);
		currentTest=testmenu;
		testmenu->RegisterTest<test::TestClearColor>("Clear Color");
		testmenu->RegisterTest<test::TestTexture2D>("Texture 2D");

		// test::TestClearColor test;

        while (!glfwWindowShouldClose(window)) {
            mrenderer.Clear();

			ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
			
			// test.OnUpdata(0.0);
			// test.OnRenderer();

			if(currentTest)
			{
				currentTest->OnUpdata(0.0);
				currentTest->OnRenderer();
				ImGui::Begin("Test");

				if(currentTest!=testmenu&&ImGui::Button("<-"))
				{
					delete currentTest;
					currentTest=testmenu;
				}
				currentTest->OnImGuiRenderer();
				ImGui::End();
			}

			// test.OnImGuiRenderer();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // 交换buffer，进行显示
            glfwSwapBuffers(window);
            // 分发事件
            glfwPollEvents();
        }
	delete currentTest;
	if(currentTest!=testmenu)
		delete testmenu;
    }

    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    //终止窗口
    glfwTerminate();
    return 0;
}