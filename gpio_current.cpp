/*
 * main.cpp
 *
 *  Created on: 18 jan 2022
 *      Author: fue034
 *      Description:
 *
 *
 */

//https://github.com/pjueon/JetsonGPIO
//https://github.com/valentis/jetson-nano-gpio-example
//https://github.com/jetsonhacks/jetsonTX1GPIO

#include <iostream>
// for delay function.
#include <chrono>
#include <thread>

#include <stdio.h>
#include <stdlib.h>                     // for exit()
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/mman.h>

// for signal handling
#include <signal.h>

#include "JetsonGPIO.h"

using namespace std;

static bool end_this_program = false;

inline void delay(int s) { this_thread::sleep_for(chrono::seconds(s)); }

void signalHandler(int s) { end_this_program = true; }
bool enabled = false;

# define CAM_OUT_TIME	1

// fake thread to simulate the enable signal given from the user
// it enables the cameras for 4 minutes and then
// disables them for another 4 minutes
void* enableThread(void*  arg )
{
	enabled = true;
	while(true)
	{
		/*
		if (enabled == true)
			enabled = false;
		else if (enabled == false)
			enabled = true;
		*/
		sleep(4*CAM_OUT_TIME);
	}
}

pthread_t enThread;
int start_thread()
{
	int ret;

	{
			ret = pthread_create( &enThread, NULL, enableThread, NULL );
			if (ret != 0)
				return -1;
			// If we need the threads to be started in the assigned order then we need to leave some time for the thread
			// to be started before launching the next one - this has to do with the OS scheduler ...
			//usleep(50);
	}
	return 0;
}

#define test_pin 2

