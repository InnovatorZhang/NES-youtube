#pragma once

#include<cstdint>

// 表示游戏映射name table的方式
enum MIRROR {
	HARDWARE,  // 由硬件指定，即文件中读取的信息指定
	HORIZONTAL,  // 横向
	VERTICAL,  // 纵向
	ONESCREEN_LO,
	ONESCREEN_HI
};

class Mapper {
public:
	Mapper(uint8_t prgBanks, uint8_t chrBanks);
	~Mapper();

public:
	// 将CPU传过来的地址转化为对应的PRG ROM上的地址值，CPU的0x8000-0xFFFF被Mapper所转换
	// 定义一个纯虚函数，要求子类一定要实现一个自己的版本，第一个参数为传过来的CPU地址，第二个为映射后的地址，第三个在有一些Mapper中才有用
	virtual bool cpuMapRead(uint16_t addr, uint32_t& mapped_addr, uint8_t& data) = 0;  
	virtual bool cpuMapWrite(uint16_t addr, uint32_t& mapped_addr, uint8_t data = 0) = 0;

	// 将PPU传过来的地址转化为对应的PRG ROM上的地址值，PPU的0x0000-0x1FFF被Mapper转换
	// 定义一个纯虚函数，要求子类一定要实现一个自己的版本，第一个参数为传过来的CPU地址，第二个为映射后的地址
	virtual bool ppuMapRead(uint16_t addr, uint32_t& mapped_addr) = 0;
	virtual bool ppuMapWrite(uint16_t addr, uint32_t& mapped_addr) = 0;

	virtual void reset() = 0;  // 重置Mapper状态

	// 提供一个返回镜像模式的接口,虚函数，Mapper提供一个默认实现，子类可以自己选择是否改写，利用动态绑定
	virtual MIRROR mirror();

	// 中断接口
	virtual bool irqState();
	virtual void irqClear();  // 不提供默认实现

	// 扫描线计数
	virtual void scanline();  // 不提供默认实现

protected:
	uint8_t nPRGBanks = 0;  // 程序段的个数，一段16KB
	uint8_t nCHRBanks = 0;  // 图形段的个数，一段8KB
};