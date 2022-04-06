#include"Mapper_001.h"

Mapper_001::Mapper_001(uint8_t prgBanks, uint8_t chrBanks):Mapper(prgBanks, chrBanks)
{
	vRAMStatic.resize(32 * 1024);  // Mapper存在32KB的RAM
}

Mapper_001::~Mapper_001()
{
}

bool Mapper_001::cpuMapRead(uint16_t addr, uint32_t& mapped_addr, uint8_t& data)
{
	if (addr >= 0x6000 && addr <= 0x7FFF) {
		// 将CPU的 0x6000-0x7FFF映射到自带的RAM
		mapped_addr = 0xFFFFFFFF;  // 设置为该值，告诉Cartridge，mapper中有RAM，所以直接从mapper中读取数据

		// 从mapper带的RAM中读取数据
		data = vRAMStatic[addr & 0x1FFF];  // 映射到8KB地址空间，

		return true;
	}
	else if (addr >= 0x8000) {
		if (nControlRegister & 0x08) {
			// mapper上的控制寄存器的第4位，控制映射模式是16KB模式还是32KB模式
			// 16KB模式
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
			// 32KB模式
			mapped_addr = nPRGBankSelect32 * 0x8000 + (addr & 0x7FFF);
			return true;
		}
	}

	return false;
}

bool Mapper_001::cpuMapWrite(uint16_t addr, uint32_t& mapped_addr, uint8_t data)
{
	if (addr >= 0x6000 && addr <= 0x7FFF) {
		// 如果CPU写入这一段内存，表示CPU写入mapper中带有的RAM
		mapped_addr = 0xFFFFFFFF;  // 设置为该值，告诉Cartridge，mapper中有RAM，所以直接从mapper中写入数据

		// 写入到RAM
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
				// 通过addr的13和14位获取目标寄存器
				uint8_t nTargetRegister = (addr >> 13) & 0x03;

				if (nTargetRegister == 0) {  // 0x8000 - 0x9FFF
					// 设置控制寄存器
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
		// 地址在这一段时，且nCHRBank为0时直接返回当前地址
		if (nCHRBanks == 0) {
			mapped_addr = addr;

			return true;
		}
		else {
			if (nControlRegister & 0x10) {
				// 4KB CHR BANK模式
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
				// 8KB CHR BANK模式
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
