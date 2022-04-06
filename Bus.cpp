#include"Bus.h"

/*
* Ĭ�Ϲ��캯�� 
*/
Bus::Bus()
{
	// �ڹ��캯��������CPU
	cpu.ConnectBus(this);
}

/*
* ��Ϊû�ж�̬������Դ����������������Ϊ��
*/
Bus::~Bus()
{
}

/*
* ����CPU��������д�����ݵķ������÷�����CPU�������ߵĲ�ͬ��ַ���룬ת��Ϊ��Ӧ���������ӵ��豸�ĵ�ַ
*/
void Bus::cpuWrite(uint16_t addr, uint8_t data)
{
	/*
	* ����δ����ĵ�ַ�ռ䣬��0x4000-0x401F��δ�����ĵ�ַ �� 0x4020-0x5FFF��ε�ַ�������ε�ַ�а��� APU(��������Ĵ���)��SRAM�ĵ�ַ�ռ����չROM�ռ�
	*  0x4020-0x5FFF Nesdev ����̳����ƪ���������������ӣ�������������������һЩ Mapper ��չ�ã��󲿷�����ò��� https://links.jianshu.com/go?to=https%3A%2F%2Fforums.nesdev.com%2Fviewtopic.php%3Ff%3D3%26t%3D12936
	*/
	if (cart->cpuWrite(addr, data)) {
		// ����ӳ�����CPU�� 0x8000-0xFFFF ��һ�ε�ַ�ռ�,ÿ��CPUд����һ�ε�ַ�ռ��ϵĵ�ַʱ������ӳ�䵽cartridge�豸��ȥ
		// д��������cartridge����
		// ��ʱ��ʱ��ֹӳ�䵽��һ��ռ�ģ���Щ��Ϸ���ϵ�RAM��ӳ�䵽0x6000-0x7FFF��һ�ε�ַ��ȥ
	}
	else if (addr >= 0x0000 && addr <= 0x1FFF) {
		// ����ӳ�����CPU�� 0x0000-0x1FFF ��һ�ε�ַ�ռ�,ÿ��CPUд����һ�ε�ַ�ռ��ϵĵ�ַʱ������ӳ�䵽2KB��С�ڴ��豸��
		// ��Ȼnes���ڴ�Ѱַ�ռ���8KB��ô�󣬵�ʵ����nesֻ��2KB�ڴ棬���nes�в�����mirror������8KB��Ѱַ�ռ�ӳ�䵽2KB���ڴ���
		// ʹ��&�����������0x07FF���ɽ���ַ 0x0000-0x1FFF =�� 0x0000-0x07FF
		cpuRam[addr & 0x07FF] = data;  
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF) {
		// ����ӳ�����CPU�� 0x2000-0x3FFF ��һ�ε�ַ�ռ�,ÿ��CPUд����һ�ε�ַ�ռ��ϵĵ�ַʱ������ӳ�䵽PPU��8���Ĵ�����ȥ
		// ��Ȼnes��PPUѰַ�ռ���8KB��ô�󣬵�ʵ����PPU��ֻ��8���Ĵ�����Ҳ����8��byte�����nes�в�����mirror������8KB��Ѱַ�ռ�ӳ�䵽8byte�ļĴ�����ַ��ȥ
		// ʹ��&�����������0x07FF���ɽ���ַ 0x2000-0x3FFF =�� 0x2000-0x2007
		ppu.cpuWrite(addr & 0x0007, data);
	}
	else if (addr == 0x4014) {
		// ���cpu���ַ0x4014д�����ݣ�����ζ��CPU��Ҫ����dma���������ÿһ��dma���䶼�ᴫ��һ����page��Ҳ����256�ֽڣ�����ʱcpu���ᴫ����Ҫ�����page����Ϊdata
		// �����ڴ˴���Ҫ�������page�ţ�Ҳ����dataֵ������dma_page������ָ����Ҫ�����page��������ҳ��ҳ��ƫ������Ϊ0,��ʾ�Ӹ�ҳ�ĵ�һ���ֽڿ�ʼ����
		// ͬʱ����dma_transfer��־��������Ϊtrue����ʾ����dma�������
		// ��dma_transferΪtrueʱ��CPU��������ֱ���˴�dma�������������һ����Ҫ512��CPUʱ��
		dma_page = data;
		dma_addr = 0x00;
		dma_transfer = true;
	}
	else if (addr >= 0x4016 && addr <= 0x4017) {
		// ��������ַ�ֱ����һ������������ҵ��ֱ��Ĵ����ĵ�ַ��nes��Ϸ�ֱ�һ���˸���λ����Ӧcontroler��8��bit
		// �Ӹߵ��͵�λ�ֱ�ӳ�䵽���°˸�����
		// A B SELECT START UP DOWN LEFT RIGHT
		// controlerһ��������ַ��0x0��0x1
		// ����ʹ��&������� 0x4016-0x4017 ӳ�䵽 0x0-0x1
		// ��ʱCPU����д�����ݣ����ǻ�ȡ��ʱ��controler�Ĵ���״̬��������controller_state�����У��Թ�����cpuRead����������ȡ
		// ��ȡ��ʱ��Ƚ���֣����������� 8 �������պÿ����� 1 �� byte ��ʾ����һ�ξͿ����ˣ����� NES ��ȡ��ʱ��ȴ�Ǵ��еģ��� 8 �Σ�ÿ�ζ�һ��������������Ӧ����Ϊ�˼����Ե�����������
		// ���Ե�CPUд�����ַ��ʱ����ʵָʾ���浱ǰ���ֱ��Ĵ�����ֵ
		controller_state[addr & 0x0001] = controller[addr & 0x0001];
	}

}

