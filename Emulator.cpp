#include<string>
#include<iostream>
#include <SFML/Window.hpp>
#include"Bus.h"
#include"CPU6502.h"

class Emulator {
public:
	Emulator(std::string path, uint8_t scale = 1) : nes(scale) {
		// ����������
		m_window.create(sf::VideoMode(VirtualScreen::NESVideoWidth * scale, VirtualScreen::NESVideoHeight * scale),
			"MY_NES", sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);
		// ������ֱͬ��
		m_window.setVerticalSyncEnabled(true); 
		// ������60֡
		m_window.setFramerateLimit(120);

		// ������Ϸ��
		cart = std::make_shared<Cartridge>(path);

		// ����Ƿ���سɹ�
		if (!cart->ImageValid()) {
			return;
		}

		// ����Ϸ�����ӵ�������
		nes.insertCartridge(cart);

		// ����Bus״̬���������г����׼��
		nes.reset();
	}

private:
	// ����nes�е����
	Bus nes;
	std::shared_ptr<Cartridge> cart;
	// SFML�����е�Window
	sf::RenderWindow m_window;
	

public:
	// �����û�������
	bool run()
	{
		while (m_window.isOpen()) {
			// ���һ��
			nes.controller[0] = 0x00;
			// ʹ��sfml������ṩ�ļ��̼���¼�API
			nes.controller[0] |= sf::Keyboard::isKeyPressed(sf::Keyboard::J) << 7;     // A Button
			nes.controller[0] |= sf::Keyboard::isKeyPressed(sf::Keyboard::K) << 6;     // B Button
			nes.controller[0] |= sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) << 5;     // Select
			nes.controller[0] |= sf::Keyboard::isKeyPressed(sf::Keyboard::Return) << 4;     // Start
			nes.controller[0] |= sf::Keyboard::isKeyPressed(sf::Keyboard::W) << 3;
			nes.controller[0] |= sf::Keyboard::isKeyPressed(sf::Keyboard::S) << 2;
			nes.controller[0] |= sf::Keyboard::isKeyPressed(sf::Keyboard::A) << 1;
			nes.controller[0] |= sf::Keyboard::isKeyPressed(sf::Keyboard::D) << 0;

			// ��Ҷ���
			// Handle input for controller in port #2
			nes.controller[1] = 0x00;
			nes.controller[1] |= sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1) << 7;     // A Button
			nes.controller[1] |= sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2) << 6;     // B Button
			nes.controller[1] |= sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad3) << 5;     // Select
			nes.controller[1] |= sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad5) << 4;     // Start
			nes.controller[1] |= sf::Keyboard::isKeyPressed(sf::Keyboard::Up) << 3;
			nes.controller[1] |= sf::Keyboard::isKeyPressed(sf::Keyboard::Down) << 2;
			nes.controller[1] |= sf::Keyboard::isKeyPressed(sf::Keyboard::Left) << 1;
			nes.controller[1] |= sf::Keyboard::isKeyPressed(sf::Keyboard::Right) << 0;


			do { nes.clock(); } while (!nes.ppu.frame_complete);
			nes.ppu.frame_complete = false;

			// ��SFML�ĵ�ǰ֡����Ļ��
			sf::Event event;
			while (m_window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed ||
					(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
				{
					m_window.close();
					return true;
				}
			}
			m_window.clear();
			m_window.draw(nes.ppu.GetVirtualScreen());
			m_window.display();
		}

		return true;
	}
};



int main() {
	// ��һ������Ϊnes�ļ�·�����ڶ���Ϊ��Ƶ���ڷŴ�������240x256����ٱ�
	Emulator emulator("game/LegendofZelda.nes", 4);
	emulator.run();

	return 0;
}
