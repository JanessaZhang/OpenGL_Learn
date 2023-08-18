#include "texture.h"

#include "vendor/stb_image/stb_image.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

Texture::Texture(const std::string& path) :
    m_FilePath(path), m_LocalBuffer(nullptr), m_width(0), m_height(0),
    m_BPP(0) {
    //1. stbi����ͼ��
    // stbi_set_flip_vertically_on_load(1);  //���·�תͼƬ
    // m_LocalBuffer = stbi_load(path.c_str(), &m_width, &m_height, &m_BPP, 4);

    // std::cout<<":img.channels:"<<m_BPP<<std::endl;
    
	//2. ʹ��opencv����ͼ�����������
	cv::Mat image=cv::imread(path);
	cv::flip(image,image,0);
    m_width=image.cols;
	m_height=image.rows;
	cv::cvtColor(image,image,cv::COLOR_BGR2RGB);
	// cv::cvtColor(image,image,cv::COLOR_BGR2RGBA);   //png

	std::cout<<":img.channels:"<<image.channels()<<std::endl;

    glGenTextures(1, &m_RendererID);  //��������buffer
    glBindTexture(GL_TEXTURE_2D,
                  m_RendererID);  //��buffer m_RendererID���ǰ󶨵�����
    //����������� (����)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // ����2D����  �������ݸ�opengl

	// 1.stbi��ͼ����������
    // glTexImage2D(GL_TEXTURE_2D,
    //              0,
    //              GL_RGBA8,
    //              m_width,
    //              m_height,
    //              0,
    //              GL_RGBA,
    //              GL_UNSIGNED_BYTE,
    //              m_LocalBuffer);

	// 2.opencv��ͼ����������
	// glTexImage2D(GL_TEXTURE_2D,   //png
    //              0,
    //              GL_RGBA8,
    //              m_width,
    //              m_height,
    //              0,
    //              GL_RGBA,
    //              GL_UNSIGNED_BYTE,
    //              image.ptr());
	glTexImage2D(GL_TEXTURE_2D,     //jpg
                 0,
                 GL_RGB8,
                 m_width,
                 m_height,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 image.ptr());

    glBindTexture(GL_TEXTURE_2D, 0);

    if (m_LocalBuffer)
        stbi_image_free(m_LocalBuffer);
}

//Texture::~Texture() {
//   glDeleteTextures(1, &m_RendererID);
//}

void Texture::Bind(unsigned int slot )const  //Ĭ�ϰ󶨵�������0
{
    //ָ��������
	glActiveTexture(GL_TEXTURE0+slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

}
void Texture::Unbind()const {
    glBindTexture(GL_TEXTURE_2D, 0);
}
