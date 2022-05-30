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
#define PORT "8888"
#define BLYNK_GREEN     "#23C48E"
#define BLYNK_BLUE      "#04C0F8"
#define BLYNK_YELLOW    "#ED9D00"
#define BLYNK_RED       "#D3435C"
#define BLYNK_DARK_BLUE "#5F7CD8"

#include <BlynkApiWiringPi.h>
#include <BlynkSocket.h>
#include <BlynkOptionsParser.h>
#include <fstream>
#include <string.h>
#include <stdio.h>
//#include <unistd.h>

BlynkTimer timer;

static BlynkTransportSocket _blynkTransport;
BlynkSocket Blynk(_blynkTransport);
#define R_RATIO 5.63	//Voltage Divide Resistance =  1/(R2/R1+R2)  (where R1 = 47K and R2 = 220K) 
#define MBR_RATIO 5.63	//Voltage Divide Resistance =  1/(R2/R1+R2)  (where R1 = 47K and R2 = 220K) 
static const char *auth, *serv;
static uint16_t port;
float ds18b20(float results[]);
int read_esp8266(char* msg,char * temp);
void refresh();
float fudge = R_RATIO;
float fudge_mb = MBR_RATIO;
#include <BlynkWidgets.h>

void getCPUTemperature(float *v){
    FILE *fp;
    char str_temp[15];
    float CPU_temp;
    // CPU temperature data is stored in this directory.
    fp=fopen("/sys/class/thermal/thermal_zone0/temp","r");
    fgets(str_temp,15,fp);  
    *v = atof(str_temp)/1000.0;   // convert to float
    fclose(fp);
}

void refresh(){
    
    float v,results[4],tokens[5];
	char str[80],cmd[20];
	int j =0;

    Blynk.setProperty(V6,  "label",  "Inside Temp")  ;
	read_esp8266("MCP",str);
    Blynk.virtualWrite(V6, str); 

	read_esp8266("DS0",str);
	char * token = strtok(str,",");
	j =0;
	while (token !=NULL) {
		tokens[j++] = atof(token);
	   	token = strtok(NULL,",");	
	}
    Blynk.setProperty(V1,  "label",  "Outside Temp")  ;
    Blynk.virtualWrite(V1, tokens[0]); 

	sprintf(cmd,"ADC_%f_%f_%f_%f",fudge,fudge_mb,0.0,0.0);
	read_esp8266(cmd,str);
	//printf("%s\n",str);
	token = strtok(str,",");
	j =0;
	while (token !=NULL) {
		tokens[j++] = atof(token);
	   	token = strtok(NULL,",");	
	}
    Blynk.setProperty(V2,  "label",  "house voltage")  ;
    Blynk.virtualWrite(V2, tokens[0]);
    //Blynk.virtualWrite(V3, tokens[1]); // engine battery when wired
    Blynk.setProperty(V3,  "label",  "esp8266 voltae")  ;
    Blynk.virtualWrite(V3, tokens[1]); // esp8266 v3.3
	// check if "low V is trigger"
	if (tokens[4]){
		Blynk.setProperty(V2, "color", BLYNK_RED); 
	}
	else {
		Blynk.setProperty(V2, "color", BLYNK_GREEN);
   }


}
void setup()
{
   	Blynk.begin(auth, serv, port);
  	timer.setInterval(15*60000, refresh);
	refresh();
}
void loop()
{
    Blynk.run();
    timer.run();
	//delay(1000);
}


int main(int argc, char* argv[])
{
	
	parse_options(argc, argv, auth, serv, port);
    printf("Welocme to RV!");
    setup();
    while(true) {
		loop();
    }

    return 0;
}

BLYNK_WRITE(V11)
{
	char temp[80];
	read_esp8266("CLR",temp);
}
BLYNK_WRITE(V0)
{
	Blynk.virtualWrite(V7, 255);
	Blynk.virtualWrite(V8, 0);
	refresh();
	Blynk.virtualWrite(V8, 255);
	Blynk.virtualWrite(V7, 0);
}
BLYNK_WRITE(V5)
{
	//Read Custom fudge and apply
	fudge = param.asFloat();
    Blynk.virtualWrite(V5, fudge);
}
BLYNK_WRITE(V9)
{
	//Read Custom fudge and apply
	fudge_mb = param.asFloat();
    Blynk.virtualWrite(V9, fudge_mb);
}

BLYNK_WRITE(V4)
{
	//reset 
	fudge = R_RATIO;
    Blynk.virtualWrite(V5, fudge);
}
BLYNK_WRITE(V10)
{
	//reset 
	fudge_mb = MBR_RATIO;
    Blynk.virtualWrite(V9, fudge_mb);
}
BLYNK_WRITE(V14) {
	
	char str[80];
	char devices[][5] = {"ADC","DS0","MCP","BME","ALL"};
  	int index =  param.asInt();
	switch (index ) {
		case 5:
			for (int i =0 ;i<4;i++) {	
				sprintf(str,"RST_%s",devices[i]);
				read_esp8266(str,str);
			}
			break;
		default:
			sprintf(str,"RST_%s",devices[index-1]);
			read_esp8266(str,str);
			break ;
	}
}
BLYNK_WRITE(V15) {
	char str[80];
	float tokens[5],v;
  	int index =  param.asInt();
	int i =0;
	char *token ;
	switch (index ) {
  		case 1:
			read_esp8266("BME",str);
			token = strtok(str,"_");
			while (token !=NULL) {
				tokens[i++] = atof(token);
	   			token = strtok(NULL,"_");	
			}
    		Blynk.setProperty(V6,  "label",  "BME Temp")  ;
    		Blynk.virtualWrite(V6, tokens[0]); 
    		Blynk.setProperty(V1,  "label",  "BME Pressue")  ;
    		Blynk.virtualWrite(V1, tokens[1]); 
    		Blynk.setProperty(V2,  "label",  "BME  ~Altitude")  ;
    		Blynk.virtualWrite(V2, tokens[2]); 
    		Blynk.setProperty(V3,  "label",  "BME Humidity")  ;
    		Blynk.virtualWrite(V3, tokens[3]); 
			break;
		case 2:
    		getCPUTemperature(&v);
    		Blynk.virtualWrite(V6,  v); 
    		Blynk.setProperty(V6,  "label",  "Pi CPU Temperature")  ;
			break;
	}
}
