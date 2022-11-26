//gcc -o test test.c -lwiringPi -lwiringPiDev

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAXTIMINGS 85
#define DHT11_PIN 7

//array which stores measurements from the DHT11 sensor
int dht11_data[5] = {0, 0, 0, 0, 0};

//function for reading data on the DHT11 sensor
void read_dht11()
{
	uint8_t laststate = HIGH;
	uint8_t counter = 0;
	uint8_t j = 0;
	uint8_t i;
	
	dht11_data[0] = dht11_data[1] = dht11_data[2] = dht11_data[3] = dht11_data[4] = 0;

	pinMode( DHT11_PIN, OUTPUT );
	digitalWrite( DHT11_PIN, LOW );
	delay( 18 );
	digitalWrite( DHT11_PIN, HIGH );
	delayMicroseconds( 40 );
	pinMode( DHT11_PIN, INPUT );
	
	for (i = 0; i < MAXTIMINGS; i++)
	{
		counter = 0;
		while (digitalRead(DHT11_PIN) == laststate)
		{
			counter++;
			delayMicroseconds(1);
			if (counter == 255)
			{
				break;
			}
		}
		laststate = digitalRead(DHT11_PIN);
 
		if (counter == 255)
			break;
 
		//reading the measurement results from the sensor
		if ((i >= 4) && (i % 2 == 0))
		{
			dht11_data[j / 8] <<= 1;
			if (counter > 16)
				dht11_data[j / 8] |= 1;
			j++;
		}
	}
 
	//checking if measurement is correct
	if ((j >= 40) && (dht11_data[4] == ((dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3]) & 0xFF)))
	{
		printf( "Humidity = %d.%d %% Temperature = %d.%d C\n", dht11_data[0], dht11_data[1], dht11_data[2], dht11_data[3]);
	}
	else  
	{
		printf( "Data not good, skip\n" );
	}
}

int main()
{
	printf( "Raspberry Pi wiringPi DHT11 Temperature test program\n" );
 
	if (wiringPiSetup() == -1)
		exit(1);
 
	while (1)
	{
		read_dht11();
		delay(1000); 
	}

	return 0;
}
