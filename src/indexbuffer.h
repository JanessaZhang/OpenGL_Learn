#ifndef INDEXBUFFER_HEADER_H
#define INDEXBUFFER_HEADER_H

class indexbuffer{
private:
	unsigned int m_RendererID;
	unsigned int m_count;
public:
	indexbuffer(const unsigned int* data,unsigned int count);
	~indexbuffer();

	void bind() const;
	void unbind() const;
	
	inline unsigned int getcount() const {return m_count;}
};

#endif