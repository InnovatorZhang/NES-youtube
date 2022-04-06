#include"Mapper_000.h"

/*
* Mapper_000��Ĺ��캯��
* ֱ�ӵ��ø���Ķ�Ӧ������ʼ������
*/
Mapper_000::Mapper_000(uint8_t prgBanks, uint8_t chrBanks) :Mapper(prgBanks, chrBanks) 
{

}
/*
* ��������
*/
Mapper_000::~Mapper_000()
{
	// û�ж�̬���붯̬�ռ䣬���Ժ�����Ϊ��
}

/*
* �˺�������CPU��ȡ����ĵ�ַ�� 0x8000-0xFFFF ӳ�䵽Cartridge�ϵ�PRGMemory�ϵ���ʵ��ַ��
* 
*/
bool Mapper_000::cpuMapRead(uint16_t addr, uint32_t& mapped_addr, uint8_t& data)
{
	// �����Ϸ����PRG��ֻ��һ��,���ܴ�Сֻ��16KB
	// Mapper_000��ʹ�þ���������СΪ32KB�ĵ�ַ�� 0x8000-0xFFFF ӳ�䵽 0x0000-0x3FFF ��һ�δ�СΪ16KB��PRGMemory��
	// 0x8000 -> 0xBFFF => 0x0000 -> 3FFF
	// 0xC000 -> 0xFFFF => 0x0000 -> 3FFF
	// ���������
	// 0x8000 -> 0xFFFF => 0x0000 -> 7FFF
	if (addr >= 0x8000 && addr <= 0xFFFF) {
		// ����PRG��Ĵ�Сѡ��ӳ�䷽ʽ
		mapped_addr = addr & (nPRGBanks > 1 ? 0x7FFF : 0x3FFF);

		return true;
	}

	return false;
}

/*
* �˺�������CPUд������ĵ�ַ�� 0x8000-0xFFFF ӳ�䵽Cartridge�ϵ�PRGMemory�ϵ���ʵ��ַ��
*
*/
bool Mapper_000::cpuMapWrite(uint16_t addr, uint32_t& mapped_addr, uint8_t data)
{
	// �����Ϸ����PRG��ֻ��һ��,���ܴ�Сֻ��16KB
	// Mapper_000��ʹ�þ���������СΪ32KB�ĵ�ַ�� 0x8000-0xFFFF ӳ�䵽 0x0000-0x3FFF ��һ�δ�СΪ16KB��PRGMemory��
	// 0x8000 -> 0xBFFF => 0x0000 -> 3FFF
	// 0xC000 -> 0xFFFF => 0x0000 -> 3FFF
	// ���������
	// 0x8000 -> 0xFFFF => 0x0000 -> 7FFF
	if (addr >= 0x8000 && addr <= 0xFFFF) {
		// ����PRG��Ĵ�Сѡ��ӳ�䷽ʽ
		mapped_addr = addr & (nPRGBanks > 1 ? 0x7FFF : 0x3FFF);

		return true;
	}

	return false;
}

/*
* �˺�������PCPU��ȡ����ĵ�ַ�� 0x0000-0x1FFF ӳ�䵽Cartridge�ϵ�CHRMemory��ʵ��ַ��
*
*/
bool Mapper_000::ppuMapRead(uint16_t addr, uint32_t& mapped_addr)
{
	// ��ΪPPU�ĵ�ַ�οռ���CHRMemory��ַ�θպ��غ�
	// ���ֻҪ��Ѱַ��Χ�ڣ�ֱ�ӷ�����ͬ��ַ����
	if (addr >= 0x0000 && addr <= 0x1FFF) {
		mapped_addr = addr;

		return true;
	}
	return false;
}

/*
* �˺�������PCPUд������ĵ�ַ�� 0x0000-0x1FFF ӳ�䵽Cartridge�ϵ�CHRMemory��ʵ��ַ��
*
*/
bool Mapper_000::ppuMapWrite(uint16_t addr, uint32_t& mapped_addr)
{
	// ��ΪPPU�ĵ�ַ�οռ���CHRMemory��ַ�θպ��غ�
	// ���ֻҪ��Ѱַ��Χ�ڣ�ֱ�ӷ�����ͬ��ַ����
	if (addr >= 0x0000 && addr <= 0x1FFF) {
		// ���nCHRBANKΪ0�����ʾCartridge�������õ�RAM����˿���д������
		if (nCHRBanks == 0) {
			mapped_addr = addr;

			return true;
		}
	}
	return false;
}

/*
* Mapper_000�б���Ϊ�ռ���
*/
void Mapper_000::reset()
{
}
