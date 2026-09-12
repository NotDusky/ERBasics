#ifndef ER_TIME_H
#define ER_TIME_H

#include "Arduino.h"

#ifndef ERB_LOG_LEVEL
#define ERB_LOG_LEVEL 1
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
  Coroutine();
  /** Constructor to initialize a coroutine with a function, delay, and looped flag */
  Coroutine(Callback func, unsigned long delay, bool looped = false);

  /** Set the coroutine's function, delay, and looped flag */
  void set(Callback func, unsigned long delay, bool is_looped = false);
  
  /** Cancel the coroutine */
  void cancel();

  /** Check if the coroutine is ready to be executed */
  bool check();

  /** Check if the coroutine is empty */
  bool is_empty();

  /** Run the coroutine and reschedule it if it's looped */
  void run();

  /** Set the looped flag for the coroutine */
  void set_looped(bool is_looped);

  /** Check if the coroutine is set to loop */
  bool is_looped();

  /** Reschedule the coroutine to run after a new delay */
  void reschedule(unsigned long delay);

  /** Get the scheduled execution time for the coroutine */
  unsigned long get_execute_at();

  /** Set the callback function for the coroutine */
  void set_callback(Callback func);

  /** Get the callback function for the coroutine */
  Callback get_function();

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
class ERTime {
public:
  ERTime();
  ERTime(int max_coroutines);

  ~ERTime();

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
  void clear_coroutines();

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
  /** The maximum number of coroutines that can be managed */
  const int max_coroutines = 1;
  /** The previous time in milliseconds when update() was called */
  unsigned long prev_millis = 0;
  /** The time in milliseconds since the last update() call */
  unsigned long d_time = 0;
  /** Array of coroutines managed by the time system */
  Coroutine *coroutines;
};

extern ERTime time;

#endif