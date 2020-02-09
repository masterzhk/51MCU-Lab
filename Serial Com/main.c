#include <REG51.h>

#define MS10 56320

sbit p34 = 0xB4;

void InitTimer0();
void StartTimer0();
void StopTimer0();
void Sleep10ms(unsigned int time);

void InitSerialPort();
void Send();

unsigned int g_timeCount = 0;

unsigned char g_message[] = "Hello World!";

void main()
{
	InitTimer0();
	InitSerialPort();
	
	while(1)
	{
		if (p34 == 0)
		{
			Sleep10ms(1);
			if (p34 == 0)
			{
				P1 = 0x00;
				while(p34 == 0);
				Send();
				P1 = 0xff;
			}
		}
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
	
	// Do Not Check The Stop Bit
	SM2 = 0;
	
	// Disable Reading
	REN = 0;
	
	// Set Timer 1 To Mode 2
	TMOD = TMOD & 0x0f;
	TMOD = TMOD | 0x20;
	
	TH1 = 0xFE;
	TL1 = 0xFE;
	
	TR1 = 1;
}

void Send()
{
	unsigned char index = 0;
	
	for(index = 0; index < 12; ++index)
	{
		SBUF = g_message[index];
		while(!TI);
		TI = 0;
	}
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