#include"Mapper.h"

/*
* Mapper��Ĺ��캯��
* ��һ��������ʾ����ε�������ÿ���δ�СΪ16KB
* �ڶ���������ʾͼ��ε�������ÿ���δ�СΪ8KB
*/
Mapper::Mapper(uint8_t prgBanks, uint8_t chrBanks)
{
	nPRGBanks = prgBanks;
	nCHRBanks = chrBanks;

	// ��ʼ��ʱreset����
	reset();
}

/*
* ��������
*/
Mapper::~Mapper()
{
	// û�ж�̬���붯̬�ռ䣬���Ժ�����Ϊ��
}

/*
* ��Ȼ�Ǵ��麯�������ṩһ����ʵ��
*/
void Mapper::reset() {

}

/*
* ����name table�ľ���ʽ��Ĭ����Ӳ��ָ����Ҳ������nes�ļ�ͷ�е���Ϣָ��
*/
MIRROR Mapper::mirror()
{
	return MIRROR::HARDWARE;
}

/*
* �жϽӿ�
*/
bool Mapper::irqState()
{
	// Ĭ�Ϸ���false
	return false;
}
/*
* ��Ϊ���࣬δ�ṩĬ��ʵ��
*/
void Mapper::irqClear()
{
}

/*
* ��Ϊ���࣬δ�ṩĬ��ʵ��
*/
void Mapper::scanline()
{
}
