#include <REG51.H>

#define RED  0xFC;
#define GREEN 0xF3;
#define YELLOW 0xCF;

void sleep(unsigned int time)
{
	int index1, index2;
	
	for(index1 = 0; index1 <= time; ++index1)
	for(index2 = 0; index2 <= 114; ++index2);
}

int main()
{
	P1 = 0xC0;
	sleep(1000);
	while(1)
	{
		P1 = RED;
		sleep(30000);
		P1 = GREEN;
		sleep(15000);
		P1 = YELLOW;
		sleep(3000);
	}
}