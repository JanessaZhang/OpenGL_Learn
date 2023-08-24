#ifndef RENDERER_HEADER_H
#define RENDERER_HEADER_H

#include <glad/glad.h>
#include "vertexarray.h"
#include "indexbuffer.h"
#include "shader.h"
#include <string>


#define ASSERT(x) if(!(x) __debugbreak());
#define GLCall(x) GLClearError();\
    x;\
	ASSERT(GLLogCall(#x,__FILE,__LINE__))

void GL_Clear_Error();
bool GLLogCall(const char* function,const char* file,int line);

class renderer
{
private:
	/* data */
public:
	void Clear()const;
	void Draw(vertexarray& va,indexbuffer& ib,shader& mshader)const;
	// void Draw(vertexarray& va,indexbuffer& ib,shader& mshader,std::string type)const;
	void Draw(vertexarray& va,shader& mshader,int point_num,std::string type)const;

	void Draw(int first,int count)const;
};


#endif