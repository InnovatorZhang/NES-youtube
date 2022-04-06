#include"Mapper_001.h"

Mapper_001::Mapper_001(uint8_t prgBanks, uint8_t chrBanks):Mapper(prgBanks, chrBanks)
{
	vRAMStatic.resize(32 * 1024);  // Mapper����32KB��RAM
}

Mapper_001::~Mapper_001()
{
}

bool Mapper_001::cpuMapRead(uint16_t addr, uint32_t& mapped_addr, uint8_t& data)
{
	if (addr >= 0x6000 && addr <= 0x7FFF) {
		// ��CPU�� 0x6000-0x7FFFӳ�䵽�Դ���RAM
		mapped_addr = 0xFFFFFFFF;  // ����Ϊ��ֵ������Cartridge��mapper����RAM������ֱ�Ӵ�mapper�ж�ȡ����

		// ��mapper����RAM�ж�ȡ����
		data = vRAMStatic[addr & 0x1FFF];  // ӳ�䵽8KB��ַ�ռ䣬

		return true;
	}
	else if (addr >= 0x8000) {
		if (nControlRegister & 0x08) {
			// mapper�ϵĿ��ƼĴ����ĵ�4λ������ӳ��ģʽ��16KBģʽ����32KBģʽ
			// 16KBģʽ
			if (addr >= 0x8000 && addr <= 0xBFFF) {
				mapped_addr = nPRGBankSelect16Lo * 0x4000 + (addr & 0x3FFF);
				return true;
			}
			else if (addr >= 0xC000 && addr <= 0xFFFF) {
				mapped_addr = nPRGBankSelect16Hi * 0x4000 + (addr & 0x3FFF);
				return true;
			}
		}
		else {
			// 32KBģʽ
			mapped_addr = nPRGBankSelect32 * 0x8000 + (addr & 0x7FFF);
			return true;
		}
	}

	return false;
}

bool Mapper_001::cpuMapWrite(uint16_t addr, uint32_t& mapped_addr, uint8_t data)
{
	if (addr >= 0x6000 && addr <= 0x7FFF) {
		// ���CPUд����һ���ڴ棬��ʾCPUд��mapper�д��е�RAM
		mapped_addr = 0xFFFFFFFF;  // ����Ϊ��ֵ������Cartridge��mapper����RAM������ֱ�Ӵ�mapper��д������

		// д�뵽RAM
		vRAMStatic[addr & 0x1FFF] = data;

		return true;
	}
	else if (addr >= 0x8000) {
		if (data & 0x80) {
			nLoadRegister = 0x00;
			nLoadRegisterCount = 0x00;
			nControlRegister |= 0x0C;
		}
		else {
			nLoadRegister >>= 1;
			nLoadRegister |= (data & 0x01) << 4;
			nLoadRegisterCount++;

			if (nLoadRegisterCount == 5) {
				// ͨ��addr��13��14λ��ȡĿ��Ĵ���
				uint8_t nTargetRegister = (addr >> 13) & 0x03;

				if (nTargetRegister == 0) {  // 0x8000 - 0x9FFF
					// ���ÿ��ƼĴ���
					nControlRegister = nLoadRegister & 0x1F;

					switch (nControlRegister & 0x03) {
					case 0:mirrormode = ONESCREEN_LO; break;
					case 1:mirrormode = ONESCREEN_HI; break;
					case 2:mirrormode = VERTICAL; break;
					case 3:mirrormode = HORIZONTAL; break;
					}
				}
				else if (nTargetRegister == 1) {  // 0xA000 - 0xBFFF
					if (nControlRegister & 0x10) {
						nCHRBankSelect4Lo = nLoadRegister & 0x1F;
					}
					else {
						nCHRBankSelect8 = nLoadRegister & 0x1E;
					}
				}
				else if (nTargetRegister == 2) {  // 0xC000 - 0xDFFF
					if (nControlRegister & 0x10) {
						nCHRBankSelect4Hi = nLoadRegister & 0x1F;
					}
				}
				else if (nTargetRegister == 3) {
					uint8_t nPRGMode = (nControlRegister >> 2) & 0x03;

					if (nPRGMode == 0 || nPRGMode == 1) {
						nPRGBankSelect32 = (nLoadRegister & 0x0E) >> 1;
					}
					else if (nPRGMode == 2) {
						nPRGBankSelect16Lo = 0;
						nPRGBankSelect16Hi = nLoadRegister & 0x0F;
					}
					else if (nPRGMode == 3) {
						nPRGBankSelect16Lo = nLoadRegister & 0x0F;
						nPRGBankSelect16Hi = nPRGBanks - 1;
					}
				}

				nLoadRegister = 0x00;
				nLoadRegisterCount = 0;
			}
		}
	}

	return false;
}

bool Mapper_001::ppuMapRead(uint16_t addr, uint32_t& mapped_addr)
{
	if (addr >= 0x0000 && addr <= 0x1FFF) {
		// ��ַ����һ��ʱ����nCHRBankΪ0ʱֱ�ӷ��ص�ǰ��ַ
		if (nCHRBanks == 0) {
			mapped_addr = addr;

			return true;
		}
		else {
			if (nControlRegister & 0x10) {
				// 4KB CHR BANKģʽ
				if (addr >= 0x0000 && addr <= 0x0FFF) {
					mapped_addr = nCHRBankSelect4Lo * 0x1000 + (addr & 0x0FFF);
					return true;
				}
				else if (addr >= 0x1000 && addr <= 0x1FFF) {
					mapped_addr = nCHRBankSelect4Hi * 0x1000 + (addr & 0x0FFF);
					return true;
				}
			}
			else {
				// 8KB CHR BANKģʽ
				mapped_addr = nCHRBankSelect8 * 0x2000 + (addr & 0x1FFF);
				return true;
			}
		}
	}

	return false;
}

bool Mapper_001::ppuMapWrite(uint16_t addr, uint32_t& mapped_addr)
{
	if (addr >= 0x0000 && addr <= 0x1FFF) {
		if (nCHRBanks == 0) {
			mapped_addr = addr;
			return true;
		}
		return true;
	}

	return false;
}



void Mapper_001::reset()
{
	nControlRegister = 0x1C;
	nLoadRegister = 0x00;
	nLoadRegisterCount = 0x00;

	nCHRBankSelect4Lo = 0;
	nCHRBankSelect4Hi = 0;
	nCHRBankSelect8 = 0;

	nPRGBankSelect32 = 0;
	nPRGBankSelect16Lo = 0;
	nPRGBankSelect16Hi = nPRGBanks - 1;
}

MIRROR Mapper_001::mirror()
{

	return mirrormode;
}
