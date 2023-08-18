#include "TestBatchRender.h"
#include "../renderer.h"
#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtc/matrix_transform.hpp"
#include "../vendor/imgui/imgui.h"

namespace test
{
	TestBatchRender::TestBatchRender()
        :m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
        m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
        m_Translation(glm::vec3(0, 0, 0))
	{
		float size_x=100.0;
		float size_y=100.0;
		float init_x=-50.0;
		float init_y=-50.0;
		float init_z=0.0;

		float x_offset=100;
		float y_offset=100;
		float positions[] = {
								init_x,       init_y,       init_z,0.15,0.6,0.96,1.0,0.0,0.0,0.0,
								init_x+size_x,init_y,       init_z,0.15,0.6,0.96,1.0,1.0,0.0,0.0,
								init_x+size_x,init_y+size_y,init_z,0.15,0.6,0.96,1.0,1.0,1.0,0.0,
								init_x,       init_y+size_y,init_z,0.15,0.6,0.96,1.0,0.0,1.0,0.0,

								init_x+x_offset,       init_y+y_offset,       init_z,1.0,0.93,0.24,1.0,0.0,0.0,1.0,
								init_x+size_x+x_offset,init_y+y_offset,       init_z,1.0,0.93,0.24,1.0,1.0,0.0,1.0,
								init_x+size_x+x_offset,init_y+size_y+y_offset,init_z,1.0,0.93,0.24,1.0,1.0,1.0,1.0,
								init_x+x_offset,       init_y+size_y+y_offset,init_z,1.0,0.93,0.24,1.0,0.0,1.0,1.0
							};

        unsigned int indices[] = {
									0, 1, 2, 2, 3, 0,
									4, 5, 6, 6, 7, 4
								 };

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m_VAO = std::make_unique<vertexarray>();

        m_VertexBuffer = std::make_unique<vertexbuffer>(positions, 8*10 * sizeof(float));
        vertexbufferlayout layout;
        layout.Push<float>(3); // 坐标属性 x, y, z, 
		layout.Push<float>(4); // 颜色属性 r, g, b, a
		layout.Push<float>(2); 
		layout.Push<float>(1);
        m_VAO->AddBuffer(*m_VertexBuffer, layout);

        m_IndexBuffer = std::make_unique<indexbuffer>(indices, 12);

        m_shader = std::make_unique<shader>("res/shader/Batch.shader");
		m_shader->Bind();

		m_Texture[0]=std::make_unique<Texture>("res/shader/4.jpg");
		m_Texture[1]=std::make_unique<Texture>("res/shader/5.jpg");

        int samplers[2];
        for (size_t i = 0; i < 2; i++)
        {
			samplers[i]=i;
			std::cout<<samplers[i]<<std::endl;
            m_Texture[i]->Bind(i);
        }
		
        m_shader->SetUniform1iv("u_Textures", 2,samplers);

	}

	TestBatchRender::~TestBatchRender() {}

	void TestBatchRender::OnUpdata(float deltatime) {}

	void TestBatchRender::OnRenderer()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

        renderer mrenderer; // 每帧这个renderer都要不一样嘛

		{
			glm::mat4 modele =
				glm::translate(glm::mat4(1.0), m_Translation);  //????
			glm::mat4 mvp = m_Proj * m_View * modele;
			m_shader->Bind();
			m_shader->SetUniformMat4("u_MVP", mvp);
			mrenderer.Draw(*m_VAO, *m_IndexBuffer, *m_shader);
        }
	}

	void TestBatchRender::OnImGuiRenderer()
	{
		ImGui::SliderFloat3(
						"Translation",
						&m_Translation.x,
						0.0f,
						960.0f);  // Edit 1 float using a slider from 0.0f to 1.0f

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}
