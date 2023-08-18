#include "vertexbuffer.h"
#include "renderer.h"

vertexbuffer::vertexbuffer(const void* data,unsigned int size)
{
	glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

// vertexbuffer::~vertexbuffer()
// {
// 	glDeleteBuffers(1,&m_RendererID);
// }

void vertexbuffer::bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void vertexbuffer::buffer_data(const void* data,unsigned int size)
{
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void vertexbuffer::unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

