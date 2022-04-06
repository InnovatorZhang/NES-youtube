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
	Cartridge(const std::string& sFileName);  // ָ���ļ����Ĺ��캯��
	~Cartridge();

	bool ImageValid();  // �����ļ��Ƿ��ȡ�ɹ���״̬�����ļ���ȡ�ɹ�Ϊtrue����֮��false

private:
	bool bImageValid = false;  // ָʾ�ļ��Ƿ��ȡ�ɹ�
	MIRROR hw_mirror = HORIZONTAL;  // Ĭ��ֵ����ΪMapper���ж����ö�ٱ���HORIZONTAL

	uint8_t nMapperID = 0;
	uint8_t nPRGBanks = 0;  // ָʾ�ж��ٸ�����Σ�ÿ����Ϊ16KB
	uint8_t nCHRBanks = 0;  // ָʾ�ж��ٸ�ͼ��Σ�ÿ����Ϊ8KB

	std::vector<uint8_t> vPRGMemory;  // �����ļ��ж�ȡ������PRG������
	std::vector<uint8_t> vCHRMemory;  // �����ļ��ж�ȡ������CHR������

	std::shared_ptr<Mapper> pMapper;  // mapperָ�룬���ö�̬������ָ�����Mapper���࣬��̬���麯��

public:
	// CPU���߶�д���ݣ�ӳ�����CPU��0x8000-0xFFFF���ݶε�����
	bool cpuRead(uint16_t addr, uint8_t& data);
	bool cpuWrite(uint16_t addr, uint8_t data);

	// PPU���߶�д���ݣ�ӳ�����PPU��0x0000-0x1FFF���ݶε�����
	bool ppuRead(uint16_t addr, uint8_t& data);
	bool ppuWrite(uint16_t addr, uint8_t data);

	void reset(); // ����mapper��״̬������������Ϸ���������������mapper��reset����

	MIRROR Mirror();  // ����Mirror��Ϣ�Ľӿ�

	std::shared_ptr<Mapper> GetMapper();  // ����Mapperָ��Ľӿ�
};
