#include <REG51.H>

#define RED  0xFC;
#define GREEN 0xF3;
#define YELLOW 0xCF;
#define BLUE 0x3F;

#define MS10 56320

unsigned int g_timeCount = 0;

void InitTimer0();
void StartTimer0();
void StopTimer0();

void sleep10ms(unsigned int time);

void main()
{
	InitTimer0();
	
	P1 = 0x00;
	
	sleep10ms(100);
	while(1)
	{
		P1 = RED;
		sleep10ms(1000);
		
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
	
	// Set Timer0 To Mode 1
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
	g_timeCount = 0;
	StartTimer0();
	while(g_timeCount < time);
	StopTimer0();
}

void Timer0Routine() interrupt 1
{
	++g_timeCount;
	
	TL0 = MS10;
	TH0 = MS10 >> 8;
}