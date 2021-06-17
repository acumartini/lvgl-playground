#include <Arduino.h>
#include <SPI.h>
#include <IntervalTimer.h>
#include <USBHost_t36.h>
#include <Audio.h>
#include "imxrt_hw.h"
#include "imxrt.h"

#include "display/display.h"

using namespace display;

IntervalTimer updateTimer;
void update();
uint32_t heartbeat = 0;
bool millis_20 = false;

extern unsigned long _heap_end;
extern char *__brkval;

void setup()
{
	Serial.begin(115200);
	// while(!Serial && millis() < 2000);
	while(!Serial);
		
	updateTimer.priority(255);
	updateTimer.begin(update, 1000);

	DC.begin();
	DC.render();
	Serial.printf("\r\nfreeram at end of DC.render() = %d bytes\r\n\n", (char *)&_heap_end - __brkval); delay(10);
}

void loop()
{
	if (millis_20) {
		lv_task_handler();
		millis_20 = false;
	}
}

void update()
{
	lv_tick_inc(1);

	if (heartbeat % 10 == 0) {
		DC.update();
	}

	if (heartbeat % 21 == 0) {
		millis_20 = true;
	}

	heartbeat++;
}
