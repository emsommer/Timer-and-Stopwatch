#include <stdint.h>
	
//UPDATE IN LAB
volatile uint32_t * KeyBase = (volatile uint32_t *) 0x00021100;
volatile uint32_t * TimerOne = (volatile uint32_t *) 0x00021080;
volatile uint32_t * TimerTwo = (volatile uint32_t *) 0x00021060;
volatile uint32_t * HEX0 = (volatile uint32_t * ) 0x000210e0;
volatile uint32_t * HEX1 = (volatile uint32_t * ) 0x000210c0;
volatile uint32_t * HEX2 = (volatile uint32_t * ) 0x000210a0;
volatile uint32_t * HEX3 = (volatile uint32_t * ) 0x00021040;
volatile uint32_t * HEX4 = (volatile uint32_t * ) 0x00021020;
volatile uint32_t * HEX5 = (volatile uint32_t * ) 0x00021000;

int PrintSevenSegment_PIO(uint32_t counter);
uint32_t value(uint32_t counter);

int main()
{
	uint32_t timerValue2=0;
	uint32_t timerLow2=0;
	uint32_t timerHigh2=0;
	uint32_t ms = 0;
	uint32_t outputValue = 0;
	int buttonPressNumber = 0;
	uint32_t KeyThree = 0;

	//display 0 on the seven seg originally 
	PrintSevenSegment_PIO(0);
	
	//always
	while(1)
	{
		//read key three
		KeyThree = KeyBase[3] & 0x8;
		
		//spin while key3 not pressed
		while(0x8 != KeyThree)
		{
			KeyThree = KeyBase[3] & 0x8;

			//if 1 second timer has timed out (meaning increment the second counter)
			if((TimerOne[0] & 0x1) == 0x1)
				{
					// if 3rd button press no need to output the rollover of timerOne (also reset the outputValue)
					if(buttonPressNumber == 3)
					{
						outputValue = 0;
						PrintSevenSegment_PIO(outputValue);
						break;
					}
			
				//if timer 1 rollover increment output value and print it
				outputValue++;
				PrintSevenSegment_PIO(outputValue);
				TimerOne[0] = 0x0;
			}

			if(buttonPressNumber == 3)
			{
				buttonPressNumber = 0;
			}
		}
		
		//ONCE KEY THREE IS PRESSED......
		
		//update number of button presses
		buttonPressNumber++;
		
		//clear key press 
		KeyBase[3] = 0x8;
		
		//if 1st press of button
		if(buttonPressNumber == 1)
		{
			TimerTwo[2] = 0xFFFFFFFF;
			TimerTwo[3] = 0xFFFFFFFF;

			//start timer 1 and timer 2 in continuous mode
			TimerOne[1] = 0x6;
			TimerTwo[1] = 0x6;
		}
		
		//if 2nd press of button
		if(buttonPressNumber == 2)
			{
				//stop both timers
				TimerOne[1] = 0x8;
				TimerTwo[1] = 0x8;

				//snapshot timer 2, calculate difference between start and stop in ms and print to hex
				
				//Request a snapshot (write garbage to snapl).
 				TimerTwo[4] = 0x4242;
 				TimerTwo[5] = 0x4242;

 				//Read snapl and snaph and combine into 32 bit value.
  				timerLow2 = (uint16_t) TimerTwo[4];
  				timerHigh2 = (uint16_t) TimerTwo[5];
  				timerValue2 = (timerHigh2 << 16) | timerLow2;

				//calculate ms inbetween start and stop
				ms = (0xFFFFFFFF-timerValue2)/50000;

				//print ms 
				PrintSevenSegment_PIO(ms);
			}
		
		//if 3rd press of button
		if(buttonPressNumber == 3)
		{
			//reset both timers and start them again
			
			//reset the timers by setting the time out bit to a 1 manually
			TimerTwo[2] = 0xFFFFFFFF;
			TimerTwo[3] = 0xFFFFFFFF;
			
			outputValue = 0;

			//clear time out bit
			TimerOne[0] = 0x0;
			TimerTwo[0] = 0x0;

			//start timers
			TimerOne[1] = 0x6;
			TimerTwo[1] = 0x6;

		}
		
		//if timer 1 times out increment the outputValue and display it on hex displays
		if((TimerOne[0] & 0x1) == 0x1)
		{
			// if 3rd button press no need to output the rollover of timerOne (also reset the outputValue)
			if(buttonPressNumber == 3)
			{
				outputValue = 0;
				PrintSevenSegment_PIO(outputValue);
				break;
			}
			
			//if timer 1 rollover increment output value and print it
			outputValue++;
			PrintSevenSegment_PIO(outputValue);
			TimerOne[0] = 0x0;
		}
		
		//roll button press over back to 0 if needed
		if(buttonPressNumber == 3)
		{
			buttonPressNumber = 0;
		}
		
	}
	
return 0;
}


int PrintSevenSegment_PIO(uint32_t counter)
{

  //Storage for each counter value
  uint32_t digits = 0;

  //Loop to collect each individual digit from the counter
  for(int i = 0 ; i < 6 ; i++)
    {
      //digit gathers the 6 individual hex digits to be printed to each hex display
      digits = (counter >> (i * 4)) & 0xF;

      //Send digit to HEX0
      if(0 == i)
        {
          *HEX0 = value(digits);
        }
      //Send digits to HEX1
      if(1 == i)
        {
          *HEX1 = value(digits);
        }
      //Send digits to HEX2
      if(2 == i)
        {
          *HEX2 = value(digits);
        }
      if(3 == i)
        {
          *HEX3 = value(digits);
        }
      if(4 == i)
        {
          *HEX4 = value(digits);
        }
      if(5 == i)
        {
          *HEX5 = value(digits);
        }
    }

  return 0;
}

//Method used to convert the decimal value to the hex value to be displayed by the sevenSegDisplays
uint32_t value(uint32_t counter) 
{ 
	uint32_t disp;
	if(counter == 0)
	{
		disp = 0x40;
	}
	
	if(counter == 1)
	{
		disp = 0x79;
	}
	
	if(counter == 2)
	{
		disp = 0x24;
	}
	
	if(counter == 3)
	{
		disp = 0x30;
	}
	
	if(counter == 4)
	{
		disp = 0x19;
	}
	
	if(counter == 5)
	{
		disp = 0x12;
	}
	
	if(counter == 6)
	{
		disp = 0x2;
	}
	
	if(counter == 7)
	{
		disp = 0x78;
	}
	
	if(counter == 8)
	{
		disp = 0x0;
	}
	
	if(counter == 9)
	{
		disp = 0x18;
	}
	
	if(counter == 10)
	{
		disp = 0x8;
	}
	
	if(counter == 11)
	{
		disp = 0x3;
	}
	
	if(counter == 12)
	{
		disp = 0x46;
	}
	
	if(counter == 13)
	{
		disp = 0x21;
	}
	
	if(counter == 14)
	{
		disp = 0x6;
	}
	
	if(counter == 15)
	{
		disp = 0xE;
	}
	
	return disp;
}

