#pragma once

#include<cstdint>
#include<memory>

#include "olcPixelGameEngine.h"
#include "Cartridge.h"

class PPU2C02 {
public:
	PPU2C02();
	~PPU2C02();

private:
	// tblName����vram
	uint8_t tblName[2][1024];  // 2KB��С��name table��PPU�������洢��Ϸ�����Ŀռ䣬һ������table��ÿ��table��СΪ1KB������960KBÿһ���ֽ�����pattern table�е�8x8���ؿ飬ʣ���64KBָʾ�õĵ�ɫ��(Palette)
	uint8_t tblPalette[32];  // ��ɫ�̣�����ǰʮ���ֽ��Ǳ���ר�ã�����ʮ���ֽ��������ã���Ȼ��32�ֽڣ�������ʾ25����ɫ����Ϊÿ4���ֽڶ����ظ�һ���ֽڱ���ɫ
	
	std::shared_ptr<Cartridge> cart;  // ָ����Ϸ����ָ��

private:
	olc::Pixel palScreen[0x40];  // nes֧�ֵ�������ɫ��һ��52��
	olc::Sprite sprScreen = olc::Sprite(256, 240);  // ģ���nes��240x256�ֱ��ʵ���ʾ����ע�⣺�����Ƿ��Ŷ���ģ����Ը�ֵʱ��Ϊ�ڶ�λ����Ϊ��һά

public:
	olc::Sprite& GetScreen();  // �ӿڣ���ȡ��Ļ��ʾ
	olc::Pixel& GetColourFromPaletteRam(uint8_t palette, uint8_t pixel);  // ����ָ���ĵ�ɫ�������ϵõ�������ֵ��ȡ��ɫ
	// ָʾ��ǰ֡�Ƿ����
	bool frame_complete = false;

private:
	// �ṹ���е�ð�ű�ʾλ�� https://www.jianshu.com/p/8f0f5851cb48
	union {
		struct {
			uint8_t unused : 5;
			uint8_t sprite_overflow : 1;
			uint8_t sprite_zero_hit : 1;
			uint8_t vertical_blank : 1;
		};
		uint8_t reg;
	} status;  // ��Ӧ��ַΪ0x2002��status�Ĵ���

	union {
		struct {
			uint8_t grayscale : 1;
			uint8_t render_background_left : 1;
			uint8_t render_sprites_left : 1;
			uint8_t render_background : 1;
			uint8_t render_sprites : 1;
			uint8_t enhance_red : 1;
			uint8_t enhance_green : 1;
			uint8_t enhance_blue : 1;
		};
		uint8_t reg;
	} mask;  // ��Ӧ��ַΪ0x2001��status�Ĵ���

	union PPUCRTL {
		struct {
			uint8_t nametable_x : 1;
			uint8_t nametable_y : 1;
			uint8_t increment_mode : 1;
			uint8_t pattern_sprite : 1;
			uint8_t pattern_background : 1;
			uint8_t sprite_size : 1;
			uint8_t slave_mode : 1;
			uint8_t enable_nmi : 1;
		};
		uint8_t reg;
	} control;  // ��Ӧ��ַΪ0x2000��status�Ĵ���

	union loopy_register {
		struct
		{
			uint16_t coarse_x : 5;
			uint16_t coarse_y : 5;
			uint16_t nametable_x : 1;
			uint16_t nametable_y : 1;
			uint16_t fine_y : 3;
			uint16_t unused : 1;
		};
		uint16_t reg = 0x0000;
	};  // ��Ӧ��ַΪ0x2000��status�Ĵ����������������θüĴ�����ֵ��������Ļ�Ĺ���

	loopy_register vram_addr;  // vram�ĵ�ַ����name table�еĵ�ַ��ָʾcpu��name table�δ�д��ֵ
	loopy_register tram_addr;  // ��ʱ����vram_addrֵ�����渳ֵ��vram_addr����Ϊvram_addr��bufferʹ��

	uint8_t fine_x = 0x00;  // ��ÿһ��8x8����С���еĺ���ƫ����

	uint8_t address_latch = 0x00;  // ָʾ�ǵڼ���д��address��vram_addr�У���Ϊһ������д��ַ��������Ϊ��8λ���ݣ���ַΪ16λ��
	uint8_t ppu_data_buffer = 0x00;  // ��Ϊ��һЩ������Ҫ�Ӻ�һ�����ڲ��ܶ�����������Ҫһ������ռ䣬����һ��ֵ����¼PPU���ߵ���һ���������ݣ���Щ��Ϸ���ڶ�ȡstatus�Ĵ���ʱ�õ�

