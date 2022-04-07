#include "PPU2C02.h"

/*
* ���캯��
* �ڹ��캯���г�ʼ��nes��֧�ֵ�������ɫ
* ��Ȼ��NES�ĵ�ɫ��(palette)��ʹ��һ���ֽ�ָʾʹ�õ���ɫ����һ��֧�ֵ���ɫȴֻ��52�֣���Ϊ�ظ���һЩ��ɫ  https://www.jianshu.com/p/c5b02614e4a7
*/
PPU2C02::PPU2C02(uint8_t scale)
{
	// ��ʼ��������Ļ,ģ���nes��240x256�ֱ��ʵ���ʾ����ע�⣺�����Ƿ��Ŷ���ģ����Ը�ֵʱ��Ϊ�ڶ�λ����Ϊ��һά
	// ����������Ϊ���ش�С
	m_emulatorScreen.create(VirtualScreen::NESVideoWidth, VirtualScreen::NESVideoHeight, scale, sf::Color::White);
}

/*
* ��������
*/
PPU2C02::~PPU2C02()
{
	// û�ж�̬���붯̬�ռ䣬���Ժ�����Ϊ��
}


/*
* �������Ա�attribute table���е�2��bit + ͼ����pattern table����2��bit��϶��ɵ�ɫ��������ȷ����ǰ���ص���ɫ  https://www.jianshu.com/p/c5b02614e4a7
* ������ɫ��ɫ���0x3F00��ʼ����16����Ŀ
* ������ɫ��ɫ���0x3F10��ʼ����16����Ŀ
*/
sf::Color PPU2C02::GetColourFromPaletteRam(uint8_t palette, uint8_t pixel)
{
	// palette������bit0��bit1�������Ա��е�2��bit������palette�����ĸ�2λ;palette��bit3��ʾѡ��0x3F00��ʼ�ı�����ɫ�廹��0x3F10��ʼ�ľ����ɫ��
	// pixel������bit0��bit1����ͼ�����е�2��bit������palette�����ĵ�2λ
	// ��������������������ܹ�������ɫ���е�һ��32��Ŀ��
	// ����ָ����ͣ�
	// 0x3F00��ʾ��ɫ��Ӹõ�ַ��ʼ
	// (palette << 2) + pixel ��ȡ��ɫ����
	// ppuRead() ��ȡ������Ŀ���ַ�е�palScreen��ɫ����
	// & 0x3F ��ֹԽ�磬��ΪpalScreenֻ��64����Ŀ
	// palScreen[]�õ����յ�������ɫ
	return sf::Color(nes_colors[ppuRead(0x3F00 + (palette << 2) + pixel) & 0x3F]);
}

/*
* // �ӿڣ���ȡ��Ļ��ʾ
*/
VirtualScreen& PPU2C02::GetVirtualScreen()
{
	return m_emulatorScreen;
}

/*
* ������ģ�����CPU��PPU�е�8���Ĵ����ķ���
* Ѱַ��ΧΪ 0x00-0x07 �ֱ��ʾ8���Ĵ���
*/
uint8_t PPU2C02::cpuRead(uint16_t addr, bool rdonly)
{
	uint8_t data = 0x00;  // ��ʼ�����ر���

	switch (addr) {
		// Control�Ĵ��������ܱ�CPU��
	case 0x0000: 
		break;

		// Mask�Ĵ��������ܱ�CPU��
	case 0x0001: 
		break;

		// Status�Ĵ���
	case 0x0002: 
		// ״̬�Ĵ����б���ֻ�����3λ�������õģ�����һЩ��ϷҲʹ��������5λ
		// ����5λ���ϴ�һppu����ͣ�����������
		data = (status.reg & 0xE0) | (ppu_data_buffer & 0x1F);  // �ֱ�ȡ��status�е����3λ��ppu_data_buffer�ĵ�5λ

		// ����status�Ĵ����е�vertical_blank״̬λΪ0
		status.vertical_blank = 0;

		// ��ȡ�Ĵ��������address_latch״̬λ����Ϊ��·��������Ƶ�
		break;

		// OAM��Object Attribute Memory�������ַ�ļĴ��������ܱ�CPU��
	case 0x0003: 
		break;

		// OAM��Object Attribute Memory���������ݵļĴ���
	case 0x0004:
		// ����oam_addr��ַָ��������
		data = pOAM[oam_addr];
		break;

		// Scoll�Ĵ��������ܱ�CPU��
	case 0x0005: 
		break;

		// PPU ��ַ�Ĵ��������ܱ�CPU��ȡ
	case 0x0006: 
		break;

		// PPU ���ݼĴ���
	case 0x0007:
		// �����ȡ����������name table����ô��ȡ���Ƴ�һ��ʱ������
		// ���Ӧ������һ��read����ָ��������
		data = ppu_data_buffer;  // ��ȡ��һ�ε�����
		// ��ʱ������һ�ε����ݵ�buffer��,��ȡ���ݵĵ�ַ��vram_addr.regָ��
		ppu_data_buffer = ppuRead(vram_addr.reg);

		// �������ȡ���������Ե�ɫ��(paettle)����ô�����Ƴ�һ��ʱ�����ڣ�ֱ�ӷ��ص�ǰ����
		if (vram_addr.reg >= 0x3F00) {
			data = ppu_data_buffer;
		}

		// ����control�Ĵ����Ķ�ȡģʽλ��increment_mode����ÿ�ζ�ȡ�ĵ�ַ���Զ���������
		// �����ˮƽ����ģʽ����ַֻ����1λ���ƶ�����һ�У�������ֱ����ģʽ��ÿһ������32λ����Ϊ��Ҫ����һ����name table
		vram_addr.reg += (control.increment_mode ? 32 : 1);
		break;
	}

	return data;
}

