#include "quadrature_encoder.h"
/*HARDWARE INPUT*/
static uint8_t encoder_clockwise_pin = 0, encoder_counterclockwise_pin = 0;

/*CONDITIONS*/
static bool pins_are_set = false;
static bool encoder_limit_is_set = false;
static bool encoder_is_active = false;
static volatile bool Is_ClockWise = true;
static bool is_radian = true;

/*POSITION RELATED VARIABLES*/
static volatile int16_t encoder_position_counter = 0;
static int16_t encoder_positive_limit = 2048;
static int16_t encoder_positive_limit_round = 2 * encoder_positive_limit;
static int16_t encoder_negative_limit = -encoder_positive_limit;
static int16_t encoder_negative_limit_round = 2 * encoder_negative_limit;
static float encoder_scale = (PI) / encoder_positive_limit;


// FUNCTIONS HEADERS
/*ISR FUNCTIONS*/
static inline void encoder_counter_module();
static void encoder_update_clockwise_position();
static void encoder_update_counterclockwise_position();

/*SETUP ENCODER HEADERS*/
static bool encoder_set_pins(uint8_t clockwise_pin, uint8_t counterclockwise_pin);
static bool encoder_set_limit(uint16_t max_value);  // ENCODER MAX ABSOLUTE VALUE


// FUNCTIONS DEFINITIONS
/*ENCODER VALUES*/
float encoder_get_angle(){
  return encoder_position_counter * encoder_scale;
}

int16_t encoder_get_counter_value(){
  return encoder_position_counter;
}

bool encoder_is_turning_clockwise(){
  return Is_ClockWise;
}

/*SETUP*/
bool encoder_set(uint8_t clockwise_pin, uint8_t counterclockwise_pin, bool radians, uint16_t max_value){
  if(encoder_set_limit(max_value)){
    encoder_deactivate();
    encoder_set_radians(radians);
    return encoder_set_pins(clockwise_pin,counterclockwise_pin);
  }
  return false;
}

static bool encoder_set_limit(uint16_t max_value){
  if (max_value > 0){
    encoder_positive_limit = max_value;
    encoder_negative_limit = -encoder_positive_limit;
    encoder_positive_limit_round = 2 * encoder_positive_limit;
    encoder_negative_limit_round = 2 * encoder_negative_limit;
    encoder_limit_is_set = true;
    return true;
  }
  return false;
}

static bool encoder_set_pins(uint8_t clockwise_pin, uint8_t counterclockwise_pin){
  if (clockwise_pin != counterclockwise_pin){
    encoder_clockwise_pin = clockwise_pin;
    encoder_counterclockwise_pin = counterclockwise_pin;
    pinMode(encoder_clockwise_pin, INPUT_PULLUP);
    pinMode(encoder_counterclockwise_pin, INPUT_PULLUP);
    pins_are_set = true;
    return true;
  }
  return false;
}

bool encoder_set_reference_count_value(int16_t value){
  encoder_position_counter = (int16_t)(value) % encoder_positive_limit_round;
  return true;
}

bool encoder_set_reference_value(float value){
  encoder_position_counter = (int16_t)(value / encoder_scale) % encoder_positive_limit_round;
  return true;
}

bool encoder_set_actual_position_to_zero_reference(){
  return encoder_set_reference_value(0);
}

/*ACTIVATION ENCODER*/
bool encoder_activate(){
  if (pins_are_set && encoder_limit_is_set){
    if(!encoder_is_active){
      attachInterrupt(digitalPinToInterrupt(encoder_clockwise_pin), encoder_update_clockwise_position, CHANGE);
      attachInterrupt(digitalPinToInterrupt(encoder_counterclockwise_pin), encoder_update_counterclockwise_position, CHANGE);
      encoder_is_active = true;
      return true;
    }
  }
  return false;
}

bool encoder_deactivate(){
  if (encoder_is_active){
    detachInterrupt(digitalPinToInterrupt(encoder_clockwise_pin));
    detachInterrupt(digitalPinToInterrupt(encoder_counterclockwise_pin));
    encoder_is_active = false;
    return true;
  }
  return false;
}

/*RUNTIME ISR*/
static void encoder_update_clockwise_position(){
  Is_ClockWise = digitalRead(encoder_clockwise_pin) == digitalRead(encoder_counterclockwise_pin);
  encoder_position_counter += Is_ClockWise ? 1 : -1;
  encoder_counter_module();
}

static void encoder_update_counterclockwise_position(){
  Is_ClockWise = digitalRead(encoder_clockwise_pin) != digitalRead(encoder_counterclockwise_pin);
  encoder_position_counter += Is_ClockWise ? 1 : -1;
  encoder_counter_module();
}

static inline void encoder_counter_module(){
  if (encoder_position_counter >= encoder_positive_limit){
    encoder_position_counter += encoder_negative_limit_round;
  }
  else{if(encoder_position_counter <= encoder_negative_limit){
    encoder_position_counter += encoder_positive_limit_round;
    }
  }
}

bool encoder_set_radians(bool radians){
    is_radian = radians;
    if(is_radian){
        encoder_scale = PI / encoder_positive_limit;
    }
    else{
        encoder_scale = 180 / encoder_positive_limit;
    }
    return is_radian;
}

bool encoder_is_radians(){
    return is_radian;
}
