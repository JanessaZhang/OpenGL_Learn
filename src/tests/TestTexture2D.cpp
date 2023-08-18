#include "TestTexture2D.h"
#include "../renderer.h"
#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtc/matrix_transform.hpp"
#include "../vendor/imgui/imgui.h"

namespace test {

TestTexture2D::TestTexture2D()
:m_TranslationA(200,200,0),m_TranslationB(400,200,0),
m_Proj(glm::ortho(0.0, 960.0, 0.0, 540.0, -1.0, 1.0)),
m_View(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))
 {
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

	m_VAO=std::make_unique<vertexarray>();

	m_VertexBuffer=std::make_unique<vertexbuffer>(positions, sizeof(float) * 4 * 4);
    vertexbufferlayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);
    m_VAO->AddBuffer(*m_VertexBuffer, layout);

	m_IndexBuffer=std::make_unique<indexbuffer>(indices, 6); 

    m_shader=std::make_unique<shader>("res/shader/basic.shader");

    m_shader->Bind();
	glm::vec4 vec4_v(0.2, 0.3, 0.5, 1.0);
    m_shader->SetUniform4f("u_color", vec4_v);

	m_Texture=std::make_unique<Texture>("res/shader/4.jpg");
    m_shader->SetUniform1i("u_Texture", 0);
}

TestTexture2D::~TestTexture2D() {}

void TestTexture2D::OnUpdata(float deltatime) {}

void TestTexture2D::OnRenderer()  //äÖÈ¾
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

	renderer mrenderer;
	m_Texture->Bind();
    {
        glm::mat4 modele =
            glm::translate(glm::mat4(1.0), m_TranslationA);  //????
        glm::mat4 mvp = m_Proj * m_View * modele;
		m_shader->Bind();
        m_shader->SetUniformMat4("u_MVP", mvp);
        mrenderer.Draw(*m_VAO, *m_IndexBuffer, *m_shader);
    }

    {
        glm::mat4 modele =
            glm::translate(glm::mat4(1.0), m_TranslationB);  //????
        glm::mat4 mvp = m_Proj * m_View * modele;
        m_shader->SetUniformMat4("u_MVP", mvp);
        mrenderer.Draw(*m_VAO, *m_IndexBuffer, *m_shader);
    }
}

void TestTexture2D::OnImGuiRenderer()  // GUI
{
    ImGui::SliderFloat3(
        "Translation A",
        &m_TranslationA.x,
        0.0f,
        960.0f);  // Edit 1 float using a slider from 0.0f to 1.0f

    ImGui::SliderFloat3(
        "Translation B",
        &m_TranslationB.x,
        0.0f,
        960.0f);  // Edit 1 float using a slider from 0.0f to 1.0f

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                -500.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
}

}
