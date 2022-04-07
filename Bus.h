#pragma once

#include<cstdint>
#include<array>

#include"CPU6502.h"
#include"PPU2C02.h"
#include"Cartridge.h"

class Bus {
public:
	Bus(uint8_t scale = 1);
	~Bus();

public:
	CPU6502 cpu;
	PPU2C02 ppu;
	std::shared_ptr<Cartridge> cart;  // 游戏卡
	uint8_t cpuRam[2 * 1024];  // 2KB的cpu内存
	uint8_t controller[2];  // 地址为0x4016和0x4017的两个寄存器，分别对应两个手柄

public:  // 暴露给CPU的方法
	void cpuWrite(uint16_t addr, uint8_t data);  // cpu通过总线进行写数据的方法
	uint8_t cpuRead(uint16_t addr, bool bReadOnly = false);  // cpu通过总线进行读数据的方法

private:
	uint32_t nSystemClockCounter = 0; // 统计一共经过了多少个时钟周期
	uint8_t controller_state[2];  // 作为保存controller寄存器中值的cache

private:
	// DMA传输所需变量，快速将CPU内存中数据传输到PPU的Object Attribute Memory中
	// 如果不通过DMA传输，而是直接通过CPU向PPU指定寄存器写数据，那么速度太慢
	// 所以直接通过DMA的方式，一次性传256个字节数据到PPUObject Attribute Memory中，只需要消耗512CPU时钟周期
	// 在这个512个时钟周期中，CPU将会被挂起等待，直到数据传输完
	// 每次刚好传输一个page(256字节)大小的数据，所以这里将page号与byte号分开存储，共同形成一个16字节地址

	uint8_t dma_page = 0x00;  // DMA传输的page号
	uint8_t dma_addr = 0x00;  // 指定page中的偏移量
	uint8_t dma_data = 0x00;  // 传输的数据

	// DMA传输需要准确的时钟周期，一次传输需要512个时钟周期去读写256字节CPU ram中的数据到PPU oam中，
	// 一次读取操作后跟着一次写，CPU必须在偶数时钟周期读，奇数时间写，所以用一个dma_dummy变量来控制
	// 该变量用以保证每次是从偶数时钟周期开始操作的
	bool dma_dummy = true;

	// 设置一个变量，表示dma传输发生了，需要将CPU挂起
	bool dma_transfer = false;

public: // 系统调用接口
	void insertCartridge(const std::shared_ptr<Cartridge>& cartridge);  // 将游戏卡连接到总线上
	void reset();  // 重置系统
	void clock(); // 时钟滴答
};