/*
* ������ģ�����CPU��PPU�е�8���Ĵ����ķ���
* Ѱַ��ΧΪ 0x00-0x07 �ֱ��ʾ8���Ĵ���
*/
void PPU2C02::cpuWrite(uint16_t addr, uint8_t data)
{
	switch (addr)
	{
		// control�Ĵ���
	case 0x0000:
		// ������д��control�Ĵ���
		control.reg = data;
		// ����cpuд�����������tram_addr����������ָ������д��4��name table�е�ĳһ��
		tram_addr.nametable_x = control.nametable_x;
		tram_addr.nametable_y = control.nametable_y;
		break;

		// Mask�Ĵ���
	case 0x0001:
		// ������д��mask�Ĵ���
		mask.reg = data;
		break;

		// status�Ĵ���,cpu����ֱ����status�Ĵ���д������
	case 0x0002:
		break;

		// OAM��Object Attribute Memory�������ַ�ļĴ���
	case 0x0003:
		// ���OAMֻ��256�ֽڣ����Ե�ַֻ��Ҫ8λ����dataֱ�Ӹ�ֵ��oam_addr��ַ��������
		oam_addr = data;
		break;

		// OAM��Object Attribute Memory���������ݵļĴ���
	case 0x0004:
		// ��oam_addrָ���OAM�ڴ��ַд������
		pOAM[oam_addr] = data;
		break;

		// scoll�Ĵ�����������Ļ�Ĺ���
	case 0x0005:
		// ��Ϊ����һ����16λ��������Ҫ��Ϊ���ν��д��䣬ͨ��address_latch������ʶ���ǵڼ��δ���
		if (address_latch == 0) {
			// address_latch����0ʱһ���ǵ�һ�δ���
			// ��һ�δ����д����Ļ��������ص���Ϣ
			fine_x = data & 0x07;  // fine_x����ÿ��8x8tile�еľ��������,��ΧΪ0-7
			tram_addr.coarse_x = data >> 3;  // coarse_xָʾ�Ǻ����32��tile�е���һ������ΧΪ0-31
			address_latch = 1;  // ����Ϊ1����������
		}
		else {
			// ��ʱ�ǵڶ��δ��䣬�ڶ��δ����д����Ļ��������ص���Ϣ
			tram_addr.fine_y = data & 0x07;  // fine_y����ÿ��8x8tile�еľ��������,��ΧΪ0-7
			tram_addr.coarse_y = data >> 3;  // coarse_xָʾ�������32��tile�е���һ������ΧΪ0-31
			address_latch = 0;  // ����Ϊ0
		}
		break;

		// PPU ��ַ����Ĵ���
	case 0x0006:
		// ��ַһ��16λ�����Է����δ��䣬ʹ��address_latch��������
		if (address_latch == 0) {
			// ��һ�δ�����ֽ�,�ȱ�����tram_addr.reg�����У����������ٸ�ֵ��vram_addr.reg
			tram_addr.reg = (uint16_t)((data & 0x3F) << 8) | (tram_addr.reg & 0x00FF);  // ��ߵ���һλû�����ã����� &��0x3F��ע�⣺�����ͬʱ����tram_addr.reg�ĵ�8λ����
			address_latch = 1;  // ����Ϊ1����������
		}
		else {
			// �ڶ��δ�����ֽ�,
			tram_addr.reg = (tram_addr.reg & 0xFF00) | data;  // ����8λ��ֵ
			vram_addr = tram_addr;  // �������ݴ�����ɣ���tram_addr���ݸ�ֵ��vram_addr
			address_latch = 0;  // ����Ϊ0
		}
		break;

		// PPU ���ݴ���Ĵ���
	case 0x0007:
		// ��vram_addrָ���ĵ�ַд�����ݣ�����name table��д������
		ppuWrite(vram_addr.reg, data);
		// ����control�Ĵ����Ķ�ȡģʽλ��increment_mode����ÿ�ζ�ȡ�ĵ�ַ���Զ���������
		// �����ˮƽ����ģʽ����ַֻ����1λ���ƶ�����һ�У�������ֱ����ģʽ��ÿһ������32λ����Ϊ��Ҫ����һ����name table
		vram_addr.reg += (control.increment_mode ? 32 : 1);
		break;
	}
}

/*
* �˺���ģ���ʱPPU�����ϵĶ�ȡ
* ����PPU�������϶�ȡ���ݵķ������÷�����PPU�������ߵĲ�ͬ��ַ���룬ת��Ϊ��Ӧ���������ӵ��豸�ĵ�ַ
* �ڱ�ģ�����У�û��CPU������һ����ΪPPU���ߵ������һ���࣬����ֱ����Ƴ�һ������Ƕ����PPU����
* ͬ����ƵĻ��� tblName tblPalette �������ڴ��ַ����ģ����ѡ��ֱ�ӽ��������ڴ�Ƕ����PPU����
* PPU��Ѱַ�ռ�һ����16KB
* ppuRead��������ppu�����ж�����ת��Ϊ��ӦPPU�ڴ��ϵĵ�ַ
*/
uint8_t PPU2C02::ppuRead(uint16_t addr, bool rdonly)
{
	uint8_t data = 0x00;  // ��ʼ�����ر���
	addr &= 0x3FFF;  // ����ַ��Χ������0x0000-0x3FFF�䣬Ҳ����16KB�ڣ���ΪPPU��Ѱַ�ռ�һ����16KB

	if (addr >= 0x0000 && addr <= 0x1FFF) {
		// ����ӳ�����PPU�� 0x0000-0x1FFF ��һ�ε�ַ�ռ�,ÿ��PPUд����һ�ε�ַ�ռ��ϵĵ�ַʱ������ӳ�䵽cartridge�豸��ȥ
		// ����������cartridge�������ﴫ���ʱdata������
		cart->ppuRead(addr, data);
	}
	else if (addr >= 0x2000 && addr <= 0x3EFF) {
		// ����ӳ�����PPU�� 0x2000-0x3EFF ��һ�ε�ַ�ռ�
		// ʵ�ʵ�name tableֻ��2KB��С����ʵ������0x2000-0x3EFF��ô��һ�ε�ַ�ռ�ӳ�䵽��2KB�ڴ���
		// ���Ƚ� 0x2000-0x3EFF ӳ��Ϊ4KB��С
		// Ȼ����Ҫ������Ļ�Ĳ�ͬ����ʽ��������4KB��С���������ӳ�䵽2KB�Ŀռ���ȥ������ֻ��2��1KB��С��name table��ͨ������ķ�ʽ��װ��4��name table
		// ����ͨ�� &���� �� 0x0FFF ��ȡӳ����ֵ��ӳ�䵽4KB�ռ�
		addr &= 0x0FFF;

		// ������Ļ�ľ���ʽ����4KB�ռ�ӳ�䵽2KB��ʵ�ʿռ���
		if (cart->Mirror() == MIRROR::VERTICAL) {
			// �������ģʽΪ��ֱ,��ֱ��L1��L3ӳ�䵽��һ���������Ʊ���L2��L4ӳ�䵽�ڶ����������Ʊ�����ͼ��ʾ����Ļ����ʾΪ
			// 1 2
			// 1 2
			if (addr >= 0x0000 && addr <= 0x03FF) {
				data = tblName[0][addr & 0x03FF];
			}
			else if (addr >= 0x0400 && addr <= 0x07FF) {
				data = tblName[1][addr & 0x03FF];
			}
			else if (addr >= 0x0800 && addr <= 0x0BFF) {
				data = tblName[0][addr & 0x03FF]; // ��ֱ����ģʽ�µ�3KBӳ�䵽��1KB
			}
			else if (addr >= 0x0C00 && addr <= 0x0FFF) {
				data = tblName[1][addr & 0x03FF];  // ��ֱ����ģʽ�µ�4KBӳ�䵽��2KB
			}
		}
		else if (cart->Mirror() == MIRROR::HORIZONTAL) {
			// �������ģʽΪˮƽ,ˮƽ����L1��L2ӳ�䵽��һ���������Ʊ���L3��L4ӳ�䵽�ڶ����������Ʊ�����ͼ��ʾ����Ļ����ʾΪ
			// 1 1
			// 2 2
			if (addr >= 0x0000 && addr <= 0x03FF) {
				data = tblName[0][addr & 0x03FF];
			}
			else if (addr >= 0x0400 && addr <= 0x07FF) {
				data = tblName[0][addr & 0x03FF];  // ˮƽ����ģʽ�µ�2KBӳ�䵽��1KB
			}
			else if (addr >= 0x0800 && addr <= 0x0BFF) {
				data = tblName[1][addr & 0x03FF]; 
			}
			else if (addr >= 0x0C00 && addr <= 0x0FFF) {
				data = tblName[1][addr & 0x03FF];  // ˮƽ����ģʽ�µ�4KBӳ�䵽��2KB
			}
		}
	}
	else if (addr >= 0x3F00 && addr <= 0x3FFF) {
		// ����ӳ�����PPU�� 0x3F00-0x3FFF ��һ�ε�ַ�ռ�,����һ�δ�С�Ŀռ�ӳ�䵽 32�ֽ� ��
		// �� 0x3F00-0x3FFF =�� 0x0000-0x001F
		// �����Ӧ����PPU�� 32�ֽ� �ĵ�ɫ�����ݣ�����ÿһ���ֽڱ����� NES �в�ͬ��ɫ��������һ��64�����������������52����ɫ����Ϊ���ظ�
		// ͬʱ��Ϊÿ�ĸ��ֽڵ�ɫ�嶼���ظ�������ɫ�������Ļ�����ͬʱ����25�ֲ�ͬ��ɫ��Ҳ����˵�� 32�ֽ� �������25�ֲ�ͬ����ɫ����  32-(32/4)+1 = 25

		// ���Ƚ���ַӳ�䵽 0x0000-0x001F��
		addr &= 0x001F;

		if (addr == 0x0010) {
			addr = 0x0000;
		}
		else if (addr == 0x0014) {
			addr = 0x0004;
		}
		else if (addr == 0x0018) {
			addr = 0x0008;
		}
		else if (addr == 0x001C) {
			addr = 0x000C;
		}

		// �ӵ�ɫ���л�ȡ��������ͬʱ��������һ��������ı���ɫ��ʾЧ��������Ӧ��Ҳû����
		data = tblPalette[addr] & (mask.grayscale ? 0x30 : 0x3F);
	}

	return data;
}

