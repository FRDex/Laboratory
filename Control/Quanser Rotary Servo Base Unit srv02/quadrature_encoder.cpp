#include <quadrature_encoder.h>

// ENCODER STATUS
static bool encoder_is_active = false;
static bool pins_are_set = false;
static bool encoder_limit_is_set = false;

// ENCODER PINS
static uint8_t encoder_clockwise_pin = 0, encoder_counterclockwise_pin = 0;
// ENCODER LIMITS
static int16_t encoder_positive_limit = 1024, encoder_negative_limit = -1024;

// POSITION RELATED VARIABLES
volatile int16_t encoder_position_counter = 0; // POSITION COUNTER
bool Is_ClockWise = true; // TURN STATUS


// ISR FUNCTIONS
inline void quaser_encoder_counter_module();
void encoder_update_clockwise_position();
void encoder_update_counterclockwise_position();

//SETUP ENCODER FUNCTIONS
// SET ENCODER PINS, RETURN TRUE IF THE VALUES WERE UPDATED CORRECTLY
bool set_encoder_pins(uint8_t clockwise_pin, uint8_t counterclockwise_pin);
// SET ENCODER MAX VALUE
bool set_encoder_limit(int16_t max_value);


/*DEFINITIONS*/

/*POSITION DATA RELATED*/
bool get_turn_status(){ //TRUE IF IT IS CLOCKWISE
  return Is_ClockWise;
}

inline int16_t get_encoder_value(){
  return encoder_position_counter;
}


bool set_encoder_reference_value(int16_t value){
  encoder_position_counter = value % encoder_positive_limit;
  return true;
}

bool set_encoder_zero_reference(){
  return set_encoder_reference_value(0);
}

/*ENCODER SETUP*/
bool set_encoder(uint8_t clockwise_pin, uint8_t counterclockwise_pin, int16_t max_value){
  set_encoder_pins(clockwise_pin,counterclockwise_pin);
  set_encoder_limit(max_value);
  return active_encoder();
}

// SET ENCODER PINS, RETURN TRUE IF THE VALUES WERE UPDATED CORRECTLY
bool set_encoder_pins(uint8_t clockwise_pin, uint8_t counterclockwise_pin){
  if (clockwise_pin != counterclockwise_pin){
    encoder_clockwise_pin = clockwise_pin;
    counterclockwise_pin = counterclockwise_pin;
    pins_are_set = true;
    return true;
  }
  return false;
}

// SET ENCODER MAX VALUE
bool set_encoder_limit(int16_t max_value){
  if (max_value != 0){
    encoder_positive_limit = abs(max_value);
    encoder_negative_limit = -encoder_positive_limit;
    encoder_limit_is_set = true;
    return true;
  }
  return false;
}

// SET PINS AS INPUTS AND ATTACH INTTERUPTS
bool active_encoder(){
  static uint8_t prev_encoder_clockwise_pin = 0, prev_encoder_counterclockwise_pin = 0;
  if (pins_are_set && encoder_limit_is_set){
    if((prev_encoder_clockwise_pin != encoder_clockwise_pin) && (prev_encoder_counterclockwise_pin != encoder_counterclockwise_pin)){
      //IF IT WAS PREVIOUSLY ACTIVE WITH OTHER PINS, WE DEACTIVATE THOSE OTHER PINS
      deactivate_encoder();
      //SET PINS AS INPUTS
      pinMode(encoder_clockwise_pin, INPUT_PULLUP);
      pinMode(counterclockwise_pin, INPUT_PULLUP);
      //INTERRUPTIONS
      attachInterrupt(digitalPinToInterrupt(encoder_clockwise_pin), encoder_update_clockwise_position, CHANGE);
      attachInterrupt(digitalPinToInterrupt(counterclockwise_pin), encoder_update_counterclockwise_position, CHANGE);
      //SET ENCODER STATUS
      encoder_is_active = true;
      //SAVE PIN VALUES
      prev_encoder_clockwise_pin = encoder_clockwise_pin;
      prev_encoder_counterclockwise_pin = encoder_counterclockwise_pin;
      //RETURN CHECKPOINT
      return true;
    }
  }
  return false;
}

// RETURN TRUE IF THE ENCODER WAS ACTIVE AND COULD BE DEACTIVATED
bool deactivate_encoder(){
  if (encoder_is_active){
    //INTERRUPTIONS
    detachInterrupt(digitalPinToInterrupt(encoder_clockwise_pin));
    detachInterrupt(digitalPinToInterrupt(encoder_counterclockwise_pin));
    //DEACTIVATE ENCODER
    encoder_is_active = false;
    //RETURN CHECKPOINT
    return true;
  }
  return false;
}

/*RUNTIME*/

// ISR COUNTER MODULE
inline void quaser_encoder_counter_module(){
  if (encoder_position_counter >= QUANSER_ENCODER_CLOCKWISE_LIMIT){
    encoder_position_counter -= QUANSER_ENCODER_MAX_MARKS_MEASURES;
  }
  else{if(encoder_position_counter <= QUANSER_ENCODER_COUNTERCLOCKWISE_LIMIT){
    encoder_position_counter += QUANSER_ENCODER_MAX_MARKS_MEASURES;
    }
  }
}

// CLOCKWISE FLANK ISR
void encoder_update_clockwise_position(){
  Is_ClockWise = digitalRead(ENCODER_CHANNEL_1) == digitalRead(ENCODER_CHANNEL_2);
  encoder_position_counter += Is_ClockWise ? 1 : -1;
  quaser_encoder_counter_module();
}

// COUNTERCLOCKWISE FLANK ISR
void encoder_update_counterclockwise_position(){
  Is_ClockWise = digitalRead(ENCODER_CHANNEL_1) != digitalRead(ENCODER_CHANNEL_2);
  encoder_position_counter += Is_ClockWise ? 1 : -1;
  quaser_encoder_counter_module();
}
