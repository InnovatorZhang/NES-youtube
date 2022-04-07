#include<string>
#include<iostream>
#include <SFML/Window.hpp>
#include"Bus.h"
#include"CPU6502.h"

class Emulator {
public:
	Emulator(std::string path, uint8_t scale = 1) : nes(scale) {
		// 创建主窗口
		m_window.create(sf::VideoMode(VirtualScreen::NESVideoWidth * scale, VirtualScreen::NESVideoHeight * scale),
			"MY_NES", sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);
		// 开启垂直同步
		m_window.setVerticalSyncEnabled(true); 
		// 限制在60帧
		m_window.setFramerateLimit(120);

		// 加载游戏卡
		cart = std::make_shared<Cartridge>(path);

		// 检查是否加载成功
		if (!cart->ImageValid()) {
			return;
		}

		// 将游戏卡连接到总线上
		nes.insertCartridge(cart);

		// 重置Bus状态，做好运行程序的准备
		nes.reset();
	}

private:
	// 定义nes中的组件
	Bus nes;
	std::shared_ptr<Cartridge> cart;
	// SFML方法中的Window
	sf::RenderWindow m_window;
	

public:
	// 接受用户的输入
	bool run()
	{
		while (m_window.isOpen()) {
			// 玩家一号
			nes.controller[0] = 0x00;
			// 使用sfml框架中提供的键盘检测事件API
			nes.controller[0] |= sf::Keyboard::isKeyPressed(sf::Keyboard::J) << 7;     // A Button
			nes.controller[0] |= sf::Keyboard::isKeyPressed(sf::Keyboard::K) << 6;     // B Button
			nes.controller[0] |= sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) << 5;     // Select
			nes.controller[0] |= sf::Keyboard::isKeyPressed(sf::Keyboard::Return) << 4;     // Start
			nes.controller[0] |= sf::Keyboard::isKeyPressed(sf::Keyboard::W) << 3;
			nes.controller[0] |= sf::Keyboard::isKeyPressed(sf::Keyboard::S) << 2;
			nes.controller[0] |= sf::Keyboard::isKeyPressed(sf::Keyboard::A) << 1;
			nes.controller[0] |= sf::Keyboard::isKeyPressed(sf::Keyboard::D) << 0;

			// 玩家二号
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

			// 将SFML的当前帧到屏幕上
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
	// 第一个参数为nes文件路径，第二个为视频窗口放大倍数，比240x256大多少倍
	Emulator emulator("game/LegendofZelda.nes", 4);
	emulator.run();

	return 0;
}
