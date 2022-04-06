#include"Bus.h"

/*
* 默认构造函数 
*/
Bus::Bus()
{
	// 在构造函数中连接CPU
	cpu.ConnectBus(this);
}

/*
* 因为没有动态分配资源，所以析构函数中为空
*/
Bus::~Bus()
{
}

/*
* 定义CPU在总线上写入数据的方法，该方法将CPU传到总线的不同地址翻译，转换为对应总线上连接的设备的地址
*/
void Bus::cpuWrite(uint16_t addr, uint8_t data)
{
	/*
	* 其他未处理的地址空间，即0x4000-0x401F中未处理到的地址 与 0x4020-0x5FFF这段地址，这两段地址中包含 APU(声音处理寄存器)和SRAM的地址空间和扩展ROM空间
	*  0x4020-0x5FFF Nesdev 的论坛上有篇解释这块区域的帖子，简单来讲，该区域用于一些 Mapper 扩展用，大部分情况用不到 https://links.jianshu.com/go?to=https%3A%2F%2Fforums.nesdev.com%2Fviewtopic.php%3Ff%3D3%26t%3D12936
	*/
	if (cart->cpuWrite(addr, data)) {
		// 这里映射的是CPU的 0x8000-0xFFFF 这一段地址空间,每当CPU写入这一段地址空间上的地址时，将其映射到cartridge设备上去
		// 写操作交给cartridge处理
		// 有时候时不止映射到这一点空间的，有些游戏卡上的RAM会映射到0x6000-0x7FFF这一段地址上去
	}
	else if (addr >= 0x0000 && addr <= 0x1FFF) {
		// 这里映射的是CPU的 0x0000-0x1FFF 这一段地址空间,每当CPU写入这一段地址空间上的地址时，将其映射到2KB大小内存设备上
		// 虽然nes的内存寻址空间有8KB这么大，但实际上nes只有2KB内存，因此nes中采用了mirror技术将8KB的寻址空间映射到2KB的内存上
		// 使用&运算符与掩码0x07FF即可将地址 0x0000-0x1FFF =》 0x0000-0x07FF
		cpuRam[addr & 0x07FF] = data;  
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF) {
		// 这里映射的是CPU的 0x2000-0x3FFF 这一段地址空间,每当CPU写入这一段地址空间上的地址时，将其映射到PPU的8个寄存器上去
		// 虽然nes的PPU寻址空间有8KB这么大，但实际上PPU上只有8个寄存器，也就是8个byte，因此nes中采用了mirror技术将8KB的寻址空间映射到8byte的寄存器地址上去
		// 使用&运算符与掩码0x07FF即可将地址 0x2000-0x3FFF =》 0x2000-0x2007
		ppu.cpuWrite(addr & 0x0007, data);
	}
	else if (addr == 0x4014) {
		// 如果cpu向地址0x4014写入数据，这意味着CPU想要开启dma传输操作，每一次dma传输都会传输一整个page（也就是256字节），这时cpu将会传入需要传输的page号作为data
		// 所以在此处需要将传入的page号（也就是data值）赋给dma_page变量，指定需要传输的page，并将该页的页面偏移量置为0,表示从该页的第一个字节开始传输
		// 同时，将dma_transfer标志变量设置为true，表示开启dma传输操作
		// 当dma_transfer为true时，CPU将被挂起，直到此次dma传输操作结束，一共需要512个CPU时钟
		dma_page = data;
		dma_addr = 0x00;
		dma_transfer = true;
	}
	else if (addr >= 0x4016 && addr <= 0x4017) {
		// 这两个地址分别代表一号玩家与二号玩家的手柄寄存器的地址，nes游戏手柄一共八个键位，对应controler的8个bit
		// 从高到低的位分别映射到以下八个操作
		// A B SELECT START UP DOWN LEFT RIGHT
		// controler一共两个地址，0x0与0x1
		// 所以使用&运算符将 0x4016-0x4017 映射到 0x0-0x1
		// 此时CPU不是写入数据，而是获取此时的controler寄存器状态，保存在controller_state变量中，以供后面cpuRead操作慢慢读取
		// 读取的时候比较奇怪，按道理来讲 8 个按键刚好可以用 1 个 byte 表示，读一次就可以了，但是 NES 读取的时候却是串行的，读 8 次，每次读一个按键，这样做应该是为了兼容性第三方控制器
		// 所以当CPU写这个地址的时候其实指示保存当前的手柄寄存器的值
		controller_state[addr & 0x0001] = controller[addr & 0x0001];
	}

}

