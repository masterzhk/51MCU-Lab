#include <REG51.H>
#include <INTRINS.H>

#define D P0
#define MS10 56320

sbit RS = P1 ^ 0;
sbit RW = P1 ^ 1;
sbit E = P2 ^ 5;

void InitTimer0();
void StartTimer0();
void StopTimer0();
void Sleep10ms(unsigned int time);

unsigned char GetKeyPress();

/*
写指令
rs：寄存器选择 0-指令寄存器；1-数据寄存器
d：数据/指令
*/
void WriteInstruction(bit rs, unsigned char d);

/*
读指令
rs：寄存器选择 0-指令寄存器；1-数据寄存器
return：寄存器内容
*/
unsigned char ReadInstruction(bit rs);

/*
清屏
*/
void ClearDisplay();

/*
归位
*/
void ReturnHome();

/*
设置输入模式
direction：输入后光标移动方向 0-向右，1-向左
shift：输入后显示屏内容同步移动开关
*/
void SetEntryMode(bit direction, bit shift);

/*
显示控制
display：显示开关
cursor：光标显示开关
blink：光标闪烁开关
*/
void DisplayControl(bit display, bit cursor, bit blink);

/*
位移
sc：屏蔽/光标选择 0-光标，1-屏幕
direction：方向 0-左，1-右
*/
void Shift(bit sc, bit direction);

/*
功能设置
dataLength：数据总线长度 0-4位，1-8位
lineCount：显示行数 0-单行，1-双行
font：字形 0-5*7，1-5*11
*/
void SetFunction(bit dataLength, bit lineCount, bit font);

/*
设置CGRAM地址
address：地址
*/
void SetCGAddress(unsigned char address);

/*
设置DDRAM地址
address：地址
*/
void SetDDAddress(unsigned char address);

/*
读状态字
返回：高位为忙状态位，低7位为AC地址位
*/
unsigned char ReadBFAndAddress();

/*
写数据
d：数据
*/
void WriteData(unsigned char d);

/*
读数据
返回：数据
*/
unsigned char ReadData();

/*
读空闲状态字
返回：AC地址
*/
unsigned char WaitLcd();

unsigned int g_timeCount = 0;

/*
1 2 3 A
4 5 6 B
7 8 9 C
* 0 # D
*/
unsigned char g_keyCode[4][4] = { {0x31, 0x32, 0x33, 0x41}, {0x34, 0x35, 0x36, 0x42}, {0x37, 0x38, 0x39, 0x43}, {0x2a, 0x30, 0x23, 0x44} };

unsigned char asciiCode[] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};

void main()
{
	unsigned char ac = 0x00;
	unsigned char keyValue = 0x00;
	unsigned char index = 0;
	
	InitTimer0();
	
	SetFunction(1, 1, 1);
	SetEntryMode(1, 0);
	ClearDisplay();
	DisplayControl(1, 1, 0);
	
	while(1)
	{
		keyValue = GetKeyPress();
		switch(keyValue)
		{
			case 0x00:
				break;
			
			case 0x2A:
				Shift(1, 0);
				break;
			
			case 0x23:
				Shift(1, 1);
				break;
				
			case 0x41:
				ReturnHome();
				break;
			
			case 0x42:
				ac = WaitLcd();
			  --ac;
				SetDDAddress(ac);
				WriteData(' ');
				SetDDAddress(ac);
				break;
			
			case 0x43:
				ClearDisplay();
				break;
			
			default:
				WriteData(keyValue);
				break;
		}
	}
}

void WriteInstruction(bit rs, unsigned char d)
{	
	WaitLcd();
	
	RS = rs;
	RW = 0;
	D = d;
	E = 1;
	_nop_();
	E = 0;
	_nop_();
}

unsigned char ReadInstruction(bit rs)
{
	unsigned char d;
	
	D = 0xff; // 释放IO口，不然接收到的数据不对
	RS = rs;
	RW = 1;
	E = 1;
	_nop_();
	d = D;
	
	return d;
}

