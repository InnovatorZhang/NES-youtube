#pragma once
#include<cstdint>
#include<memory>
#include<SFML/Config.hpp>

#include "Cartridge.h"
#include "VirtualScreen.h"

class PPU2C02 {
public:
	PPU2C02(uint8_t scale = 1); // 表示一个像素扩展多大
	~PPU2C02();

private:
	// tblName又名vram
	uint8_t tblName[2][1024];  // 2KB大小的name table，PPU中用来存储游戏背景的空间，一共两个table，每个table大小为1KB，其中960KB每一个字节索引pattern table中的8x8像素块，剩余的64KB指示用的调色板(Palette)
	uint8_t tblPalette[32];  // 调色盘，里面前十六字节是背景专用，后面十六字节是人物用，虽然有32字节，但最多表示25种颜色，因为每4个字节都会重复一个字节背景色
	
	std::shared_ptr<Cartridge> cart;  // 指向游戏卡的指针

private:
	// 初始化NES支持的颜色,定义8字节色彩，RGBA
	sf::Uint32 nes_colors[0x40] = {
			0x666666ff, 0x002a88ff, 0x1412a7ff, 0x3b00a4ff, 0x5c007eff, 0x6e0040ff, 0x6c0600ff, 0x561d00ff,
			0x333500ff, 0x0b4800ff, 0x005200ff, 0x004f08ff, 0x00404dff, 0x000000ff, 0x000000ff, 0x000000ff,
			0xadadadff, 0x155fd9ff, 0x4240ffff, 0x7527feff, 0xa01accff, 0xb71e7bff, 0xb53120ff, 0x994e00ff,
			0x6b6d00ff, 0x388700ff, 0x0c9300ff, 0x008f32ff, 0x007c8dff, 0x000000ff, 0x000000ff, 0x000000ff,
			0xfffeffff, 0x64b0ffff, 0x9290ffff, 0xc676ffff, 0xf36affff, 0xfe6eccff, 0xfe8170ff, 0xea9e22ff,
			0xbcbe00ff, 0x88d800ff, 0x5ce430ff, 0x45e082ff, 0x48cddeff, 0x4f4f4fff, 0x000000ff, 0x000000ff,
			0xfffeffff, 0xc0dfffff, 0xd3d2ffff, 0xe8c8ffff, 0xfbc2ffff, 0xfec4eaff, 0xfeccc5ff, 0xf7d8a5ff,
			0xe4e594ff, 0xcfef96ff, 0xbdf4abff, 0xb3f3ccff, 0xb5ebf2ff, 0xb8b8b8ff, 0x000000ff, 0x000000ff,
	};
	// 虚拟显示屏幕
	VirtualScreen m_emulatorScreen;

public:
	VirtualScreen& GetVirtualScreen(); // 接口，获取屏幕显示
	sf::Color GetColourFromPaletteRam(uint8_t palette, uint8_t pixel);  // 根据指定的调色板号与组合得到的像素值获取颜色
	// 指示当前帧是否完成
	bool frame_complete = false;

private:
	// 结构体中的冒号表示位域。 https://www.jianshu.com/p/8f0f5851cb48
	union {
		struct {
			uint8_t unused : 5;
			uint8_t sprite_overflow : 1;
			uint8_t sprite_zero_hit : 1;
			uint8_t vertical_blank : 1;
		};
		uint8_t reg;
	} status;  // 对应地址为0x2002的status寄存器

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
	} mask;  // 对应地址为0x2001的status寄存器

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
	} control;  // 对应地址为0x2000的status寄存器

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
	};  // 对应地址为0x2000的status寄存器，保存连续两次该寄存器的值，控制屏幕的滚动

	loopy_register vram_addr;  // vram的地址，即name table中的地址，指示cpu在name table何处写入值
	loopy_register tram_addr;  // 暂时保存vram_addr值，后面赋值给vram_addr，作为vram_addr的buffer使用

	uint8_t fine_x = 0x00;  // 在每一个8x8像素小格中的横向偏移量

	uint8_t address_latch = 0x00;  // 指示是第几次写入address到vram_addr中，因为一共两次写地址操作（因为有8位数据，地址为16位）
	uint8_t ppu_data_buffer = 0x00;  // 因为有一些数据需要延后一个周期才能读到，所以需要一个缓存空间，保存一下值；记录PPU总线的上一个传输数据，有些游戏会在读取status寄存器时用到

	// 下面两个值表示了当前绘画的像素位置
	int16_t scanline = 0;  // 注意：这里是有符号数，因为scanline数值可以为负数，这里表示当前电子枪在屏幕上的纵坐标
	int16_t cycle = 0;  // 指示当前电子枪在屏幕横坐标的位置

	// 生成背景相关的一系列变量
	uint8_t bg_next_tile_id = 0x00;
	uint8_t bg_next_tile_attrib = 0x00;
	uint8_t bg_next_tile_lsb = 0x00;
	uint8_t bg_next_tile_msb = 0x00;
	uint16_t bg_shifter_pattern_lo = 0x0000;
	uint16_t bg_shifter_pattern_hi = 0x0000;
	uint16_t bg_shifter_attrib_lo = 0x0000;
	uint16_t bg_shifter_attrib_hi = 0x0000;

	// 生成精灵相关的一系列变量
	// OAM (Object Attribute Memory)是PPU中的一块存储，没有连接到任何总线上，其中保存的是64个8x8或8x16像素组成的精灵
	struct sObjectAttributeEntry
	{
		uint8_t y;  // 精灵在屏幕上的y坐标
		uint8_t id;  // 图案表(pattern table)中精灵的索引号
		uint8_t attribute;  // Bits 0 -1——颜色的最重要的两位。Bit 5——表示这个精灵是否比背景拥有显示的优先权。Bit 6——表示是否水平翻转精灵。Bit 7——表示是否垂直翻转精灵。
		uint8_t x;  // 精灵在屏幕上的x坐标
	} OAM[64];

	// CPU向PPU中的OAM写数据时有两种方式，一种是通过DMA写，一种是直接CPU写，DMA方式会快很多
	// 下面这个变量是给CPU写这种方式使用的
	uint8_t oam_addr = 0x00;

	sObjectAttributeEntry spriteScanline[8];  // nes限制一个扫描线上最多只能出现八个精灵，所以用该变量保存这最多8个精灵
	uint8_t sprite_count = 0;  // 记录一个扫描线上有多少个精灵
	uint8_t sprite_shifter_pattern_lo[8];  // 每一个精灵对应的绘制变量
	uint8_t sprite_shifter_pattern_hi[8];  // 每一个精灵对应的绘制变量

	// 精灵0命中标志位，该功能的作用是，如果 PPU 在渲染的时候，如果 background 的不透明像素与 sprite 0 的不透明像素重叠的时候，会产生 sprite 0 hits，并且在当前帧只会产生一次
	// 那么它的作用是什么？主要用来分割屏幕。之前在 background 时介绍过，CPU 通过 PPUSCROLL 来控制背景移动，但是如果希望屏幕上半部分静止，下半部分移动呢？
	bool bSpriteZeroHitPossible = false;
	bool bSpriteZeroBeingRendered = false;

public:
	uint8_t* pOAM = (uint8_t*)OAM;  // 指向PPU中OAM内存的指针

	// 与主总线交互的方法，其实就是提供CPU读写地址0x2000-0x2007这8个PPU寄存器的接口
	uint8_t cpuRead(uint16_t addr, bool rdonly = false); 
	void cpuWrite(uint16_t addr, uint8_t data);

	// 与PPU总线读写的方法，这里PPU总线并没有实现为一个类，而是直接包含在PPU中了，所有PPU总线连接的内存都在PPU类中，比如tblname和tblPalette
	uint8_t ppuRead(uint16_t addr, bool rdonly = false);
	void ppuWrite(uint16_t addr, uint8_t data);

	// 提供给外部的接口
	void ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge);  // 连接到游戏卡，因为pattern table的8KB空间在游戏卡中
	void reset();  // 重置PPU状态
	void clock();  // 时钟滴答
	bool nmi = false;  // 是否引发中断
};