/*
* 定义CPU在总线上读取数据的方法，该方法将CPU传到总线的不同地址翻译，转换为对应总线上连接的设备的地址
*/
uint8_t Bus::cpuRead(uint16_t addr, bool bReadOnly)
{
	uint8_t data = 0x00;
	/*
	* 其他未处理的地址空间，即0x4000-0x401F中未处理到的地址 与 0x4020-0x5FFF这段地址，这两段地址中包含 APU(声音处理寄存器)和SRAM的地址空间和扩展ROM空间
	*  0x4020-0x5FFF Nesdev 的论坛上有篇解释这块区域的帖子，简单来讲，该区域用于一些 Mapper 扩展用，大部分情况用不到 https://links.jianshu.com/go?to=https%3A%2F%2Fforums.nesdev.com%2Fviewtopic.php%3Ff%3D3%26t%3D12936
	*/
	if (cart->cpuRead(addr, data)) {
		// 这里映射的是CPU的 0x8000-0xFFFF 这一段地址空间,每当CPU写入这一段地址空间上的地址时，将其映射到cartridge设备上去
		// 读操作交给cartridge处理，data这里传的是引用
		// 有时候时不止映射到这一点空间的，有些游戏卡上的RAM会映射到0x6000-0x7FFF这一段地址上去
	}
	else if (addr >= 0x0000 && addr <= 0x1FFF) {
		// 这里映射的是CPU的 0x0000-0x1FFF 这一段地址空间,每当CPU读取这一段地址空间上的地址时，将其映射到2KB大小内存设备上
		// 虽然nes的内存寻址空间有8KB这么大，但实际上nes只有2KB内存，因此nes中采用了mirror技术将8KB的寻址空间映射到2KB的内存上
		// 使用&运算符与掩码0x07FF即可将地址 0x0000-0x1FFF =》 0x0000-0x07FF
		data = cpuRam[addr & 0x07FF];
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF) {
		// 这里映射的是CPU的 0x2000-0x3FFF 这一段地址空间,每当CPU读取这一段地址空间上的地址时，将其映射到PPU的8个寄存器上去
		// 虽然nes的PPU寻址空间有8KB这么大，但实际上PPU上只有8个寄存器，也就是8个byte，因此nes中采用了mirror技术将8KB的寻址空间映射到8byte的寄存器地址上去
		// 使用&运算符与掩码0x07FF即可将地址 0x2000-0x3FFF =》 0x2000-0x2007
		data = ppu.cpuRead(addr & 0x0007, bReadOnly);
	}
	else if (addr >= 0x4016 && addr <= 0x4017) {
		// 这两个地址分别代表一号玩家与二号玩家的手柄寄存器的地址，nes游戏手柄一共八个键位，对应controler的8个bit
		// 从高到低的位分别映射到以下八个操作
		// A B SELECT START UP DOWN LEFT RIGHT
		// controler一共两个地址，0x0与0x1
		// 所以使用&运算符将 0x4016-0x4017 映射到 0x0-0x1
		// 读取的时候比较奇怪，按道理来讲 8 个按键刚好可以用 1 个 byte 表示，读一次就可以了，但是 NES 读取的时候却是串行的，读 8 次，每次读一个按键，这样做应该是为了兼容性第三方控制器
		// 因此CPU每读取一次这段地址，我们就将controller_state保存的最高一位返回，然后再左移一位，保证下次读的是另外一个值
		data = (controller_state[addr & 0x0001] & 0x80) > 0;
		controller_state[addr & 0x0001] <<= 1;
	}

	return data;
}


/*
* 定义将游戏卡连接到两条总线上的方法，在Bus中获取cartridge的指针
*/
void Bus::insertCartridge(const std::shared_ptr<Cartridge>& cartridge)
{
	cart = cartridge;  // 将游戏卡连接到CPU总线上,CPU的0x8000-0xFFFF需要映射到cartridge的PRG段上
	ppu.ConnectCartridge(cartridge);  // 将游戏卡连接到PPU总线上，PPU的0x0000-0x1FFF需要映射到cartridge的CHR段上
}

