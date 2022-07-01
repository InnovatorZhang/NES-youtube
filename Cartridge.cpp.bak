#include "Cartridge.h"

/*
* Cartridged的有参构造
* 该函数接受一个路径名，以此读取nes游戏文件
*/
Cartridge::Cartridge(const std::string& sFileName)
{
	// 定义一个文件头结构体
	struct sHeader {
		char name[4];  // 前四个字节分别保存 'N' 'E' 'S' 0x1A 这四个值
		uint8_t prg_rom_chunks;  // 程序镜像数量
		uint8_t chr_rom_chunks;  // 图形镜像数量
		uint8_t mapper1;  // bit4~bit7这4个bit为MapperID的低4位，bit0表示屏幕镜像方式，0水平，1垂直；bit1表示游戏卡中是否有带电池的RAM，映射到0x6000-0x7FFF；bit2表示是否有trainer；bit3表示是否4屏。
		uint8_t mapper2;  // bit4~bit7这4个bit为MapperID的高4位，其他4位没有作用
		uint8_t prg_ram_size;  // 一般用不到，都是默认16KB
		uint8_t tv_system1;  // 一般用不到
		uint8_t tv_system2;  // 一般用不到
		char unused[5];  // 头文件中没有使用的字节，可能是为了以后扩展做准备，也可能指示为了对齐16字节
	} header;

	bImageValid = false;  // 指示nes文件是否可用

	std::ifstream ifs;  // 文件读取流，读取nes文件

	ifs.open(sFileName, std::ifstream::binary);  // 以字节流的方式读取
	if (ifs.is_open()) {
		// 文件打开成功，进入下面流程
		ifs.read((char*)&header, sizeof(sHeader));  // 将文件流中的前16字节读取到定义的header当中

		// 检查mapper1的mapper1中的trainer位(mapper1字节的第3个bit)，不为0则表示存在512字节的trainer数据，不用管他，直接移动文件curr跳过这512字节
		if (header.mapper1 & 0x04) {
			ifs.seekg(512, std::ios_base::cur);
		}

		// 获取Mapper类型ID，MapperID的低4位保存在header中mapper1的高4位，高4位保存在header中mapper2的高4位，组装获取MapperID
		nMapperID = ((header.mapper2 >> 4) << 4 | (header.mapper1) >> 4);
		// 获取屏幕镜像方式，header中mapper1中bit0表示屏幕镜像方式，0水平,1垂直
		hw_mirror = (header.mapper1 & 0x01) ? VERTICAL : HORIZONTAL;

		// 默认文件类型为1
		uint8_t nFileType = 1;
		if ((header.mapper2 & 0x0C) == 0x08) {
			nFileType = 2;
		}

		// 对不同文件类型分别做处理
		if (nFileType == 1) {
			// 获取程序镜像数量，每个程序镜像大小为16KB，将Cartridge类的程序内存空间resize到对应大小
			nPRGBanks = header.prg_rom_chunks;
			vPRGMemory.resize(nPRGBanks * 16 * 1024);
			// 从文件中读取对应大小到游戏卡内存中
			ifs.read((char*)vPRGMemory.data(), vPRGMemory.size());

			// 获取图形镜像数量，每个图形镜像大小为8KB，将Cartridge类的图形内存空间resize到对应大小
			nCHRBanks = header.chr_rom_chunks;
			if (nCHRBanks == 0) {
				// 因为游戏写游戏卡中带有电池，可以自己提供一个8KB大小的RAM
				vCHRMemory.resize(8 * 1024);
			}
			else {
				// 根据镜像数量分配空间
				vCHRMemory.resize(nCHRBanks * 8 * 1024);
			}
			// 从文件中读取对应大小到游戏卡内存中
			ifs.read((char*)vCHRMemory.data(), vCHRMemory.size());
		}
		else if (nFileType == 2) {
			// 从prg_ram_size中获取程序镜像数量，每个程序镜像大小为16KB，将Cartridge类的程序内存空间resize到对应大小
			nPRGBanks = ((header.prg_ram_size & 0x07) << 8) | header.prg_rom_chunks;
			vPRGMemory.resize(nPRGBanks * 16 * 1024);
			// 从文件中读取对应大小到游戏卡内存中
			ifs.read((char*)vPRGMemory.data(), vPRGMemory.size());

			// 从prg_ram_size中获取图形镜像数量，每个图形镜像大小为8KB，将Cartridge类的图形内存空间resize到对应大小
			nCHRBanks = ((header.prg_ram_size & 0x38) << 8) | header.chr_rom_chunks;
			vCHRMemory.resize(8 * 1024);
			// 从文件中读取对应大小到游戏卡内存中
			ifs.read((char*)vCHRMemory.data(), vCHRMemory.size());
		}

		// 根据MapperID加载mapper,这里利用多态+动态绑定，将子类指针绑定到父类指针，动态绑定调用方法
		// 先创建一个mapper，后面再添加
		switch (nMapperID) {
		case 0: pMapper = std::make_shared<Mapper_000>(nPRGBanks, nCHRBanks); break;
		case 1: pMapper = std::make_shared<Mapper_001>(nPRGBanks, nCHRBanks); break;
		case 2: pMapper = std::make_shared<Mapper_002>(nPRGBanks, nCHRBanks); break;
		case 3: pMapper = std::make_shared<Mapper_003>(nPRGBanks, nCHRBanks); break;
		case 4: pMapper = std::make_shared<Mapper_004>(nPRGBanks, nCHRBanks); break;
		case 66: pMapper = std::make_shared<Mapper_066>(nPRGBanks, nCHRBanks); break;
		}

		bImageValid = true;  // 表示读取成功
		ifs.close();  // 关闭文件流
	}
}

