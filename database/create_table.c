//c99 create_table.c -o create_table -lwiringPi -lwiringPiDev `mysql_config --cflags --libs`

#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <stdint.h>
#include <time.h>

#define MAXTIMINGS 85
#define DHT11_PIN 7

//macro that indicates whether the data is stored in the database indefinitely or deleted after a certain time
#define KEEP_DATA 1

//array which stores measurements from the DHT11 sensor
int dht11_data[5] = {0, 0, 0, 0, 0};

//variables used to store data in database
time_t datetime;
double temperature;
double humidity;

int correct;  //value 1 if measurement from sensor is correct, 0 if not 
	
//function used to avoid unnecessary repetition
void finish_with_error(MYSQL *con)
{
	fprintf(stderr, "%s\n", mysql_error(con));
	mysql_close(con);
	exit(1);
}

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
	delay(18);
	digitalWrite( DHT11_PIN, HIGH );
	delayMicroseconds(40);
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
		datetime = time(NULL);
		humidity = dht11_data[0] + (double)dht11_data[1]/10;
		temperature = dht11_data[2] + (double)dht11_data[3]/10;
		
		correct = 1;
	}
	else  
	{
		//printf( "Data not good, skip\n" );
		correct = 0;
	}
}

int main(int argc, char *argv[])
{
	//argument specifies after how long the measurement is repeated
	int sec = atoi(argv[1]);
	
	if (wiringPiSetup() == -1)
		exit(1);
		
	MYSQL *con = mysql_init(NULL);

	if (con == NULL)
	{
	  fprintf(stderr, "%s\n", mysql_error(con));
	  exit(1);
	}

	if (mysql_real_connect(con, "localhost", "raspberry", "pi", "DHT11_SENSOR", 0, NULL, 0) == NULL)
	{
	  fprintf(stderr, "%s\n", mysql_error(con));
	  mysql_close(con);
	  exit(1);
	}

	if (mysql_query(con, "DROP TABLE IF EXISTS sensor")) {
		finish_with_error(con);
	}

	//creating table for measurements in database
	if (mysql_query(con, "CREATE TABLE sensor(datetime VARCHAR(255) NOT NULL, temperature DOUBLE NOT NULL, humidity DOUBLE NOT NULL)")) {
		finish_with_error(con);
	}

	char *st = "INSERT INTO sensor(datetime, temperature, humidity) VALUES('%s', %.1f, %.1f)";
	size_t st_len = strlen(st);
	size_t datetime_size;
	char query[200];
	int len;
	int num = 0;
	
	while (1)
	{
		read_dht11();
		if (correct)
		{
			datetime_size = strlen(ctime(&datetime));
			len = snprintf(query, st_len + datetime_size, st, ctime(&datetime), temperature, humidity);
			
			//insert data in table 
			if (mysql_real_query(con, query, len)) 
			{
				finish_with_error(con);
			}
			//wait for next measurement
			delay(sec * 1000); 
			
			num++;
			
			//if macro KEEP_DATA is zero, old data is deleted after 100 measurements			
			if (num == 100 && !KEEP_DATA)
			{
				if (mysql_query(con, "DELETE FROM sensor")) {
					finish_with_error(con);
				}
				num = 0;
			}
		}
		
	}

	mysql_close(con);
  
	return 0;
}
