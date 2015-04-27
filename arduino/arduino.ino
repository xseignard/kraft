#include <SPI.h>
#include <DMD.h>
#include <TimerOne.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <CountDownTimer.h>
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

// -----------------------------------------------------------
// Counter stuff
// -----------------------------------------------------------
CountDownTimer T;
boolean paused = false;
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
	// start 72h timer
	T.SetTimer(72,0,0);
	T.StartTimer();
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
	T.Timer();
	if (T.TimeHasChanged()) {
		String time = formatTime();
		// flash when one hour has elapsed
		if (T.ShowSeconds() == 0 && T.ShowMinutes() == 0) {
			flash(2);
		}
		// randomly display a movitational message
		//else if (T.ShowSeconds() == random(1,60) && !mainPanel) {
		else if (T.ShowSeconds() == 55 && !mainPanel) {
			String randomMessage = randomMessages[random(0, 5)];
			dmd.clearScreen(true);
			scrollText(randomMessage, scrollInterval);
		}
		// else display counter
		else {
			// chunkfive
			//dmd.drawString(52, 1, time + "    ");
			// tertre
			drawString(58, 1, time + "    ");
		}
	}
}

void handlePacket(char* text) {
	// pause/play
	if (text[0] == 'p') {
		paused = !paused;
		if (paused) {
			T.PauseTimer();
		}
		else {
			T.ResumeTimer();
		}
	}
	// reset
	else if (text[0] == 'r') {
		T.ResetTimer();
	}
	// set hours/minutes/seconds of the timer
	else if (text[0] == 't') {
		String hours = String(text[1]) + String(text[2]);
		String minutes = String(text[4]) + String(text[5]);
		String seconds = String(text[7]) + String(text[8]);
		T.SetTimer(hours.toInt(), minutes.toInt(), seconds.toInt());
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

String formatTime() {
	String time = "";
	if (T.ShowHours() < 10) time += "0";
	time += T.ShowHours();
	time += ":";
	if (T.ShowMinutes() < 10) time += "0";
	time += T.ShowMinutes();
	time += ":";
	if (T.ShowSeconds() < 10) time += "0";
	time += T.ShowSeconds();
	return time;
}

void flash(int times) {
	for(int i = 0; i < times; i++) {
		dmd.clearScreen(false);
		delay(500);
		dmd.clearScreen(true);
		delay(500);
	}
}

