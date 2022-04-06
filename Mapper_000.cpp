#include"Mapper_000.h"

/*
* Mapper_000类的构造函数
* 直接调用父类的对应函数初始化即可
*/
Mapper_000::Mapper_000(uint8_t prgBanks, uint8_t chrBanks) :Mapper(prgBanks, chrBanks) 
{

}
/*
* 析构函数
*/
Mapper_000::~Mapper_000()
{
	// 没有动态申请动态空间，所以函数体为空
}

/*
* 此函数负责将CPU读取请求的地址段 0x8000-0xFFFF 映射到Cartridge上的PRGMemory上的真实地址段
* 
*/
bool Mapper_000::cpuMapRead(uint16_t addr, uint32_t& mapped_addr, uint8_t& data)
{
	// 如果游戏卡中PRG块只有一块,即总大小只有16KB
	// Mapper_000会使用镜像技术将大小为32KB的地址段 0x8000-0xFFFF 映射到 0x0000-0x3FFF 这一段大小为16KB的PRGMemory上
	// 0x8000 -> 0xBFFF => 0x0000 -> 3FFF
	// 0xC000 -> 0xFFFF => 0x0000 -> 3FFF
	// 如果有两块
	// 0x8000 -> 0xFFFF => 0x0000 -> 7FFF
	if (addr >= 0x8000 && addr <= 0xFFFF) {
		// 根据PRG块的大小选择映射方式
		mapped_addr = addr & (nPRGBanks > 1 ? 0x7FFF : 0x3FFF);

		return true;
	}

	return false;
}

/*
* 此函数负责将CPU写入请求的地址段 0x8000-0xFFFF 映射到Cartridge上的PRGMemory上的真实地址段
*
*/
bool Mapper_000::cpuMapWrite(uint16_t addr, uint32_t& mapped_addr, uint8_t data)
{
	// 如果游戏卡中PRG块只有一块,即总大小只有16KB
	// Mapper_000会使用镜像技术将大小为32KB的地址段 0x8000-0xFFFF 映射到 0x0000-0x3FFF 这一段大小为16KB的PRGMemory上
	// 0x8000 -> 0xBFFF => 0x0000 -> 3FFF
	// 0xC000 -> 0xFFFF => 0x0000 -> 3FFF
	// 如果有两块
	// 0x8000 -> 0xFFFF => 0x0000 -> 7FFF
	if (addr >= 0x8000 && addr <= 0xFFFF) {
		// 根据PRG块的大小选择映射方式
		mapped_addr = addr & (nPRGBanks > 1 ? 0x7FFF : 0x3FFF);

		return true;
	}

	return false;
}

/*
* 此函数负责将PCPU读取请求的地址段 0x0000-0x1FFF 映射到Cartridge上的CHRMemory真实地址段
*
*/
bool Mapper_000::ppuMapRead(uint16_t addr, uint32_t& mapped_addr)
{
	// 因为PPU的地址段空间与CHRMemory地址段刚好重合
	// 因此只要在寻址范围内，直接返回相同地址即可
	if (addr >= 0x0000 && addr <= 0x1FFF) {
		mapped_addr = addr;

		return true;
	}
	return false;
}

/*
* 此函数负责将PCPU写入请求的地址段 0x0000-0x1FFF 映射到Cartridge上的CHRMemory真实地址段
*
*/
bool Mapper_000::ppuMapWrite(uint16_t addr, uint32_t& mapped_addr)
{
	// 因为PPU的地址段空间与CHRMemory地址段刚好重合
	// 因此只要在寻址范围内，直接返回相同地址即可
	if (addr >= 0x0000 && addr <= 0x1FFF) {
		// 如果nCHRBANK为0，这表示Cartridge内有内置的RAM，因此可以写入数据
		if (nCHRBanks == 0) {
			mapped_addr = addr;

			return true;
		}
	}
	return false;
}

/*
* Mapper_000中保持为空即可
*/
void Mapper_000::reset()
{
}
