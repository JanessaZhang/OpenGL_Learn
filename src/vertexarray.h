#ifndef VERTEXARRAY_HEADER_H
#define VERTEXARRAY_HEADER_H

#include "vertexbuffer.h"
#include "vertexbufferlayout.h"

class vertexarray
{
private:
	unsigned int m_RendererID;
public:
	vertexarray(/* args */);
	~vertexarray();

	void AddBuffer(const vertexbuffer& vb,const vertexbufferlayout& layout);
	void Bind() const;
	void Unbind() const;
};

#endif