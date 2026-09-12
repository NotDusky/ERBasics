#include "ERTime.h"

Coroutine::Coroutine() : function(nullptr), execute_at(0), delay(0), looped(false) {};

Coroutine::Coroutine(Callback func, unsigned long delay, bool looped)
    : function(func), execute_at(millis() + delay), delay(delay), looped(looped) {}

void Coroutine::set(Callback func, unsigned long delay, bool is_looped) {
  function = func;
  this->delay = delay;
  looped = is_looped;
  execute_at = millis() + delay;
}

void Coroutine::cancel() {
  function = nullptr;
  looped = false;
  execute_at = 0;
}

bool Coroutine::check() {
    return (millis() >= execute_at && function != nullptr);
}

bool Coroutine::is_empty() {
    return (function == nullptr);
}

void Coroutine::run() {
  if (function) {
    function();
  }
  if (looped) {
    execute_at += delay;
  } else {
    cancel();
  }
}

void Coroutine::set_looped(bool is_looped) {
  looped = is_looped;
}

bool Coroutine::is_looped() {
  return looped;
}

void Coroutine::reschedule(unsigned long delay) {
  execute_at = millis() + delay;
}

unsigned long Coroutine::get_execute_at() {
  return execute_at;
}

void Coroutine::set_callback(Callback func) {
  function = func;
}

Callback Coroutine::get_function() {
  return function;
}

ERTime::ERTime() : prev_millis(0), d_time(0) {
  coroutines = new Coroutine[1]; // Default to 1 coroutine if no max is specified
}

ERTime::ERTime(int max_coroutines) : max_coroutines(max_coroutines), prev_millis(0), d_time(0) {
  coroutines = new Coroutine[max_coroutines];
}

ERTime::~ERTime() {
  delete[] coroutines; // Clean up the dynamically allocated array
}

void ERTime::setup() {
  prev_millis = millis();
  // Explicitly initialize all coroutines to prevent garbage data
  for (int i = 0; i < max_coroutines; i++) {
    coroutines[i].cancel();
  }
  #if ERB_LOG_LEVEL >= 1
  Serial.print("Time system initialized with a maximum of ");
  Serial.print(max_coroutines);
  Serial.println(" coroutines.");
  #endif
}

void ERTime::update() {
  uint32_t current_millis = millis();
  d_time = current_millis - prev_millis;
  prev_millis = current_millis;

  for (int i = 0; i < max_coroutines; i++) {
    // Safety check: ensure function pointer is valid before checking
    if (coroutines[i].get_function() != nullptr && coroutines[i].check()) {
      coroutines[i].run();
    }
  }
}

unsigned long ERTime::delta_time() {
  return d_time;
}

void ERTime::cancel_coroutine(Callback function) {
  for (int i = 0; i < max_coroutines; i++) {
    if (coroutines[i].get_function() == function) {
      coroutines[i].cancel();
      return;
    }
  }
}

void ERTime::cancel_coroutine(int index) {
  if (index >= 0 && index < max_coroutines) {
    coroutines[index].cancel();
  }
}

void ERTime::clear_coroutines() {
  for (int i = 0; i < max_coroutines; i++) {
    coroutines[i].cancel();
  }
}

int ERTime::start_coroutine(Callback function, long delay, bool looped) {
  for (int i = 0; i < max_coroutines; i++) {
    if (coroutines[i].is_empty()) {
      coroutines[i].set(function, delay, looped);
      return i; // Return the index of the started coroutine
    }
  }
  return -1; // Return -1 if no available slot was found
}
