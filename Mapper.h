#pragma once

#include<cstdint>

// ��ʾ��Ϸӳ��name table�ķ�ʽ
enum MIRROR {
	HARDWARE,  // ��Ӳ��ָ�������ļ��ж�ȡ����Ϣָ��
	HORIZONTAL,  // ����
	VERTICAL,  // ����
	ONESCREEN_LO,
	ONESCREEN_HI
};

class Mapper {
public:
	Mapper(uint8_t prgBanks, uint8_t chrBanks);
	~Mapper();

public:
	// ��CPU�������ĵ�ַת��Ϊ��Ӧ��PRG ROM�ϵĵ�ֵַ��CPU��0x8000-0xFFFF��Mapper��ת��
	// ����һ�����麯����Ҫ������һ��Ҫʵ��һ���Լ��İ汾����һ������Ϊ��������CPU��ַ���ڶ���Ϊӳ���ĵ�ַ������������һЩMapper�в�����
	virtual bool cpuMapRead(uint16_t addr, uint32_t& mapped_addr, uint8_t& data) = 0;  
	virtual bool cpuMapWrite(uint16_t addr, uint32_t& mapped_addr, uint8_t data = 0) = 0;

	// ��PPU�������ĵ�ַת��Ϊ��Ӧ��PRG ROM�ϵĵ�ֵַ��PPU��0x0000-0x1FFF��Mapperת��
	// ����һ�����麯����Ҫ������һ��Ҫʵ��һ���Լ��İ汾����һ������Ϊ��������CPU��ַ���ڶ���Ϊӳ���ĵ�ַ
	virtual bool ppuMapRead(uint16_t addr, uint32_t& mapped_addr) = 0;
	virtual bool ppuMapWrite(uint16_t addr, uint32_t& mapped_addr) = 0;

	virtual void reset() = 0;  // ����Mapper״̬

	// �ṩһ�����ؾ���ģʽ�Ľӿ�,�麯����Mapper�ṩһ��Ĭ��ʵ�֣���������Լ�ѡ���Ƿ��д�����ö�̬��
	virtual MIRROR mirror();

	// �жϽӿ�
	virtual bool irqState();
	virtual void irqClear();  // ���ṩĬ��ʵ��

	// ɨ���߼���
	virtual void scanline();  // ���ṩĬ��ʵ��

protected:
	uint8_t nPRGBanks = 0;  // ����εĸ�����һ��16KB
	uint8_t nCHRBanks = 0;  // ͼ�ζεĸ�����һ��8KB
};