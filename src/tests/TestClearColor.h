#ifndef TEST_CLEAR_COLOR_HEADER_H
#define TEST_CLEAR_COLOR_HEADER_H

#include "Test.h"

namespace test {
class TestClearColor : public Test {
public:
    TestClearColor();
    ~TestClearColor();

    void OnUpdata(float deltatime) override;
    void OnRenderer() override;  //‰÷»æ
    void OnImGuiRenderer() override;  // GUI

private:
	float m_ClearColor[4];
};

}

#endif