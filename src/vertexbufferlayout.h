#ifndef VERTEXBUFFERLAYOUT_HEADER_H
#define VERTEXBUFFERLAYOUT_HEADER_H

#include <glad/glad.h>

#include <vector>

struct VertexBufferElement {
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int GetSizeOfType(unsigned int type) {
        switch (type) {
            case GL_FLOAT:
                return 4;
            case GL_UNSIGNED_INT:
                return 4;
            case GL_UNSIGNED_BYTE:
                return 1;
        }

        return 0;
    }
};

class vertexbufferlayout {
private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_stride;

public:
    vertexbufferlayout(/* args */) : m_stride(0) {}


    template <typename T>
    void Push(unsigned int count);

    inline const std::vector<VertexBufferElement> GetElements() const {
        return m_Elements;
    }
    inline unsigned int GetStride() const { return m_stride; }
};

template <>
void vertexbufferlayout::Push<float>(unsigned int count);

template <>
void vertexbufferlayout::Push<unsigned int>(unsigned int count);
template <>
void vertexbufferlayout::Push<unsigned char>(unsigned int count);

#endif