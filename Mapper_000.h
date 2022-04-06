#pragma once

#include "Mapper.h"

class Mapper_000 :public Mapper {
public:
	Mapper_000(uint8_t prgBanks, uint8_t chrBanks);
	~Mapper_000();

public:
	// 复写父类的这几个纯虚函数
	virtual bool cpuMapRead(uint16_t addr, uint32_t& mapped_addr, uint8_t& data) override;
	virtual bool cpuMapWrite(uint16_t addr, uint32_t& mapped_addr, uint8_t data = 0) override;

	virtual bool ppuMapRead(uint16_t addr, uint32_t& mapped_addr) override;
	virtual bool ppuMapWrite(uint16_t addr, uint32_t& mapped_addr) override;

	// 复写一下reset虚函数
	virtual void reset() override;
};