/*
* Cartridged的析构函数
*/
Cartridge::~Cartridge()
{
	// 因为没有动态申请空间，所以函数体为空
}

/*
* 返回文件是否加载成功
*/
bool Cartridge::ImageValid()
{
	return bImageValid;
}

/*
* CPU总线读写数据，映射的是CPU的0x8000-0xFFFF数据段的数据,在CPU通过总线发起这个地址段的读请求之后
* Cartidge将传进来的地址送进自身的mapper电路中，最终将 0x8000-0xFFFF 这个地址段的地址转换为 vPRGMemory 内存中真实的地址
* 最后根据该地址对data赋值，读取数据流程结束
*/
bool Cartridge::cpuRead(uint16_t addr, uint8_t& data)
{
	uint32_t mapped_addr = 0;  // 保存转换后的地址，因为有些mapper可以提供给超过64KB数据，所以这里使用uint32_t类型的数据
	// 通过mapper转换CPU地址-》vPRGMemory地址
	if (pMapper->cpuMapRead(addr, mapped_addr, data)) {
		// 地址转换成功，根据转换后的地址提供数据
		if (mapped_addr == 0xFFFFFFFF) {
			// 如果mapper返回该值，直接返回ture即可
			return true;
		}
		else {
			// 根据转换后的地址读取数据
			data = vPRGMemory[mapped_addr];
		}
		return true;
	}

	return false;
}

/*
* CPU总线读写数据，映射的是CPU的0x8000-0xFFFF数据段的数据,在CPU通过总线发起这个地址段的写请求之后
* Cartidge将传进来的地址送进自身的mapper电路中，最终将 0x8000-0xFFFF 这个地址段的地址转换为 vPRGMemory 内存中真实的地址
* 最后根据该地址对vPRGMemory写入数据
* 在一些基于RAM的Cartridge上是可以写入数据的
*/
bool Cartridge::cpuWrite(uint16_t addr, uint8_t data)
{
	uint32_t mapped_addr = 0;  // 保存转换后的地址，因为有些mapper可以提供给超过64KB数据，所以这里使用uint32_t类型的数据
	// 通过mapper转换CPU地址-》vPRGMemory地址
	if (pMapper->cpuMapWrite(addr, mapped_addr, data)) {
		// 地址转换成功，根据转换后的地址写入数据
		if (mapped_addr == 0xFFFFFFFF) {
			// 如果mapper返回该值，直接返回ture即可
			return true;
		}
		else {
			// 根据转换后的地址读取数据
			vPRGMemory[mapped_addr] = data;
		}
		return true;
	}

	return false;
}

/*
* PPU总线读写数据，映射的是PPU的0x0000-0x1FFF数据段的数据
* Cartidge将传进来的地址送进自身的mapper电路中，最终将 0x8000-0xFFFF 这个地址段的地址转换为 vCHRMemory 内存中真实的地址
* 最后根据该地址对data赋值，读取数据流程结束
*/
bool Cartridge::ppuRead(uint16_t addr, uint8_t& data)
{
	uint32_t mapped_addr = 0;  // 保存转换后的地址，因为有些mapper可以提供给超过64KB数据，所以这里使用uint32_t类型的数据
	// 通过mapper转换CPU地址-》vCHRMemory地址
	if (pMapper->ppuMapRead(addr, mapped_addr)) {
		// 地址转换成功，根据转换后的地址提供数据
		data = vCHRMemory[mapped_addr];

		return true;
	}

	return false;
}

/*
* PPU总线读写数据，映射的是PPU的0x0000-0x1FFF数据段的数据
* Cartidge将传进来的地址送进自身的mapper电路中，最终将 0x8000-0xFFFF 这个地址段的地址转换为 vCHRMemory 内存中真实的地址
* 最后根据该地址对vPRGMemory写入数据
* 在一些基于RAM的Cartridge上是可以写入数据的
*/
bool Cartridge::ppuWrite(uint16_t addr, uint8_t data)
{
	uint32_t mapped_addr = 0;  // 保存转换后的地址，因为有些mapper可以提供给超过64KB数据，所以这里使用uint32_t类型的数据
	// 通过mapper转换CPU地址-》vCHRMemory地址
	if (pMapper->ppuMapWrite(addr, mapped_addr)) {
		// 地址转换成功，根据转换后的地址写入数据
		vCHRMemory[mapped_addr] = data;

		return true;
	}

	return false;
}

/*
*  重置mapper的状态，不是重置游戏卡，这个方法调用mapper的reset函数
*/
void Cartridge::reset()
{
	// 调用mapper的reset方法
	if (pMapper != nullptr) {
		pMapper->reset();
	}
}

/*
* 返回游戏的镜像方式信息
*/
MIRROR Cartridge::Mirror()
{
	// 镜像方式可以由mapper动态指定
	// 查询mapper的镜像方式，如果是由硬件指定，那么返回文件中读取的镜像信息
	// 若不是硬件指定，那么返回mapper获取的镜像信息
	MIRROR mir = pMapper->mirror();
	if (mir == MIRROR::HARDWARE) {
		return hw_mirror;
	}

	return mir;
}

/*
* 返回对应的mapper指针
*/
std::shared_ptr<Mapper> Cartridge::GetMapper()
{
	return pMapper;
}
