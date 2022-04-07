#pragma once
#include <SFML/Graphics.hpp>

class VirtualScreen : public sf::Drawable
{
public:
    void create(unsigned int width, unsigned int height, float pixel_size, sf::Color color);
    void setPixel(std::size_t x, std::size_t y, sf::Color color);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::Vector2u m_screenSize;
    float m_pixelSize; //virtual pixel size in real pixels
    sf::VertexArray m_vertices;

public:
    // 定义NES的屏幕大小
    static const int NESVideoWidth = 256;
    static const int NESVideoHeight = 240;
};

