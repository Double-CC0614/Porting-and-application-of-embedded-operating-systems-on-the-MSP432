#include <rtthread.h>
int main(void)
{
	uint32_t i=0;
	WDT_A_holdTimer();
	GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN2);
while(1)
	{
		GPIO_toggleOutputOnPin(GPIO_PORT_P2,GPIO_PIN2);
		for(i=0;i<100000;i++);
		rt_thread_delay(500);
	} 
}