/*
* �˺���ģ���ʱPPU�����ϵĶ�ȡ
* ����PPU��������д�����ݵķ������÷�����PPU�������ߵĲ�ͬ��ַ���룬ת��Ϊ��Ӧ���������ӵ��豸�ĵ�ַ
* �ڱ�ģ�����У�û��CPU������һ����ΪPPU���ߵ������һ���࣬����ֱ����Ƴ�һ������Ƕ����PPU����
* ͬ����ƵĻ��� tblName tblPalette �������ڴ��ַ����ģ����ѡ��ֱ�ӽ��������ڴ�Ƕ����PPU����
* PPU��Ѱַ�ռ�һ����16KB
* ppuWrite��������ppu������д����ת��Ϊ��ӦPPU�ڴ��ϵĵ�ַ
*/
void PPU2C02::ppuWrite(uint16_t addr, uint8_t data)
{
	addr &= 0x3FFF;  // ����ַ��Χ������0x0000-0x3FFF�䣬Ҳ����16KB�ڣ���ΪPPU��Ѱַ�ռ�һ����16KB

	if (addr >= 0x0000 && addr <= 0x1FFF) {
		// ����ӳ�����PPU�� 0x0000-0x1FFF ��һ�ε�ַ�ռ�,ÿ��PPUд����һ�ε�ַ�ռ��ϵĵ�ַʱ������ӳ�䵽cartridge�豸��ȥ
		// ����������cartridge����
		// ʵ����PPUӦ�ò���дCHRMemory�е����ݣ�����Cartridge����һ����RAM��������ROM
		cart->ppuWrite(addr, data);
	}
	else if (addr >= 0x2000 && addr <= 0x3EFF) {
		// ����ӳ�����PPU�� 0x2000-0x3EFF ��һ�ε�ַ�ռ�
		// ʵ�ʵ�name tableֻ��2KB��С����ʵ������0x2000-0x3EFF��ô��һ�ε�ַ�ռ�ӳ�䵽��2KB�ڴ���
		// ���Ƚ� 0x2000-0x3EFF ӳ��Ϊ4KB��С
		// Ȼ����Ҫ������Ļ�Ĳ�ͬ����ʽ��������4KB��С���������ӳ�䵽2KB�Ŀռ���ȥ������ֻ��2��1KB��С��name table��ͨ������ķ�ʽ��װ��4��name table
		// ����ͨ�� &���� �� 0x0FFF ��ȡӳ����ֵ��ӳ�䵽4KB�ռ�
		addr &= 0x0FFF;

		// ������Ļ�ľ���ʽ����4KB�ռ�ӳ�䵽2KB��ʵ�ʿռ���
		if (cart->Mirror() == MIRROR::VERTICAL) {
			// �������ģʽΪ��ֱ,��ֱ��L1��L3ӳ�䵽��һ���������Ʊ���L2��L4ӳ�䵽�ڶ����������Ʊ�����ͼ��ʾ����Ļ����ʾΪ
			// 1 2
			// 1 2
			if (addr >= 0x0000 && addr <= 0x03FF) {
				tblName[0][addr & 0x03FF] = data;
			}
			else if (addr >= 0x0400 && addr <= 0x07FF) {
				tblName[1][addr & 0x03FF] = data;
			}
			else if (addr >= 0x0800 && addr <= 0x0BFF) {
				tblName[0][addr & 0x03FF] = data; // ��ֱ����ģʽ�µ�3KBӳ�䵽��1KB
			}
			else if (addr >= 0x0C00 && addr <= 0x0FFF) {
				tblName[1][addr & 0x03FF] = data;  // ��ֱ����ģʽ�µ�4KBӳ�䵽��2KB
			}
		}
		else if (cart->Mirror() == MIRROR::HORIZONTAL) {
			// �������ģʽΪˮƽ,ˮƽ����L1��L2ӳ�䵽��һ���������Ʊ���L3��L4ӳ�䵽�ڶ����������Ʊ�����ͼ��ʾ����Ļ����ʾΪ
			// 1 1
			// 2 2
			if (addr >= 0x0000 && addr <= 0x03FF) {
				tblName[0][addr & 0x03FF] = data;
			}
			else if (addr >= 0x0400 && addr <= 0x07FF) {
				tblName[0][addr & 0x03FF] = data;  // ˮƽ����ģʽ�µ�2KBӳ�䵽��1KB
			}
			else if (addr >= 0x0800 && addr <= 0x0BFF) {
				tblName[1][addr & 0x03FF] = data;
			}
			else if (addr >= 0x0C00 && addr <= 0x0FFF) {
				tblName[1][addr & 0x03FF] = data;  // ˮƽ����ģʽ�µ�4KBӳ�䵽��2KB
			}
		}
	}
	else if (addr >= 0x3F00 && addr <= 0x3FFF) {
		// ����ӳ�����PPU�� 0x3F00-0x3FFF ��һ�ε�ַ�ռ�,����һ�δ�С�Ŀռ�ӳ�䵽 32�ֽ� ��
		// �� 0x3F00-0x3FFF =�� 0x0000-0x001F
		// �����Ӧ����PPU�� 32�ֽ� �ĵ�ɫ�����ݣ�����ÿһ���ֽڱ����� NES �в�ͬ��ɫ��������һ��64�����������������52����ɫ����Ϊ���ظ�
		// ͬʱ��Ϊÿ�ĸ��ֽڵ�ɫ�嶼���ظ�������ɫ�������Ļ�����ͬʱ����25�ֲ�ͬ��ɫ��Ҳ����˵�� 32�ֽ� �������25�ֲ�ͬ����ɫ����  32-(32/4)+1 = 25

		// ���Ƚ���ַӳ�䵽 0x0000-0x001F��
		addr &= 0x001F;

		if (addr == 0x0010) {
			addr = 0x0000;
		}
		else if (addr == 0x0014) {
			addr = 0x0004;
		}
		else if (addr == 0x0018) {
			addr = 0x0008;
		}
		else if (addr == 0x001C) {
			addr = 0x000C;
		}

		// ���ɫ����д����������
		tblPalette[addr] = data;
	}
}