	// ��������ֵ��ʾ�˵�ǰ�滭������λ��
	int16_t scanline = 0;  // ע�⣺�������з���������Ϊscanline��ֵ����Ϊ�����������ʾ��ǰ����ǹ����Ļ�ϵ�������
	int16_t cycle = 0;  // ָʾ��ǰ����ǹ����Ļ�������λ��

	// ���ɱ�����ص�һϵ�б���
	uint8_t bg_next_tile_id = 0x00;
	uint8_t bg_next_tile_attrib = 0x00;
	uint8_t bg_next_tile_lsb = 0x00;
	uint8_t bg_next_tile_msb = 0x00;
	uint16_t bg_shifter_pattern_lo = 0x0000;
	uint16_t bg_shifter_pattern_hi = 0x0000;
	uint16_t bg_shifter_attrib_lo = 0x0000;
	uint16_t bg_shifter_attrib_hi = 0x0000;

	// ���ɾ�����ص�һϵ�б���
	// OAM (Object Attribute Memory)��PPU�е�һ��洢��û�����ӵ��κ������ϣ����б������64��8x8��8x16������ɵľ���
	struct sObjectAttributeEntry
	{
		uint8_t y;  // ��������Ļ�ϵ�y����
		uint8_t id;  // ͼ����(pattern table)�о����������
		uint8_t attribute;  // Bits 0 -1������ɫ������Ҫ����λ��Bit 5������ʾ��������Ƿ�ȱ���ӵ����ʾ������Ȩ��Bit 6������ʾ�Ƿ�ˮƽ��ת���顣Bit 7������ʾ�Ƿ�ֱ��ת���顣
		uint8_t x;  // ��������Ļ�ϵ�x����
	} OAM[64];

	// CPU��PPU�е�OAMд����ʱ�����ַ�ʽ��һ����ͨ��DMAд��һ����ֱ��CPUд��DMA��ʽ���ܶ�
	// ������������Ǹ�CPUд���ַ�ʽʹ�õ�
	uint8_t oam_addr = 0x00;

	sObjectAttributeEntry spriteScanline[8];  // nes����һ��ɨ���������ֻ�ܳ��ְ˸����飬�����øñ������������8������
	uint8_t sprite_count = 0;  // ��¼һ��ɨ�������ж��ٸ�����
	uint8_t sprite_shifter_pattern_lo[8];  // ÿһ�������Ӧ�Ļ��Ʊ���
	uint8_t sprite_shifter_pattern_hi[8];  // ÿһ�������Ӧ�Ļ��Ʊ���

	// ����0���б�־λ���ù��ܵ������ǣ���� PPU ����Ⱦ��ʱ����� background �Ĳ�͸�������� sprite 0 �Ĳ�͸�������ص���ʱ�򣬻���� sprite 0 hits�������ڵ�ǰֻ֡�����һ��
	// ��ô����������ʲô����Ҫ�����ָ���Ļ��֮ǰ�� background ʱ���ܹ���CPU ͨ�� PPUSCROLL �����Ʊ����ƶ����������ϣ����Ļ�ϰ벿�־�ֹ���°벿���ƶ��أ�
	bool bSpriteZeroHitPossible = false;
	bool bSpriteZeroBeingRendered = false;

public:
	uint8_t* pOAM = (uint8_t*)OAM;  // ָ��PPU��OAM�ڴ��ָ��

	// �������߽����ķ�������ʵ�����ṩCPU��д��ַ0x2000-0x2007��8��PPU�Ĵ����Ľӿ�
	uint8_t cpuRead(uint16_t addr, bool rdonly = false); 
	void cpuWrite(uint16_t addr, uint8_t data);

	// ��PPU���߶�д�ķ���������PPU���߲�û��ʵ��Ϊһ���࣬����ֱ�Ӱ�����PPU���ˣ�����PPU�������ӵ��ڴ涼��PPU���У�����tblname��tblPalette
	uint8_t ppuRead(uint16_t addr, bool rdonly = false);
	void ppuWrite(uint16_t addr, uint8_t data);

	// �ṩ���ⲿ�Ľӿ�
	void ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge);  // ���ӵ���Ϸ������Ϊpattern table��8KB�ռ�����Ϸ����
	void reset();  // ����PPU״̬
	void clock();  // ʱ�ӵδ�
	bool nmi = false;  // �Ƿ������ж�
};