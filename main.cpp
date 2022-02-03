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

#ifdef RASPBERRY
  #include <BlynkApiWiringPi.h>
#else
  #include <BlynkApiLinux.h>
#endif
#include <BlynkSocket.h>
#include <BlynkOptionsParser.h>
#include "./myCode_blynk/DHT.hpp"
#include <fstream>


static BlynkTransportSocket _blynkTransport;
BlynkSocket Blynk(_blynkTransport);
#define DHT11_Pin  22		//define the pin of sensor
static const char *auth, *serv;
static uint16_t port;
float ds18b20(float results[]);
int readADC(float results[]);
float fudge = 1/.1747;
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

    DHT dht;			//create a DHT class object
    int chk;
    chk = dht.readDHT11(DHT11_Pin);
    if(chk == DHTLIB_OK)
    	Blynk.virtualWrite(V6, dht.temperature * 9.0 / 5.0 + 32.0); 

    float v,results[4] ;
    getCPUTemperature(&v);
    Blynk.virtualWrite(V1,  v); 

    readADC(results);
    Blynk.virtualWrite(V2, results[0]*fudge);
    Blynk.virtualWrite(V3, results[1]);
    Blynk.virtualWrite(V5, fudge);
    printf("fudge %f\n",fudge);
}
BLYNK_WRITE(V5)
{
	fudge = param.asFloat();
}

BLYNK_WRITE(V4)
{
	fudge = 1/.1747;
    	Blynk.virtualWrite(V5, fudge);


}
BLYNK_WRITE(V7)
{
	  switch (param.asInt())
  {
    case 1: // Item 1
      printf("Item 1 selected\n");
      break;
    case 2: // Item 2
      printf("Item 2 selected\n");
      break;
  }
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

