#include "PPU2C02.h"

/*
* 构造函数
* 在构造函数中初始化nes中支持的所有颜色
* 虽然在NES的调色板(palette)中使用一个字节指示使用的颜色，但一共支持的颜色却只有52种，因为重复了一些颜色  https://www.jianshu.com/p/c5b02614e4a7
*/
PPU2C02::PPU2C02(uint8_t scale)
{
	// 初始化虚拟屏幕,模拟的nes的240x256分辨率的显示屏，注意：这里是反着定义的，所以赋值时行为第二位，列为第一维
	// 第三个参数为像素大小
	m_emulatorScreen.create(VirtualScreen::NESVideoWidth, VirtualScreen::NESVideoHeight, scale, sf::Color::White);
}

/*
* 析构函数
*/
PPU2C02::~PPU2C02()
{
	// 没有动态申请动态空间，所以函数体为空
}


/*
* 跟据属性表（attribute table）中的2个bit + 图案表（pattern table）中2个bit组合而成调色盘索引，确定当前像素的颜色  https://www.jianshu.com/p/c5b02614e4a7
* 背景颜色调色板从0x3F00开始，有16个条目
* 精灵颜色调色板从0x3F10开始，有16个条目
*/
sf::Color PPU2C02::GetColourFromPaletteRam(uint8_t palette, uint8_t pixel)
{
	// palette变量的bit0与bit1就是属性表中的2个bit，这是palette索引的高2位;palette的bit3表示选择0x3F00开始的背景调色板还是0x3F10开始的精灵调色板
	// pixel变量的bit0与bit1就是图案表中的2个bit，这是palette索引的低2位
	// 这两个变量组合起来就能够索引调色板中的一共32条目了
	// 下面指令解释：
	// 0x3F00表示调色板从该地址开始
	// (palette << 2) + pixel 获取颜色索引
	// ppuRead() 读取保存在目标地址中的palScreen颜色索引
	// & 0x3F 防止越界，因为palScreen只有64个条目
	// palScreen[]得到最终的像素颜色
	return sf::Color(nes_colors[ppuRead(0x3F00 + (palette << 2) + pixel) & 0x3F]);
}

/*
* // 接口，获取屏幕显示
*/
VirtualScreen& PPU2C02::GetVirtualScreen()
{
	return m_emulatorScreen;
}

/*
* 本方法模拟的是CPU对PPU中的8个寄存器的访问
* 寻址范围为 0x00-0x07 分别表示8个寄存器
*/
uint8_t PPU2C02::cpuRead(uint16_t addr, bool rdonly)
{
	uint8_t data = 0x00;  // 初始化返回变量

	switch (addr) {
		// Control寄存器，不能被CPU读
	case 0x0000: 
		break;

		// Mask寄存器，不能被CPU读
	case 0x0001: 
		break;

		// Status寄存器
	case 0x0002: 
		// 状态寄存器中本来只有最高3位是有作用的，但有一些游戏也使用了其他5位
		// 其他5位由上次一ppu总线停留的数据填充
		data = (status.reg & 0xE0) | (ppu_data_buffer & 0x1F);  // 分别取出status中的最高3位与ppu_data_buffer的第5位

		// 设置status寄存器中的vertical_blank状态位为0
		status.vertical_blank = 0;

		// 读取寄存器会清除address_latch状态位，因为电路是这样设计的
		break;

		// OAM（Object Attribute Memory）传输地址的寄存器，不能被CPU读
	case 0x0003: 
		break;

		// OAM（Object Attribute Memory）传输数据的寄存器
	case 0x0004:
		// 返回oam_addr地址指定的数据
		data = pOAM[oam_addr];
		break;

		// Scoll寄存器，不能被CPU读
	case 0x0005: 
		break;

		// PPU 地址寄存器，不能被CPU读取
	case 0x0006: 
		break;

		// PPU 数据寄存器
	case 0x0007:
		// 如果读取的数据来自name table，那么读取会推迟一个时钟周期
		// 因此应返回上一次read请求指定的数据
		data = ppu_data_buffer;  // 获取上一次的数据
		// 此时更新这一次的数据到buffer中,读取数据的地址由vram_addr.reg指定
		ppu_data_buffer = ppuRead(vram_addr.reg);

		// 但如果读取的数据来自调色板(paettle)，那么不用推迟一个时钟周期，直接返回当前数据
		if (vram_addr.reg >= 0x3F00) {
			data = ppu_data_buffer;
		}

		// 基于control寄存器的读取模式位（increment_mode），每次读取的地址会自动的增长。
		// 如果是水平增长模式，地址只增加1位，移动到下一行；若是竖直增长模式，每一次增加32位，因为需要跳过一整行name table
		vram_addr.reg += (control.increment_mode ? 32 : 1);
		break;
	}

	return data;
}

