#include<iostream>
#include"Bus.h"
#include"CPU6502.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class Emulator : public olc::PixelGameEngine {
public:
	Emulator() {
		sAppName = "emulator";
	}

private:
	// 定义nes中的组件
	Bus nes;
	std::shared_ptr<Cartridge> cart;
	// 控制时钟的变量
	float fResidualTime = 0.0f;

	// 创建时调用的方法
	bool OnUserCreate(){
		// 加载游戏卡
		cart = std::make_shared<Cartridge>("game/mario.nes");

		// 检查是否加载成功
		if (!cart->ImageValid()) {
			return false;
		}

		// 将游戏卡连接到总线上
		nes.insertCartridge(cart);

		// 重置Bus状态，开始运行程序
		nes.reset();

		return true;
	}

	// 接受用户的输入
	bool OnUserUpdate(float fElapsedTime)
	{
		Clear(olc::DARK_BLUE);

		// 玩家一号
		nes.controller[0] = 0x00;
		nes.controller[0] |= GetKey(olc::Key::K).bHeld ? 0x80 : 0x00;     // A Button
		nes.controller[0] |= GetKey(olc::Key::J).bHeld ? 0x40 : 0x00;     // B Button
		nes.controller[0] |= GetKey(olc::Key::SHIFT).bHeld ? 0x20 : 0x00;     // Select
		nes.controller[0] |= GetKey(olc::Key::ENTER).bHeld ? 0x10 : 0x00;     // Start
		nes.controller[0] |= GetKey(olc::Key::W).bHeld ? 0x08 : 0x00;
		nes.controller[0] |= GetKey(olc::Key::S).bHeld ? 0x04 : 0x00;
		nes.controller[0] |= GetKey(olc::Key::A).bHeld ? 0x02 : 0x00;
		nes.controller[0] |= GetKey(olc::Key::D).bHeld ? 0x01 : 0x00;

		// 玩家二号
		// Handle input for controller in port #2
		nes.controller[1] = 0x00;
		nes.controller[1] |= GetKey(olc::Key::NP1).bHeld ? 0x80 : 0x00;     // A Button
		nes.controller[1] |= GetKey(olc::Key::NP2).bHeld ? 0x40 : 0x00;     // B Button
		nes.controller[1] |= GetKey(olc::Key::NP3).bHeld ? 0x20 : 0x00;     // Select
		nes.controller[1] |= GetKey(olc::Key::NP5).bHeld ? 0x10 : 0x00;     // Start
		nes.controller[1] |= GetKey(olc::Key::UP).bHeld ? 0x08 : 0x00;
		nes.controller[1] |= GetKey(olc::Key::DOWN).bHeld ? 0x04 : 0x00;
		nes.controller[1] |= GetKey(olc::Key::LEFT).bHeld ? 0x02 : 0x00;
		nes.controller[1] |= GetKey(olc::Key::RIGHT).bHeld ? 0x01 : 0x00;

		// 按R重置nes
		if (GetKey(olc::Key::R).bPressed) nes.reset();

		if (fResidualTime > 0.0f)
			fResidualTime -= fElapsedTime;
		else
		{
			fResidualTime += (1.0f / 60.0f) - fElapsedTime;
			do { nes.clock(); } while (!nes.ppu.frame_complete);
			nes.ppu.frame_complete = false;
		}

		// 将当前帧到屏幕上
		DrawSprite(0, 0, &nes.ppu.GetScreen(), 2);
		return true;
	}
};


int main() {
	Emulator emulator;

	emulator.Construct(512, 480, 1, 1);  // 两倍大于256x240，后面两个参数控制横向和纵向的扩张比例
	emulator.Start();

	return 0;
}
