//#define BLYNK_DEBUG
#define BLYNK_PRINT stdout
#ifdef RASPBERRY
#include <BlynkApiWiringPi.h>
#else
#include <BlynkApiLinux.h>
#endif
#include <BlynkSocket.h>
#include <BlynkOptionsParser.h>
#define ALARM 12.4
#define START 12.6

static BlynkTransportSocket _blynkTransport;
BlynkSocket Blynk(_blynkTransport);

int readADC(float results[]);
static const char *auth, *serv;
static uint16_t port;

#include <BlynkWidgets.h>
BlynkTimer timer;
#include <DHT.h>
#define DHTTYPE DHT11 // DHT 11
define DHTPin  22 
DHT dht(DHTPIN, DHTTYPE);

void foo() {
	float h = dht.readHumidity();
        float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

	int rowIndex = 0;
    	float results[4] ;
    	readADC(results);
  	//Blynk.virtualWrite(V0, "add", rowIndex, "My Event", millis() / 1000);
  	Blynk.virtualWrite(V0, "add", rowIndex, "My Event", results[0]);
  	Blynk.virtualWrite(V1,  results[0]);
	printf("%f\n",results[0]);
	rowIndex++;
}
void setup()
{
  	Blynk.virtualWrite(V0, "clr");
	// Display digital clock every 10 seconds
  	timer.setInterval(10000L, foo);

	Blynk.begin(auth, serv, port);
    	Blynk.virtualWrite(V0, START);
    	Blynk.virtualWrite(V1, ALARM);
}

void loop()
{
	Blynk.run();
	timer.run();


}

int main(int argc, char* argv[])
{
	
	parse_options(argc, argv, auth, serv,port);

	setup();
	printf("my VoltStart example\n");
	while(true) {
		loop();
	}

	return 0;
}