/*
* 本方法模拟的是CPU对PPU中的8个寄存器的访问
* 寻址范围为 0x00-0x07 分别表示8个寄存器
*/
void PPU2C02::cpuWrite(uint16_t addr, uint8_t data)
{
	switch (addr)
	{
		// control寄存器
	case 0x0000:
		// 将数据写入control寄存器
		control.reg = data;
		// 跟据cpu写入的数据配置tram_addr变量，这里指定的是写入4个name table中的某一个
		tram_addr.nametable_x = control.nametable_x;
		tram_addr.nametable_y = control.nametable_y;
		break;

		// Mask寄存器
	case 0x0001:
		// 将数据写入mask寄存器
		mask.reg = data;
		break;

		// status寄存器,cpu不能直接向status寄存器写入数据
	case 0x0002:
		break;

		// OAM（Object Attribute Memory）传输地址的寄存器
	case 0x0003:
		// 因此OAM只有256字节，所以地址只需要8位，将data直接赋值给oam_addr地址变量即可
		oam_addr = data;
		break;

		// OAM（Object Attribute Memory）传输数据的寄存器
	case 0x0004:
		// 向oam_addr指向的OAM内存地址写入数据
		pOAM[oam_addr] = data;
		break;

		// scoll寄存器，控制屏幕的滚动
	case 0x0005:
		// 因为数据一共有16位，所以需要分为两次进行传输，通过address_latch变量来识别是第几次传输
		if (address_latch == 0) {
			// address_latch等于0时一定是第一次传输
			// 第一次传输会写入屏幕横坐标相关的信息
			fine_x = data & 0x07;  // fine_x控制每个8x8tile中的具体横坐标,范围为0-7
			tram_addr.coarse_x = data >> 3;  // coarse_x指示是横向的32个tile中的哪一个，范围为0-31
			address_latch = 1;  // 设置为1，控制流程
		}
		else {
			// 此时是第二次传输，第二次传输会写入屏幕纵坐标相关的信息
			tram_addr.fine_y = data & 0x07;  // fine_y控制每个8x8tile中的具体横坐标,范围为0-7
			tram_addr.coarse_y = data >> 3;  // coarse_x指示是纵向的32个tile中的哪一个，范围为0-31
			address_latch = 0;  // 重置为0
		}
		break;

		// PPU 地址传输寄存器
	case 0x0006:
		// 地址一共16位，所以分两次传输，使用address_latch控制流程
		if (address_latch == 0) {
			// 第一次传输高字节,先备份在tram_addr.reg变量中，传输完了再赋值给vram_addr.reg
			tram_addr.reg = (uint16_t)((data & 0x3F) << 8) | (tram_addr.reg & 0x00FF);  // 最高的那一位没有作用，所以 &上0x3F；注意：传输的同时保持tram_addr.reg的低8位不变
			address_latch = 1;  // 设置为1，控制流程
		}
		else {
			// 第二次传输低字节,
			tram_addr.reg = (tram_addr.reg & 0xFF00) | data;  // 给低8位赋值
			vram_addr = tram_addr;  // 两次数据传输完成，将tram_addr数据赋值给vram_addr
			address_latch = 0;  // 重置为0
		}
		break;

		// PPU 数据传输寄存器
	case 0x0007:
		// 向vram_addr指定的地址写入数据，即向name table中写入数据
		ppuWrite(vram_addr.reg, data);
		// 基于control寄存器的读取模式位（increment_mode），每次读取的地址会自动的增长。
		// 如果是水平增长模式，地址只增加1位，移动到下一行；若是竖直增长模式，每一次增加32位，因为需要跳过一整行name table
		vram_addr.reg += (control.increment_mode ? 32 : 1);
		break;
	}
}

/*
* 此函数模拟的时PPU总线上的读取
* 定义PPU在总线上读取数据的方法，该方法将PPU传到总线的不同地址翻译，转换为对应总线上连接的设备的地址
* 在本模拟器中，没有CPU总线类一样，为PPU总线单独设计一个类，而是直接设计成一个函数嵌入在PPU类中
* 同样设计的还有 tblName tblPalette 这两段内存地址，此模拟器选择直接将这两段内存嵌入在PPU类中
* PPU的寻址空间一共有16KB
* ppuRead函数负责将ppu的所有读请求转化为对应PPU内存上的地址
*/
uint8_t PPU2C02::ppuRead(uint16_t addr, bool rdonly)
{
	uint8_t data = 0x00;  // 初始化返回变量
	addr &= 0x3FFF;  // 将地址范围控制在0x0000-0x3FFF间，也就是16KB内，因为PPU的寻址空间一共有16KB

	if (addr >= 0x0000 && addr <= 0x1FFF) {
		// 这里映射的是PPU的 0x0000-0x1FFF 这一段地址空间,每当PPU写入这一段地址空间上的地址时，将其映射到cartridge设备上去
		// 读操作交给cartridge处理，这里传入的时data的引用
		cart->ppuRead(addr, data);
	}
	else if (addr >= 0x2000 && addr <= 0x3EFF) {
		// 这里映射的是PPU的 0x2000-0x3EFF 这一段地址空间
		// 实际的name table只有2KB大小，但实际上有0x2000-0x3EFF这么大一段地址空间映射到这2KB内存上
		// 首先将 0x2000-0x3EFF 映射为4KB大小
		// 然后需要跟据屏幕的不同镜像方式来控制这4KB大小的数据如何映射到2KB的空间上去，本来只有2个1KB大小的name table，通过镜像的方式假装有4个name table
		// 这里通过 &运算 与 0x0FFF 获取映射后的值，映射到4KB空间
		addr &= 0x0FFF;

		// 跟据屏幕的镜像方式，将4KB空间映射到2KB的实际空间上
		if (cart->Mirror() == MIRROR::VERTICAL) {
			// 如果镜像模式为竖直,竖直将L1和L3映射到第一个物理名称表，将L2和L4映射到第二个物理名称表，如下图所示。屏幕上显示为
			// 1 2
			// 1 2
			if (addr >= 0x0000 && addr <= 0x03FF) {
				data = tblName[0][addr & 0x03FF];
			}
			else if (addr >= 0x0400 && addr <= 0x07FF) {
				data = tblName[1][addr & 0x03FF];
			}
			else if (addr >= 0x0800 && addr <= 0x0BFF) {
				data = tblName[0][addr & 0x03FF]; // 垂直镜像模式下第3KB映射到第1KB
			}
			else if (addr >= 0x0C00 && addr <= 0x0FFF) {
				data = tblName[1][addr & 0x03FF];  // 垂直镜像模式下第4KB映射到第2KB
			}
		}
		else if (cart->Mirror() == MIRROR::HORIZONTAL) {
			// 如果镜像模式为水平,水平镜像将L1和L2映射到第一个物理名称表，将L3和L4映射到第二个物理名称表，如下图所示。屏幕上显示为
			// 1 1
			// 2 2
			if (addr >= 0x0000 && addr <= 0x03FF) {
				data = tblName[0][addr & 0x03FF];
			}
			else if (addr >= 0x0400 && addr <= 0x07FF) {
				data = tblName[0][addr & 0x03FF];  // 水平镜像模式下第2KB映射到第1KB
			}
			else if (addr >= 0x0800 && addr <= 0x0BFF) {
				data = tblName[1][addr & 0x03FF]; 
			}
			else if (addr >= 0x0C00 && addr <= 0x0FFF) {
				data = tblName[1][addr & 0x03FF];  // 水平镜像模式下第4KB映射到第2KB
			}
		}
	}
	else if (addr >= 0x3F00 && addr <= 0x3FFF) {
		// 这里映射的是PPU的 0x3F00-0x3FFF 这一段地址空间,将这一段大小的空间映射到 32字节 上
		// 即 0x3F00-0x3FFF =》 0x0000-0x001F
		// 这里对应的是PPU中 32字节 的调色板数据，里面每一个字节保存着 NES 中不同颜色的索引，一共64个索引，但最多索引52种颜色，因为有重复
		// 同时因为每四个字节调色板都会重复背景颜色，因此屏幕上最多同时存在25种不同颜色，也就是说这 32字节 种最多有25种不同的颜色索引  32-(32/4)+1 = 25

		// 首先将地址映射到 0x0000-0x001F间
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

		// 从调色板中获取到索引，同时对索引做一点调整，改变颜色显示效果，不做应该也没问题
		data = tblPalette[addr] & (mask.grayscale ? 0x30 : 0x3F);
	}

	return data;
}

