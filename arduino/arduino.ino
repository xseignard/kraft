#include <SPI.h>
#include <DMD.h>
#include <TimerOne.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
//#include "ChunkFive.h"
#include "Tertre.h"
//#include "SystemFont5x7.h"

// -----------------------------------------------------------
// DMD stuff
// -----------------------------------------------------------
#define DISPLAYS_WIDE 5
#define DISPLAYS_HIGH 1
DMD dmd(DISPLAYS_WIDE,DISPLAYS_HIGH);
int brightness = 30;
boolean mainPanel = false;
int scrollInterval = 5;

// -----------------------------------------------------------
// Ethernet/UDP stuff
// -----------------------------------------------------------
// one
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 2, 2);
// two
//byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF };
//IPAddress ip(192, 168, 2, 3);
// three
//byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xFE };
//IPAddress ip(192, 168, 2, 4);
unsigned int localPort = 8888;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
EthernetUDP Udp;

// -----------------------------------------------------------
// Random messages
// -----------------------------------------------------------
const String one = "Kraft FC // SERIGRAPHIE ULTRA";
const String two = "KRAFT FC";
const String three = "ALLEZ LES KRAFTS!!";
const String four = "KRAFTY // GO GO KRAFTY!";
const String five = "WOOOOOOOOO!";
const String randomMessages[5] = {one, two, three, four, five};

void setup() {
	// ethernet communication
	Ethernet.begin(mac, ip);
	Udp.begin(localPort);
	// set brightness of the display
	pinMode(9, OUTPUT);
	analogWrite(9, 30);
	// select font
	//dmd.selectFont(ChunkFive);
	dmd.selectFont(Tertre);
	// initialize timer for spi communication
	Timer1.initialize(5000);
	Timer1.attachInterrupt(ScanDMD);
	// flash to indicate everything is ok
	flash(1);
	delay(1000);
}

void loop() {
	int packetSize = Udp.parsePacket();
	if (packetSize) {
		// clear previous things stored in the buffer
		memset(packetBuffer, 0, sizeof(packetBuffer));
		Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
		handlePacket(packetBuffer);
	}
}

void handlePacket(char* text) {
	// set hours/minutes/seconds of the timer
	if (text[0] == 't') {
		String hours = String(text[1]) + String(text[2]);
		String minutes = String(text[4]) + String(text[5]);
		String seconds = String(text[7]) + String(text[8]);
		//flash screen when it ends
		if (hours == "00" && minutes == "00" && seconds == "00") {
			flash(5);
		}
		//flash screen when hours changes
		else if (minutes == "00" && seconds == "00") {
			flash(2);
		}
		// randomly display a movitational message
		else if (random(20) == 10 && !mainPanel) {
			String randomMessage = randomMessages[random(0, 5)];
			dmd.clearScreen(true);
			scrollText(randomMessage, scrollInterval);
		}
		// or the time
		else {
			String timeToDisplay = hours + ":" + minutes + ":" + seconds;
			drawString(58, 1, timeToDisplay + "    ");
		}
	}
	// random mode
	else if (text[0] == 'r') {
		String randomMessage = randomMessages[random(0, 5)];
		dmd.clearScreen(true);
		scrollText(randomMessage, scrollInterval);
	}
	// set scrolling speed
	else if (text[0] == 's') {
		String msg = String(text[1]) + String(text[2]) + String(text[3]);
		scrollInterval = msg.toInt();
		scrollInterval = constrain(scrollInterval, 5, 100);
	}
	// set a defined brightness
	else if (text[0] == 'b') {
		String msg = String(text[1]) + String(text[2]) + String(text[3]);
		brightness = msg.toInt();
		brightness = constrain(brightness, 0, 255);
	}
	// increase brightness
	else if (text[0] == '+') {
		brightness += 10;
		brightness = constrain(brightness, 0, 255);
	}
	// decrease brightness
	else if (text[0] == '-') {
		brightness -= 10;
		brightness = constrain(brightness, 0, 255);
	}
}

void ScanDMD() { 
	dmd.scanDisplayBySPI();
	analogWrite(9, brightness);
}

void drawString(int x, int y, String text) {
	int length = text.length() + 1;
	char textChar[length];
	text.toCharArray(textChar, length);
	dmd.drawString(x, y, textChar, strlen(textChar), GRAPHICS_NORMAL);
}

void scrollText(String text, int interval) {
	int length = text.length() + 1;
	char textChar[length];
	text.toCharArray(textChar, length);
	dmd.drawMarquee(textChar, strlen(textChar),(32*DISPLAYS_WIDE)-1,1);
	long start = millis();
	long timer = start;
	boolean ret = false;
	while(!ret){
		if ((timer + interval) < millis()) {
			ret = dmd.stepMarquee(-1,0);
			timer = millis();
		}
	}
}

void flash(int times) {
	int oldBrightness = brightness;
	brightness = 255;
	for(int i = 0; i < times; i++) {
		dmd.clearScreen(false);
		delay(500);
		dmd.clearScreen(true);
		delay(500);
	}
	brightness = oldBrightness;
}

