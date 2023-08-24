#ifndef VERTEXBUFFER_HEADER_H
#define VERTEXBUFFER_HEADER_H

class vertexbuffer{
private:
	unsigned int m_RendererID;
public:
	vertexbuffer(const void* data,unsigned int size);
	~vertexbuffer();

	void bind() const;
	void buffer_data(const void* data,unsigned int size);
	void unbind() const;
};

	
#endif