#include "aging_policy.h"
#include "time_cache.h"
#include <Arduino.h>
#include <Wire.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/atomic.h>

//#define DEBUG

#define SLEEP_INTERVAL_MSEC 1000
#define SLEEP_INTERVAL_KEY WDTO_1S
#include "debug.h"

int wakePin = 2; // pin used for waking up
int led = 13;

#define INTERRUPT_PIN 2
#define TRIGGER_PIN 9

typedef enum run_mode_ { IDLE = 0, TIME_INTERRUPT, PIN_INTERRUPT } Run_mode;

void process_action();
void process_time_passed();
void pin_interrupt_handler();
void set_eco_mode_if_needed();

bool current_eco_mode = false;
bool require_eco_mode = false;
#define NO_SLOT -1
int last_updated_slot = NO_SLOT;
DayCache usage_cache;

aging_policy policy = make_day_aging_policy();

volatile Run_mode run_mode = IDLE;

void setup() {

  pinMode(INTERRUPT_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);

  // attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), process_action,
  //                 RISING); // use interrupt 0 (pin 2) and run
  //                          // function wakeUpNow when pin 2 gets LOW
#ifdef DEBUG
  Serial.begin(115200);
  DebugLog("Done running setup" CR);
#endif
}

void sleepNow() {
#ifdef DEBUG
  Serial.flush();
#endif
  ADCSRA &= ~(1 << ADEN); // ADC off
  wdt_enable(SLEEP_INTERVAL_KEY);
  WDTCSR |= (1 << WDIE);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  cli();          // sleep mode is set here
  sleep_enable(); // enables the sleep bit in the mcucr register
  sleep_bod_disable();
  sei();
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), pin_interrupt_handler,
                  RISING); // use interrupt 0 (pin 2) and run function
  sleep_mode();            // here the device is actually put to sleep!!
  sleep_cpu(); // first thing after waking from sleep: disable sleep...
  // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
  detachInterrupt(digitalPinToInterrupt(
      INTERRUPT_PIN)); // disables interrupt 0 on pin 2 so the wakeUpNow
                       // code will not be executed during normal running
                       // time.
  sleep_disable();
  sei();
  ADCSRA |= (1 << ADEN);
}

ISR(WDT_vect) { run_mode = TIME_INTERRUPT; }
void pin_interrupt_handler() { run_mode = PIN_INTERRUPT; }

inline void adjust_millis_after_sleep() {
  // Update the millis() and micros() counters, so duty cycle
  // calculations remain correct. This is a hack, fiddling with
  // Arduino's internal variables, which is needed until
  // https://github.com/arduino/Arduino/issues/5087 is fixed.
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    int millis_slept = SLEEP_INTERVAL_MSEC;
    extern volatile unsigned long timer0_millis;
    extern volatile unsigned long timer0_overflow_count;
    timer0_millis += millis_slept;
    // timer0 uses a /64 prescaler and overflows every 256 timer ticks
    timer0_overflow_count +=
        microsecondsToClockCycles((uint32_t)millis_slept * 1000) / (64 * 256);
  }
}

bool should_update_slot(const unsigned int slot) {
  if (slot != last_updated_slot) {
    last_updated_slot = slot;
    return true;
  }
  return false;
}

void loop() {
  DebugLog(CR);
  switch (run_mode) {
  case IDLE:
    DebugLog("Loop - Idle, nothing to do" CR);
    break;
  case TIME_INTERRUPT:
    DebugLog("Loop - processing time interrupt" CR);
    adjust_millis_after_sleep();
    process_time_passed();
    set_eco_mode_if_needed();
    break;
  case PIN_INTERRUPT:
    DebugLog("Loop - processing pin interrupt" CR);
    process_action();
    break;
  }
  run_mode = IDLE;
  // #ifdef DEBUG
  //   Serial.println("State:");
  //   Serial.print("Time:                 ");
  //   Serial.println(millis());
  //   Serial.print("Current/Required eco: ");
  //   Serial.print(current_eco_mode);
  //   Serial.println(require_eco_mode);

  // #endif
  DebugLog("Loop - going to sleep... zzz!" CR);
  sleepNow(); // sleep function called here
  DebugLog("Loop - Woke up from sleep!" CR);
}

void set_eco_mode_if_needed() {
  if (current_eco_mode != require_eco_mode) {
    DebugLog("set_eco - Echo mode change detected: triggering update to: ");
    DebugLog(require_eco_mode);
    DebugLog(CR);
    current_eco_mode = require_eco_mode;
    digitalWrite(TRIGGER_PIN, HIGH);
    delay(400);
    digitalWrite(TRIGGER_PIN, LOW);
  } else {
    DebugLog("set_eco - Echo mode hasn't changed: nothing to do" CR);
  }
}

void age_cache_if_needed() {
  msec now = millis();
  DebugLog("Now we're at time: ");
  DebugLog(now);
  DebugLog(CR);
  if (is_within_same_period(policy, now) == false) {
    DebugLog(
        "age_cache_if_needed - We're not in the same period anymore - Aging "
        "cache now!" CR);
    set_period(policy, now);
    age_cache(usage_cache);
    last_updated_slot = NO_SLOT;
  } else {
    DebugLog("age_cache_if_needed - Same period - No need to age cache." CR);
  }
}

void process_action() {

  msec time_now = millis();
  SlotIndex slot = time_to_slot(time_now);
  if (should_update_slot(slot) == true) {
    DebugLog(
        "process_action - Action triggered - setting activity to cache. " CR);
    add_activity_to_slot(usage_cache, slot);
    DebugLog("process_action - Increased slot: ");
    DebugLog(slot);
    DebugLog(CR);
  } else {
    DebugLog("process_action - Action triggered within the same slot - nothing "
             "to do. " CR);
  }
}

// this is not considering the slot in the cache !Im working with slot[0] all
// the
//     time !!

void process_time_passed() {
  age_cache_if_needed();
  msec now = millis();
  SlotIndex slot = time_to_slot(now);
  if (is_timeslot_active(usage_cache, slot)) {
    DebugLog("process_time_passed - Time slot ");
    DebugLog(slot);
    DebugLog(" is active" CR);
    require_eco_mode = false;
  } else {
    DebugLog("process_time_passed - Time slot ");
    DebugLog(slot);
    DebugLog(" is not active" CR);
    require_eco_mode = true;
  }
}