int main()
{
	start_thread();
    // When CTRL+C pressed, signalHandler will be called
    //signal(SIGINT, signalHandler);

    /*	 Pin Out MAP
     *   *{ "216"s, "{}"s, "6000d000.gpio", "7", "4",   "GPIO9",     "AUD_MCLK",  None,        None },
		 *{ "50"s,  "{}"s, "6000d000.gpio", "11", "17", "UART1_RTS", "UART2_RTS", None,        None },
		 *{ "79"s,  "{}"s, "6000d000.gpio", "12", "18", "I2S0_SCLK", "DAP4_SCLK", None,        None },
		 *{ "14"s,  "{}"s, "6000d000.gpio", "13", "27", "SPI1_SCK",  "SPI2_SCK",  None,        None },
		 *{ "194"s, "{}"s, "6000d000.gpio", "15", "22", "GPIO12",    "LCD_TE",    None,        None },
		 *{ "232"s, "{}"s, "6000d000.gpio", "16", "23", "SPI1_CS1",  "SPI2_CS1",  None,        None },
		 *{ "15"s,  "{}"s, "6000d000.gpio", "18", "24", "SPI1_CS0",  "SPI2_CS0",  None,        None },
		 *{ "16"s,  "{}"s, "6000d000.gpio", "19", "10", "SPI0_MOSI", "SPI1_MOSI", None,        None },
		 *{ "17"s,  "{}"s, "6000d000.gpio", "21", "9",  "SPI0_MISO", "SPI1_MISO", None,        None },
		 *{ "13"s,  "{}"s, "6000d000.gpio", "22", "25", "SPI1_MISO", "SPI2_MISO", None,        None },

		 *{ "18"s,  "{}"s, "6000d000.gpio", "23", "11", "SPI0_SCK",  "SPI1_SCK",  None,        None },
		 *{ "19"s,  "{}"s, "6000d000.gpio", "24", "8",  "SPI0_CS0",  "SPI1_CS0",  None,        None },
		 *{ "20"s,  "{}"s, "6000d000.gpio", "26", "7",  "SPI0_CS1",  "SPI1_CS1",  None,        None },
		 *{ "149"s, "{}"s, "6000d000.gpio", "29", "5",  "GPIO01",    "CAM_AF_EN", None,        None },
		 *{ "200"s, "{}"s, "6000d000.gpio", "31", "6",  "GPIO11",    "GPIO_PZ0",  None,        None },
		 // Older versions of L4T have a DT bug which instantiates a bogus device
		 // which prevents this library from using this PWM channel.
		 *{ "168"s, "{}"s, "6000d000.gpio", "32", "12", "GPIO07",    "LCD_BL_PW", "7000a000.pwm", 0 },
		 *{ "38"s,  "{}"s, "6000d000.gpio", "33", "13", "GPIO13",    "GPIO_PE6",  "7000a000.pwm", 2 },
		 *{ "76"s,  "{}"s, "6000d000.gpio", "35", "19", "I2S0_FS",   "DAP4_FS",   None,        None },
		 *{ "51"s,  "{}"s, "6000d000.gpio", "36", "16", "UART1_CTS", "UART2_CTS", None,        None },
		 *{ "12"s,  "{}"s, "6000d000.gpio", "37", "26", "SPI1_MOSI", "SPI2_MOSI", None,        None },
		 *{ "77"s,  "{}"s, "6000d000.gpio", "38", "20", "I2S0_DIN",  "DAP4_DIN",  None,        None },
		 *{ "78"s,  "{}"s, "6000d000.gpio", "40", "21", "I2S0_DOUT", "DAP4_DOUT", None,        None }
     */

	int output_pin[4] = {10,9,25,8};
	/*										Pin#   GPIO#
	 	 *{ "16"s,  "{}"s, "6000d000.gpio", "19", "10", "SPI0_MOSI", "SPI1_MOSI", None,        None },
		 *{ "17"s,  "{}"s, "6000d000.gpio", "21", "9",  "SPI0_MISO", "SPI1_MISO", None,        None },
		 *{ "13"s,  "{}"s, "6000d000.gpio", "22", "25", "SPI1_MISO", "SPI2_MISO", None,        None },
		 *{ "19"s,  "{}"s, "6000d000.gpio", "24", "8",  "SPI0_CS0",  "SPI1_CS0",  None,        None },
	*/

	GPIO::setwarnings(false);
    // Pin Setup.
    GPIO::setmode(GPIO::BCM);
    // set pin as an output pin with optional initial state of HIGH

    for (int i=0;i<4;i++)
    	GPIO::setup(output_pin[i], GPIO::OUT, GPIO::LOW);

    int curr_value[4] = {GPIO::HIGH};
    bool shutDown = false;


    while (true)
    {

    	curr_value[test_pin] = GPIO::LOW;
		GPIO::output(output_pin[test_pin], curr_value[test_pin]);
		sleep(2);

		curr_value[test_pin] = GPIO::HIGH;
		GPIO::output(output_pin[test_pin], curr_value[test_pin]);
		sleep(2);
    	/*
    	for (int i=0;i<4;i++)
		{

			curr_value[i] = GPIO::LOW;
			GPIO::output(output_pin[i], curr_value[i]);
			//GPIO::setup(output_pin[i], GPIO::OUT, GPIO::LOW);
		}

    	sleep(2);
    	for (int i=0;i<4;i++)
		{

			curr_value[i] = GPIO::HIGH;
			GPIO::output(output_pin[i], curr_value[i]);
			//GPIO::setup(output_pin[i], GPIO::OUT, GPIO::HIGH);
		}
    	sleep(2);
    	*/
    }


/*
    while (!end_this_program)
    {
    	if (enabled)
    	{
    		shutDown = false;
			for (int i=1;i<4;i++)
			{
				sleep(1);
				curr_value[i] = GPIO::HIGH;
				GPIO::output(output_pin[i], curr_value[i]);

				for (int j=0; j<CAM_OUT_TIME; j++)
				{
					if (!end_this_program)
						sleep(1);
					else
					{
						j = CAM_OUT_TIME;
						i=4;
						for (int k=0;k<4;k++)
							GPIO::setup(output_pin[k], GPIO::OUT, GPIO::LOW);
					}
				}
				if (end_this_program)
					break;
				curr_value[i] = GPIO::LOW;
				GPIO::output(output_pin[i], curr_value[i]);
			}
    	}
    	else
    	{
    		if ((!shutDown) || end_this_program)
    		{
    			for (int i=0;i<4;i++)
    				GPIO::setup(output_pin[i], GPIO::OUT, GPIO::LOW);
    			shutDown = true;
    		}
    		sleep(1);
    	}
    }
*/
    GPIO::cleanup();

    return 0;
}
