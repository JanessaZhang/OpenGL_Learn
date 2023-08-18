#ifndef TEST_TEXTURE2D_HEADER_H
#define TEST_TEXTURE2D_HEADER_H

#include "Test.h"
#include "../vendor/glm/glm.hpp"
#include "../vertexbuffer.h"
#include "../vertexarray.h"
#include "../vertexbufferlayout.h"
#include "../texture.h"
#include <memory>

namespace test {
class TestTexture2D : public Test {
public:
    TestTexture2D();
    ~TestTexture2D();

    void OnUpdata(float deltatime) override;
    void OnRenderer() override;  //‰÷»æ
    void OnImGuiRenderer() override;  // GUI

private:
	std::unique_ptr<vertexarray> m_VAO;
	std::unique_ptr<vertexbuffer> m_VertexBuffer;
	std::unique_ptr<indexbuffer> m_IndexBuffer;
	std::unique_ptr<shader> m_shader;
	std::unique_ptr<Texture> m_Texture;
    
	glm::mat4 m_Proj,m_View;
	glm::vec3 m_TranslationA,m_TranslationB;

};

}

#endif