#pragma once
#include <SFML/Graphics.hpp>

class VirtualScreen : public sf::Drawable
{
public:
    // 创建每个顶点的位置以及初始值
    void create(unsigned int width, unsigned int height, float pixel_size, sf::Color color);
    // 设置每一个点的像素值，每一次一个像素，由两个三角形，6个顶点组成，所以在该方法处赋值像素对应的6个顶点
    void setPixel(std::size_t x, std::size_t y, sf::Color color);

private:
    // 必须要复写该方法，定义了如何画出自身
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::Vector2u m_screenSize;  // 屏幕大小，nes是256x240
    float m_pixelSize; //virtual pixel size in real pixels
    sf::VertexArray m_vertices;  // 顶点数组， m_vertices.setPrimitiveType(sf::Triangles)，表示三个顶点一组，画三角形，由三角形组成像素

public:
    // 定义NES的屏幕大小
    static const int NESVideoWidth = 256;
    static const int NESVideoHeight = 240;
};

