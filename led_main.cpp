//#define BLYNK_DEBUG
#define BLYNK_PRINT stdout
#ifdef RASPBERRY
#include <BlynkApiWiringPi.h>
#else
#include <BlynkApiLinux.h>
#endif
#include <BlynkSocket.h>
#include <BlynkOptionsParser.h>

static BlynkTransportSocket _blynkTransport;
BlynkSocket Blynk(_blynkTransport);

static const char *auth, *serv;
static uint16_t port;

#include <BlynkWidgets.h>

int buttonPin = 17; //GPIO17 Pin on the Pi
bool buttonState = false; //Used to store the previous state of the button

void setup()
{
	Blynk.begin(auth, serv, port);
	pinMode(buttonPin, INPUT); //Set GPIO17 as input
	pullUpDnControl (buttonPin, PUD_UP); //Set GPIO17 internal pull up
}

void loop()
{
	Blynk.run();

	if(buttonState != digitalRead(buttonPin)) //check the button state against its last known value, if true:
	{
		if(digitalRead(buttonPin) == TRUE) //if true, set the Virtual Pin "V0" to a value of 0 (full off)
		{
			Blynk.virtualWrite(V0, 0);
		}
		else{
			Blynk.virtualWrite(V0, 255); //Else we set the virtual pin "V0" to a value of 255 (full on)
		}
	}
	else {} //if last value = current value, we do nothing.
	buttonState = digitalRead(buttonPin); //update the button state.
	
	
}

int main(int argc, char* argv[])
{
	
	parse_options(argc, argv, auth, serv,port);

	setup();
	printf("led example\n");
	while(true) {
		loop();
	}

	return 0;
}
