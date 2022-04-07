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
	std::shared_ptr<Cartridge> cart;  // ��Ϸ��
	uint8_t cpuRam[2 * 1024];  // 2KB��cpu�ڴ�
	uint8_t controller[2];  // ��ַΪ0x4016��0x4017�������Ĵ������ֱ��Ӧ�����ֱ�

public:  // ��¶��CPU�ķ���
	void cpuWrite(uint16_t addr, uint8_t data);  // cpuͨ�����߽���д���ݵķ���
	uint8_t cpuRead(uint16_t addr, bool bReadOnly = false);  // cpuͨ�����߽��ж����ݵķ���

private:
	uint32_t nSystemClockCounter = 0; // ͳ��һ�������˶��ٸ�ʱ������
	uint8_t controller_state[2];  // ��Ϊ����controller�Ĵ�����ֵ��cache

private:
	// DMA����������������ٽ�CPU�ڴ������ݴ��䵽PPU��Object Attribute Memory��
	// �����ͨ��DMA���䣬����ֱ��ͨ��CPU��PPUָ���Ĵ���д���ݣ���ô�ٶ�̫��
	// ����ֱ��ͨ��DMA�ķ�ʽ��һ���Դ�256���ֽ����ݵ�PPUObject Attribute Memory�У�ֻ��Ҫ����512CPUʱ������
	// �����512��ʱ�������У�CPU���ᱻ����ȴ���ֱ�����ݴ�����
	// ÿ�θպô���һ��page(256�ֽ�)��С�����ݣ��������ｫpage����byte�ŷֿ��洢����ͬ�γ�һ��16�ֽڵ�ַ

	uint8_t dma_page = 0x00;  // DMA�����page��
	uint8_t dma_addr = 0x00;  // ָ��page�е�ƫ����
	uint8_t dma_data = 0x00;  // ���������

	// DMA������Ҫ׼ȷ��ʱ�����ڣ�һ�δ�����Ҫ512��ʱ������ȥ��д256�ֽ�CPU ram�е����ݵ�PPU oam�У�
	// һ�ζ�ȡ���������һ��д��CPU������ż��ʱ�����ڶ�������ʱ��д��������һ��dma_dummy����������
	// �ñ������Ա�֤ÿ���Ǵ�ż��ʱ�����ڿ�ʼ������
	bool dma_dummy = true;

	// ����һ����������ʾdma���䷢���ˣ���Ҫ��CPU����
	bool dma_transfer = false;

public: // ϵͳ���ýӿ�
	void insertCartridge(const std::shared_ptr<Cartridge>& cartridge);  // ����Ϸ�����ӵ�������
	void reset();  // ����ϵͳ
	void clock(); // ʱ�ӵδ�
};