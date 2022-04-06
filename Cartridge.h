#pragma once

#include<cstdint>
#include<string>
#include<fstream>
#include<vector>

#include"Mapper_000.h"
#include"Mapper_001.h"
#include"Mapper_002.h"
#include"Mapper_003.h"
#include"Mapper_004.h"
#include"Mapper_066.h"

class Cartridge {
public:
	Cartridge(const std::string& sFileName);  // 指定文件名的构造函数
	~Cartridge();

	bool ImageValid();  // 返回文件是否读取成功的状态，若文件读取成功为true，反之，false

private:
	bool bImageValid = false;  // 指示文件是否读取成功
	MIRROR hw_mirror = HORIZONTAL;  // 默认值设置为Mapper类中定义的枚举变量HORIZONTAL

	uint8_t nMapperID = 0;
	uint8_t nPRGBanks = 0;  // 指示有多少个程序段，每个段为16KB
	uint8_t nCHRBanks = 0;  // 指示有多少个图像段，每个段为8KB

	std::vector<uint8_t> vPRGMemory;  // 保存文件中读取出来的PRG段数据
	std::vector<uint8_t> vCHRMemory;  // 保存文件中读取出来的CHR段数据

	std::shared_ptr<Mapper> pMapper;  // mapper指针，利用多态，可以指向各种Mapper子类，动态绑定虚函数

public:
	// CPU总线读写数据，映射的是CPU的0x8000-0xFFFF数据段的数据
	bool cpuRead(uint16_t addr, uint8_t& data);
	bool cpuWrite(uint16_t addr, uint8_t data);

	// PPU总线读写数据，映射的是PPU的0x0000-0x1FFF数据段的数据
	bool ppuRead(uint16_t addr, uint8_t& data);
	bool ppuWrite(uint16_t addr, uint8_t data);

	void reset(); // 重置mapper的状态，不是重置游戏卡，这个方法调用mapper的reset函数

	MIRROR Mirror();  // 返回Mirror信息的接口

	std::shared_ptr<Mapper> GetMapper();  // 返回Mapper指针的接口
};
