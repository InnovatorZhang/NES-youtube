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
	// ����nes�е����
	Bus nes;
	std::shared_ptr<Cartridge> cart;
	// ����ʱ�ӵı���
	float fResidualTime = 0.0f;

	// ����ʱ���õķ���
	bool OnUserCreate(){
		// ������Ϸ��
		cart = std::make_shared<Cartridge>("game/mario.nes");

		// ����Ƿ���سɹ�
		if (!cart->ImageValid()) {
			return false;
		}

		// ����Ϸ�����ӵ�������
		nes.insertCartridge(cart);

		// ����Bus״̬����ʼ���г���
		nes.reset();

		return true;
	}

	// �����û�������
	bool OnUserUpdate(float fElapsedTime)
	{
		Clear(olc::DARK_BLUE);

		// ���һ��
		nes.controller[0] = 0x00;
		nes.controller[0] |= GetKey(olc::Key::K).bHeld ? 0x80 : 0x00;     // A Button
		nes.controller[0] |= GetKey(olc::Key::J).bHeld ? 0x40 : 0x00;     // B Button
		nes.controller[0] |= GetKey(olc::Key::SHIFT).bHeld ? 0x20 : 0x00;     // Select
		nes.controller[0] |= GetKey(olc::Key::ENTER).bHeld ? 0x10 : 0x00;     // Start
		nes.controller[0] |= GetKey(olc::Key::W).bHeld ? 0x08 : 0x00;
		nes.controller[0] |= GetKey(olc::Key::S).bHeld ? 0x04 : 0x00;
		nes.controller[0] |= GetKey(olc::Key::A).bHeld ? 0x02 : 0x00;
		nes.controller[0] |= GetKey(olc::Key::D).bHeld ? 0x01 : 0x00;

		// ��Ҷ���
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

		// ��R����nes
		if (GetKey(olc::Key::R).bPressed) nes.reset();

		if (fResidualTime > 0.0f)
			fResidualTime -= fElapsedTime;
		else
		{
			fResidualTime += (1.0f / 60.0f) - fElapsedTime;
			do { nes.clock(); } while (!nes.ppu.frame_complete);
			nes.ppu.frame_complete = false;
		}

		// ����ǰ֡����Ļ��
		DrawSprite(0, 0, &nes.ppu.GetScreen(), 2);
		return true;
	}
};


int main() {
	Emulator emulator;

	emulator.Construct(512, 480, 1, 1);  // ��������256x240�����������������ƺ������������ű���
	emulator.Start();

	return 0;
}