/*
* ����CPU�������϶�ȡ���ݵķ������÷�����CPU�������ߵĲ�ͬ��ַ���룬ת��Ϊ��Ӧ���������ӵ��豸�ĵ�ַ
*/
uint8_t Bus::cpuRead(uint16_t addr, bool bReadOnly)
{
	uint8_t data = 0x00;
	/*
	* ����δ����ĵ�ַ�ռ䣬��0x4000-0x401F��δ�����ĵ�ַ �� 0x4020-0x5FFF��ε�ַ�������ε�ַ�а��� APU(��������Ĵ���)��SRAM�ĵ�ַ�ռ����չROM�ռ�
	*  0x4020-0x5FFF Nesdev ����̳����ƪ���������������ӣ�������������������һЩ Mapper ��չ�ã��󲿷�����ò��� https://links.jianshu.com/go?to=https%3A%2F%2Fforums.nesdev.com%2Fviewtopic.php%3Ff%3D3%26t%3D12936
	*/
	if (cart->cpuRead(addr, data)) {
		// ����ӳ�����CPU�� 0x8000-0xFFFF ��һ�ε�ַ�ռ�,ÿ��CPUд����һ�ε�ַ�ռ��ϵĵ�ַʱ������ӳ�䵽cartridge�豸��ȥ
		// ����������cartridge����data���ﴫ��������
		// ��ʱ��ʱ��ֹӳ�䵽��һ��ռ�ģ���Щ��Ϸ���ϵ�RAM��ӳ�䵽0x6000-0x7FFF��һ�ε�ַ��ȥ
	}
	else if (addr >= 0x0000 && addr <= 0x1FFF) {
		// ����ӳ�����CPU�� 0x0000-0x1FFF ��һ�ε�ַ�ռ�,ÿ��CPU��ȡ��һ�ε�ַ�ռ��ϵĵ�ַʱ������ӳ�䵽2KB��С�ڴ��豸��
		// ��Ȼnes���ڴ�Ѱַ�ռ���8KB��ô�󣬵�ʵ����nesֻ��2KB�ڴ棬���nes�в�����mirror������8KB��Ѱַ�ռ�ӳ�䵽2KB���ڴ���
		// ʹ��&�����������0x07FF���ɽ���ַ 0x0000-0x1FFF =�� 0x0000-0x07FF
		data = cpuRam[addr & 0x07FF];
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF) {
		// ����ӳ�����CPU�� 0x2000-0x3FFF ��һ�ε�ַ�ռ�,ÿ��CPU��ȡ��һ�ε�ַ�ռ��ϵĵ�ַʱ������ӳ�䵽PPU��8���Ĵ�����ȥ
		// ��Ȼnes��PPUѰַ�ռ���8KB��ô�󣬵�ʵ����PPU��ֻ��8���Ĵ�����Ҳ����8��byte�����nes�в�����mirror������8KB��Ѱַ�ռ�ӳ�䵽8byte�ļĴ�����ַ��ȥ
		// ʹ��&�����������0x07FF���ɽ���ַ 0x2000-0x3FFF =�� 0x2000-0x2007
		data = ppu.cpuRead(addr & 0x0007, bReadOnly);
	}
	else if (addr >= 0x4016 && addr <= 0x4017) {
		// ��������ַ�ֱ����һ������������ҵ��ֱ��Ĵ����ĵ�ַ��nes��Ϸ�ֱ�һ���˸���λ����Ӧcontroler��8��bit
		// �Ӹߵ��͵�λ�ֱ�ӳ�䵽���°˸�����
		// A B SELECT START UP DOWN LEFT RIGHT
		// controlerһ��������ַ��0x0��0x1
		// ����ʹ��&������� 0x4016-0x4017 ӳ�䵽 0x0-0x1
		// ��ȡ��ʱ��Ƚ���֣����������� 8 �������պÿ����� 1 �� byte ��ʾ����һ�ξͿ����ˣ����� NES ��ȡ��ʱ��ȴ�Ǵ��еģ��� 8 �Σ�ÿ�ζ�һ��������������Ӧ����Ϊ�˼����Ե�����������
		// ���CPUÿ��ȡһ����ε�ַ�����Ǿͽ�controller_state��������һλ���أ�Ȼ��������һλ����֤�´ζ���������һ��ֵ
		data = (controller_state[addr & 0x0001] & 0x80) > 0;
		controller_state[addr & 0x0001] <<= 1;
	}

	return data;
}


