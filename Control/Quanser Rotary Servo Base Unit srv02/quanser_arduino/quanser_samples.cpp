#include "quanser_samples.h"

/*LOCAL TIMER VARIABLES*/
static double clock_frequency = 16e6;
static volatile uint16_t *timer, *OCRA;
static volatile uint8_t *TIMSK, *timer_reg_a, *timer_reg_b;
static bool noise_canceler;
static uint16_t prescaller = 1;
static double timer_frequency = clock_frequency / prescaller;
static double max_time_sample = 1;

/*SAMPLE POINTS VARIABLES*/
static uint16_t points = 1;
static uint16_t motor_samples = 1;
static uint16_t timer_value_between_samples = 1;

// FUNCTIONS HEADERS
void return_to_start_position();
inline void local_delay(uint16_t local_delay);
uint16_t time_to_position(uint16_t initial_time);
inline void get_motor_generated_samples(bool move_clockwise, uint16_t points_samples[]);
bool set_prescaller_mask_value();
void get_input_pulse_width();
bool set_sample_points(uint16_t sample_points);

/*GET SAMPLES*/
void get_quanser_samples(){
  set_sample_data();
  uint16_t points_samples[points];

  Serial.println("Clockwise Samples");
  get_motor_generated_samples(true, points_samples);
  for (unsigned long int index = 0; index < points; ++index){
    Serial.print(points_samples[index]);
  }
  Serial.println();

  Serial.println("Counterclockwise Samples");
  get_motor_generated_samples(false, points_samples);
  for (unsigned long int index = 0; index < points; ++index){
    Serial.print(points_samples[index]);
  }
  Serial.println();
}

inline void get_motor_generated_samples(bool move_clockwise, uint16_t points_samples[]){
  return_to_start_position();
  uint16_t index = 0;
  *timer = 0;
  motor_move(move_clockwise);
  while (index < motor_samples){
    points_samples[index] = encoder_get_counter_value();
    ++index;
    local_delay(timer_value_between_samples);
  }
  motors_stop();
  while (index < points){
    points_samples[index] = encoder_get_counter_value();
    ++index;
    local_delay(timer_value_between_samples);
  }
}

inline void local_delay(uint16_t local_delay){
  while((*timer) < local_delay){
    // WAIT FOR
  }
  *timer = 0;
}

/*SET SAMPLES*/
void set_sample_data(){
  Serial.println("Waiting for commands to start");
  Serial.println("Enter the sampling time in seconds");
  double sampling_time = Serial.read();
  Serial.println("Enter the motor active time in seconds");
  double motor_active_time = Serial.read();
  motor_active_time = (motor_active_time > sampling_time) ? sampling_time : motor_active_time;
  Serial.println("Enter the total samples");
  while(!set_sample_points(Serial.read()));

  //CALCULATE BEST PRESCALLER FOR THE SAMPLING
  float time_between_samples = sampling_time / points;
  double max_timer_value = max_timer_value / clock_frequency;
  uint8_t prescaller_values[6] = {0,1,8,64,256,1024};
  prescaller = prescaller_values[5];
  for (uint8_t k = 1; k < 6 ; ++k) {
    if ((max_timer_value * prescaller_values[k]) > time_between_samples){
      prescaller = prescaller_values[k];
      break;
    }
  }
  set_prescaller_mask_value();

  timer_frequency = clock_frequency / prescaller;
  timer_value_between_samples = (sampling_time * timer_frequency) / points;
  time_between_samples = timer_value_between_samples / timer_frequency;
  sampling_time = points * time_between_samples;
  motor_samples = motor_active_time / time_between_samples;

  Serial.print("Clock frequency ");
  Serial.print(clock_frequency);
  Serial.println(" Hz\n");
  
  Serial.print("Prescaller ");
  Serial.println(prescaller);
  
  Serial.print("Timer frequency ");
  Serial.println(timer_frequency);
  
  Serial.print("Time between samples ");
  Serial.print(time_between_samples);
  Serial.println(" seconds\n");
  
  Serial.print("Clock value between samples ");
  Serial.println(timer_value_between_samples);
  
  Serial.print("The sampling time will be of ");
  Serial.print(sampling_time);
  Serial.println(" seconds");
  
  Serial.println("The total samples will be ");
  Serial.println(points);
  
  Serial.print("The motors will be active for ");
  Serial.print(motor_active_time);
  Serial.println(" seconds");
  
  Serial.print("The motors samples will be ");
  Serial.println(motor_samples);
}


bool set_sample_points(uint16_t sample_points){
  if(points > 0){
      points = points;
      return true;
  }
  else{
    Serial.println("Error, please enter a positive points value");
    return false;
  }

}


void return_to_start_position(){
  //Position must be equal to start_position and motors should be off
  //START CONDITIONS
  motors_stop();
  local_delay(100);
  if (encoder_get_counter_value() == 0){
    return;
  }
  //FIND THE TIME TO REACH THE POSITION
  uint16_t time_to_zero = time_to_position((uint16_t)(-1));
  uint16_t previous_time_to_zero = time_to_zero;
  uint16_t wait_time_to_zero;

//USE THE INITIAL TIME TO REACH THE DESIRED Position
  while(encoder_get_counter_value() != 0){
    wait_time_to_zero = (time_to_zero + previous_time_to_zero) / 2;
    previous_time_to_zero = time_to_zero;
    time_to_zero = time_to_position(wait_time_to_zero);
  }
}

uint16_t time_to_position(uint16_t initial_time){
  motors_stop();
  uint16_t time_to_position;
  *timer = 0;
  bool initial_turn = encoder_get_counter_value() < 0;
  motor_move(initial_turn);
  while ((initial_turn != (encoder_get_counter_value() < 0)) && (*timer < initial_time)){
      //WAIT TILL CROSS THE POSITION
  }
  time_to_position = *timer;
  time_to_position = (time_to_position > initial_time) ? initial_time : time_to_position;
  motors_stop();
  return time_to_position;
}

/*SET TIMER*/
bool set_sample_timer(double clock_frequency_value, uint16_t prescaller_value, uint16_t& timer_ref,uint16_t& OCRA_ref, uint8_t &TIMSK_ref,uint8_t& timer_reg_a_ref, uint8_t& timer_reg_b_ref, bool active_noise_canceler){
  clock_frequency = clock_frequency_value;
  prescaller = prescaller_value;
  timer = &timer_ref;
  OCRA = &OCRA_ref;
  TIMSK = &TIMSK_ref;
  timer_reg_a = &timer_reg_a_ref;
  timer_reg_b = &timer_reg_b_ref;
  noise_canceler = active_noise_canceler;
  return set_prescaller_mask_value();
}

bool set_prescaller_mask_value(){
  bool setup = false;
  *TIMSK = 0;
  *timer_reg_a = 0;
  *OCRA = (uint16_t)(-1);
  *timer_reg_b = noise_canceler ? 0b11000000 : 0b00000000;
  uint16_t prescaller_values[6] = {0,1,8,64,256,1024};
  for (uint8_t k = 0; k < 6; ++k){
    if (prescaller == prescaller_values[k]){
      *timer_reg_b |= k;
       setup = true;
    }
  }
  max_time_sample = clock_frequency * (1 << 16) * prescaller;
  *timer = 0;
  return setup;
}