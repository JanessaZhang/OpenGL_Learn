#include "renderer.h"
#include<iostream>

void GL_Clear_Error()
{
	while(glGetError()!=GL_NO_ERROR);
}

bool GLLogCall(const char* function,const char* file,int line){
	while(GLenum error = glGetError())
	{
		std::cout<<"[opengl error]  ( "<<error<<" )"<<function<<"  "<<file<<"  :"<<line<<std::endl;
		return false;
	}
	return true;
}

void renderer::Clear()const{
	glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1, 0.75, 0.25, 1.0);
}


void renderer::Draw(vertexarray& va,indexbuffer& ib,shader& mshader)const{
	mshader.Bind();
    va.Bind();
    // ib.bind();
    glDrawElements(GL_TRIANGLES, ib.getcount(), GL_UNSIGNED_INT, nullptr);
}

void renderer::Draw(int first,int count)const{
    glDrawArrays(GL_TRIANGLES, first, count);
}

void renderer::Draw(vertexarray& va,shader& mshader,int point_num,std::string type)const{
	mshader.Bind();
    va.Bind();
	if(type=="points"){
		// glPointSize(10.0);
		glDrawArrays(GL_POINTS, 0, point_num);
		// glLineWidth(20.0);
		glDrawArrays(GL_LINE_LOOP, 0, point_num);
	}
}