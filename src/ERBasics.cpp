#include "ERBasics.h"

namespace ERBasics {
	unsigned int time_cycle(unsigned long t, unsigned long interval) {
		return t / interval;
	}
	unsigned int time_cycle(unsigned long t, unsigned long interval, unsigned int overflow) {
		return (t / interval) % overflow;
	}

	void throw_error(char* err_str, bool stop) {
		Serial.println(err_str);
		if (stop) while (true);
	}
}

// ---- Class: Button ----------------------------------

Button::Button(uint8_t pin, int interval) : pin(pin) {
	button.attach(pin);
	button.interval(interval);
}

Button::Button(uint8_t pin, int interval, VoidCallback _on_press) : pin(pin) {
	button.attach(pin);
	button.interval(interval);
	set_on_press_callback(_on_press);
}

Button::Button(uint8_t pin, int interval, VoidCallback _on_press, VoidCallback _on_release) : pin(pin) {
	button.attach(pin);
	button.interval(interval);
	set_on_press_callback(_on_press);
	set_on_release_callback(_on_release);
}

Button::Button(uint8_t pin, int interval, VoidCallback _on_press, VoidCallback _on_release, VoidCallback _on_hold) : pin(pin) {
	button.attach(pin);
	button.interval(interval);
	set_on_press_callback(_on_press);
	set_on_release_callback(_on_release);
	set_on_hold_callback(_on_hold);
}

void Button::set_on_press_callback(VoidCallback callback) {
	this->on_press = callback;
}
void Button::set_on_release_callback(VoidCallback callback) {
	this->on_release = callback;
}
void Button::set_on_hold_callback(VoidCallback callback) {
	this->on_hold = callback;
}

void Button::check_button() {
	button.update();
	if (button.fell()) {
		if (on_press != nullptr) on_press();
		update_led_state(true);
	}
	else if (button.rose()) {
		if (on_release != nullptr) on_release();
		update_led_state(false);
	}
	if (button.read() == 0) {
		if (on_hold != nullptr) on_hold();
	}
}

void Button::update_led_state(bool fell) {}
void Button::set_led_state(LEDState led_state) {}

// ---- Class: Button ----------------------------------

// ---- Class: MonoButton ------------------------------



// ---- Class: MonoButton ------------------------------

// ---- Class: Puzzle ----------------------------------

Puzzle::Puzzle() {}

Puzzle::Puzzle(VoidCallback on_lock) {
	set_lock_callback(on_lock);
}

Puzzle::Puzzle(VoidCallback on_activate, VoidCallback on_complete, VoidCallback on_lock) {
	set_activate_callback(on_activate);
	set_complete_callback(on_complete);
	set_lock_callback(on_lock);
}

Puzzle::Puzzle(VoidCallback on_activate, VoidCallback on_complete, VoidCallback on_lock, VoidCallback on_set) {
	set_activate_callback(on_activate);
	set_complete_callback(on_complete);
	set_lock_callback(on_lock);
	set_set_callback(on_set);
}

void Puzzle::set_activate_callback(VoidCallback callback) {
	this->activate = callback;
}
void Puzzle::set_complete_callback(VoidCallback callback) {
	this->complete = callback;
}
void Puzzle::set_lock_callback(VoidCallback callback) {
	this->lock = callback;
}
void Puzzle::set_set_callback(VoidCallback callback) {
	this->set = callback;
}

void Puzzle::attach_active_loop(VoidCallback callback) {
	this->active_loop = callback;
}

void Puzzle::set_state(PuzzleState _state) {
	state = _state;
	switch (_state) {
	case PUZZLE_ACTIVE:
		if (activate != nullptr) {
			Serial.println("Puzzle Activated");
			activate();
		}
		break;
	case PUZZLE_COMPLETE:
		if (complete != nullptr) {
			Serial.println("Puzzle Completed");
			complete();
		}
		break;
	case PUZZLE_LOCKED:
		if (lock != nullptr) {
			Serial.println("Puzzle Locked");
			lock();
		}
		break;
	case PUZZLE_SET:
		if (set != nullptr) {
			Serial.println("Puzzle Set");
			set();
		}
		break;
	}
}
PuzzleState Puzzle::get_state() {
	return state;
}

void Puzzle::run() {
	if (state == PUZZLE_ACTIVE) {
		if (active_loop != nullptr) {
			active_loop();
		}
	}
}

// ---- Class: Puzzle ----------------------------------
