#include <REG51.H>

#define MS10 56320

unsigned int g_timeCount = 0;

unsigned char g_ledCode[16] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71 };

/*
1 2 3 A
4 5 6 B
7 8 9 C
* 0 # D
*/
unsigned char g_keyCode[4][4] = { {0x31, 0x32, 0x33, 0x41}, {0x34, 0x35, 0x36, 0x42}, {0x37, 0x38, 0x39, 0x43}, {0x2a, 0x30, 0x23, 0x44} };

void InitTimer0();
void StartTimer0();
void StopTimer0();
void Sleep10ms(unsigned int time);

unsigned char GetKeyPress();

void InitSerialPort();
void Send(unsigned char value);

void main()
{
	unsigned displayValue = 0x00;
	unsigned char keyValue = 0x00;
	
	InitTimer0();
	InitSerialPort();
	
	while(1)
	{
		keyValue = GetKeyPress();
		
		if (keyValue != 0x00)
		{
			Send(keyValue);
			displayValue = keyValue;
		}
		
		P2 = 0x0f;
		P0 = g_ledCode[(displayValue & 0xf0) >> 4];
		P2 = 0x4f;
		P2 = 0x0f;
		
		P2 = 0x0f;
		P0 = g_ledCode[displayValue & 0x0f];
		P2 = 0x8f;
		P2 = 0x0f;
	}
}

unsigned char GetKeyPress()
{
	unsigned char row, col;
	unsigned char p;
	
	row = 0;
	col = 0;
	
	P1 = 0x0f;
	if(P1 != 0x0f)
	{
		Sleep10ms(1);
		if(P1 != 0x0f)
		{
			row = 0;
			col = 0;
			
			p = P1;
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
			
			P1 = 0xf0;
			p = P1;
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
			
			while(P1 != 0xf0);
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

void InitSerialPort()
{
	// Enable Interrupt
	EA = 1;
	
	// Enable Serial Interrupt
	ES = 1;
	
	// Set Serial Port To Mode 1
	SM1 = 1;

	// Set Timer 1 To Mode 2
	TMOD = TMOD & 0x0f;
	TMOD = TMOD | 0x20;
	
	TH1 = 0xFE;
	TL1 = 0xFE;
	
	TR1 = 1;
}

void Send(unsigned char value)
{
	SBUF = value;
	while(!TI);
	TI = 0;
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

void Timer0Routine() interrupt 1
{
	++g_timeCount;
	
	TL0 = MS10;
	TH0 = MS10 >> 8;
}

void UARTRoutine() interrupt 4
{
	
}