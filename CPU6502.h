#pragma once

#include<vector>
#include<string>

class Bus;  // 前置声明，因为Bus类和CPU类存在循环引用的情况，在CPU6502.cpp中也引入了Bus.h

class CPU6502 {
public:
	CPU6502();
	~CPU6502();

public:
	// 定义CPU中存在的寄存器
	uint8_t a = 0x00;  // 累加寄存器
	uint8_t x = 0x00;  // X寄存器
	uint8_t y = 0x00;  // Y寄存器
	uint8_t stkp = 0x00;  // 栈寄存器
	uint16_t pc = 0x0000;  // 程序计数器，6502CPU寻址空间为64KB，所以需要16位地址总线，16位地址
	uint8_t status = 0x00;  // 状态寄存器

	// 外部事件触发的函数，在硬件中就表示该针脚收到了电信号
	void reset();  // 重置CPU状态
	void irq();  // 中断函数，若状态寄存器的I标志位为1，则可以被屏蔽
	void nmi();  // 中断函数，不可被屏蔽
	void clock();  // 时钟滴答

	void ConnectBus(Bus* n);  // 将CPU连接到总线上

	// 枚举出status寄存器存在的八个状态，方便后面对status寄存器进行操作
	enum FLAGS6502 {
		C = (1 << 0),  // 进位标志位
		Z = (1 << 1),  // 零标志位
		I = (1 << 2),  // 中断标志位
		D = (1 << 3),  // 十进制标志位，对6502CPU没有用
		B = (1 << 4),  // Break标志位，对一些指令有用
		U = (1 << 5),  // 未使用位标志 unused
		V = (1 << 6),  // 溢出标志位
		N = (1 << 7)   // 负数标志位 
	};

private:
	// 包装了对status寄存器操作的函数
	uint8_t GetFlag(FLAGS6502 f);
	void SetFlag(FLAGS6502 f, bool v);

	// 一些辅助变量，方便操作
	uint8_t fetched = 0x00;  // 从地址中取出的数据的存放地，所谓ALU单元的输入值
	uint16_t temp = 0x0000;  // 一个临时变量，可以被用在任何地方
	uint16_t addr_abs = 0x0000;  // 保存所需的数据地址
	uint16_t addr_rel = 0x0000;  // 保存相对地址
	uint8_t opcode = 0x00;  // 不同指令的地址，6502的操作码有8位，所以最多256种指令，实际上只有56个有效指令
	uint8_t cycles = 0;  // 指示当前指定还有多少时钟周期执行完的变量
	uint32_t clock_count = 0;  // 记录CPU一共执行了多少周期

	// 总线类的指针
	Bus* bus = nullptr;
	uint8_t read(uint16_t addr);  // CPU的读取操作，就是通过总线读写
	void write(uint16_t addr, uint8_t date);  // CPU的写数据操作

	// 根据指令的类型确定如何取出数据，除了操作指令中隐含了数据的指令，其他都需要从指定地址读取
	uint8_t fetch();

	// 用一个结构体表示CPU6502中的每一个指令
	// 第一个元素表示指令名称,第二个表示操作函数地址，这里使用的是类成员函数指针
	// 第三个是取地址方式，采用的也是类成员函数指针
	// 第四个元素表示指令所需周期，不过根绝取地址是否跨页了，最终所用时钟周期可能会大于该值
	struct INSTRUCTION
	{
		std::string name;
		uint8_t(CPU6502::* operate)(void) = nullptr;
		uint8_t(CPU6502::* addrmode)(void) = nullptr;
		uint8_t cycles = 0;
	};

	// 根据每个指令对应的编号，保存指令对应的函数入口，方便根据指令码找到对应操作
	std::vector<INSTRUCTION> lookup;

private:
	// 6502CPU一共有12种寻址模式
	// 我们令每个寻址函数返回一个值，表示该操作是否需要一个额外的时钟周期
	// 寻址模式讲解 https://www.jianshu.com/p/579682bb2f9c
	uint8_t IMP();  uint8_t IMM();
	uint8_t ZP0();  uint8_t ZPX();
	uint8_t ZPY();  uint8_t REL();
	uint8_t ABS();  uint8_t ABX();
	uint8_t ABY();  uint8_t IND();
	uint8_t IZX();  uint8_t IZY();

private:
	// 6502CPU一共有56中不同的指令
	// 返回值表示该操作有可能需要一个额外周期

	uint8_t ADC();  uint8_t AND();  uint8_t ASL();  uint8_t BCC();
	uint8_t BCS();  uint8_t BEQ();  uint8_t BIT();  uint8_t BMI();
	uint8_t BNE();  uint8_t BPL();  uint8_t BRK();  uint8_t BVC();
	uint8_t BVS();  uint8_t CLC();  uint8_t CLD();  uint8_t CLI();
	uint8_t CLV();  uint8_t CMP();  uint8_t CPX();  uint8_t CPY();
	uint8_t DEC();  uint8_t DEX();  uint8_t DEY();  uint8_t EOR();
	uint8_t INC();  uint8_t INX();  uint8_t INY();  uint8_t JMP();
	uint8_t JSR();  uint8_t LDA();  uint8_t LDX();  uint8_t LDY();
	uint8_t LSR();  uint8_t NOP();  uint8_t ORA();  uint8_t PHA();
	uint8_t PHP();  uint8_t PLA();  uint8_t PLP();  uint8_t ROL();
	uint8_t ROR();  uint8_t RTI();  uint8_t RTS();  uint8_t SBC();
	uint8_t SEC();  uint8_t SED();  uint8_t SEI();  uint8_t STA();
	uint8_t STX();  uint8_t STY();  uint8_t TAX();  uint8_t TAY();
	uint8_t TSX();  uint8_t TXA();  uint8_t TXS();  uint8_t TYA();

	// 其他所有6502未定义的操作全都导向如下方法
	uint8_t XXX();

};