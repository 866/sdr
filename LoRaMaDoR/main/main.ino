#include "Packet.h"
#include "Network.h"
#include "Display.h"
#include "ArduinoBridge.h"

const long int AVG_BEACON_TIME = 30000;

Ptr<Network> Net;

void setup()
{
	Serial.begin(115200);
	oled_init();
	oled_show("setup", "", "", "");
	char *callsign = arduino_nvram_callsign_load();
	Net = net(callsign);
	oled_show("net ok", callsign, "", "");
	Serial.print(callsign);
	Serial.println(" ready");
	Serial.println();
	free(callsign);
}

long nextSendTime = millis() + 5000;

void loop()
{
	if (millis() > nextSendTime) {
		send_message();
		long int next = random(AVG_BEACON_TIME / 2,
				AVG_BEACON_TIME * 3 / 2);
		nextSendTime = millis() + next;
		return;
	}
	while (Serial.available() > 0) {
		cli_type(Serial.read());
	}
	Net->run_tasks(millis());
}

void send_message()
{
	// oled_show("send_message()");
	Net->send("QC", Params(), "LoRaMaDoR 73!");
	// oled_show("send_message() ok");
}

void app_recv(Ptr<Packet> pkt)
{
	char *msg = new char[400];
	char *msga = new char[80];
	char *msgb = new char[80];
	char *msgc = new char[80];
	snprintf(msg, 400, "RSSI %d %s < %s id %ld params %s msg %s",
				pkt->rssi(), pkt->to(), pkt->from(),
				pkt->ident(), pkt->sparams(),
				pkt->msg().cold());
	cli_showpkt(msg);
	snprintf(msga, 80, "%s < %s", pkt->to(), pkt->from());
	snprintf(msgb, 80, "id %ld rssi %d", pkt->ident(), pkt->rssi());
	snprintf(msgc, 80, "p %s", pkt->sparams());
	oled_show(msga, msgb, msgc, pkt->msg().cold());
	delete msg, msga, msgb, msgc;
}

char cli_buffer[400];
unsigned int cli_buffer_len = 0;

void cli_type(char c) {
	if (c == 13) {
		cli_enter();
	} else if (c == 8 || c == 127) {
		if (cli_buffer_len > 0) {
			cli_buffer[--cli_buffer_len] = 0;
			Serial.print((char) 8);
			Serial.print(' ');
			Serial.print((char) 8);
		}
	} else if (cli_buffer_len >= (sizeof(cli_buffer) - 1)) {
		return;
	} else {
		cli_buffer[cli_buffer_len++] = c;
		cli_buffer[cli_buffer_len] = 0;
		Serial.print(c);
	}
}

void cli_enter() {
	Serial.println();
	if (cli_buffer_len == 0) {
		return;
	}
	// Serial.print("Typed: ");
	// Serial.println(cli_buffer);
	cli_parse(cli_buffer);
	cli_buffer_len = 0;
	cli_buffer[cli_buffer_len] = 0;
}

void cli_showpkt(const char *msg) {
	Serial.println();
	Serial.println(msg);
	Serial.print(cli_buffer);
}

void cli_parse(const char *b)
{
	while (*b == ' ') {
		++b;
	}

	if (*b == '!') {
		cli_parse_meta(b);
	} else {
		Serial.println("FIXME parse packet");
	}
}

void cli_parse_meta(const char *b)
{
	if (strncmp(b, "!callsign ", 10)) {
		cli_parse_callsign(b + 10);
	} else {
		Serial.println("Unknown cmd");
	}
}

void cli_parse_callsign(const char *b)
{
	if (! Packet::check_callsign(b, strlen(b))) {
		Serial.println("Invalid callsign");
		return;
	}
	if (b[0] == 'Q') {
		Serial.println("Invalid Q callsign");
		return;
	}
	arduino_nvram_callsign_save(b);
	Serial.println("Callsign saved, restarting...");
	ESP.restart();
}