/*
* 重置CPU总线状态以及连接在总线上的所有设备的状态
*/
void Bus::reset()
{
	cart->reset();  // 重置cartridge状态，其实是重置cartridge中的Mapper状态
	cpu.reset();  // 重置cpu状态
	ppu.reset();  // 重置ppu状态
	nSystemClockCounter = 0;  // 重置总时钟计数器
	dma_page = 0x00;  // 重置dma传输page号变量
	dma_addr = 0x00;  // 重置dma传输页面偏移量变量
	dma_data = 0x00;  // 重置dma传输数据的值
	dma_dummy = true; // 重置保证dma传输从偶数时钟开启的变量
	dma_transfer = false;  // 重置是否发生了dma传输的状态变量 
}

/*
* 表示在总线上传输的时钟信号，时钟没滴答一次，连接在总线上的做出相应的操作
* 本函数是模拟器的核心函数，程序运行的速度取决该函数被调用的次数
*/
void Bus::clock()
{
	// 总线上每传来一个时钟信号PPU就响应一次
	ppu.clock();

	// CPU的时钟速度比PPU慢3倍，所以ppu每相应3次，CPU只响应1次
	if (nSystemClockCounter % 3 == 0) {
		// 若是此时正在进行dma传输，那么CPU将会被挂起，一次dma传输操作共传输256KB数据
		// 一共会消耗512个CPU时钟，dma_transfer变量指示是否在进行dma传输
		if (dma_transfer) {
			// dma传输还有一个要求，dma传输的开始时间必须要是偶数时钟
			// 所以使用dma_dummy控制dma传输一定开始在偶数时钟
			if (dma_dummy) {
				// 如果当前系统时钟是奇数，那么三个系统时钟后一定是偶数，当前时钟若是偶数则会再等待三个系统时钟，等到系统时钟奇数了再将dma_dummy置为false
				if (nSystemClockCounter % 2 == 1) {
					dma_dummy = false;  // 设置为false，下一次一定会进入dma传输
				}
			}
			else {
				// 现在系统时钟一定是偶数了，所以开始进行dma传输
				// dma传输一个字节需要两个CPU时钟周期(3个系统时钟周期)，在偶数系统时钟周期读取数据，奇数系统时间周期写数据到PPU的OAM内存中
				if (nSystemClockCounter % 2 == 0) {
					// 在偶数系统时钟周期读取数据到dma_data中
					dma_data = cpuRead(dma_page << 8 | dma_addr);  // page号+在该page中的偏移量确定读取的数据，数据保存在dma_data变量中
				}
				else {
					// 奇数系统时钟周期写入数据到PPU的OAM内存中
					ppu.pOAM[dma_addr] = dma_data;  // 因为OAM内存一共256字节，所以可以直接使用dma_addr来索引
					dma_addr++;  // 向后移动索引
					// 因为dma_addr只有8比特，所以最大只能表示255，所以当dma_addr等于0时说明dma传输完成了
					if (dma_addr == 0x00) {
						dma_dummy = true;  // 置为true，希望下一次dma传输能够保证从偶数系统时钟开启
						dma_transfer = false;  // 置为false，表示此次dma传输结束，取消CPU挂起
					}
				}
			}
		}
		else {
			// 如果没有进行dma操作，则CPU未被挂起，CPU响应一次
			cpu.clock();
		}
	}

	// PPU具有触发中断的能力，若PPU发出中断信号，则表示PPU的绘制进入vertical blanking阶段，就是超出屏幕显示范围之外的部分
	// 当PPU发出该信号时，CPU应进入nmi（不可屏蔽中断），CPU的nmi中断会调用CPU寻址范围中的0xFFFA-0xFFFB中保存函数入口，从而做出对应的操作
	// 在每一个系统周期都需要检查以下PPU是否释放了nmi信号
	if (ppu.nmi) {
		ppu.nmi = false;  // 重置状态
		cpu.nmi();  // 调用对应CPU的nmi()中断
	}

	nSystemClockCounter++;
}
