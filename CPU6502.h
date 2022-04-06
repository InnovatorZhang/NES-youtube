#pragma once

#include<vector>
#include<string>

class Bus;  // ǰ����������ΪBus���CPU�����ѭ�����õ��������CPU6502.cpp��Ҳ������Bus.h

class CPU6502 {
public:
	CPU6502();
	~CPU6502();

public:
	// ����CPU�д��ڵļĴ���
	uint8_t a = 0x00;  // �ۼӼĴ���
	uint8_t x = 0x00;  // X�Ĵ���
	uint8_t y = 0x00;  // Y�Ĵ���
	uint8_t stkp = 0x00;  // ջ�Ĵ���
	uint16_t pc = 0x0000;  // �����������6502CPUѰַ�ռ�Ϊ64KB��������Ҫ16λ��ַ���ߣ�16λ��ַ
	uint8_t status = 0x00;  // ״̬�Ĵ���

	// �ⲿ�¼������ĺ�������Ӳ���оͱ�ʾ������յ��˵��ź�
	void reset();  // ����CPU״̬
	void irq();  // �жϺ�������״̬�Ĵ�����I��־λΪ1������Ա�����
	void nmi();  // �жϺ��������ɱ�����
	void clock();  // ʱ�ӵδ�

	void ConnectBus(Bus* n);  // ��CPU���ӵ�������

	// ö�ٳ�status�Ĵ������ڵİ˸�״̬����������status�Ĵ������в���
	enum FLAGS6502 {
		C = (1 << 0),  // ��λ��־λ
		Z = (1 << 1),  // ���־λ
		I = (1 << 2),  // �жϱ�־λ
		D = (1 << 3),  // ʮ���Ʊ�־λ����6502CPUû����
		B = (1 << 4),  // Break��־λ����һЩָ������
		U = (1 << 5),  // δʹ��λ��־ unused
		V = (1 << 6),  // �����־λ
		N = (1 << 7)   // ������־λ 
	};

private:
	// ��װ�˶�status�Ĵ��������ĺ���
	uint8_t GetFlag(FLAGS6502 f);
	void SetFlag(FLAGS6502 f, bool v);

	// һЩ�����������������
	uint8_t fetched = 0x00;  // �ӵ�ַ��ȡ�������ݵĴ�ŵأ���νALU��Ԫ������ֵ
	uint16_t temp = 0x0000;  // һ����ʱ���������Ա������κεط�
	uint16_t addr_abs = 0x0000;  // ������������ݵ�ַ
	uint16_t addr_rel = 0x0000;  // ������Ե�ַ
	uint8_t opcode = 0x00;  // ��ָͬ��ĵ�ַ��6502�Ĳ�������8λ���������256��ָ�ʵ����ֻ��56����Чָ��
	uint8_t cycles = 0;  // ָʾ��ǰָ�����ж���ʱ������ִ����ı���
	uint32_t clock_count = 0;  // ��¼CPUһ��ִ���˶�������

	// �������ָ��
	Bus* bus = nullptr;
	uint8_t read(uint16_t addr);  // CPU�Ķ�ȡ����������ͨ�����߶�д
	void write(uint16_t addr, uint8_t date);  // CPU��д���ݲ���

	// ����ָ�������ȷ�����ȡ�����ݣ����˲���ָ�������������ݵ�ָ���������Ҫ��ָ����ַ��ȡ
	uint8_t fetch();

	// ��һ���ṹ���ʾCPU6502�е�ÿһ��ָ��
	// ��һ��Ԫ�ر�ʾָ������,�ڶ�����ʾ����������ַ������ʹ�õ������Ա����ָ��
	// ��������ȡ��ַ��ʽ�����õ�Ҳ�����Ա����ָ��
	// ���ĸ�Ԫ�ر�ʾָ���������ڣ���������ȡ��ַ�Ƿ��ҳ�ˣ���������ʱ�����ڿ��ܻ���ڸ�ֵ
	struct INSTRUCTION
	{
		std::string name;
		uint8_t(CPU6502::* operate)(void) = nullptr;
		uint8_t(CPU6502::* addrmode)(void) = nullptr;
		uint8_t cycles = 0;
	};

	// ����ÿ��ָ���Ӧ�ı�ţ�����ָ���Ӧ�ĺ�����ڣ��������ָ�����ҵ���Ӧ����
	std::vector<INSTRUCTION> lookup;

private:
	// 6502CPUһ����12��Ѱַģʽ
	// ������ÿ��Ѱַ��������һ��ֵ����ʾ�ò����Ƿ���Ҫһ�������ʱ������
	// Ѱַģʽ���� https://www.jianshu.com/p/579682bb2f9c
	uint8_t IMP();  uint8_t IMM();
	uint8_t ZP0();  uint8_t ZPX();
	uint8_t ZPY();  uint8_t REL();
	uint8_t ABS();  uint8_t ABX();
	uint8_t ABY();  uint8_t IND();
	uint8_t IZX();  uint8_t IZY();

private:
	// 6502CPUһ����56�в�ͬ��ָ��
	// ����ֵ��ʾ�ò����п�����Ҫһ����������

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

	// ��������6502δ����Ĳ���ȫ���������·���
	uint8_t XXX();

};