/*
* ���ӵ���Ϸ������Ϊpattern table��8KB�ռ�����Ϸ����
* ��ָ��ָ����Ϸ��
*/
void PPU2C02::ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge)
{
	this->cart = cartridge;
}

/*
* ����PPU״̬��һ��ȷ��״̬
*/
void PPU2C02::reset()
{
	fine_x = 0x00;
	address_latch = 0x00;
	ppu_data_buffer = 0x00;
	scanline = 0;
	cycle = 0;
	bg_next_tile_id = 0x00;
	bg_next_tile_attrib = 0x00;
	bg_next_tile_lsb = 0x00;
	bg_next_tile_msb = 0x00;
	bg_shifter_pattern_lo = 0x0000;
	bg_shifter_pattern_hi = 0x0000;
	bg_shifter_attrib_lo = 0x0000;
	bg_shifter_attrib_hi = 0x0000;
	status.reg = 0x00;
	mask.reg = 0x00;
	control.reg = 0x00;
	vram_addr.reg = 0x0000;
	tram_addr.reg = 0x0000;
}

/*
* PPU��ʱ�Ӳ���
* ������PPUÿ��ʱ�����Ĳ���
*/
void PPU2C02::clock()
{
	// ���ȶ��弸��lambda���ʽ���ֱ���ͬ�Ĳ���

	// ��������ƶ�����ǹ����ͳ���ӻ������أ��ĺ������˺�������8x8���ؿ����ƶ�
	auto IncrementScrollX = [&]() {
		// ������Ʊ�������mask�Ĵ����� render_backgroundλ�� render_spriteλ Ϊ1���Ǿ��ƶ�����һ����
		if (mask.render_background || mask.render_sprites) {
			// һ��name table��30x32��8x8��
			// �ں����ƶ�ʱ�����ƶ�����һ��name table������ 0���ƶ��� 1�ţ�Ҳ���ܴ� 1���ƶ��� 0��,�����ͨ��vram�����е�nametable_x��������
			if (vram_addr.coarse_x == 31) {
				// �ƶ�����һ��name table��ȥ
				vram_addr.coarse_x = 0;  // ���¼���coarse_x
				vram_addr.nametable_x = ~vram_addr.nametable_x;  // ���� 0���ƶ��� 1�ţ�Ҳ���ܴ� 1���ƶ��� 0��
			}
			else {
				// ֻ���ڱ���ͬһ��name table���ƶ�
				vram_addr.coarse_x++;
			}
		}
	};

	// ���������ƶ�����ǹ����ͳ���ӻ������أ��ĺ������˺������ƻ������̵������ƶ�
	auto IncrementScrollY = [&]() {
		// ���������ƶ�����ǹ�������ƶ��������һЩ����Ϊname table������������8x8�鱻������attribute table
		// һ��name table��30x32��8x8��
		// ���������ÿһ�����ض������ڴ˴���vram_addr.fine_y��������

		// ������Ʊ�������mask�Ĵ����� render_backgroundλ�� render_spriteλ Ϊ1���ǾͿ�ʼ�����ƶ�
		if (mask.render_background || mask.render_sprites) {
			// ������겻��Խ��һ��8x8�飬��ôֻ������vram.fine_yֵ
			if (vram_addr.fine_y < 7) {
				vram_addr.fine_y++;
			}
			else {
				// ������꼴��������һ�������8x8�飬��ô��Ҫ����vram_addr.coarse_y���ƶ�

				vram_addr.fine_y = 0;  // ����ָ����һ�������ʼλ��

				// �������ƶ�ʱ�����ƶ�����һ��name table������ 0���ƶ��� 1�ţ�Ҳ���ܴ� 1���ƶ��� 0��,�����ͨ��vram�����е�nametable_y��������
				// ע�⣺����ֻ��30��8x8��
				if (vram_addr.coarse_y == 29) {
					// ��Ҫ������һ��name table
					vram_addr.coarse_y = 0;  // ���¼���coarse_y
					vram_addr.nametable_y = ~vram_addr.nametable_y;  // ���� 0���ƶ��� 1�ţ�Ҳ���ܴ� 1���ƶ��� 0��
				}
				else if (vram_addr.coarse_y == 31) {
					// �����ʱָ��ָ����attribute table����Ҫ��ָ���ƶ�����name table�ĳ�ʼ��
					vram_addr.coarse_y = 0;  //�ƶ�����name table�ĳ�ʼ��
				}
				else {
					// �������ֱ�������ƶ�һ�鼴��
					vram_addr.coarse_y++;
				}
			}
		}
	};

	// ʹ��tram_addr�еĺ�������Ϣ����vram_addr�еĺ�������Ϣ
	auto TransferAddressX = [&]() {
		// ������Ʊ�������mask�Ĵ����� render_backgroundλ�� render_spriteλ Ϊ1
		if (mask.render_background || mask.render_sprites) {
			vram_addr.nametable_x = tram_addr.nametable_x;
			vram_addr.coarse_x = tram_addr.coarse_x;  // ������ֻ��coarse_x����Ϣ��fine_x���������̿���
		}
	};

	// ʹ��tram_addr�е���������Ϣ����vram_addr�е���������Ϣ
	auto TransferAddressY = [&]() {
		// ������Ʊ�������mask�Ĵ����� render_backgroundλ�� render_spriteλ Ϊ1
		if (mask.render_background || mask.render_sprites) {
			vram_addr.nametable_y = tram_addr.nametable_y;
			vram_addr.coarse_y = tram_addr.coarse_y;
			vram_addr.fine_y = tram_addr.fine_y;  // ������fine_yҲҪ����
		}
	};


	// Ϊ "��Ч" �ı�������λ��������ݣ�׼����ɨ�����������������8�����ء�
	auto LoadBackgroundShifters = [&]() {
		// ÿһ��PPU���£����Ƕ�Ҫ����һ�����ء���Щ��λ����;��λ1λ��Ϊ���غϳ����ṩ������Ҫ�Ķ�������Ϣ��
		// ����16λ����Ϊ�����8λ�����ڻ��Ƶ�8�����أ������8λ�ǽ�Ҫ���Ƶ�8�����ء���Ȼ������ζ�������λ������λ����MSB��
		// Ȼ����"fine_x "����Ҳ�����з��������ã����ں���ῴ����������ʵ�����ǿ���ѡ��ǰ8λ�е��κ�һλ��

		// ���������Ƶ���һ��pattern������������
		bg_shifter_pattern_lo = (bg_shifter_pattern_lo & 0xFF00) | bg_next_tile_lsb;
		bg_shifter_pattern_hi = (bg_shifter_pattern_hi & 0xFF00) | bg_next_tile_msb;

		// ����λ������ÿ�����ض��б仯������ÿ8�����ض��б仯����Ϊ�˷��������������pattern��λ��ͬ����
		// �����������ǰ�attribute table���ֽڵĵ�2λ ����ǰ8�����غ���һ��8������ʹ�õĵ�ɫ�壬�������� "���� "Ϊ8λ���ֽڡ�
		// 0b��ʾ2������ ((0x01 & 0b01) & 0xFF)==0xFF,Ҳ����˵��������Ὣ һ��bit 1��չΪ 8λbit1 ��һ��bit 0��չΪһ��8λbit0
		bg_shifter_attrib_lo = (bg_shifter_attrib_lo & 0xFF00) | ((bg_next_tile_attrib & 0b01) ? 0xFF : 0x00);  
		bg_shifter_attrib_hi = (bg_shifter_attrib_hi & 0xFF00) | ((bg_next_tile_attrib & 0b10) ? 0xFF : 0x00);

	};

	// ÿ�����ڣ��洢ģʽ��������Ϣ����λ������������λ1λ��������Ϊÿ�����ڣ������������һ�����ء�
	// ����ζ����Զ��ԣ���λ����״̬��ɨ���ߵ�8�����ز��������Ƶ�������ͬ���ġ�
	auto UpdateShifter = [&]() {
		// ����������Ʊ�������mask�Ĵ����� render_backgroundλΪ1
		if (mask.render_background) {
			// ��ͼ��������һλ
			bg_shifter_pattern_lo <<= 1;
			bg_shifter_pattern_hi <<= 1;

			// ����Ӧ�����Կ�����һλ
			bg_shifter_attrib_lo <<= 1;
			bg_shifter_attrib_hi <<= 1;
		}

		// ���������Ʊ�������mask�Ĵ����� render_spritesλΪ1����cycle�ڻ��Ƶ�ʱ��
		if (mask.render_sprites && cycle >= 1 && cycle < 258) {
			for (int i = 0; i < sprite_count; i++) {
				if (spriteScanline[i].x > 0) {
					// �����δ���Ƶ��������ڵ�λ�ã�ֱ�ӽ���Ӧ��xֵ��һ��x��ʾ�������ڵ�λ���뵱ǰ���Ƶ���λ�õľ���
					spriteScanline[i].x--;
				}
				else {
					// ������Ƶ����鱾���ˣ��Ǿͽ���Ӧ��ͼ�������һλ
					sprite_shifter_pattern_lo[i] <<= 1;
					sprite_shifter_pattern_hi[i] <<= 1;
				}
			}
		}
	};


	// ����1��ɨ����֮�⣬���е�ɨ���߶��û���˵���ǿɼ��ġ�Ԥ��Ⱦɨ����-1�����������õ�һ���ɼ�ɨ����0�� "��λ�� "�ġ�
	// �������if������ -1��239��scanline�Ļ���
	if (scanline >= -1 && scanline < 240) {
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		// ��ʼ���Ʊ���
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (scanline == 0 && cycle == 0) {
			// ������һ��ʱ������
			cycle = 1;
		}

		if (scanline == -1 && cycle == 1) {
			// ��ʼ�µ�ͼ��֡���ƣ���Ҫ���status��vertical_blank��־λ
			status.vertical_blank = 0;
			// ������������־λ��һ��scanline�����8������
			status.sprite_overflow = 0;
			// �������0���б�־λ
			status.sprite_zero_hit = 0;
			// ������о���ͼ����λ��
			for (int i = 0; i < 8; i++) {
				sprite_shifter_pattern_lo[i] = 0x00;
				sprite_shifter_pattern_hi[i] = 0x00;
			}
		}

		if ((cycle >= 2 && cycle < 258) || (cycle >= 321 && cycle < 338)) {
			// ֻҪ������ָ���������ھ͸�����λ��
			UpdateShifter();

			// ����Щ�����У����������ռ��ʹ���ɼ������ݡ�"��λ�� "�Ѿ���ǰһ��scanline����ʱԤװ�����scanline��ʼ�����ݡ�
			// һ�������뿪�ɼ����򣬾ͽ�������״̬��ֱ��Ϊ��һ��ɨ����Ԥװ��λ����
			
			// ���˵��ǣ����ڱ�����Ⱦ��˵������ÿ2��ʱ�����ھͻᾭ��һ���൱���ظ����¼����С�
			switch ((cycle - 1) % 8) {
			case 0:
				// ����ǰ�ı���ͼ������ؽ���λ��
				LoadBackgroundShifters();

				// ��ȡ��һ������ͼ����ID
				// vram_addr.reg�ĵ�12λ�� nametable_y: 1 nametable_x : 1; coarse_y : 5;coarse_x : 5; ��ɣ��պÿ�����������Ӧ��λ��
				// vram_addr.reg & 0x0FFF ��ȡ���ĸ�name table�У��Լ������λ�ã�Ȼ�����0x2000ƫ������ȡ���յ�ַ
				// Ȼ���name table�ж�ȡpattern table�е�����id
				bg_next_tile_id = ppuRead(0x2000 | (vram_addr.reg & 0x0FFF));  // �����Ƕ�ȡname table
				break;

			case 2:
				// ��ȡ��һ����Ӧ��attribute����ֽ�

				// �ع�һ�£�ÿ��name table�����е�Ԫ�����ǲ���ͼ����������Ϣ�����Ǵ�����ɫ������Ϣ�������ĸ���ɫ��Ӧ������Ļ�ϵ��ĸ�����
				// ��Ҫ���ǣ�������Ƭ�͵�ɫ��֮�䲻��1��1�Ķ�Ӧ��ϵ�����п��������ݿ�������64���ֽڡ���ˣ����ǿ��Լ�����Щ���Ի�Ӱ�쵽��Ļ�ϵ�8x8�������������
				// ���ǵ�256x240�Ĺ����ֱ��ʣ����ǿ��Խ�һ������ÿ����������Ļ�ռ�����32x32���أ���4x4����Ƭ���ĸ�ϵͳ��ɫ�屻�����������Ⱦ������һ����ɫ�����ֻ��2λ��ָ����
				// ��ˣ��������ֽڿ���ָ��4����ͬ�ĵ�ɫ�塣��ˣ������������Խ�һ�����裬һ����ɫ�屻Ӧ����4x4��Ƭ����2x2��Ƭ��ϡ�
				// ������Ƭ�ڱ��� "���� "һ����ɫ�����ʵ����Ϊʲô��һЩ��Ϸ����ῴ����Ļ��Ե����ɫʧ���ԭ��

				// ��֮ǰѡ��ͼ����IDʱһ�������ǿ���ʹ��ѭ���Ĵ����ĵײ�12λ����������Ҫʹʵ�����ȸ���һ�㣬��Ϊ����Ҫ�Ĳ����ض��Ŀ飬����һ��4x4��������ֽڣ�
				// ���߻��仰˵�����ǰ�32x32�ĵ�ַ����4���õ�һ����Ч��8x8��ַ��Ȼ��������ַƫ�Ƶ�Ŀ��name table�����Բ��֡�
				// https://www.jianshu.com/p/c5b02614e4a7

				// ��������ҵ���һ�����Ӧ�����Ե�2��bit
				bg_next_tile_attrib = ppuRead(0x23C0 | (vram_addr.nametable_y << 11)
					| (vram_addr.nametable_x << 10)
					| ((vram_addr.coarse_y >> 2) << 3)
					| (vram_addr.coarse_x >> 2));

				if (vram_addr.coarse_y & 0x02) {
					bg_next_tile_attrib >>= 4;
				}
				if (vram_addr.coarse_x & 0x02) {
					bg_next_tile_attrib >>= 2;
				}
				bg_next_tile_attrib &= 0x03;

				break;

			case 4:
				// ��ͼ���洢���л�ȡ��һ���������LSBλƽ�� �Ѿ���nametable�ж�ȡ����ƬID�����ǽ�ʹ�����ID������ͼ���洢���У�
				// ���ҵ���ȷ�ľ��飨���辫��λ�ڸô洢����8x8���صı߽��ϣ���ʹ����8x16�ľ��飬����Ҳ����ˣ���Ϊ������Ƭ����8x8����
				// ���ھ�������Ϊ1���أ������Ϊ8���أ����ǿ��Խ����������б�ʾΪһ���ֽڣ������ͼ���洢����ƫ�ƺ����ס�
				// �ܹ���8KB������������Ҫһ��13λ�ĵ�ַ��

				// ������ͣ�
				// (control.pattern_background << 12) �ҵ���ȷ��pattern table������pattern����pattern��ÿһ����СΪ8KB
				// ((uint16_t)bg_next_tile_id << 4) ��Ϊÿһ��ͼ�����16���ֽڣ�������Ҫ����16
				// (vram_addr.fine_y) ָʾ���Ƶ���ǰͼ�������һ����
				// +0 ��������ƫ����

				bg_next_tile_lsb = ppuRead((control.pattern_background << 12)
					+ ((uint16_t)bg_next_tile_id << 4)
					+ (vram_addr.fine_y) + 0);

				break;

			case 6:
				// ����һ�����ƣ������ǻ�ȡ�������MSBλƽ��
				// ע�⣺���������+8����Ϊ��Ҫƫ��8���ֽڻ�ȡMSB
				bg_next_tile_msb = ppuRead((control.pattern_background << 12)
					+ ((uint16_t)bg_next_tile_id << 4)
					+ (vram_addr.fine_y) + 8);
				break;

			case 7:
				// ��������Ƭ�� "ָ�� "ˮƽ�����ӵ��������ڴ��е���һ����Ƭ��
				// ע������ܻ��Խ���ֱ�ı߽磬���е㸴�ӣ�������ʵ�ֹ����Ǳز����ٵġ�
				IncrementScrollX();

				break;
			}
		}

		// ��ǰscanline�Ĳ��ɼ�������
		if (cycle == 256) {
			IncrementScrollY();
		}

		if (cycle == 257) {
			// ׼����һ��ɨ���е����ݺ͸���vram�еĺ���������
			LoadBackgroundShifters();
			TransferAddressX();
		}

		if (cycle == 338 || cycle == 340) {
			// ��ȡ��һ��ͼ����id
			bg_next_tile_id = ppuRead(0x2000 | (vram_addr.reg) & 0x0FFF);
		}

		if (scanline == -1 && cycle >= 280 && cycle < 305) {
			// �ոս�����vertical blank״̬�����Ը���vram�е���������Ϣ������Ⱦ����
			TransferAddressY();
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		// ��ʼ���ƾ���
		///////////////////////////////////////////////////////////////////////////////////////////////////////////

		// PPU�ڲ����Ʊ�����Ƭ���������������ؾ�����Ϣ��ȡ����֮���ǣ��ҽ�һ����ִ�����о����������
		// NES�������������ġ���ʹ���Ǹ����׿������������Ĺ��̡�
		if (cycle == 257 && scanline >= 0) {
			// ��ʱ������Ϊ257ʱ������scanline�п�������Ļ���
			// ������������������һ��scaline�л���ֶ��ٸ����飬Ȼ��Ԥ���ص�buffer����

			// �����������buffer�е�����
			std::memset(spriteScanline, 0xFF, 8 * sizeof(sObjectAttributeEntry));

			// sprite_count ���ƻ��Ƶľ��������һ��ɨ���������8��
			sprite_count = 0;

			// ���������λ��
			for (uint8_t i = 0; i < 8; i++) {
				sprite_shifter_pattern_lo[i] = 0;
				sprite_shifter_pattern_hi[i] = 0;
			}

			// ������������Щ��������һ��ɨ�������ǿɼ��ġ�������Ҫ����OAM��ֱ�������ҵ�8�������Yλ�ú͸߶ȶ�����һ��ɨ���ߵĴ�ֱ��Χ�ڡ�
			// һ�������ҵ���8�����þ���OAM�����Ǿ�ֹͣ�����ڣ�ע����������9�����顣����Ϊ�����г���8�����������£��ҿ������þ��������־��
			
			uint8_t nOAMEntry = 0;  // ��¼����OAM�ڴ�����Ŀ���±�

			// ��������¾���0���б�־��״̬
			bSpriteZeroHitPossible = false;

			// ��Ϊ����Ļ������ȼ���ȡ������OAM�ڴ��е�˳��ģ�������������֮���˳�����ȼ�spriteScanline��û�б仯�ģ���Ȼ��ǰ������
			while (nOAMEntry < 64 && sprite_count < 9) {
				// �ڼ��㾫��λ����ɨ���ߵľ���ʱ��Ҫת��Ϊ�з�������������֪���Ƿ��ཻ��
				int16_t diff = ((int16_t)scanline - (int16_t)OAM[nOAMEntry].y);

				// �������Ϊ��������ô��龫��������Ƿ���scanline֮�ϣ���Ϊ����y���������Ͻ�y����
				// ͬʱ����Ϊ�����Ϊ8x8��С��8x16��С��������Ҫ����ʵ�ʴ�С�����ж�
				if (diff >= 0 && diff < (control.sprite_size ? 16 : 8)) {
					// ������if��䣬˵��������Ҫ������
					// �������д��8������
					if (sprite_count < 8) {
						// �ڻ���0�ž����ʱ������ ����0���б�־
						if (nOAMEntry == 0) {
							bSpriteZeroHitPossible = true;
						}
						// ��OAM�о������ݴ��䵽buffer��
						memcpy(&spriteScanline[sprite_count], &OAM[nOAMEntry], sizeof(sObjectAttributeEntry));
						// ���ƾ��������+1
						sprite_count++;
					}
				}
				// ��������
				nOAMEntry++;
			}

			// �����Ҫ���Ƶľ�����������8��������status�Ĵ�����sprite overflow��־
			status.sprite_overflow = (sprite_count > 8);
		}

		if (cycle == 340) {
			// cycleΪ340��ʱ�򣬽�����scanline�����λ�ã��ڴ˴�׼�� ������λ��
			for (uint8_t i = 0; i < sprite_count; i++) {
				// ������Ҫ����ȷ�Ĵ�ֱƫ������ȡ�����8λ��ģʽ������Ĵ�СҲ��Ӱ����һ�㣬
				// ��Ϊ���������8�л�16�иߡ����⣬�����ڴ�ֱ��ˮƽ�����϶����� ��ת �����������кܶ�����Ҫ��
				
				uint8_t sprite_pattern_bits_lo = 0x00, sprite_pattern_bits_hi = 0x00;
				uint16_t sprite_pattern_addr_lo = 0x00, sprite_pattern_addr_hi = 0x00;

				// ȷ������ģʽ���ݵ��ֽڵ��ڴ��ַ������ֻ��Ҫloģʽ�ĵ�ַ����Ϊhiģʽ�ĵ�ַ������lo��ַƫ��8��
				if (!control.sprite_size) {
					// 8x8�ľ����С
					if (!(spriteScanline[i].attribute & 0x80)) {  // �Ƿ�ֱ��ת����
						// ����ֱ��ת
						sprite_pattern_addr_lo =
							(control.pattern_sprite << 12)  // ��һ��ͼ����? 0KB ���� 4KB ƫ����
							| (spriteScanline[i].id << 4)  // ͼ����ʼλ��������? һ����Ĵ�СΪ16��������Ҫ����16
							| (scanline - spriteScanline[i].y); //���Ƶ���һ���� (0->7)
					}
					else {
						// ��ֱ��ת
						sprite_pattern_addr_lo =
							(control.pattern_sprite << 12)  // ��һ��ͼ����? 0KB ���� 4KB ƫ����
							| (spriteScanline[i].id << 4)  // ͼ����ʼλ��������? һ����Ĵ�СΪ16��������Ҫ����16
							| (7 - (scanline - spriteScanline[i].y)); // ���Ƶ���һ���� (7->0)
					}
				}
				else {
					// 8x16�����С
					if (!(spriteScanline[i].attribute & 0x80)) {  // �Ƿ�ֱ��ת����
						// ����ֱ��ת
						if (scanline - spriteScanline[i].y < 8)
						{
							// ��ȡǰ���
							sprite_pattern_addr_lo =
								((spriteScanline[i].id & 0x01) << 12)  // ��һ��ͼ����? 0KB or 4KB offset
								| ((spriteScanline[i].id & 0xFE) << 4)  // ͼ����ʼλ��������? һ����Ĵ�СΪ16��������Ҫ����16
								| ((scanline - spriteScanline[i].y) & 0x07); // ���Ƶ���һ���� (0->7)
						}
						else
						{
							// ��ȡ����
							sprite_pattern_addr_lo =
								((spriteScanline[i].id & 0x01) << 12)   // ��һ��ͼ����? 0KB or 4KB offset
								| (((spriteScanline[i].id & 0xFE) + 1) << 4)  // ͼ����ʼλ��������? һ����Ĵ�СΪ16��������Ҫ����16
								| ((scanline - spriteScanline[i].y) & 0x07);  // ���Ƶ���һ���� (0->7)
						}
					}
					else {
						// ��ֱ��ת
						if (scanline - spriteScanline[i].y < 8)
						{
							// ��ȡǰ���
							sprite_pattern_addr_lo =
								((spriteScanline[i].id & 0x01) << 12)    // ��һ��ͼ����? 0KB or 4KB offset
								| (((spriteScanline[i].id & 0xFE) + 1) << 4)    // ͼ����ʼλ��������? һ����Ĵ�СΪ16��������Ҫ����16
								| (7 - (scanline - spriteScanline[i].y) & 0x07);  // ���Ƶ���һ���� (7->0)
						}
						else
						{
							// ��ȡ����
							sprite_pattern_addr_lo =
								((spriteScanline[i].id & 0x01) << 12)     // ��һ��ͼ����? 0KB or 4KB offset
								| ((spriteScanline[i].id & 0xFE) << 4)    // ͼ����ʼλ��������? һ����Ĵ�СΪ16��������Ҫ����16
								| (7 - (scanline - spriteScanline[i].y) & 0x07);  // ���Ƶ���һ���� (7->0)
						}
					}
				}

				// ͼ���ĸ�8λ��Զ����ڵ�8λƫ��8���ֽ�
				sprite_pattern_addr_hi = sprite_pattern_addr_lo + 8;

				// ���ڸ���ͼ����ĵ�ַ��ȡ�����Ӧ��ͼ��
				sprite_pattern_bits_lo = ppuRead(sprite_pattern_addr_lo);
				sprite_pattern_bits_hi = ppuRead(sprite_pattern_addr_hi);

				// ʵ�־���ˮƽ��ת����
				if (spriteScanline[i].attribute & 0x40) {  // ����־λ
					// ˮƽ��ת������lambda����  https://stackoverflow.com/a/2602885
					auto flipbyte = [](uint8_t b)
					{
						b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
						b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
						b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
						return b;
					};

					// ��ʼ��ת
					sprite_pattern_bits_lo = flipbyte(sprite_pattern_bits_lo);
					sprite_pattern_bits_hi = flipbyte(sprite_pattern_bits_hi);
				}

				// �����յĵ��ľ���ͼ�����ؽ� ��λ�����棬׼����һ��ɨ���еĻ���
				sprite_shifter_pattern_lo[i] = sprite_pattern_bits_lo;
				sprite_shifter_pattern_hi[i] = sprite_pattern_bits_hi;
			}
		}
	}

	// ���ƽ������ɣ������κ���
	if (scanline == 240) {
		
	}

	// vertical blank����
	if (scanline >= 241 && scanline < 261) {
		if (scanline == 241 && cycle == 1) {
			// ������һ��ָ����ʱ������status�Ĵ�����vertical_blankλ
			// ���controler�Ĵ��������ͷ��жϵĻ����Ǿ��ͷ��ж�֪ͨCPU��һ֡��ͼ����������
			status.vertical_blank = 1;

			if (control.enable_nmi) {
				nmi = true;
			}
		}
	}


	///////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////// ��װ�����뾫���ͼ��
	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	// �����׶�====================================================
	// ����5��������ϻ�ȡ��ǰ���ص���ɫ
	uint8_t bg_pixel = 0x00;  // 2���ص�����ֵ
	uint8_t bg_palette = 0x00;  // 3���صĵ�ɫ������

	// ֻ��mask��render_backgroundΪ1���ܹ����б����Ļ���
	if (mask.render_background) {
		// ͨ��ѡ����ص�λ����������ѡ����ȡ���ھ�ϸ��x������
		// ��������Ч���ǽ����еı�����Ⱦƫ��һ���趨��������������ƽ��������
		uint16_t bit_mux = 0x8000 >> fine_x;

		// ����λ����ȡ������ͼ������
		uint8_t p0_pixel = (bg_shifter_pattern_lo & bit_mux) > 0;
		uint8_t p1_pixel = (bg_shifter_pattern_hi & bit_mux) > 0;

		// ��ϸߵ�λ���γ�2���ص�����ֵ
		bg_pixel = (p1_pixel << 1) | p0_pixel;

		// ����λ����ȡ����������ֵ
		uint8_t bg_pal0 = (bg_shifter_attrib_lo & bit_mux) > 0;
		uint8_t bg_pal1 = (bg_shifter_attrib_hi & bit_mux) > 0;

		// ���attribute table�еĶ�Ӧ�ߵ�λ����ȡ��ɫ������
		bg_palette = (bg_pal1 << 1) | bg_pal0;
	}


	// ����׶�====================================================
	// ����5��������ϻ�ȡ��ǰ���ص���ɫ
	uint8_t fg_pixel = 0x00;  // 2���ص�����ֵ
	uint8_t fg_palette = 0x00;  // 3���صĵ�ɫ������
	// ����1�����ر�ʾ�������ȼ����Ƿ�ȱ������Ȼ���
	uint8_t fg_priority = 0x00;  //Ϊ1��ʾ�ȱ�����Ҫ����֮��������Ҫ

	// ֻ��mask��render_backgroundΪ1���ܹ����о���Ļ���
	if (mask.render_sprites) {
		// ������ǰscanline�е����о���

		bSpriteZeroBeingRendered = false;  // ָʾ�Ƿ��ǵ�0������

		for (uint8_t i = 0; i < sprite_count; i++) {
			// spriteScanline[i].xΪ0ʱ���������Ƶ��˵�ǰ����
			if (spriteScanline[i].x == 0) {
				// ע�� Fine X�������������ھ��飬��ϷӦ�ñ��������뱳���Ĺ�ϵ���������Ǿ�����λ����MSB����

				// ����λ����ȡ������ͼ������
				uint8_t fg_pixel_lo = (sprite_shifter_pattern_lo[i] & 0x80) > 0;
				uint8_t fg_pixel_hi = (sprite_shifter_pattern_hi[i] & 0x80) > 0;
				
				// ��ϸߵ�λ���γ�2���ص�����ֵ
				fg_pixel = (fg_pixel_hi << 1) | fg_pixel_lo;

				// ����λ����ȡ����������ֵ
				fg_palette = (spriteScanline[i].attribute & 0x03) + 0x04;  // ����ĵ�ɫ������ֱ�Ӵ���spriteScanline[i].attribute�����2λ�ģ�+0x04��ʾѡ�����ھ����0x3F10����ɫ��
				// ��ȡ���ȼ���ȷ�������뾫��˭��ʾ����Ļ�ϣ������ʵ������½�ǽ��Ч��
				fg_priority = (spriteScanline[i].attribute & 0x20) == 0;

				// ������ز�͸�����Ǿͽ�����Ƴ���
				if (fg_pixel != 0) {
					if (i == 0) { 
						bSpriteZeroBeingRendered = true;
					}
					// ���������������ĵ�һ����͸���������ؼ��ɣ���ΪԽ��ǰ�ľ������ȼ�Խ�ߣ�ֻҪ������λ�������ĵ�һ����͸������������ľ������ɫ����
					break;
				}
			}
		}
	}


	// ��ϱ����뾫��
	// ������������������ӵ���˱��������뾫�����أ��������������ǽ�����ϣ�������ջ�������Ļ�ϵ����ص�
	uint8_t pixel = 0x00;  // ���յ�����
	uint8_t palette = 0x00;  // ���յĵ�ɫ��

	if (bg_pixel == 0 && fg_pixel == 0) {
		// ����������ض�Ϊ0����ʾ͸��
		// �Ǿͻ�������ɫ
		pixel = 0x00;
		palette = 0x00;
	}
	else if (bg_pixel == 0 && fg_pixel > 0) {
		// ����͸�������鲻͸����������������
		pixel = fg_pixel;
		palette = fg_palette;
	}
	else if (bg_pixel > 0 && fg_pixel == 0) {
		// ������͸��������͸����������������
		pixel = bg_pixel;
		palette = bg_palette;
	}
	else if (bg_pixel > 0 && fg_pixel > 0) {
		// ����͸���Ļ�����Ҫ�������ȼ�������һ������ʾ
		if (fg_priority) {
			// ����������ȼ��Ƚϸߣ�ֱ����ʾ����
			pixel = fg_pixel;
			palette = fg_palette;
		}
		else {
			// ������ʾ����
			pixel = bg_pixel;
			palette = bg_palette;
		}

		// ����0���м��
		if (bSpriteZeroHitPossible && bSpriteZeroBeingRendered) {
			// �ھ����뱳����������Ƶ�����²ż��
			if (mask.render_background & mask.render_sprites) {
				// ��Ļ�����Ե���ض��Ŀ�������������ۡ�������ƽ������ʱ�Ĳ�һ�£���Ϊ�����X�������>=0����
				if (~(mask.render_background_left | mask.render_sprites_left)) {
					if (cycle >= 9 && cycle < 258) {
						status.sprite_zero_hit = 1;
					}
				}
				else {
					if (cycle >= 1 && cycle < 258) {
						status.sprite_zero_hit = 1;
					}
				}
			}
		}
	}

	// ���ڣ���ʾ����Ļ�ϵ���������ֵ���ɫ��ֵ����ȷ���ˣ�������Ļ�ϻ���������
	// ��������ֵ
	// ע�⣺����ĵ�һ�������������꣬�ڶ��������Ǻ�����
	if (cycle>0 && cycle <= 256 && scanline>=0 && scanline <= 240) {
		m_emulatorScreen.setPixel(cycle - 1, scanline, GetColourFromPaletteRam(palette, pixel));
	}

	// ���ƻ��ƽ���
	cycle++;

	if (cycle >= 341) {
		// ����һ���ˣ�����cycle����������
		cycle = 0;
		scanline++;
		if (scanline >= 261) {
			// scanline�ƶ���-1
			scanline = -1;
			// ָʾ��һ֡�������
			frame_complete = true;
		}
	}
}
