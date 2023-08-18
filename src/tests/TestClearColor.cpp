#include "TestClearColor.h"
#include "../renderer.h"
#include "../vendor/imgui/imgui.h"

namespace test {

TestClearColor::TestClearColor():m_ClearColor{0.2,0.3,0.8,1.0} {}

TestClearColor::~TestClearColor() {}

void TestClearColor::OnUpdata(float deltatime) {}

void TestClearColor::OnRenderer()  //‰÷»æ
{
	glClearColor(m_ClearColor[0],m_ClearColor[1],m_ClearColor[2],m_ClearColor[3]);
	glClear(GL_COLOR_BUFFER_BIT);
}

void TestClearColor::OnImGuiRenderer()  // GUI
{
	ImGui::ColorEdit4("Clear Color",m_ClearColor);
}

}
