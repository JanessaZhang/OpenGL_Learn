#ifndef TEST_HEADER_H
#define TEST_HEADER_H

#include <functional>
#include <vector>
#include <string>
#include <iostream>

namespace test {
class Test {
private:
    /* data */
public:
    Test() {}
    ~Test() {}

    virtual void OnUpdata(float deltatime) {}
    virtual void OnRenderer() {}  //‰÷»æ
    virtual void OnImGuiRenderer() {}  // GUI
};

class TestMenu:public Test {
private:
    /* data */
public:
    TestMenu(Test*& currentTestPointer);

    void OnImGuiRenderer() override;

	template<typename T>
	void RegisterTest(const std::string& name)
	{
		std::cout<<"register test :"<<name<<std::endl;
		m_Tests.push_back(std::make_pair(name,[](){return new T();}));
	}

private:
	Test*& m_CurrentTest;
	std::vector<std::pair<std::string,std::function<Test*()>>> m_Tests;
};


}

#endif