void ClearDisplay()
{
	WriteInstruction(0, 0x01);
}

void ReturnHome()
{
	WriteInstruction(0, 0x02);
}

void SetEntryMode(bit direction, bit shift)
{
	unsigned char d = 0x04;
	d = d | ((unsigned char)(direction) << 1) | (unsigned char)(shift);
	WriteInstruction(0, d);
}

void DisplayControl(bit display, bit cursor, bit blink)
{
	unsigned char d = 0x08;
	d = d | ((unsigned char)(display) << 2) | ((unsigned char)(cursor) << 1) | (unsigned char)(blink);
	WriteInstruction(0, d);
}

void Shift(bit sc, bit direction)
{
	unsigned char d = 0x10;
	d = d | ((unsigned char)(sc) << 3) | ((unsigned char)(direction) << 2);
	WriteInstruction(0, d);
}

void SetFunction(bit dataLength, bit lineCount, bit font)
{
	unsigned char d = 0x20;
	d = d | ((unsigned char)(dataLength) << 4) | ((unsigned char)(lineCount) << 3) | ((unsigned char)(font) << 2);
	WriteInstruction(0, d);
}

void SetCGAddress(unsigned char address)
{
	unsigned char d = 0x40;
	d = d | (address & 0x3f);
	WriteInstruction(0, d);
}

void SetDDAddress(unsigned char address)
{
	unsigned char d = 0x80;
	d = d | (address & 0x7f);
	WriteInstruction(0, d);
}

unsigned char ReadBFAndAddress()
{
	return ReadInstruction(0);
}

void WriteData(unsigned char d)
{
	WaitLcd();
	WriteInstruction(1, d);
}

unsigned char ReadData()
{
	return ReadInstruction(1);
}

unsigned char WaitLcd()
{
	unsigned char d = 0x00;
	
	do
	{
		d = ReadBFAndAddress();
	}
	while((d & 0x80) == 0x80);
	
	return d;
}

void InitTimer0()
{
	// Enable Interrupt
	EA = 1;
	
	// Enable Timer 0 Interrupt
	ET0 = 1;
	
	// Set Timer 0 To Mode 1
	TMOD = TMOD & 0xf0;
	TMOD = TMOD | 0x01;
}

void StartTimer0()
{
	TL0 = MS10;
	TH0 = MS10 >> 8;
	
	TR0 = 1; // Start Timing
}

void StopTimer0()
{
	TR0 = 0; // Stop Timing
}

void Sleep10ms(unsigned int time)
{
	g_timeCount = 0;
	StartTimer0();

	while(time - g_timeCount > 0);

	StopTimer0();
}

unsigned char GetKeyPress()
{
	unsigned char row, col;
	unsigned char p;
	
	row = 0;
	col = 0;
	
	P3 = 0x0f;
	if(P3 != 0x0f)
	{
		Sleep10ms(1);
		if(P3 != 0x0f)
		{
			row = 0;
			col = 0;
			
			p = P3;
			if ((p & 0x08) == 0x00)
			{
				col = 1;
			}
			else if ((p & 0x04) == 0x00)
			{
				col = 2;
			}
			else if ((p & 0x02) == 0x00)
			{
				col = 3;
			}
			else if ((p & 0x01) == 0x00)
			{
				col = 4;
			}
			
			P3 = 0xf0;
			p = P3;
			if ((p & 0x80) == 0x00)
			{
				row = 1;
			}
			else if ((p & 0x40) == 0x00)
			{
				row = 2;
			}
			else if ((p & 0x20) == 0x00)
			{
				row = 3;
			}
			else if ((p & 0x10) == 0x00)
			{
				row = 4;
			}
			
			while(P3 != 0xf0);
		}
	}
	
	if ((col == 0) || (row == 0))
	{
		return 0x00;
	}
	else
	{
		return g_keyCode[row - 1][col - 1];
	}
}

void Timer0Routine() interrupt 1
{
	++g_timeCount;
	
	TL0 = MS10;
	TH0 = MS10 >> 8;
}