#ifndef ER_BASICS_H
#define ER_BASICS_H

#include <ArxTypeTraits.h>
#include "EREssentialFunctions.h"
#include "EREnumerations.h"

typedef void(*VoidCallback) ();

namespace ERBasics {
	unsigned int time_cycle(unsigned long t, unsigned long interval);
	unsigned int time_cycle(unsigned long t, unsigned long interval, unsigned int overflow);

	void throw_error(char* err_str, bool stop);
}

#ifndef ER_NO_TIME
#include "ERTime.h"
#endif

class Button {
public:
	uint8_t pin;
	Bounce button = Bounce();
	ButtonLEDMode mode = MB_NONE;

	Button(uint8_t pin, int interval);
	Button(uint8_t pin, int interval, VoidCallback _on_press);
	Button(uint8_t pin, int interval, VoidCallback _on_press, VoidCallback _on_release);
	Button(uint8_t pin, int interval, VoidCallback _on_press, VoidCallback _on_release, VoidCallback _on_hold);

	void set_on_press_callback(VoidCallback callback);
	void set_on_release_callback(VoidCallback callback);
	void set_on_hold_callback(VoidCallback callback);

	void check_button();

	virtual void update_led_state(bool fell);
	virtual void set_led_state(LEDState led_state);

	VoidCallback on_press;
	VoidCallback on_release;
	VoidCallback on_hold;
};

class MonoButton : Button {
public:
	ButtonLEDMode led_mode = MB_NONE;

	void update_led_state();
	void set_led_state(LEDState led_state);
};

template<template<uint8_t DATA_PIN, EOrder RGB_ORDER> class CHIPSET, uint8_t DATA_PIN, EOrder C_ORDER = RGB>
class LEDStrip {
public:
	CRGB* leds;
	uint16_t num_leds, num_sections;
	uint16_t leds_per_section;
	const uint8_t data_pin;

	LEDStrip(uint16_t num_leds, uint16_t brightness = 100)
		: num_leds(num_leds), data_pin(DATA_PIN) {
		leds = (CRGB*)malloc(num_leds);
		FastLED.addLeds<CHIPSET, DATA_PIN, C_ORDER>(leds, num_leds)
			.setCorrection(TypicalLEDStrip)
			.setDither(brightness < 255);
		FastLED.setBrightness(brightness);
	}

  LEDStrip(CRGB* leds, uint16_t num_leds, uint16_t brightness = 100)
    : num_leds(num_leds), data_pin(DATA_PIN) {
    this->leds = leds;
    FastLED.addLeds<CHIPSET, DATA_PIN, C_ORDER>(this->leds, num_leds)
      .setCorrection(TypicalLEDStrip)
      .setDither(brightness < 255);
    FastLED.setBrightness(brightness);
  }  

	void fill(CRGB color) {
		fill_solid(leds, num_leds, color);
		FastLED.show();
	}

	void clear() {
		fill_solid(leds, num_leds, CRGB::Black);
		FastLED.show();
	}

	void set_leds_per_section(uint16_t num) {
		if (!num) return;
		leds_per_section = num;
		num_sections = num_leds / num + (num_leds % num != 0);
	}

	void set_led(uint16_t position, CRGB color) {
		leds[position] = color;
		FastLED.show();
	}

	void set_range(uint16_t start, uint16_t end, CRGB color) {
		fill_solid(leds + start, start - min(end, num_leds), color);
		FastLED.show();
	}

	void set_section(uint16_t section, CRGB color) {
		uint16_t offset = leds_per_section * section;
		uint16_t num = min(leds_per_section, num_leds - offset);
		fill_solid(leds + offset, num, color);
		FastLED.show();
	}

	void set_section_range(uint16_t start, uint16_t end, CRGB color) {
		uint16_t offset = leds_per_section * start;
		uint16_t num = min(end * leds_per_section, num_leds) - offset;
		fill_solid(leds + offset, num, color);
		FastLED.show();
	}
};

class Puzzle {
public:
	Puzzle();
	Puzzle(VoidCallback on_lock);
	Puzzle(VoidCallback on_activate, VoidCallback on_complete, VoidCallback on_lock);
	Puzzle(VoidCallback on_activate, VoidCallback on_complete, VoidCallback on_lock, VoidCallback on_set);

	void set_activate_callback(VoidCallback callback);
	void set_complete_callback(VoidCallback callback);
	void set_lock_callback(VoidCallback callback);
	void set_set_callback(VoidCallback callback);

	void attach_active_loop(VoidCallback callback);

	void(*lock)();
	void(*set)();
	void(*activate)();
	void(*complete)();

	void set_state(PuzzleState state);
	PuzzleState get_state();

	void run();

private:
	PuzzleState state = PUZZLE_SET;
	VoidCallback active_loop;
};

#include "ERFastSerial.h"

#endif