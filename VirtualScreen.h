#pragma once
#include <SFML/Graphics.hpp>

class VirtualScreen : public sf::Drawable
{
public:
    // ����ÿ�������λ���Լ���ʼֵ
    void create(unsigned int width, unsigned int height, float pixel_size, sf::Color color);
    // ����ÿһ���������ֵ��ÿһ��һ�����أ������������Σ�6��������ɣ������ڸ÷�������ֵ���ض�Ӧ��6������
    void setPixel(std::size_t x, std::size_t y, sf::Color color);

private:
    // ����Ҫ��д�÷�������������λ�������
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::Vector2u m_screenSize;  // ��Ļ��С��nes��256x240
    float m_pixelSize; //virtual pixel size in real pixels
    sf::VertexArray m_vertices;  // �������飬 m_vertices.setPrimitiveType(sf::Triangles)����ʾ��������һ�飬�������Σ����������������

public:
    // ����NES����Ļ��С
    static const int NESVideoWidth = 256;
    static const int NESVideoHeight = 240;
};