/*
* 此函数模拟的时PPU总线上的读取
* 定义PPU在总线上写入数据的方法，该方法将PPU传到总线的不同地址翻译，转换为对应总线上连接的设备的地址
* 在本模拟器中，没有CPU总线类一样，为PPU总线单独设计一个类，而是直接设计成一个函数嵌入在PPU类中
* 同样设计的还有 tblName tblPalette 这两段内存地址，此模拟器选择直接将这两段内存嵌入在PPU类中
* PPU的寻址空间一共有16KB
* ppuWrite函数负责将ppu的所有写请求转化为对应PPU内存上的地址
*/
void PPU2C02::ppuWrite(uint16_t addr, uint8_t data)
{
	addr &= 0x3FFF;  // 将地址范围控制在0x0000-0x3FFF间，也就是16KB内，因为PPU的寻址空间一共有16KB

	if (addr >= 0x0000 && addr <= 0x1FFF) {
		// 这里映射的是PPU的 0x0000-0x1FFF 这一段地址空间,每当PPU写入这一段地址空间上的地址时，将其映射到cartridge设备上去
		// 读操作交给cartridge处理
		// 实际上PPU应该不会写CHRMemory中的内容，除非Cartridge中这一段是RAM，而不是ROM
		cart->ppuWrite(addr, data);
	}
	else if (addr >= 0x2000 && addr <= 0x3EFF) {
		// 这里映射的是PPU的 0x2000-0x3EFF 这一段地址空间
		// 实际的name table只有2KB大小，但实际上有0x2000-0x3EFF这么大一段地址空间映射到这2KB内存上
		// 首先将 0x2000-0x3EFF 映射为4KB大小
		// 然后需要跟据屏幕的不同镜像方式来控制这4KB大小的数据如何映射到2KB的空间上去，本来只有2个1KB大小的name table，通过镜像的方式假装有4个name table
		// 这里通过 &运算 与 0x0FFF 获取映射后的值，映射到4KB空间
		addr &= 0x0FFF;

		// 跟据屏幕的镜像方式，将4KB空间映射到2KB的实际空间上
		if (cart->Mirror() == MIRROR::VERTICAL) {
			// 如果镜像模式为竖直,竖直将L1和L3映射到第一个物理名称表，将L2和L4映射到第二个物理名称表，如下图所示。屏幕上显示为
			// 1 2
			// 1 2
			if (addr >= 0x0000 && addr <= 0x03FF) {
				tblName[0][addr & 0x03FF] = data;
			}
			else if (addr >= 0x0400 && addr <= 0x07FF) {
				tblName[1][addr & 0x03FF] = data;
			}
			else if (addr >= 0x0800 && addr <= 0x0BFF) {
				tblName[0][addr & 0x03FF] = data; // 垂直镜像模式下第3KB映射到第1KB
			}
			else if (addr >= 0x0C00 && addr <= 0x0FFF) {
				tblName[1][addr & 0x03FF] = data;  // 垂直镜像模式下第4KB映射到第2KB
			}
		}
		else if (cart->Mirror() == MIRROR::HORIZONTAL) {
			// 如果镜像模式为水平,水平镜像将L1和L2映射到第一个物理名称表，将L3和L4映射到第二个物理名称表，如下图所示。屏幕上显示为
			// 1 1
			// 2 2
			if (addr >= 0x0000 && addr <= 0x03FF) {
				tblName[0][addr & 0x03FF] = data;
			}
			else if (addr >= 0x0400 && addr <= 0x07FF) {
				tblName[0][addr & 0x03FF] = data;  // 水平镜像模式下第2KB映射到第1KB
			}
			else if (addr >= 0x0800 && addr <= 0x0BFF) {
				tblName[1][addr & 0x03FF] = data;
			}
			else if (addr >= 0x0C00 && addr <= 0x0FFF) {
				tblName[1][addr & 0x03FF] = data;  // 水平镜像模式下第4KB映射到第2KB
			}
		}
	}
	else if (addr >= 0x3F00 && addr <= 0x3FFF) {
		// 这里映射的是PPU的 0x3F00-0x3FFF 这一段地址空间,将这一段大小的空间映射到 32字节 上
		// 即 0x3F00-0x3FFF =》 0x0000-0x001F
		// 这里对应的是PPU中 32字节 的调色板数据，里面每一个字节保存着 NES 中不同颜色的索引，一共64个索引，但最多索引52种颜色，因为有重复
		// 同时因为每四个字节调色板都会重复背景颜色，因此屏幕上最多同时存在25种不同颜色，也就是说这 32字节 种最多有25种不同的颜色索引  32-(32/4)+1 = 25

		// 首先将地址映射到 0x0000-0x001F间
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

		// 向调色板中写入索引数据
		tblPalette[addr] = data;
	}
}

