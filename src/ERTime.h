#ifndef ER_TIME_H
#define ER_TIME_H

#include "Arduino.h"

#ifndef MAX_COROUTINES
#define MAX_COROUTINES 1
#endif

#if (MAX_COROUTINES < 1) || (MAX_COROUTINES > 255)
#error "MAX_COROUTINES must be between 1 and 255"
#endif

/** Type definition for the coroutine callback function */
typedef void(*Callback) ();
/** 
 * Class representing a coroutine - 
 * a lightweight task that can be scheduled for execution
 */
class Coroutine {
public:
  /** Default constructor initializes an empty coroutine */
  Coroutine() : function(nullptr), execute_at(0), delay(0), looped(false) {}
  /** Constructor to initialize a coroutine with a function, delay, and looped flag */
  Coroutine(Callback func, unsigned long delay, bool looped = false)
      : function(func), execute_at(millis() + delay), delay(delay), looped(looped) {}

  /** Set the coroutine's function, delay, and looped flag */
  void set(Callback func, unsigned long delay, bool is_looped = false) {
    function = func;
    this->delay = delay;
    looped = is_looped;
    execute_at = millis() + delay;
  }
  
  /** Cancel the coroutine */
  void cancel() {
    function = nullptr;
    looped = false;
    execute_at = 0;
  }

  /** Check if the coroutine is ready to be executed */
  bool check() {
    return (millis() >= execute_at && function != nullptr);
  }

  /** Check if the coroutine is empty */
  bool is_empty() {
    return (function == nullptr);
  }

  /** Run the coroutine and reschedule it if it's looped */
  void run() {
    if (function) {
      function();
    }
    if (looped) {
      execute_at += delay;
    } else {
      cancel();
    }
  }

  /** Set the looped flag for the coroutine */
  void set_looped(bool is_looped) {
    looped = is_looped;
  }
  /** Check if the coroutine is set to loop */
  bool is_looped() {
    return looped;
  }

  /** Reschedule the coroutine to run after a new delay */
  void reschedule(unsigned long delay) {
    execute_at = millis() + delay;
  }
  /** Get the scheduled execution time for the coroutine */
  unsigned long get_execute_at() {
    return execute_at;
  }

  /** Set the callback function for the coroutine */
  void set_callback(Callback func) {
    function = func;
  }
  /** Get the callback function for the coroutine */
  Callback get_function() {
    return function;
  }

private:
  /** The callback function to be executed when the coroutine runs */
  Callback function;
  /** The time at which the coroutine is scheduled to execute */
  unsigned long execute_at = 0;
  /** The delay in milliseconds between executions for looped coroutines */
  unsigned long delay = 0;
  /** Flag indicating whether the coroutine should repeat after execution */
  bool looped = false;
};

/**
 * Class representing the time management system for scheduling and running coroutines.
 * It maintains a list of coroutines and handles their execution based on timing.
 * The system allows for starting, canceling, and updating coroutines,
 * as well as tracking the time elapsed between updates.
 */
template <unsigned int max_coroutines>
class _Time {
public:
  /** Initializes the time system */
  void setup();

  /** Updates the time system */
  void update();

  /**
   * Returns the milliseconds elapsed since the last update
   * @return The elapsed time in milliseconds
   */
  unsigned long delta_time();

  /**
   * Cancels a coroutine by its callback function
   * @param function The callback function of the coroutine to cancel
   */
  void cancel_coroutine(Callback function);
  
  /** 
   * Cancels a coroutine by its index
   * @param index The index of the coroutine to cancel
   */
  void cancel_coroutine(int index);

  /** 
   * Cancels all scheduled coroutines, effectively clearing the coroutine list.
   * This can be useful for resetting the time system or stopping all tasks at once.
   */
  void clear_coroutines() {
    for (int i = 0; i < max_coroutines; i++) {
      coroutines[i].cancel();
    }
  }

  /** 
   * Starts a coroutine with an optional delay before the first execution.
   * @param function The callback function to execute for the coroutine
   * @param delay The delay in milliseconds before the coroutine is first executed
   * (default: 0, meaning it will run on the next update)
   * @param looped Flag indicating whether the coroutine should repeat after execution
   * (default: false)
   * @return The index of the started coroutine, or -1 if the maximum number of coroutines is exceeded
   */
  int start_coroutine(Callback function, long delay = 0, bool looped = false);

private:
  /** The previous time in milliseconds when update() was called */
  unsigned long prev_millis = 0;
  /** The time in milliseconds since the last update() call */
  unsigned long d_time = 0;
  /** Array of coroutines managed by the time system */
  Coroutine coroutines[max_coroutines];
};

template <unsigned int max_coroutines>
void _Time<max_coroutines>::setup() {
  prev_millis = millis();
  // Explicitly initialize all coroutines to prevent garbage data
  for (int i = 0; i < max_coroutines; i++) {
    coroutines[i].cancel();
  }
}

template <unsigned int max_coroutines>
void _Time<max_coroutines>::update() {
  d_time = millis() - prev_millis;
  prev_millis = millis();

  for (int i = 0; i < max_coroutines; i++) {
    // Safety check: ensure function pointer is valid before checking
    if (coroutines[i].get_function() != nullptr && coroutines[i].check()) {
      coroutines[i].run();
    }
  }
}

template <unsigned int max_coroutines>
unsigned long _Time<max_coroutines>::delta_time() {
  return d_time;
}

template <unsigned int max_coroutines>
int _Time<max_coroutines>::start_coroutine(Callback function, long delay = 0, bool looped = false) {
  for (int i = 0; i < max_coroutines; i++) {
    if (coroutines[i].is_empty()) {
      coroutines[i].set(function, delay, looped);
      return i;
    }
  }
  #ifdef ERB_LOGGING
  Serial.println("Couldn't start coroutine. Coroutine limit exceeded.");
  #endif
  return -1;
}

template <unsigned int max_coroutines>
void _Time<max_coroutines>::cancel_coroutine(Callback function) {
  for (int i = 0; i < max_coroutines; i++) {
    if (coroutines[i].get_function() == function) {
      coroutines[i].cancel();
      return;
    }
  }
}

template <unsigned int max_coroutines>
void _Time<max_coroutines>::cancel_coroutine(int index) {
  coroutines[index].cancel();
}

extern _Time<MAX_COROUTINES> Time;

#endif