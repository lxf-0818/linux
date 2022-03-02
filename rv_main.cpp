/**
 * @file       main.cpp
 * @author     Volodymyr Shymanskyy
 * @license    This project is released under the MIT License (MIT)
 * @copyright  Copyright (c) 2015 Volodymyr Shymanskyy
 * @date       Mar 2015
 * @brief
 */

//#define BLYNK_DEBUG
#define BLYNK_PRINT stdout
#define IP_ADDR_ESP8266 "10.0.0.67"
#define PORT "8888"

#ifdef RASPBERRY
  #include <BlynkApiWiringPi.h>
#else
  #include <BlynkApiLinux.h>
#endif
#include <BlynkSocket.h>
#include <BlynkOptionsParser.h>
#include "./myCode_blynk/DHT.hpp"
#include <fstream>
#include <string.h>
#include <stdio.h>



static BlynkTransportSocket _blynkTransport;
BlynkSocket Blynk(_blynkTransport);
#define DHT11_Pin  22		//define the pin of sensor
#define R_RATIO 5.6840007	//Voltage Divide Resistance =  1/(R2/R1+R2)  (where R1 = 47K and R2 = 220K) 
static const char *auth, *serv;
static uint16_t port;
float ds18b20(float results[]);
int readADC(float results[]);
int read_temp_esp8266(char * temp);
int test_esp8266();
float fudge = R_RATIO;
#include <BlynkWidgets.h>

BlynkTimer tmr;
void getCPUTemperature(float *v){// sub function used to print CPU temperature
    FILE *fp;
    char str_temp[15];
    float CPU_temp;
    // CPU temperature data is stored in this directory.
    fp=fopen("/sys/class/thermal/thermal_zone0/temp","r");
    fgets(str_temp,15,fp);        // read file temp
    *v = atof(str_temp)/1000.0;   // convert to Celsius degrees
    fclose(fp);
}

BLYNK_WRITE(V0)
{
    
	Blynk.virtualWrite(V7, 255);
	Blynk.virtualWrite(V8, 0);
    	float v,results[4] ;
    	getCPUTemperature(&v);
    	Blynk.virtualWrite(V1, 0);
    	Blynk.virtualWrite(V1,  v); 

	char temp[80];
	read_temp_esp8266(temp);
	char *token = strtok(temp,",");
    	Blynk.virtualWrite(V6, 0);
      	Blynk.virtualWrite(V6, token); 
	while (token !=NULL) {
	   token = strtok(NULL,",");	
	}

    	readADC(results);
    	Blynk.virtualWrite(V2, 0);
    	Blynk.virtualWrite(V2, results[0]*fudge);
    	Blynk.virtualWrite(V3, 0);
    	Blynk.virtualWrite(V3, results[1]);
    	Blynk.virtualWrite(V5, fudge);
	Blynk.virtualWrite(V8, 255);
	Blynk.virtualWrite(V7, 0);
}
BLYNK_WRITE(V5)
{
	//Read Custom fudge and apply
	fudge = param.asFloat();
}

BLYNK_WRITE(V4)
{
	//reset 
	fudge = R_RATIO;
    	Blynk.virtualWrite(V5, fudge);
}
BLYNK_WRITE(V9)
{
	Blynk.virtualWrite(V7, 255);
	test_esp8266();
	Blynk.virtualWrite(V7, 0);
}

void setup()
{
    	Blynk.begin(auth, serv, port);
   
}
void loop()
{
    Blynk.run();
    tmr.run();
}


int main(int argc, char* argv[])
{
	
    parse_options(argc, argv, auth, serv, port);
    setup();
    printf("Welocme to RV!");
    while(true) {
        loop();
    }

    return 0;
}