/*
* 连接到游戏卡，因为pattern table的8KB空间在游戏卡中
* 将指针指向游戏卡
*/
void PPU2C02::ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge)
{
	this->cart = cartridge;
}

/*
* 重置PPU状态到一个确定状态
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
* PPU的时钟操作
* 定义了PPU每个时钟做的操作
*/
void PPU2C02::clock()
{
	// 首先定义几个lambda表达式，分别负责不同的操作

	// 定义横向移动电子枪（传统电视绘制像素）的函数，此函数控制8x8像素块间的移动
	auto IncrementScrollX = [&]() {
		// 如果绘制被允许，即mask寄存器的 render_background位或 render_sprite位 为1，那就移动到下一个块
		if (mask.render_background || mask.render_sprites) {
			// 一个name table有30x32个8x8块
			// 在横向移动时可能移动到下一个name table，即从 0号移动到 1号，也可能从 1号移动到 0号,这可以通过vram变量中的nametable_x变量控制
			if (vram_addr.coarse_x == 31) {
				// 移动到下一个name table中去
				vram_addr.coarse_x = 0;  // 重新计数coarse_x
				vram_addr.nametable_x = ~vram_addr.nametable_x;  // 即从 0号移动到 1号，也可能从 1号移动到 0号
			}
			else {
				// 只是在本地同一张name table上移动
				vram_addr.coarse_x++;
			}
		}
	};

	// 定义纵向移动电子枪（传统电视绘制像素）的函数，此函数控制绘制流程的纵向移动
	auto IncrementScrollY = [&]() {
		// 相对与横向移动电子枪，纵向移动会更复杂一些，因为name table的最下面两行8x8块被用作了attribute table
		// 一个name table有30x32个8x8块
		// 纵向滚动的每一个像素都控制在此处由vram_addr.fine_y变量决定

		// 如果绘制被允许，即mask寄存器的 render_background位或 render_sprite位 为1，那就开始纵向移动
		if (mask.render_background || mask.render_sprites) {
			// 如果坐标不会越过一个8x8块，那么只需增加vram.fine_y值
			if (vram_addr.fine_y < 7) {
				vram_addr.fine_y++;
			}
			else {
				// 如果坐标即将进入下一个纵向的8x8块，那么需要控制vram_addr.coarse_y的移动

				vram_addr.fine_y = 0;  // 首先指向下一个块的起始位置

				// 在纵向移动时可能移动到下一个name table，即从 0号移动到 1号，也可能从 1号移动到 0号,这可以通过vram变量中的nametable_y变量控制
				// 注意：纵向只有30个8x8块
				if (vram_addr.coarse_y == 29) {
					// 需要进入下一个name table
					vram_addr.coarse_y = 0;  // 重新计数coarse_y
					vram_addr.nametable_y = ~vram_addr.nametable_y;  // 即从 0号移动到 1号，也可能从 1号移动到 0号
				}
				else if (vram_addr.coarse_y == 31) {
					// 如果此时指针指向了attribute table，需要将指针移动到本name table的初始点
					vram_addr.coarse_y = 0;  //移动到此name table的初始点
				}
				else {
					// 其他情况直接纵向移动一块即可
					vram_addr.coarse_y++;
				}
			}
		}
	};

	// 使用tram_addr中的横坐标信息更新vram_addr中的横坐标信息
	auto TransferAddressX = [&]() {
		// 如果绘制被允许，即mask寄存器的 render_background位或 render_sprite位 为1
		if (mask.render_background || mask.render_sprites) {
			vram_addr.nametable_x = tram_addr.nametable_x;
			vram_addr.coarse_x = tram_addr.coarse_x;  // 这里面只有coarse_x的信息，fine_x由其他流程控制
		}
	};

	// 使用tram_addr中的纵坐标信息更新vram_addr中的纵坐标信息
	auto TransferAddressY = [&]() {
		// 如果绘制被允许，即mask寄存器的 render_background位或 render_sprite位 为1
		if (mask.render_background || mask.render_sprites) {
			vram_addr.nametable_y = tram_addr.nametable_y;
			vram_addr.coarse_y = tram_addr.coarse_y;
			vram_addr.fine_y = tram_addr.fine_y;  // 别忘了fine_y也要传输
		}
	};


	// 为 "生效" 的背景块移位器填充数据，准备在扫描线中输出接下来的8个像素。
	auto LoadBackgroundShifters = [&]() {
		// 每一次PPU更新，我们都要计算一个像素。这些移位器沿途移位1位，为像素合成器提供它所需要的二进制信息。
		// 它有16位宽，因为上面的8位是正在绘制的8个像素，下面的8位是将要绘制的8个像素。当然，这意味着所需的位总是移位器的MSB。
		// 然而，"fine_x "滚动也在其中发挥了作用，这在后面会看到，所以事实上我们可以选择前8位中的任何一位。

		// 将即将绘制的下一个pattern的数据填充进来
		bg_shifter_pattern_lo = (bg_shifter_pattern_lo & 0xFF00) | bg_next_tile_lsb;
		bg_shifter_pattern_hi = (bg_shifter_pattern_hi & 0xFF00) | bg_next_tile_msb;

		// 属性位并不是每个像素都有变化，而是每8个像素都有变化，但为了方便起见，它们与pattern移位器同步，
		// 所以这里我们把attribute table中字节的低2位 代表当前8个像素和下一个8个像素使用的调色板，并把它们 "膨胀 "为8位的字节。
		// 0b表示2进制数 ((0x01 & 0b01) & 0xFF)==0xFF,也就是说这个操作会将 一个bit 1扩展为 8位bit1 ，一个bit 0扩展为一个8位bit0
		bg_shifter_attrib_lo = (bg_shifter_attrib_lo & 0xFF00) | ((bg_next_tile_attrib & 0b01) ? 0xFF : 0x00);  
		bg_shifter_attrib_hi = (bg_shifter_attrib_hi & 0xFF00) | ((bg_next_tile_attrib & 0b10) ? 0xFF : 0x00);

	};

	// 每个周期，存储模式和属性信息的移位器将其内容移位1位。这是因为每个周期，输出都会增加一个像素。
	// 这意味着相对而言，移位器的状态与扫描线的8个像素部分所绘制的像素是同步的。
	auto UpdateShifter = [&]() {
		// 如果背景绘制被允许，即mask寄存器的 render_background位为1
		if (mask.render_background) {
			// 将图案块左移一位
			bg_shifter_pattern_lo <<= 1;
			bg_shifter_pattern_hi <<= 1;

			// 将对应的属性块左移一位
			bg_shifter_attrib_lo <<= 1;
			bg_shifter_attrib_hi <<= 1;
		}

		// 如果精灵绘制被允许，即mask寄存器的 render_sprites位为1，且cycle在绘制的时钟
		if (mask.render_sprites && cycle >= 1 && cycle < 258) {
			for (int i = 0; i < sprite_count; i++) {
				if (spriteScanline[i].x > 0) {
					// 如果还未绘制到精灵所在的位置，直接将对应的x值减一，x表示精灵所在的位置与当前绘制到的位置的距离
					spriteScanline[i].x--;
				}
				else {
					// 如果绘制到精灵本体了，那就将对应的图像块左移一位
					sprite_shifter_pattern_lo[i] <<= 1;
					sprite_shifter_pattern_hi[i] <<= 1;
				}
			}
		}
	};


	// 除了1条扫描线之外，所有的扫描线对用户来说都是可见的。预渲染扫描线-1，是用来配置第一个可见扫描线0的 "移位器 "的。
	// 下面这个if语句控制 -1到239号scanline的绘制
	if (scanline >= -1 && scanline < 240) {
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 开始绘制背景
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (scanline == 0 && cycle == 0) {
			// 跳过第一个时钟周期
			cycle = 1;
		}

		if (scanline == -1 && cycle == 1) {
			// 开始新的图像帧绘制，需要清除status的vertical_blank标志位
			status.vertical_blank = 0;
			// 清除精灵溢出标志位，一个scanline上最多8个精灵
			status.sprite_overflow = 0;
			// 清除精灵0命中标志位
			status.sprite_zero_hit = 0;
			// 清除所有精灵图案移位器
			for (int i = 0; i < 8; i++) {
				sprite_shifter_pattern_lo[i] = 0x00;
				sprite_shifter_pattern_hi[i] = 0x00;
			}
		}

		if ((cycle >= 2 && cycle < 258) || (cycle >= 321 && cycle < 338)) {
			// 只要在以上指定的区间内就更新移位器
			UpdateShifter();

			// 在这些周期中，我们正在收集和处理可见的数据。"移位器 "已经在前一个scanline结束时预装了这个scanline开始的数据。
			// 一旦我们离开可见区域，就进入休眠状态，直到为下一个扫描线预装移位器。
			
			// 幸运的是，对于背景渲染来说，我们每2个时钟周期就会经历一个相当可重复的事件序列。
			switch ((cycle - 1) % 8) {
			case 0:
				// 将当前的背景图案块加载进移位器
				LoadBackgroundShifters();

				// 获取下一个背景图案块ID
				// vram_addr.reg的低12位由 nametable_y: 1 nametable_x : 1; coarse_y : 5;coarse_x : 5; 组成，刚好可以索引到对应的位置
				// vram_addr.reg & 0x0FFF 获取在哪个name table中，以及具体的位置，然后加上0x2000偏移量获取最终地址
				// 然后从name table中读取pattern table中的坐标id
				bg_next_tile_id = ppuRead(0x2000 | (vram_addr.reg & 0x0FFF));  // 这里是读取name table
				break;

			case 2:
				// 获取下一个对应的attribute表的字节

				// 回顾一下，每个name table有两行单元格，它们不是图像块的索引信息，而是代表颜色属性信息，表明哪个调色板应用于屏幕上的哪个区域。
				// 重要的是，背景瓦片和调色板之间不是1对1的对应关系。两行块索引数据可以容纳64个字节。因此，我们可以假设这些属性会影响到屏幕上的8x8个区域的命名表。
				// 考虑到256x240的工作分辨率，我们可以进一步假设每个区域在屏幕空间中是32x32像素，或4x4个瓦片。四个系统调色板被分配给背景渲染，所以一个调色板可以只用2位来指定。
				// 因此，该属性字节可以指定4个不同的调色板。因此，我们甚至可以进一步假设，一个调色板被应用于4x4瓦片区的2x2瓦片组合。
				// 背景瓦片在本地 "共享 "一个调色板的事实就是为什么在一些游戏中你会看到屏幕边缘的颜色失真的原因。

				// 像之前选择图案块ID时一样，我们可以使用循环寄存器的底部12位，但我们需要使实现粒度更粗一点，因为我们要的不是特定的块，而是一组4x4块的属性字节，
				// 或者换句话说，我们把32x32的地址除以4，得到一个等效的8x8地址，然后把这个地址偏移到目标name table的属性部分。
				// https://www.jianshu.com/p/c5b02614e4a7

				// 下面代码找到下一个块对应的属性的2个bit
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
				// 从图案存储器中获取下一个背景块的LSB位平面 已经从nametable中读取了瓦片ID。我们将使用这个ID索引到图案存储器中，
				// 以找到正确的精灵（假设精灵位于该存储器中8x8像素的边界上，即使存在8x16的精灵，它们也是如此，因为背景瓦片总是8x8）。
				// 由于精灵的深度为1比特，但宽度为8像素，我们可以将整个精灵行表示为一个字节，因此向图案存储器中偏移很容易。
				// 总共有8KB，所以我们需要一个13位的地址。

				// 命令解释：
				// (control.pattern_background << 12) 找到正确的pattern table，背景pattern或精灵pattern，每一个大小为8KB
				// ((uint16_t)bg_next_tile_id << 4) 因为每一个图像块有16个字节，所以需要乘以16
				// (vram_addr.fine_y) 指示绘制到当前图案块的哪一行了
				// +0 表明不用偏移量

				bg_next_tile_lsb = ppuRead((control.pattern_background << 12)
					+ ((uint16_t)bg_next_tile_id << 4)
					+ (vram_addr.fine_y) + 0);

				break;

			case 6:
				// 和上一个类似，这里是获取背景块的MSB位平面
				// 注意：这里最后是+8，因为需要偏移8个字节获取MSB
				bg_next_tile_msb = ppuRead((control.pattern_background << 12)
					+ ((uint16_t)bg_next_tile_id << 4)
					+ (vram_addr.fine_y) + 8);
				break;

			case 7:
				// 将背景瓦片的 "指针 "水平地增加到命名表内存中的下一个瓦片。
				// 注意这可能会跨越名字表的边界，这有点复杂，但对于实现滚动是必不可少的。
				IncrementScrollX();

				break;
			}
		}

		// 当前scanline的不可见区域处理
		if (cycle == 256) {
			IncrementScrollY();
		}

		if (cycle == 257) {
			// 准备下一个扫描行的数据和更新vram中的横坐标数据
			LoadBackgroundShifters();
			TransferAddressX();
		}

		if (cycle == 338 || cycle == 340) {
			// 读取下一个图像块的id
			bg_next_tile_id = ppuRead(0x2000 | (vram_addr.reg) & 0x0FFF);
		}

		if (scanline == -1 && cycle >= 280 && cycle < 305) {
			// 刚刚结束了vertical blank状态，所以更新vram中的纵坐标信息，以渲染背景
			TransferAddressY();
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 开始绘制精灵
		///////////////////////////////////////////////////////////////////////////////////////////////////////////

		// PPU在不绘制背景瓦片的区域内连续加载精灵信息。取而代之的是，我将一次性执行所有精灵的评估。
		// NES并不是这样做的。这使我们更容易看到精灵评估的过程。
		if (cycle == 257 && scanline >= 0) {
			// 在时钟周期为257时结束了scanline中可视区域的绘制
			// 所以在这段区间计算下一条scaline中会出现多少个精灵，然后预加载到buffer里面

			// 首先清除以下buffer中的内容
			std::memset(spriteScanline, 0xFF, 8 * sizeof(sObjectAttributeEntry));

			// sprite_count 控制绘制的精灵个数，一个扫描线上最多8个
			sprite_count = 0;

			// 清除精灵移位器
			for (uint8_t i = 0; i < 8; i++) {
				sprite_shifter_pattern_lo[i] = 0;
				sprite_shifter_pattern_hi[i] = 0;
			}

			// 第三，评估哪些精灵在下一个扫描线中是可见的。我们需要遍历OAM，直到我们找到8个精灵的Y位置和高度都在下一个扫描线的垂直范围内。
			// 一旦我们找到了8个或用尽了OAM，我们就停止。现在，注意我数到了9个精灵。这是为了在有超过8个精灵的情况下，我可以设置精灵溢出标志。
			
			uint8_t nOAMEntry = 0;  // 记录遍历OAM内存中条目的下标

			// 先清除以下精灵0命中标志的状态
			bSpriteZeroHitPossible = false;

			// 因为精灵的绘制优先级是取决与在OAM内存中的顺序的，所以这样遍历之后的顺序优先级spriteScanline是没有变化的，依然最前最优先
			while (nOAMEntry < 64 && sprite_count < 9) {
				// 在计算精灵位置与扫描线的距离时需要转换为有符号数，这样才知道是否相交了
				int16_t diff = ((int16_t)scanline - (int16_t)OAM[nOAMEntry].y);

				// 如果距离为正数，那么检查精灵的身体是否在scanline之上，因为精灵y坐标是左上角y坐标
				// 同时，因为精灵分为8x8大小与8x16大小，所以需要根据实际大小进行判断
				if (diff >= 0 && diff < (control.sprite_size ? 16 : 8)) {
					// 进入了if语句，说明精灵需要被绘制
					// 控制最多写入8个精灵
					if (sprite_count < 8) {
						// 在绘制0号精灵的时候设置 精灵0命中标志
						if (nOAMEntry == 0) {
							bSpriteZeroHitPossible = true;
						}
						// 将OAM中精灵数据传输到buffer中
						memcpy(&spriteScanline[sprite_count], &OAM[nOAMEntry], sizeof(sObjectAttributeEntry));
						// 绘制精灵的数量+1
						sprite_count++;
					}
				}
				// 精灵索引
				nOAMEntry++;
			}

			// 如果需要绘制的精灵数量大于8个，设置status寄存器中sprite overflow标志
			status.sprite_overflow = (sprite_count > 8);
		}

		if (cycle == 340) {
			// cycle为340的时候，进入了scanline的最后位置，在此处准备 精灵移位器
			for (uint8_t i = 0; i < sprite_count; i++) {
				// 我们需要以正确的垂直偏移量提取精灵的8位行模式。精灵的大小也会影响这一点，
				// 因为精灵可能是8行或16行高。此外，精灵在垂直和水平方向上都可以 翻转 。所以这里有很多事情要做
				
				uint8_t sprite_pattern_bits_lo = 0x00, sprite_pattern_bits_hi = 0x00;
				uint16_t sprite_pattern_addr_lo = 0x00, sprite_pattern_addr_hi = 0x00;

				// 确定包含模式数据的字节的内存地址。我们只需要lo模式的地址，因为hi模式的地址总是与lo地址偏移8。
				if (!control.sprite_size) {
					// 8x8的精灵大小
					if (!(spriteScanline[i].attribute & 0x80)) {  // 是否垂直反转精灵
						// 不垂直翻转
						sprite_pattern_addr_lo =
							(control.pattern_sprite << 12)  // 哪一张图案表? 0KB 或者 4KB 偏移量
							| (spriteScanline[i].id << 4)  // 图案开始位置在哪里? 一个块的大小为16，所以需要乘以16
							| (scanline - spriteScanline[i].y); //绘制到哪一行了 (0->7)
					}
					else {
						// 垂直翻转
						sprite_pattern_addr_lo =
							(control.pattern_sprite << 12)  // 哪一张图案表? 0KB 或者 4KB 偏移量
							| (spriteScanline[i].id << 4)  // 图案开始位置在哪里? 一个块的大小为16，所以需要乘以16
							| (7 - (scanline - spriteScanline[i].y)); // 绘制到哪一行了 (7->0)
					}
				}
				else {
					// 8x16精灵大小
					if (!(spriteScanline[i].attribute & 0x80)) {  // 是否垂直反转精灵
						// 不垂直翻转
						if (scanline - spriteScanline[i].y < 8)
						{
							// 读取前半截
							sprite_pattern_addr_lo =
								((spriteScanline[i].id & 0x01) << 12)  // 哪一张图案表? 0KB or 4KB offset
								| ((spriteScanline[i].id & 0xFE) << 4)  // 图案开始位置在哪里? 一个块的大小为16，所以需要乘以16
								| ((scanline - spriteScanline[i].y) & 0x07); // 绘制到哪一行了 (0->7)
						}
						else
						{
							// 读取后半截
							sprite_pattern_addr_lo =
								((spriteScanline[i].id & 0x01) << 12)   // 哪一张图案表? 0KB or 4KB offset
								| (((spriteScanline[i].id & 0xFE) + 1) << 4)  // 图案开始位置在哪里? 一个块的大小为16，所以需要乘以16
								| ((scanline - spriteScanline[i].y) & 0x07);  // 绘制到哪一行了 (0->7)
						}
					}
					else {
						// 垂直翻转
						if (scanline - spriteScanline[i].y < 8)
						{
							// 读取前半截
							sprite_pattern_addr_lo =
								((spriteScanline[i].id & 0x01) << 12)    // 哪一张图案表? 0KB or 4KB offset
								| (((spriteScanline[i].id & 0xFE) + 1) << 4)    // 图案开始位置在哪里? 一个块的大小为16，所以需要乘以16
								| (7 - (scanline - spriteScanline[i].y) & 0x07);  // 绘制到哪一行了 (7->0)
						}
						else
						{
							// 读取后半截
							sprite_pattern_addr_lo =
								((spriteScanline[i].id & 0x01) << 12)     // 哪一张图案表? 0KB or 4KB offset
								| ((spriteScanline[i].id & 0xFE) << 4)    // 图案开始位置在哪里? 一个块的大小为16，所以需要乘以16
								| (7 - (scanline - spriteScanline[i].y) & 0x07);  // 绘制到哪一行了 (7->0)
						}
					}
				}

				// 图案的高8位永远相对于低8位偏移8个字节
				sprite_pattern_addr_hi = sprite_pattern_addr_lo + 8;

				// 现在根据图案表的地址读取精灵对应的图案
				sprite_pattern_bits_lo = ppuRead(sprite_pattern_addr_lo);
				sprite_pattern_bits_hi = ppuRead(sprite_pattern_addr_hi);

				// 实现精灵水平翻转操作
				if (spriteScanline[i].attribute & 0x40) {  // 检查标志位
					// 水平翻转操作的lambda函数  https://stackoverflow.com/a/2602885
					auto flipbyte = [](uint8_t b)
					{
						b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
						b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
						b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
						return b;
					};

					// 开始翻转
					sprite_pattern_bits_lo = flipbyte(sprite_pattern_bits_lo);
					sprite_pattern_bits_hi = flipbyte(sprite_pattern_bits_hi);
				}

				// 将最终的到的精灵图案加载进 移位器里面，准备下一个扫描行的绘制
				sprite_shifter_pattern_lo[i] = sprite_pattern_bits_lo;
				sprite_shifter_pattern_hi[i] = sprite_pattern_bits_hi;
			}
		}
	}

	// 绘制界面刚完成，不做任何事
	if (scanline == 240) {
		
	}

	// vertical blank区间
	if (scanline >= 241 && scanline < 261) {
		if (scanline == 241 && cycle == 1) {
			// 进入这一个指定点时，设置status寄存器的vertical_blank位
			// 如果controler寄存器允许释放中断的话，那就释放中断通知CPU这一帧的图像绘制完成了
			status.vertical_blank = 1;

			if (control.enable_nmi) {
				nmi = true;
			}
		}
	}


	///////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////// 组装背景与精灵的图案
	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 背景阶段====================================================
	// 下面5个比特组合获取当前像素的颜色
	uint8_t bg_pixel = 0x00;  // 2比特的像素值
	uint8_t bg_palette = 0x00;  // 3比特的调色板索引

	// 只有mask的render_background为1才能够进行背景的绘制
	if (mask.render_background) {
		// 通过选择相关的位来处理像素选择，这取决于精细的x滚动。
		// 这样做的效果是将所有的背景渲染偏移一个设定的像素数，允许平滑滚动。
		uint16_t bit_mux = 0x8000 >> fine_x;

		// 从移位器中取出背景图像像素
		uint8_t p0_pixel = (bg_shifter_pattern_lo & bit_mux) > 0;
		uint8_t p1_pixel = (bg_shifter_pattern_hi & bit_mux) > 0;

		// 组合高低位，形成2比特的像素值
		bg_pixel = (p1_pixel << 1) | p0_pixel;

		// 从移位器中取出背景属性值
		uint8_t bg_pal0 = (bg_shifter_attrib_lo & bit_mux) > 0;
		uint8_t bg_pal1 = (bg_shifter_attrib_hi & bit_mux) > 0;

		// 组合attribute table中的对应高低位，获取调色板索引
		bg_palette = (bg_pal1 << 1) | bg_pal0;
	}


	// 精灵阶段====================================================
	// 下面5个比特组合获取当前像素的颜色
	uint8_t fg_pixel = 0x00;  // 2比特的像素值
	uint8_t fg_palette = 0x00;  // 3比特的调色板索引
	// 下面1个比特表示背景优先级，是否比背景优先绘制
	uint8_t fg_priority = 0x00;  //为1表示比背景重要，反之，背景重要

	// 只有mask的render_background为1才能够进行精灵的绘制
	if (mask.render_sprites) {
		// 遍历当前scanline中的所有精灵

		bSpriteZeroBeingRendered = false;  // 指示是否是第0个精灵

		for (uint8_t i = 0; i < sprite_count; i++) {
			// spriteScanline[i].x为0时，表明绘制到了当前精灵
			if (spriteScanline[i].x == 0) {
				// 注意 Fine X滚动并不适用于精灵，游戏应该保持它们与背景的关系。所以我们就用移位器的MSB来做

				// 从移位器中取出精灵图像像素
				uint8_t fg_pixel_lo = (sprite_shifter_pattern_lo[i] & 0x80) > 0;
				uint8_t fg_pixel_hi = (sprite_shifter_pattern_hi[i] & 0x80) > 0;
				
				// 组合高低位，形成2比特的像素值
				fg_pixel = (fg_pixel_hi << 1) | fg_pixel_lo;

				// 从移位器中取出背景属性值
				fg_palette = (spriteScanline[i].attribute & 0x03) + 0x04;  // 精灵的调色板编号是直接存在spriteScanline[i].attribute的最低2位的，+0x04表示选用属于精灵的0x3F10处调色板
				// 获取优先级，确定背景与精灵谁显示在屏幕上，这可以实现马里奥进墙的效果
				fg_priority = (spriteScanline[i].attribute & 0x20) == 0;

				// 如果像素不透明，那就将其绘制出来
				if (fg_pixel != 0) {
					if (i == 0) { 
						bSpriteZeroBeingRendered = true;
					}
					// 画出该像素遇到的第一个不透明精灵像素即可，因为越靠前的精灵优先级越高，只要画出该位置遇到的第一个不透明像素所代表的精灵的颜色即可
					break;
				}
			}
		}
	}


	// 组合背景与精灵
	// 经过上面两步，我们拥有了背景像素与精灵像素，因此在下面对它们进行组合，获得最终绘制在屏幕上的像素点
	uint8_t pixel = 0x00;  // 最终的像素
	uint8_t palette = 0x00;  // 最终的调色盘

	if (bg_pixel == 0 && fg_pixel == 0) {
		// 如果两者像素都为0，表示透明
		// 那就画出背景色
		pixel = 0x00;
		palette = 0x00;
	}
	else if (bg_pixel == 0 && fg_pixel > 0) {
		// 背景透明，精灵不透明，画出精灵像素
		pixel = fg_pixel;
		palette = fg_palette;
	}
	else if (bg_pixel > 0 && fg_pixel == 0) {
		// 背景不透明，精灵透明，画出背景像素
		pixel = bg_pixel;
		palette = bg_palette;
	}
	else if (bg_pixel > 0 && fg_pixel > 0) {
		// 都不透明的话就需要根据优先级决定哪一部分显示
		if (fg_priority) {
			// 如果精灵优先级比较高，直接显示精灵
			pixel = fg_pixel;
			palette = fg_palette;
		}
		else {
			// 否则显示背景
			pixel = bg_pixel;
			palette = bg_palette;
		}

		// 精灵0命中检测
		if (bSpriteZeroHitPossible && bSpriteZeroBeingRendered) {
			// 在精灵与背景都允许绘制的情况下才检测
			if (mask.render_background & mask.render_sprites) {
				// 屏幕的左边缘有特定的开关来控制其外观。这用于平滑滚动时的不一致（因为精灵的X坐标必须>=0）。
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

	// 终于，显示在屏幕上的最终像素值与调色板值终于确定了，现在屏幕上画出该像素
	// 设置像素值
	// 注意：这里的第一个参数是纵坐标，第二个参数是横坐标
	if (cycle>0 && cycle <= 256 && scanline>=0 && scanline <= 240) {
		m_emulatorScreen.setPixel(cycle - 1, scanline, GetColourFromPaletteRam(palette, pixel));
	}

	// 控制绘制进程
	cycle++;

	if (cycle >= 341) {
		// 画完一行了，重置cycle，增加纵轴
		cycle = 0;
		scanline++;
		if (scanline >= 261) {
			// scanline移动到-1
			scanline = -1;
			// 指示这一帧绘制完毕
			frame_complete = true;
		}
	}
}