/*
* ���彫��Ϸ�����ӵ����������ϵķ�������Bus�л�ȡcartridge��ָ��
*/
void Bus::insertCartridge(const std::shared_ptr<Cartridge>& cartridge)
{
	cart = cartridge;  // ����Ϸ�����ӵ�CPU������,CPU��0x8000-0xFFFF��Ҫӳ�䵽cartridge��PRG����
	ppu.ConnectCartridge(cartridge);  // ����Ϸ�����ӵ�PPU�����ϣ�PPU��0x0000-0x1FFF��Ҫӳ�䵽cartridge��CHR����
}

/*
* ����CPU����״̬�Լ������������ϵ������豸��״̬
*/
void Bus::reset()
{
	cart->reset();  // ����cartridge״̬����ʵ������cartridge�е�Mapper״̬
	cpu.reset();  // ����cpu״̬
	ppu.reset();  // ����ppu״̬
	nSystemClockCounter = 0;  // ������ʱ�Ӽ�����
	dma_page = 0x00;  // ����dma����page�ű���
	dma_addr = 0x00;  // ����dma����ҳ��ƫ��������
	dma_data = 0x00;  // ����dma�������ݵ�ֵ
	dma_dummy = true; // ���ñ�֤dma�����ż��ʱ�ӿ����ı���
	dma_transfer = false;  // �����Ƿ�����dma�����״̬���� 
}

