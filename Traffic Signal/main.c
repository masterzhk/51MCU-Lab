#include <REG51.H>
#include <MATH.H>

#define RED  0xFC;
#define GREEN 0xF3;
#define YELLOW 0xCF;
#define BLUE 0x3F;

#define MS10 56320

// 0~F Display Code
unsigned char g_ledCode[16] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71 };

unsigned int g_timeCount = 0;

void InitTimer0();
void StartTimer0();
void StopTimer0();

void sleep10ms(unsigned int time);

void main()
{
	InitTimer0();

	sleep10ms(100);
	while(1)
	{
		P1 = RED;
		sleep10ms(3000);
		
		P1 = GREEN;
		sleep10ms(1000);
		
		P1 = YELLOW;
		sleep10ms(300);
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

void sleep10ms(unsigned int time)
{
	int i;
	int tmp;
	unsigned int displayIndex = 0;
	unsigned char displayValue = 0x00;
	unsigned char p2 = 0x0f;
	
	g_timeCount = 0;
  tmp = time;
	
	StartTimer0();
	
	// Display Remain Time
	do{
		tmp = (time - g_timeCount) / 10;
		tmp = tmp < 0 ? 0 : tmp;

		// Get The Display Value
		for(i = 3; i > displayIndex; --i)
		{
			tmp /= 10;
		}
		tmp %= 10;
		displayValue = g_ledCode[tmp];
		
		// Get The Display Index Value
		switch(displayIndex)
		{
			case 0:
				p2 = 0x1f;
				break;
			
			case 1:
				p2 = 0x2f;
				break;
				
			case 2:
				p2 = 0x4f;
			  displayValue = displayValue | 0x80; // Add The Decimal Point
				break;
			
			case 3:
				p2 = 0x8f;
				break;
		}
		
		P2 = P2 & 0x0f; // Turn Off The Display

		P0 = displayValue; // Set Up The Display Code
		P2 = p2; // Turn On The Display
		
		// Move The Display Index
		++displayIndex;
		displayIndex = displayIndex % 4;
	}
	while(time > g_timeCount);
	
	P0 = g_ledCode[0];
	P2 = P2 | 0xf0;;
	StopTimer0();
}

void Timer0Routine() interrupt 1
{
	++g_timeCount;
	
	TL0 = MS10;
	TH0 = MS10 >> 8;
}