#ifndef TEST_TEXTBATCHRENDER_HEADER_H
#define TEST_TEXTBATCHRENDER_HEADER_H

#include "Test.h"
#include "../vendor/glm/glm.hpp"
#include "../vertexbuffer.h"
#include "../vertexarray.h"
#include "../vertexbufferlayout.h"
#include "../texture.h"
#include <memory>

namespace test
{
	class TestBatchRender : public Test
	{
	private:
		std::unique_ptr<vertexarray> m_VAO;
		std::unique_ptr<vertexbuffer> m_VertexBuffer;
		std::unique_ptr<indexbuffer> m_IndexBuffer;
		std::unique_ptr<shader> m_shader;
		std::unique_ptr<Texture> m_Texture[2];

		glm::mat4 m_Proj, m_View;
		glm::vec3 m_Translation;

	public:
		TestBatchRender();
		~TestBatchRender();

   		void OnUpdata(float deltatime) override;
    	void OnRenderer() override;  //‰÷»æ
    	void OnImGuiRenderer() override;  // GUI
	};
}

#endif