/*
* ��ʾ�������ϴ����ʱ���źţ�ʱ��û�δ�һ�Σ������������ϵ�������Ӧ�Ĳ���
* ��������ģ�����ĺ��ĺ������������е��ٶ�ȡ���ú��������õĴ���
*/
void Bus::clock()
{
	// ������ÿ����һ��ʱ���ź�PPU����Ӧһ��
	ppu.clock();

	// CPU��ʱ���ٶȱ�PPU��3��������ppuÿ��Ӧ3�Σ�CPUֻ��Ӧ1��
	if (nSystemClockCounter % 3 == 0) {
		// ���Ǵ�ʱ���ڽ���dma���䣬��ôCPU���ᱻ����һ��dma�������������256KB����
		// һ��������512��CPUʱ�ӣ�dma_transfer����ָʾ�Ƿ��ڽ���dma����
		if (dma_transfer) {
			// dma���仹��һ��Ҫ��dma����Ŀ�ʼʱ�����Ҫ��ż��ʱ��
			// ����ʹ��dma_dummy����dma����һ����ʼ��ż��ʱ��
			if (dma_dummy) {
				// �����ǰϵͳʱ������������ô����ϵͳʱ�Ӻ�һ����ż������ǰʱ������ż������ٵȴ�����ϵͳʱ�ӣ��ȵ�ϵͳʱ���������ٽ�dma_dummy��Ϊfalse
				if (nSystemClockCounter % 2 == 1) {
					dma_dummy = false;  // ����Ϊfalse����һ��һ�������dma����
				}
			}
			else {
				// ����ϵͳʱ��һ����ż���ˣ����Կ�ʼ����dma����
				// dma����һ���ֽ���Ҫ����CPUʱ������(3��ϵͳʱ������)����ż��ϵͳʱ�����ڶ�ȡ���ݣ�����ϵͳʱ������д���ݵ�PPU��OAM�ڴ���
				if (nSystemClockCounter % 2 == 0) {
					// ��ż��ϵͳʱ�����ڶ�ȡ���ݵ�dma_data��
					dma_data = cpuRead(dma_page << 8 | dma_addr);  // page��+�ڸ�page�е�ƫ����ȷ����ȡ�����ݣ����ݱ�����dma_data������
				}
				else {
					// ����ϵͳʱ������д�����ݵ�PPU��OAM�ڴ���
					ppu.pOAM[dma_addr] = dma_data;  // ��ΪOAM�ڴ�һ��256�ֽڣ����Կ���ֱ��ʹ��dma_addr������
					dma_addr++;  // ����ƶ�����
					// ��Ϊdma_addrֻ��8���أ��������ֻ�ܱ�ʾ255�����Ե�dma_addr����0ʱ˵��dma���������
					if (dma_addr == 0x00) {
						dma_dummy = true;  // ��Ϊtrue��ϣ����һ��dma�����ܹ���֤��ż��ϵͳʱ�ӿ���
						dma_transfer = false;  // ��Ϊfalse����ʾ�˴�dma���������ȡ��CPU����
					}
				}
			}
		}
		else {
			// ���û�н���dma��������CPUδ������CPU��Ӧһ��
			cpu.clock();
		}
	}

	// PPU���д����жϵ���������PPU�����ж��źţ����ʾPPU�Ļ��ƽ���vertical blanking�׶Σ����ǳ�����Ļ��ʾ��Χ֮��Ĳ���
	// ��PPU�������ź�ʱ��CPUӦ����nmi�����������жϣ���CPU��nmi�жϻ����CPUѰַ��Χ�е�0xFFFA-0xFFFB�б��溯����ڣ��Ӷ�������Ӧ�Ĳ���
	// ��ÿһ��ϵͳ���ڶ���Ҫ�������PPU�Ƿ��ͷ���nmi�ź�
	if (ppu.nmi) {
		ppu.nmi = false;  // ����״̬
		cpu.nmi();  // ���ö�ӦCPU��nmi()�ж�
	}

	nSystemClockCounter++;
}
