#include"CPU6502.h"
#include"Bus.h"

/*
* 默认构造函数
*/
CPU6502::CPU6502()
{
	// 初始化查找表，根据操作码查找对应的指令
	using a = CPU6502;  // 使用别名简介名称
	lookup =
	{
		{ "BRK", &a::BRK, &a::IMM, 7 },{ "ORA", &a::ORA, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "ORA", &a::ORA, &a::ZP0, 3 },{ "ASL", &a::ASL, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PHP", &a::PHP, &a::IMP, 3 },{ "ORA", &a::ORA, &a::IMM, 2 },{ "ASL", &a::ASL, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ABS, 4 },{ "ASL", &a::ASL, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BPL", &a::BPL, &a::REL, 2 },{ "ORA", &a::ORA, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ZPX, 4 },{ "ASL", &a::ASL, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLC", &a::CLC, &a::IMP, 2 },{ "ORA", &a::ORA, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ABX, 4 },{ "ASL", &a::ASL, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "JSR", &a::JSR, &a::ABS, 6 },{ "AND", &a::AND, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "BIT", &a::BIT, &a::ZP0, 3 },{ "AND", &a::AND, &a::ZP0, 3 },{ "ROL", &a::ROL, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PLP", &a::PLP, &a::IMP, 4 },{ "AND", &a::AND, &a::IMM, 2 },{ "ROL", &a::ROL, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "BIT", &a::BIT, &a::ABS, 4 },{ "AND", &a::AND, &a::ABS, 4 },{ "ROL", &a::ROL, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BMI", &a::BMI, &a::REL, 2 },{ "AND", &a::AND, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "AND", &a::AND, &a::ZPX, 4 },{ "ROL", &a::ROL, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SEC", &a::SEC, &a::IMP, 2 },{ "AND", &a::AND, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "AND", &a::AND, &a::ABX, 4 },{ "ROL", &a::ROL, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "RTI", &a::RTI, &a::IMP, 6 },{ "EOR", &a::EOR, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "EOR", &a::EOR, &a::ZP0, 3 },{ "LSR", &a::LSR, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PHA", &a::PHA, &a::IMP, 3 },{ "EOR", &a::EOR, &a::IMM, 2 },{ "LSR", &a::LSR, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "JMP", &a::JMP, &a::ABS, 3 },{ "EOR", &a::EOR, &a::ABS, 4 },{ "LSR", &a::LSR, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BVC", &a::BVC, &a::REL, 2 },{ "EOR", &a::EOR, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "EOR", &a::EOR, &a::ZPX, 4 },{ "LSR", &a::LSR, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLI", &a::CLI, &a::IMP, 2 },{ "EOR", &a::EOR, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "EOR", &a::EOR, &a::ABX, 4 },{ "LSR", &a::LSR, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "RTS", &a::RTS, &a::IMP, 6 },{ "ADC", &a::ADC, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "ADC", &a::ADC, &a::ZP0, 3 },{ "ROR", &a::ROR, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PLA", &a::PLA, &a::IMP, 4 },{ "ADC", &a::ADC, &a::IMM, 2 },{ "ROR", &a::ROR, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "JMP", &a::JMP, &a::IND, 5 },{ "ADC", &a::ADC, &a::ABS, 4 },{ "ROR", &a::ROR, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BVS", &a::BVS, &a::REL, 2 },{ "ADC", &a::ADC, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "ADC", &a::ADC, &a::ZPX, 4 },{ "ROR", &a::ROR, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SEI", &a::SEI, &a::IMP, 2 },{ "ADC", &a::ADC, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "ADC", &a::ADC, &a::ABX, 4 },{ "ROR", &a::ROR, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "???", &a::NOP, &a::IMP, 2 },{ "STA", &a::STA, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "STY", &a::STY, &a::ZP0, 3 },{ "STA", &a::STA, &a::ZP0, 3 },{ "STX", &a::STX, &a::ZP0, 3 },{ "???", &a::XXX, &a::IMP, 3 },{ "DEY", &a::DEY, &a::IMP, 2 },{ "???", &a::NOP, &a::IMP, 2 },{ "TXA", &a::TXA, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "STY", &a::STY, &a::ABS, 4 },{ "STA", &a::STA, &a::ABS, 4 },{ "STX", &a::STX, &a::ABS, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "BCC", &a::BCC, &a::REL, 2 },{ "STA", &a::STA, &a::IZY, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "STY", &a::STY, &a::ZPX, 4 },{ "STA", &a::STA, &a::ZPX, 4 },{ "STX", &a::STX, &a::ZPY, 4 },{ "???", &a::XXX, &a::IMP, 4 },{ "TYA", &a::TYA, &a::IMP, 2 },{ "STA", &a::STA, &a::ABY, 5 },{ "TXS", &a::TXS, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 5 },{ "???", &a::NOP, &a::IMP, 5 },{ "STA", &a::STA, &a::ABX, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "???", &a::XXX, &a::IMP, 5 },
		{ "LDY", &a::LDY, &a::IMM, 2 },{ "LDA", &a::LDA, &a::IZX, 6 },{ "LDX", &a::LDX, &a::IMM, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "LDY", &a::LDY, &a::ZP0, 3 },{ "LDA", &a::LDA, &a::ZP0, 3 },{ "LDX", &a::LDX, &a::ZP0, 3 },{ "???", &a::XXX, &a::IMP, 3 },{ "TAY", &a::TAY, &a::IMP, 2 },{ "LDA", &a::LDA, &a::IMM, 2 },{ "TAX", &a::TAX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "LDY", &a::LDY, &a::ABS, 4 },{ "LDA", &a::LDA, &a::ABS, 4 },{ "LDX", &a::LDX, &a::ABS, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "BCS", &a::BCS, &a::REL, 2 },{ "LDA", &a::LDA, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 5 },{ "LDY", &a::LDY, &a::ZPX, 4 },{ "LDA", &a::LDA, &a::ZPX, 4 },{ "LDX", &a::LDX, &a::ZPY, 4 },{ "???", &a::XXX, &a::IMP, 4 },{ "CLV", &a::CLV, &a::IMP, 2 },{ "LDA", &a::LDA, &a::ABY, 4 },{ "TSX", &a::TSX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 4 },{ "LDY", &a::LDY, &a::ABX, 4 },{ "LDA", &a::LDA, &a::ABX, 4 },{ "LDX", &a::LDX, &a::ABY, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "CPY", &a::CPY, &a::IMM, 2 },{ "CMP", &a::CMP, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "CPY", &a::CPY, &a::ZP0, 3 },{ "CMP", &a::CMP, &a::ZP0, 3 },{ "DEC", &a::DEC, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "INY", &a::INY, &a::IMP, 2 },{ "CMP", &a::CMP, &a::IMM, 2 },{ "DEX", &a::DEX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "CPY", &a::CPY, &a::ABS, 4 },{ "CMP", &a::CMP, &a::ABS, 4 },{ "DEC", &a::DEC, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BNE", &a::BNE, &a::REL, 2 },{ "CMP", &a::CMP, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "CMP", &a::CMP, &a::ZPX, 4 },{ "DEC", &a::DEC, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLD", &a::CLD, &a::IMP, 2 },{ "CMP", &a::CMP, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "CMP", &a::CMP, &a::ABX, 4 },{ "DEC", &a::DEC, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "CPX", &a::CPX, &a::IMM, 2 },{ "SBC", &a::SBC, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "CPX", &a::CPX, &a::ZP0, 3 },{ "SBC", &a::SBC, &a::ZP0, 3 },{ "INC", &a::INC, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "INX", &a::INX, &a::IMP, 2 },{ "SBC", &a::SBC, &a::IMM, 2 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::SBC, &a::IMP, 2 },{ "CPX", &a::CPX, &a::ABS, 4 },{ "SBC", &a::SBC, &a::ABS, 4 },{ "INC", &a::INC, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BEQ", &a::BEQ, &a::REL, 2 },{ "SBC", &a::SBC, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "SBC", &a::SBC, &a::ZPX, 4 },{ "INC", &a::INC, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SED", &a::SED, &a::IMP, 2 },{ "SBC", &a::SBC, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "SBC", &a::SBC, &a::ABX, 4 },{ "INC", &a::INC, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
	};
}
 
/*
* 因为没有动态分配资源，所以析构函数中为空
*/
CPU6502::~CPU6502()
{
}

///////////////////////////////////////////////////////////////////////////
/*
* 将CPU连接到总线上
*/
void CPU6502::ConnectBus(Bus* n)
{
	bus = n;
}


///////////////////////////////////////////////////////////////////////////////
// 从总线上读写数据
/*
* CPU地址范围为0x0000-0xFFFF，一共64KB，CPU的读写需要通过总线发起
*/
uint8_t CPU6502::read(uint16_t addr)
{
	return bus->cpuRead(addr, false);
}

/*
* CPU地址范围为0x0000-0xFFFF，一共64KB，CPU的读写需要通过总线发起
*/
void CPU6502::write(uint16_t addr, uint8_t data) 
{
	bus->cpuWrite(addr, data);
}


////////////////////////////////////////////////////
// status寄存器设置相关方法
/*
* 返回status中指定的bit
*/
uint8_t CPU6502::GetFlag(FLAGS6502 f)
{
	return ((status & f) > 0) ? 1 : 0;
}

/*
* 设置status中指定的bit
*/
void CPU6502::SetFlag(FLAGS6502 f, bool v) 
{
	if (v) {
		status |= f;
	}
	else {
		status &= ~f;
	}
}

/////////////////////////////////////////////////////////////////////////////////
/*
* 获取当前地址指向的数据，除了隐含了数据的指令，若寻址指令类型为 implied类型，则随便返回一个
*/
uint8_t CPU6502::fetch()
{
	if (lookup[opcode].addrmode != &CPU6502::IMP) {
		fetched = read(addr_abs);
	}
	return fetched;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 暴露给外部的一些接口，在硬件中就表示对应阵脚收到了电信号

/*
* 重置CPU到指定的状态，除status外的所有寄存器设置为0x00，status寄存器中除unused位皆置为0
* 程序计数器设置为指定的地址0xFFFC-0xFFFD中的值 ,这里保存着程序下一步操作
* 允许程序员设置跳转到指定的位置，即0xFFFC-0xFFFD中保存的地址
*/
void CPU6502::reset()
{
	// 设置程序计数器为固定位置中的值
	addr_abs = 0xFFFC;
	uint16_t lo = read(addr_abs + 0);  // 读取低8位
	uint16_t hi = read(addr_abs + 1);  // 读取高8位
	pc = lo | (hi << 8);  // 设置程序计数器

	// 重置CPU中寄存器
	a = 0;
	x = 0;
	y = 0;
	stkp = 0xFD;
	status = 0x00 | U;  // 除了unused位外都置为0

	// 将CPU中的辅助变量置为0
	addr_rel = 0x0000;
	addr_abs = 0x0000;
	fetched = 0x00;

	// reset操作需要8个CPU时钟
	cycles = 8;
}


/*
* 当status中的I标志位为0时，irq中断才能被触发，也就是说，当I标志位为0时，IRQ中断被屏蔽
* IRQ可以发生在任何时间，发生irq中断后需要保存现场，即寄存器的值与程序计数器的值，同时设置好status寄存器标志位的值
* 所以将寄存器与程序计数器压栈保存，中断执行完毕后程序可以通过RTI指令回复中断执行之前的状态
* 类似reset操作，IRQ中断发生之后需要将程序计数器指向一个固定位置中保存的值，irq操作指向0xFFFE-0xFFFF中的值
*/
void CPU6502::irq()
{
	// 当status的标志位I为0时才会进入中断处理流程，否则被屏蔽
	if (GetFlag(I) == 0) {
		// 将程序计数器入栈，程序计数器为16位，所以需要入栈两次，每次保存8位
		write(0x0100 + stkp, (pc >> 8) & 0x00FF);  // 首先保存高8位
		stkp--;
		write(0x0100 + stkp, pc & 0x00FF);
		stkp--;

		// 设置status中标志位的值
		SetFlag(B, false);
		SetFlag(U, true);
		SetFlag(I, true);
		write(0x0100 + stkp, status);  // 保存status寄存器的值
		stkp--;

		// 将程序计数器设置为固定位置中的值
		addr_abs = 0xFFFE;
		uint16_t lo = read(addr_abs + 0);
		uint16_t hi = read(addr_abs + 1);
		pc = (hi << 8) | lo;

		// irq操作需要7个CPU时钟周期
		cycles = 7;
	}
}

/*
* nmi是不可屏蔽中断，该操作与irq差不多，但是设置程序计数器为另一个固定的位置中保存的值，即0xFFFA-0xFFFB
*/
void CPU6502::nmi()
{
	// 保存程序计数器中的值
	write(0x0100 + stkp, (pc >> 8) & 0x00FF);
	stkp--;
	write(0x0100 + stkp, pc & 0x00FF);
	stkp--;

	SetFlag(B, false);
	SetFlag(U, true);
	SetFlag(I, true);
	write(0x0100 + stkp, status);  // 保存status寄存器的值
	stkp--;

	// 将程序计数器设置为固定位置中的值
	addr_abs = 0xFFFA;
	uint16_t lo = read(addr_abs + 0);
	uint16_t hi = read(addr_abs + 1);
	pc = (hi << 8) | lo;

	// nmi操作需要8个CPU时钟周期
	cycles = 8;
}

/*
* 定义每一个时钟周期CPU需要做的操作
* 在每个cycles变量为0时取出一个指令，也就是上一个指令完成时取下一个指令
* 首先根据程序计数器当前的位置在lookup表中取出对应的指令，然后执行寻址函数找到对应数据地址
* 最后执行指令对应的操作
*/
void CPU6502::clock() 
{
	// 当上一个指令执行完后，取下一个指令
	if (cycles == 0) {
		// 根据程序计数器的位置取出操作码，一个操作码有8位，根据操作码在lookup查找表中查找对应指令
		opcode = read(pc);

		// 每一次执行操作前都要将status寄存器中的unused位置为1
		SetFlag(U, true);

		pc++;  // 移动程序计数器到下一位置

		cycles = lookup[opcode].cycles;  // 获取当前指令所需的执行周期

		uint8_t addtional_cycle1 = (this->*lookup[opcode].addrmode)();  // 执行当前指令的寻址操作，返回是否需要额外时钟
		uint8_t addtional_cycle2 = (this->*lookup[opcode].operate)();  // 执行当前指令的操作，返回是否需要额外时钟

		cycles += addtional_cycle1 & addtional_cycle2;  // 若以上两个操作都需要额外的一个时钟周期，那么加上一个时钟周期

		// 每一次执行操作后都要将status寄存器中的unused位置为1
		SetFlag(U, true);  
	}
	// 总时钟计数加1
	clock_count++;

	// 每一次调用时钟滴答减去一个cycle
	cycles--;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 寻址操作的实现，在6502CPU中地址一共16位，其中前8位代表page，后8位代表该页上的偏移量
// 也就是说寻址空间中一共有 256 page，每个page一共256偏移量
// 寻址操作中有一部分寻址操作需要额外的时钟周期，比如间接寻址模式中地址跨越了一个page
// 即低地址保存在 0x00FF中，高地址保存在0x0100中时就需要一个额外时钟周期

/*
* 隐含寻址方式，这种指令不需要额外的数据，比如设置状态位的指令，所以不需要计算地址
* 但此函数我们将累加寄存器(a)中的值传送到fetched变量中，这是为了少数命令的执行，比如PHA命令
* 该指令隐式指定了累加寄存器中的值
*/
uint8_t CPU6502::IMP()
{
	fetched = a;
	// 返回0表示不需要额外的时钟周期
	return 0;
}

/*
* 立即数寻址，也就是数据直接就在程序代码之中，所以这里直接将程序计数器的当前值赋给addr_abs即可
*/
uint8_t CPU6502::IMM()
{
	addr_abs = pc;
	pc++;  // 移动程序计数器到下一位置
	// 不需要额外时钟周期
	return 0;
}

/*
* 0页寻址，该指令表示地址存放在第1个page上
* 所以地址的前8位已经确定位0x00了
* 我们只需要获取后8个位
*/
uint8_t CPU6502::ZP0()
{
	addr_abs = read(pc);  // 从程序代码中读取8位地址值
	pc++;  // 移动程序计数器到下一个位置
	addr_abs &= 0x00FF;  // 确保addr_abs高8位为0
	return 0;  // 不需要额外的时钟周期
}

/*
* 0页+x寄存器偏移量寻址
* 同ZP0指令，此指令也在第1页上获取数据，所以地址高8位确定为0x00
* 与ZP0不同的点在于这里还需要加上x寄存器上的偏移量
*/

uint8_t CPU6502::ZPX()
{
	addr_abs = (read(pc) + x);  // 从程序代码中读取8位地址值,同时加上x寄存器中偏移量
	pc++;  // 移动程序计数器到下一个位置
	addr_abs &= 0x00FF;  // 确保addr_abs高8位为0
	return 0;  // 不需要额外的时钟周期
}

/*
* 0页+y寄存器偏移量寻址
* 同ZPX指令，只是所用寄存器改为y寄存器
*/
uint8_t CPU6502::ZPY()
{
	addr_abs = (read(pc) + y);  // 从程序代码中读取8位地址值,同时加上y寄存器中偏移量
	pc++;  // 移动程序计数器到下一个位置
	addr_abs &= 0x00FF;  // 确保addr_abs高8位为0
	return 0;  // 不需要额外的时钟周期
}

/*
* 相对寻址
* 此种模式是给分支指令用的，相对移动的位置必须在-128到127之间
* 也就是说你不能直接跳转到你能够寻址的任何地方
*/
uint8_t CPU6502::REL()
{
	addr_rel = read(pc);  // 从程序代码中读取出需要的相对偏移地址
	pc++;  // 移动程序计数器到下一个位置
	if (addr_rel & 0x80) {  // 检查符号位，判断是否为负数，为负数的话需要将高8位都置为1，因为是补码，正数则不管
		addr_rel |= 0xFF00;  // 将高8位都置为1，因为是补码
	}
	return 0;
}

/*
* 绝对地址寻址
* 这种寻址方式的地址是直接包含在代码中的，所以需要分两次从程序计数器指向的位置读出高低8位
* 然后组装成为最终地址
*/
uint8_t CPU6502::ABS()
{
	uint16_t lo = read(pc);  // 先读出低8位
	pc++;  // 移动程序计数器
	uint16_t hi = read(pc);  // 再读出高8位
	pc++;  // 移动程序计数器

	addr_abs = (hi << 8) | lo;  // 组装高低8位得到最终地址

	return 0;  // 不需要额外时钟周期
}

/*
* 绝对地址寻址+x寄存器中保存的偏移量
* 此寻址模式与ABS寻址类似，只是需要多加一个x寄存器中的偏移量
*/
uint8_t CPU6502::ABX()
{
	uint16_t lo = read(pc);  // 先读出低8位
	pc++;  // 移动程序计数器
	uint16_t hi = read(pc);  // 再读出高8位
	pc++;  // 移动程序计数器

	addr_abs = (hi << 8) | lo;  // 组装高低8位得到地址
	addr_abs += x;  // 加上x寄存器中偏移量获取最终地址

	if ((addr_abs & 0xFF00) != (hi << 8)) {  // 检查是否跨页了，如果跨页则需要一个额外时钟周期
		return 1;
	}

	return 0;  // 未跨页不需要额外时钟周期
}

/*
* 绝对地址寻址+y寄存器中保存的偏移量
* 此寻址模式与ABS寻址类似，只是需要多加一个y寄存器中的偏移量
*/
uint8_t CPU6502::ABY()
{
	uint16_t lo = read(pc);  // 先读出低8位
	pc++;  // 移动程序计数器
	uint16_t hi = read(pc);  // 再读出高8位
	pc++;  // 移动程序计数器

	addr_abs = (hi << 8) | lo;  // 组装高低8位得到地址
	addr_abs += y;  // 加上x寄存器中偏移量获取最终地址

	if ((addr_abs & 0xFF00) != (hi << 8)) {  // 检查是否跨页了，如果跨页则需要一个额外时钟周期
		return 1;
	}

	return 0;  // 未跨页不需要额外时钟周期
}


/*
* 间接寻址方式
* 其实就是C语言中的指针所用的方式
* 首先根据程序计数器找到一个地址，在该地址保存着变量地址，然后根据该地址找到真正的地址
* 这种寻址模式的硬件实现中存在着一个bug，在本模拟器中也将这个bug模拟出来
* 该bug就是当地址读取跨页时，地址并没有进入到下一页，而是到了该页的最开始处
* 具体来说，就是当低位存放地址为0x00FF时高位地址存放处不是0x0100，而是跳到了0x0000
*/
uint8_t CPU6502::IND()
{
	uint16_t ptr_lo = read(pc);  // 读取指针的低8位
	pc++;  // 移动程序计数器
	uint16_t ptr_hi = read(pc);  // 读取指针的高8位
	pc++;   // 移动程序计数器

	uint16_t ptr = (ptr_hi << 8) | ptr_lo;  // 组装获取指针值

	if (ptr_lo == 0x00FF) {  // 模拟bug
		addr_abs = (read(ptr & 0xFF00) << 8) | read(ptr + 0);
	}
	else {  // 从指针指向的地方获取实际的地址
		addr_abs = (read(ptr + 1) << 8) | read(ptr + 0);
	}

	return 0;  // 不需要额外的时钟周期
}

/*
* 间接0页寻址模式+x寄存器中偏移量
* 直接在0号page上找值，所以只需要8位偏移量即可，再加上x寄存器中保存的偏移量取获取最终的地址
*/
uint8_t CPU6502::IZX()
{
	uint16_t t = read(pc);  // 获取在0号page上的偏移量
	pc++;  // 移动程序计数器

	uint16_t lo = read((uint16_t)(t + (uint16_t)x + 0) & 0x00FF);  // 原始偏移量+x保存的偏移量获取低8位
	uint16_t hi = read((uint16_t)(t + (uint16_t)x + 1) & 0x00FF);  // 原始偏移量+x保存的偏移量获取高8位

	addr_abs = (hi << 8) | lo;  // 组装高低8位得到最终地址

	return 0;  // 不需要额外的时钟周期
}

/*
* 间接0页寻址模式+y寄存器中偏移量
* 注意：这里的偏移量的应用方式和IZX上的不同，这里偏移量加载最终地址上，IZX是加在指针上
* 直接在0号page上找值，所以只需要8位偏移量即可，最后在中间地址上再加上y寄存器中保存的偏移量取获取最终的地址
*/
uint8_t CPU6502::IZY()
{
	uint16_t t = read(pc);  // 获取在0号page上的偏移量
	pc++;  // 移动程序计数器

	uint16_t lo = read(t & 0x00FF);  // 获取低8位
	uint16_t hi = read((t + 1) & 0x00FF);  // 获取高8位

	addr_abs = (hi << 8) | lo;  // 组装高低8位中间地址
	addr_abs += y;  // 加上y寄存器中的偏移量，获取最终的地址，和IZX不一样哦，这里的偏移量是加在最终地址上的

	if ((addr_abs & 0xFF00) != (hi << 8)) {  // 如果跨页了就需要一个额外的时钟周期
		return 1;
	}

	return 0;  // 不需要额外的时钟周期
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 以下是指令相关函数
// 指令执行流程可以分为五步
// （1）使用fetch()函数将需要的值存入fetched变量中
// （2）开始执行计算操作
// （3）将计算得到的数据存入目标位置中
// （4）设置status寄存器中的标志位
// （5）返回是否需要额外的时钟周期

/*
* 带有Carry位的加法指令
* 计算函数为 A = A + M + C， A代表累加寄存器a，M代表Memory，也就是内存中的值,C代表status寄存器中的C标志位
* 该指令执行后需要根据情况设置 C, V, N, Z这4个标志位
* 算法解释：
* A  M  R | V | A^R | A^M |~(A^M) | 
* 0  0  0 | 0 |  0  |  0  |   1   |
* 0  0  1 | 1 |  1  |  0  |   1   |
* 0  1  0 | 0 |  0  |  1  |   0   |
* 0  1  1 | 0 |  1  |  1  |   0   |  so V = ~(A^M) & (A^R)
* 1  0  0 | 0 |  1  |  1  |   0   |
* 1  0  1 | 0 |  0  |  1  |   0   |
* 1  1  0 | 1 |  1  |  0  |   1   |
* 1  1  1 | 0 |  0  |  0  |   1   |
* 0和1代表符号位的正负，最终可以得到V = ~(A^M) & (A^R)公式，R表示result也就是相加后的结果的符号位
* 正数+正数=负数，   溢出
* 正数+正数 = 正数， 未溢出
* 负数+负数 = 负数， 未溢出
* 负数+负数 = 正数， 溢出
* 正数+负数 = 负数， 未溢出，正负相加不可能溢出
* 负数+正数 = 正数， 未溢出，正负相加不可能溢出
*/
uint8_t CPU6502::ADC()
{
	// 首先获取数据，调用fetch()函数，将会从指令对应的地址取值放入fetched变量中
	fetch();

	// 做加法，获取 累加寄存器a + 内存中取的值fetched + status寄存器C标志位 的和，存入辅助变量temp中
	temp = (uint16_t)a + (uint16_t)fetched + (uint16_t)GetFlag(C);

	// 根据结果设置标志位
	SetFlag(C, temp & 0xFF00);  // 结果若大于255，需要进位，则设置标志位C为1
	SetFlag(Z, (temp & 0x00FF) == 0);  // 如果结果为0，设置标志位Z为1
	SetFlag(V, (~((uint16_t)a ^ (uint16_t)fetched) & ((uint16_t)a ^ (uint16_t)temp)) & 0x0080);  // V = ~(A^M) & (A^R)公式求是否溢出，最后&上0x0080获取符号位的结果
	SetFlag(N, temp & 0x0080);  // 若符号位为1，设置标志位N为1

	// 将temp值加载入累加寄存器a中
	a = temp & 0x00FF;

	return 1;  // 此操作需要一个额外的时钟周期
}

/*
* 带有借位的减法指令
* 计算函数为 A = A - M - (1 - C), A代表累加寄存器a，M代表Memory，也就是内存中的值,C代表status寄存器中的C标志位
* 需要设置 C,V,N,Z这4个标志位
*/
uint8_t CPU6502::SBC()
{
	// 首先获取数据，调用fetch()函数，将会从指令对应的地址取值放入fetched变量中
	fetch();

	uint16_t value = ((uint16_t)fetched) ^ 0x00FF;  // 首先取反内存中值的每一位,每一位异或上1可以达到取反的效果

	// 这边类似加法操作，需要设置4个标志位，溢出操作的计算公式稍有不同
	temp = (uint16_t)a + value + (uint16_t)GetFlag(C);
	// 设置4个标志位
	SetFlag(C, temp & 0xFF00);  // 结果若大于255，需要进位，则设置标志位C为1
	SetFlag(Z, (temp & 0x00FF) == 0);  // 如果结果为0，设置标志位Z为1
	SetFlag(V, (temp ^ (uint16_t)a) & (temp ^ value) & 0x0080);  // 根据计算结果设置标志位V
	SetFlag(N, temp & 0x0080);  // 根据符号位设置标志位N，最后&上0x0080获取符号位的结果

	// 将temp值加载入累加寄存器a中
	a = temp & 0x00FF;

	return 1;  // 此操作需要一个额外的时钟周期
}


///////////////////////////////////////////////////////////////////////////////////////////////
// 以下指令都较简单了

/*
* 按位与指令
* 计算函数为  A = A & M， A代表累加寄存器a中的值，M代表内存中的值
* 需要设置 N，Z 标志位
*/
uint8_t CPU6502::AND()
{
	// 首先获取数据，调用fetch()函数，将会从指令对应的地址取值放入fetched变量中
	fetch();

	// 执行按位与操作
	a &= fetched;  

	// 设置标志位
	SetFlag(Z, a == 0x00);  // a若为0，设置标志位Z
	SetFlag(N, a & 0x80);  // a符号位为负数，设置标志位N

	return 1;  // 需要一个额外的时钟周期
}

/*
* 算数左移位指令
* 计算函数为 A = (A << 1) <- 0, 右边补0
* 需要设置 N，Z，C标志位
*/
uint8_t CPU6502::ASL()
{
	// 首先获取数据，调用fetch()函数，将会从指令对应的地址取值放入fetched变量中
	fetch();

	// 左移操作
	temp = (uint16_t)fetched << 1;

	// 设置标志位
	SetFlag(C, (temp & 0xFF00) > 0);  // 若移位后结果大于255，设置标志位C
	SetFlag(Z, (temp & 0x00FF) == 0x00);  // 若移位后结果为0，设置标志位Z
	SetFlag(N, temp & 0x80);  // 若移位后符号位为1，设置标志位N

	// 根据不同的寻址方式，将数据加载进不同的目标位置
	if (lookup[opcode].addrmode == &CPU6502::IMP) {
		// 如果是隐含寻址方式，那么将数据送进累加寄存器a中
		a = temp & 0x00FF;
	}
	else {
		// 若为其他寻址模式，将结果写入到addr_abs指向的位置中
		write(addr_abs, temp & 0x00FF);
	}

	return 0;  // 需要一个额外的时钟周期
}

/*
* 分支跳转指令
* 若C标志位为0，则跳转到相对地址指向的位置，即将程序计数器设置到该位置
* 计算公式为 if(C==0) pc = pc + addr_rel
*/
uint8_t CPU6502::BCC()
{
	// 若标志位C为0，跳转到指定位置
	if (GetFlag(C) == 0) {
		cycles++;  // 直接对该指令增加一个时钟周期
		addr_abs = pc + addr_rel;  // 跳转到当前程序计数器偏移相对地址这么多的位置

		// 若跨页了，时钟周期加一
		if ((addr_abs & 0xFF00) != (pc & 0xFF00)) {
			cycles++;  // 通过这种方式，对该指令直接加上一个时钟周期
		}
		pc = addr_abs;  // 设置程序计数器
	}

	return 0;  // 不需要额外时钟周期，添加周期的事在上面几行做了
}

/*
* 若C标志位为1，则跳转到相对地址指向的位置，即将程序计数器设置到该位置
* 计算公式为 if(C==1) pc = pc + addr_rel
*/
uint8_t CPU6502::BCS()
{
	// 若标志位C为0，跳转到指定位置
	if (GetFlag(C) == 1) {
		cycles++;  // 直接对该指令增加一个时钟周期
		addr_abs = pc + addr_rel;  // 跳转到当前程序计数器偏移相对地址这么多的位置

		// 若跨页了，时钟周期加一
		if ((addr_abs & 0xFF00) != (pc & 0xFF00)) {
			cycles++;  // 通过这种方式，对该指令直接加上一个时钟周期
		}
		pc = addr_abs;  // 设置程序计数器
	}

	return 0;  // 不需要额外时钟周期，添加周期的事在上面几行做了
}

/*
* 若Z标志位为0，则跳转到相对地址指向的位置，即将程序计数器设置到该位置
* 计算公式为 if(Z==0) pc = pc + addr_rel
*/
uint8_t CPU6502::BNE()
{
	// 若标志位Z为1，跳转到指定位置
	if (GetFlag(Z) == 0) {
		cycles++;  // 直接对该指令增加一个时钟周期
		addr_abs = pc + addr_rel;  // 跳转到当前程序计数器偏移相对地址这么多的位置

		// 若跨页了，时钟周期加一
		if ((addr_abs & 0xFF00) != (pc & 0xFF00)) {
			cycles++;  // 通过这种方式，对该指令直接加上一个时钟周期
		}
		pc = addr_abs;  // 设置程序计数器
	}

	return 0;  // 不需要额外时钟周期，添加周期的事在上面几行做了
}


/*
* 若Z标志位为1，则跳转到相对地址指向的位置，即将程序计数器设置到该位置
* 计算公式为 if(Z==1) pc = pc + addr_rel
*/
uint8_t CPU6502::BEQ()
{
	// 若标志位Z为1，跳转到指定位置
	if (GetFlag(Z) == 1) {
		cycles++;  // 直接对该指令增加一个时钟周期
		addr_abs = pc + addr_rel;  // 跳转到当前程序计数器偏移相对地址这么多的位置

		// 若跨页了，时钟周期加一
		if ((addr_abs & 0xFF00) != (pc & 0xFF00)) {
			cycles++;  // 通过这种方式，对该指令直接加上一个时钟周期
		}
		pc = addr_abs;  // 设置程序计数器
	}

	return 0;  // 不需要额外时钟周期，添加周期的事在上面几行做了
}


/*
* 若N标志位为0，则跳转到相对地址指向的位置，即将程序计数器设置到该位置
* 计算公式为 if(N==0) pc = pc + addr_rel
*/
uint8_t CPU6502::BPL()
{
	// 若标志位Z为1，跳转到指定位置
	if (GetFlag(N) == 0) {
		cycles++;  // 直接对该指令增加一个时钟周期
		addr_abs = pc + addr_rel;  // 跳转到当前程序计数器偏移相对地址这么多的位置

		// 若跨页了，时钟周期加一
		if ((addr_abs & 0xFF00) != (pc & 0xFF00)) {
			cycles++;  // 通过这种方式，对该指令直接加上一个时钟周期
		}
		pc = addr_abs;  // 设置程序计数器
	}

	return 0;  // 不需要额外时钟周期，添加周期的事在上面几行做了
}

/*
* 若N标志位为1，则跳转到相对地址指向的位置，即将程序计数器设置到该位置
* 计算公式为 if(N==1) pc = pc + addr_rel
*/
uint8_t CPU6502::BMI()
{
	// 若标志位Z为1，跳转到指定位置
	if (GetFlag(N) == 1) {
		cycles++;  // 直接对该指令增加一个时钟周期
		addr_abs = pc + addr_rel;  // 跳转到当前程序计数器偏移相对地址这么多的位置

		// 若跨页了，时钟周期加一
		if ((addr_abs & 0xFF00) != (pc & 0xFF00)) {
			cycles++;  // 通过这种方式，对该指令直接加上一个时钟周期
		}
		pc = addr_abs;  // 设置程序计数器
	}

	return 0;  // 不需要额外时钟周期，添加周期的事在上面几行做了
}

/*
* 若V标志位为0，则跳转到相对地址指向的位置，即将程序计数器设置到该位置
* 计算公式为 if(V==0) pc = pc + addr_rel
*/
uint8_t CPU6502::BVC()
{
	// 若标志位Z为1，跳转到指定位置
	if (GetFlag(V) == 0) {
		cycles++;  // 直接对该指令增加一个时钟周期
		addr_abs = pc + addr_rel;  // 跳转到当前程序计数器偏移相对地址这么多的位置

		// 若跨页了，时钟周期加一
		if ((addr_abs & 0xFF00) != (pc & 0xFF00)) {
			cycles++;  // 通过这种方式，对该指令直接加上一个时钟周期
		}
		pc = addr_abs;  // 设置程序计数器
	}

	return 0;  // 不需要额外时钟周期，添加周期的事在上面几行做了
}

/*
* 若V标志位为1，则跳转到相对地址指向的位置，即将程序计数器设置到该位置
* 计算公式为 if(V==1) pc = pc + addr_rel
*/
uint8_t CPU6502::BVS()
{
	// 若标志位Z为1，跳转到指定位置
	if (GetFlag(V) == 1) {
		cycles++;  // 直接对该指令增加一个时钟周期
		addr_abs = pc + addr_rel;  // 跳转到当前程序计数器偏移相对地址这么多的位置

		// 若跨页了，时钟周期加一
		if ((addr_abs & 0xFF00) != (pc & 0xFF00)) {
			cycles++;  // 通过这种方式，对该指令直接加上一个时钟周期
		}
		pc = addr_abs;  // 设置程序计数器
	}

	return 0;  // 不需要额外时钟周期，添加周期的事在上面几行做了
}

/*
* 不清楚该指令的作用
* 就是对累加寄存器a中的值与内存中指定值做按位与操作，在根据结果设置标志位
* 设置 Z，N，V 标志位
*/
uint8_t CPU6502::BIT()
{
	// 首先获取数据，调用fetch()函数，将会从指令对应的地址取值放入fetched变量中
	fetch();

	// 按位与操作
	uint8_t t = a & fetched;

	// 设置标志位
	SetFlag(Z, t == 0x00);
	SetFlag(N, fetched & 0x80);
	SetFlag(V, fetched & 0x40);

	return 0; // 不需要额外的时钟周期
}


/*
* Break指令
* 软件中断
*/
uint8_t CPU6502::BRK()
{
	pc++;  // 程序计数器先加一，为了保存下一步操作的程序计数器

	SetFlag(I, true);  // 设置status寄存器中的I标志位，屏蔽irq中断

	// 将程序计数器保存到栈中
	write(0x0100 + stkp, (pc >> 8) & 0x00FF);  // 保存高8位
	stkp--;
	write(0x0100 + stkp, pc & 0x00FF);  // 保存低8位
	stkp--;

	SetFlag(B, true);  // 设置标志位B
	write(0x0100 + stkp, status);  // 保存status寄存器到栈中
	stkp--;
	SetFlag(B, false);  // 重置标志位B

	// 程序计数器指向固定地址中保存的值
	addr_abs = 0xFFFE;
	uint16_t lo = read(addr_abs + 0);
	uint16_t hi = read(addr_abs + 1);
	pc = (hi << 8) | lo;

	return 0;  // 不需要额外的时钟周期
}


/////////////////////////////////////////////////////////////////////
// 一些设置状态位为0的指令

/*
* 清除标志位C
* 计算函数为 C=0
*/
uint8_t CPU6502::CLC()
{
	SetFlag(C, false);
	return 0;
}

/*
* 清除标志位D
* 计算函数为 D=0
*/
uint8_t CPU6502::CLD()
{
	SetFlag(D, false);
	return 0;
}

/*
* 清除标志位I
* 计算函数为 I=0
*/
uint8_t CPU6502::CLI()
{
	SetFlag(I, false);
	return 0;
}

/*
* 清除标志位V
* 计算函数为 V=0
*/
uint8_t CPU6502::CLV()
{
	SetFlag(V, false);
	return 0;
}


/*
* 设置标志位C
* 计算函数为 C=1
*/
uint8_t CPU6502::SEC()
{
	SetFlag(C, true);
	return 0;
}

/*
* 设置标志位D
* 计算函数为 D=1
*/
uint8_t CPU6502::SED()
{
	SetFlag(D, true);
	return 0;
}

/*
* 设置标志位I
* 计算函数为 I=1
*/
uint8_t CPU6502::SEI()
{
	SetFlag(I, true);
	return 0;
}


/*
* 保存累加寄存器的值到指定位置的指令
* 计算函数 M = A
*/
uint8_t CPU6502::STA()
{
	write(addr_abs, a);  // 将寄存器中的值写入到指定地址中

	return 0;  // 不需要额外时钟周期
}

/*
* 保存累加寄存器的值到指定位置的指令
* 计算函数 M = X
*/
uint8_t CPU6502::STX()
{
	write(addr_abs, x);  // 将寄存器中的值写入到指定地址中

	return 0;  // 不需要额外时钟周期
}

/*
* 保存累加寄存器的值到指定位置的指令
* 计算函数 M = Y
*/
uint8_t CPU6502::STY()
{
	write(addr_abs, y);  // 将寄存器中的值写入到指定地址中

	return 0;  // 不需要额外时钟周期
}

/*
* 将累加寄存器a的值传送给寄存器x的指令
* 计算函数 X = A
* 根据结果设置 N，Z 标志位
*/
uint8_t CPU6502::TAX()
{
	x = a;  // 将累加寄存器a的值传送给寄存器x

	// 设置标志位
	SetFlag(Z, x == 0x00);
	SetFlag(N, x & 0x80);

	return 0;  // 不需要额外时钟周期
}

/*
* 将寄存器x的值传送给累加寄存器a的指令
* 计算函数 A = X
* 根据结果设置 N，Z 标志位
*/
uint8_t CPU6502::TXA()
{
	a = x;  // 将寄存器x的值传送给累加寄存器a

	// 设置标志位
	SetFlag(Z, a == 0x00);
	SetFlag(N, a & 0x80);

	return 0;  // 不需要额外时钟周期
}


/*
* 将累加寄存器a的值传送给寄存器y的指令
* 计算函数 Y = A
* 根据结果设置 N，Z 标志位
*/
uint8_t CPU6502::TAY()
{
	y = a;  // 将累加寄存器a的值传送给寄存器y

	// 设置标志位
	SetFlag(Z, y == 0x00);
	SetFlag(N, y & 0x80);

	return 0;  // 不需要额外时钟周期
}

/*
* 将寄存器y的值传送给累加寄存器a的指令
* 计算函数 A = Y
* 根据结果设置 N，Z 标志位
*/
uint8_t CPU6502::TYA()
{
	a = y;  // 将寄存器y的值传送给累加寄存器a

	// 设置标志位
	SetFlag(Z, a == 0x00);
	SetFlag(N, a & 0x80);

	return 0;  // 不需要额外时钟周期
}

/*
* 将栈顶指针的值传送给寄存器x的指令
* 计算函数 X = stkp
* 根据结果设置 N，Z 标志位
*/
uint8_t CPU6502::TSX()
{
	x = stkp;  // 将栈顶指针的值传送给寄存器x

	// 设置标志位
	SetFlag(Z, x == 0x00);
	SetFlag(N, x & 0x80);

	return 0;  // 不需要额外时钟周期
}

/*
* 将寄存器x的值传送给栈顶指针的指令
* 计算函数 stkp = X
* 不用设置标志位
*/
uint8_t CPU6502::TXS()
{
	stkp = x;  // 将寄存器x的值传送给栈顶指针

	return 0;  // 不需要额外时钟周期
}


/*
* 比较指令，基于累加寄存器a
* 计算函数 C = (A>=M)  Z = (A-M)==0, 即若累加寄存器中的值大于等于内存中的值，设置标志位C；若等于内存中的值，设置标志位Z；
* 根据结果设置 N,C,Z 标志位
*/
uint8_t CPU6502::CMP()
{
	// 根据命令的取址方式获取数据数据放到fetched变量中
	fetch();

	temp = (uint16_t)a - (uint16_t)fetched;  // 比较大小

	// 设置标志位
	SetFlag(C, a >= fetched);
	SetFlag(Z, (temp & 0x00FF) == 0x0000);
	SetFlag(N, temp & 0x0080);

	return 1;  // 需要一个额外的时钟周期
}

/*
* 比较指令，基于寄存器x
* 计算函数 C = (X>=M)  Z = (X-M)==0, 即若累加寄存器中的值大于等于内存中的值，设置标志位C；若等于内存中的值，设置标志位Z；
* 根据结果设置 N,C,Z 标志位
*/
uint8_t CPU6502::CPX()
{
	// 根据命令的取址方式获取数据数据放到fetched变量中
	fetch();

	temp = (uint16_t)x - (uint16_t)fetched;  // 比较大小

	// 设置标志位
	SetFlag(C, x >= fetched);
	SetFlag(Z, (temp & 0x00FF) == 0x0000);
	SetFlag(N, temp & 0x0080);

	return 1;  // 需要一个额外的时钟周期
}

/*
* 比较指令，基于寄存器y
* 计算函数 C = (Y>=M)  Z = (Y-M)==0, 即若累加寄存器中的值大于等于内存中的值，设置标志位C；若等于内存中的值，设置标志位Z；
* 根据结果设置 N,C,Z 标志位
*/
uint8_t CPU6502::CPY()
{
	// 根据命令的取址方式获取数据数据放到fetched变量中
	fetch();

	temp = (uint16_t)y - (uint16_t)fetched;  // 比较大小

	// 设置标志位
	SetFlag(C, y >= fetched);
	SetFlag(Z, (temp & 0x00FF) == 0x0000);
	SetFlag(N, temp & 0x0080);

	return 1;  // 需要一个额外的时钟周期
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// 以下是一些寄存器中或内存中加减操作

/*
* 内存中值减一指令
* 计算函数  M = M-1
* 根据结果设置 N,Z 标志位
*/
uint8_t CPU6502::DEC()
{
	// 根据命令的取址方式获取数据数据放到fetched变量中
	fetch();

	temp = fetched - 1;  // 计算结果
	write(addr_abs, temp & 0x00FF);  // 写回指定内存中

	// 设置标志位
	SetFlag(Z, (temp & 0x00FF) == 0x0000);
	SetFlag(N, temp & 0x0080);

	return 0;  // 不需要额外的时钟周期
}

/*
* 寄存器x中值减一指令
* 计算函数  X = X-1
* 根据结果设置 N,Z 标志位
*/
uint8_t CPU6502::DEX()
{
	x--;  // 寄存器x中值减一

	// 设置标志位
	SetFlag(Z, x == 0x00);
	SetFlag(N, x & 0x80);

	return 0;  // 不需要额外的时钟周期
}

/*
* 寄存器y中值减一指令
* 计算函数  Y = Y-1
* 根据结果设置 N,Z 标志位
*/
uint8_t CPU6502::DEY()
{
	y--;  // 寄存器x中值减一

	// 设置标志位
	SetFlag(Z, y == 0x00);
	SetFlag(N, y & 0x80);

	return 0;  // 不需要额外的时钟周期
}

/*
* 内存中值加一指令
* 计算函数  M = M+1
* 根据结果设置 N,Z 标志位
*/
uint8_t CPU6502::INC()
{
	// 根据命令的取址方式获取数据数据放到fetched变量中
	fetch();

	temp = fetched + 1;  // 计算结果
	write(addr_abs, temp & 0x00FF);  // 写回指定内存中

	// 设置标志位
	SetFlag(Z, (temp & 0x00FF) == 0x0000);
	SetFlag(N, temp & 0x0080);

	return 0;  // 不需要额外的时钟周期
}

/*
* 寄存器x中值加一指令
* 计算函数  X = X+1
* 根据结果设置 N,Z 标志位
*/
uint8_t CPU6502::INX()
{
	x++;  // 寄存器x中值加一

	// 设置标志位
	SetFlag(Z, x == 0x00);
	SetFlag(N, x & 0x80);

	return 0;  // 不需要额外的时钟周期
}

/*
* 寄存器y中值加一指令
* 计算函数  Y = Y+1
* 根据结果设置 N,Z 标志位
*/
uint8_t CPU6502::INY()
{
	y++;  // 寄存器x中值加一

	// 设置标志位
	SetFlag(Z, y == 0x00);
	SetFlag(N, y & 0x80);

	return 0;  // 不需要额外的时钟周期
}

/*
* 异或指令
* 计算函数 A = A^M
* 根据计算结果设置 N,Z 标志位
*/
uint8_t CPU6502::EOR()
{
	// 根据命令的取址方式获取数据数据放到fetched变量中
	fetch();

	a ^= fetched;  // 按位异或操作

	// 设置标志位
	SetFlag(Z, a == 0x00);
	SetFlag(N, a & 0x80);

	return 1;  // 需要一个额外时钟周期
}

/*
* 跳转到指定位置
* 计算函数 pc = addr_abs
*/
uint8_t CPU6502::JMP()
{
	pc = addr_abs;

	return 0;  // 不需要额外时钟
}

/*
* 跳转到子例程
* 就是函数跳转，需要保存程序计数器到栈中
* pc = addr_abs
*/
uint8_t CPU6502::JSR()
{
	pc--;  // 程序计数器减一

	// 保存程序计数器到栈中
	write(0x0100 + stkp, (pc >> 8) & 0x00FF);  // 先保存高位
	stkp--;
	write(0x0100 + stkp, pc & 0x00FF);  // 再保存低位
	stkp--;

	pc = addr_abs;  // 移动程序计数器到指定地址

	return 0;  // 不需要额外时钟
}

/*
* 加载数据指令
* 加载指定地址的数据到累加寄存器a中
* 计算公式 A = M
* 根据结果设置 N，Z 标志位
*/
uint8_t CPU6502::LDA()
{
	// 根据命令的取址方式获取数据数据放到fetched变量中
	fetch();

	a = fetched;  // 将数据放入累加寄存器a中

	// 设置标志位
	SetFlag(Z, a == 0x00);
	SetFlag(N, a & 0x80);

	return 1;  // 需要一个额外时钟周期
}

/*
* 加载数据指令
* 加载指定地址的数据到寄存器x中
* 计算公式 X = M
* 根据结果设置 N，Z 标志位
*/
uint8_t CPU6502::LDX()
{
	// 根据命令的取址方式获取数据数据放到fetched变量中
	fetch();

	x = fetched;  // 将数据放入累加寄存器a中

	// 设置标志位
	SetFlag(Z, x == 0x00);
	SetFlag(N, x & 0x80);

	return 1;  // 需要一个额外时钟周期
}

/*
* 加载数据指令
* 加载指定地址的数据到寄存器y中
* 计算公式 Y = M
* 根据结果设置 N，Z 标志位
*/
uint8_t CPU6502::LDY()
{
	// 根据命令的取址方式获取数据数据放到fetched变量中
	fetch();

	y = fetched;  // 将数据放入累加寄存器a中

	// 设置标志位
	SetFlag(Z, y == 0x00);
	SetFlag(N, y & 0x80);

	return 1;  // 需要一个额外时钟周期
}

/*
* 右移指令，左边补0
* 将指定内存中的值右移一位送入目标位置中，
* 根据结果设置 C,Z,N 标志位
*/
uint8_t CPU6502::LSR()
{
	// 根据命令的取址方式获取数据数据放到fetched变量中
	fetch();

	SetFlag(C, fetched & 0x0001);

	temp = fetched >> 1;  // 右移一位

	SetFlag(Z, (temp & 0x00FF) == 0x0000);
	SetFlag(N, temp & 0x0080);

	if (lookup[opcode].addrmode == &CPU6502::IMP) {
		// 若寻址方式为隐式寻址，则将结果送入累加寄存器a中
		a = temp & 0x00FF;
	}
	else {
		// 寻址方式不为隐式寻址，将结果送入指定的内存中
		write(addr_abs, temp & 0x00FF);
	}
	
	return 0;  // 不需要额外的时钟周期
}

/*
* no operation操作
* 程序拖时间用的
*/
uint8_t CPU6502::NOP()
{
	// 如果是下列操作，则加一个时钟周期
	switch (opcode) {
	case 0x1C:
	case 0x3C:
	case 0x5C:
	case 0x7C:
	case 0xDC:
	case 0xFC:
		return 1;
	}

	return 0;  // 不进入switch则不增加时钟周期
}

/*
* 或指令
* 计算函数 A = A|M
* 根据结果设置 N,Z
*/
uint8_t CPU6502::ORA()
{
	// 根据命令的取址方式获取数据数据放到fetched变量中
	fetch();

	a |= fetched;  // 按位或操作

	// 设置标志位
	SetFlag(Z, a == 0x00);
	SetFlag(N, a & 0x80);

	return 1;  // 需要一个额外的时钟周期
}

/*
* 保存累加寄存器a到栈中的指令
* 计算函数  A->stack
*/
uint8_t CPU6502::PHA()
{
	// 保存累加寄存器a到栈中
	write(0x0100 + stkp, a);
	stkp--;

	return 0;
}

/*
* 保存状态寄存器status到栈中的指令
* 计算函数  status->stack
* 需要设置 B，U 状态位
*/
uint8_t CPU6502::PHP()
{
	// 保存累加寄存器a到栈中,需要或上B与U标志位
	write(0x0100 + stkp, status | B | U);
	stkp--;

	// 设置标志位
	SetFlag(B, false);
	SetFlag(U, false);

	return 0;  // 不需要额外的时钟周期
}

/*
* 从栈中取出累加寄存器a中的值
* 计算函数  A<-stack
* 根据结果设置 N，Z 寄存器
*/
uint8_t CPU6502::PLA()
{
	// 从栈中读取值到累加寄存器a中
	stkp++;
	a = read(0x0100 + stkp);

	//设置状态位
	SetFlag(Z, a == 0x00);
	SetFlag(N, a & 0x80);

	return 0;  // 不需要额外的时钟周期
}

/*
* 从栈中取出寄存器status中的值
* 计算函数  status<-stack
* 根据结果设置 N，Z 寄存器
*/
uint8_t CPU6502::PLP()
{
	// 从栈中读取值到寄存器status中
	stkp++;
	status = read(0x0100 + stkp);

	// 设置标志位
	SetFlag(U, true);

	return 0;  // 不需要额外的时钟周期
}

/*
* 循环左移指令
* 最右边的位为标志位C中的值
*/
uint8_t CPU6502::ROL()
{
	// 根据命令的取址方式获取数据数据放到fetched变量中
	fetch();

	temp = (uint16_t)(fetched << 1) | GetFlag(C);  // 循环左移，右边填标志位C

	// 设置标志位
	SetFlag(C, fetched & 0x80);
	SetFlag(Z, (temp & 0x00FF) == 0x0000);
	SetFlag(N, temp & 0x0080);

	if (lookup[opcode].addrmode == &CPU6502::IMP) {
		// 如果寻址模式位隐式寻址，将结果写入a中
		a = temp & 0x00FF;
	}
	else {
		// 如果寻址模式部位隐式寻址，将结果写入指定位置
		write(addr_abs, temp & 0x00FF);
	}

	return 0;  // 不需要额外的时钟周期
}

/*
* 循环右移指令
* 最左边的位为标志位C中的值
*/
uint8_t CPU6502::ROR()
{
	// 根据命令的取址方式获取数据数据放到fetched变量中
	fetch();

	temp = (uint16_t)(GetFlag(C) << 7) | (fetched >> 1);  // 循环右移，左边填标志位C

	// 设置标志位
	SetFlag(C, fetched & 0x01);
	SetFlag(Z, (temp & 0x00FF) == 0x0000);
	SetFlag(N, temp & 0x0080);

	if (lookup[opcode].addrmode == &CPU6502::IMP) {
		// 如果寻址模式位隐式寻址，将结果写入a中
		a = temp & 0x00FF;
	}
	else {
		// 如果寻址模式部位隐式寻址，将结果写入指定位置
		write(addr_abs, temp & 0x00FF);
	}

	return 0;  // 不需要额外的时钟周期
}

/*
* 从中断中恢复的指令
*/
uint8_t CPU6502::RTI()
{
	// 从栈中取出status寄存器的数据
	stkp++;
	status = read(0x0100 + stkp);

	// 设置status寄存器
	SetFlag(B, false);
	SetFlag(U, false);

	// 从栈中取出程序计数器的值
	stkp++;
	uint16_t lo = read(0x0100 + stkp);  // 先取低8位
	stkp++;
	uint16_t hi = read(0x0100 + stkp);  // 取出高8位

	pc = (hi << 8) | lo;  // 组装地址赋值给程序计数器

	return 0;  // 不需要额外的周期
}

/*
* 从子函数中返回的指令
*/
uint8_t CPU6502::RTS()
{
	// 从栈中回复程序计数器的值
	stkp++;
	uint16_t lo = read(0x0100 + stkp);  // 先取低8位
	stkp++;
	uint16_t hi = read(0x0100 + stkp);  // 取出高8位

	pc = (hi << 8) | lo;  // 组装地址赋值给程序计数器

	pc++;  // 将程序计数器加1，因为使用JSR指令跳转到子函数时，保存的是pc--的值，所以这里要加1

	return 0;  // 不需要额外的时钟周期
}

/*
* 所有非法指令被导向此指令
*/
uint8_t CPU6502::XXX()
{
	return 0;
}