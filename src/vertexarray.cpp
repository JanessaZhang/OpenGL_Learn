#include "vertexarray.h"
#include "renderer.h"

vertexarray::vertexarray()
{
	glGenVertexArrays(1, &m_RendererID);
}

vertexarray::~vertexarray()
{
}

void vertexarray::AddBuffer(const vertexbuffer& vb,const vertexbufferlayout& layout)
{
	Bind();
	vb.bind();
	const auto& elements=layout.GetElements();
	unsigned long offset=0;
	for(unsigned int i=0;i<elements.size();i++)
	{
		const auto& element =elements[i];
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i,element.count,element.type,element.normalized,layout.GetStride(),(const void*)offset);
		offset+=element.count*VertexBufferElement::GetSizeOfType(element.type);
	}
}

void vertexarray::Bind() const
{
	glBindVertexArray(m_RendererID);
}

void vertexarray::Unbind() const
{
	glBindVertexArray(0);	
}
