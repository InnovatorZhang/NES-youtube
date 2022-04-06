#include"Mapper.h"

/*
* Mapper类的构造函数
* 第一个参数表示程序段的数量，每个段大小为16KB
* 第二个参数表示图像段的数量，每个段大小为8KB
*/
Mapper::Mapper(uint8_t prgBanks, uint8_t chrBanks)
{
	nPRGBanks = prgBanks;
	nCHRBanks = chrBanks;

	// 初始化时reset以下
	reset();
}

/*
* 析构函数
*/
Mapper::~Mapper()
{
	// 没有动态申请动态空间，所以函数体为空
}

/*
* 虽然是纯虚函数，但提供一个空实现
*/
void Mapper::reset() {

}

/*
* 返回name table的镜像方式，默认由硬件指定，也就是由nes文件头中的信息指定
*/
MIRROR Mapper::mirror()
{
	return MIRROR::HARDWARE;
}

/*
* 中断接口
*/
bool Mapper::irqState()
{
	// 默认返回false
	return false;
}
/*
* 作为基类，未提供默认实现
*/
void Mapper::irqClear()
{
}

/*
* 作为基类，未提供默认实现
*/
void Mapper::scanline()
{
}
