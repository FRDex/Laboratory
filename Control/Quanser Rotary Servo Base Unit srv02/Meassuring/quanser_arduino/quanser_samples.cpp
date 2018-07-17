#include "quanser_samples.h"

/*LOCAL TIMER VARIABLES*/
static double clock_frequency = 16e6;
static bool noise_canceler;
static uint16_t prescaller = 1;
static double timer_frequency = clock_frequency / prescaller;
static double max_time_sample = 1;

/*SAMPLE POINTS VARIABLES*/
static uint16_t index = 0;
static uint16_t total_samples = 1;
static bool not_finnished = true;
static uint16_t indexes_value_position [3]; // clockwise, counterclockwise, zero
static uint16_t timer_count_between_samples = 500;

// FUNCTIONS HEADERS
int16_t read_int(); // READ A INTEGER VALUE FROM SERIAL
static void return_to_start_position();
static inline void local_delay(uint16_t local_delay);
static void get_quanser_samples(bool move_clockwise, int16_t angle, bool return_with_power, int16_t returning_angle, uint16_t *samples);
static inline void sampling(uint16_t *samples);

/*GET SAMPLES*/
void generate_quanser_samples(){
  bool move_clockwise = true, return_with_power = true;
  int16_t angle = 0, returning_angle = 0;
  String setup_conditions[2] = {"False", "True"};

  Serial.println("Waiting for commands to start");

  Serial.print("Total Sampling Time [ms, < 7000]: ");
  uint16_t total_sampling_time_ms = read_int();
  Serial.print(total_sampling_time_ms);
  Serial.println(" ms");

  Serial.print("Start Moving Clockwise? [0 / 1]: ");
  move_clockwise = read_int() > 0;
  Serial.println(setup_conditions[move_clockwise]);

  Serial.print("Max Angle [degrees, between -360 to 360]: ");
  angle = read_int();
  Serial.println(angle);
  angle *= (4096.0 / 360.0); //CONVERSION

  Serial.print("Return with power? [0 / 1]: ");
  return_with_power = read_int() > 0;
  Serial.println(setup_conditions[return_with_power]);

  if(return_with_power){
    Serial.print("Max Returning Angle [degrees, beteween -360 to 360]: ");
    returning_angle = read_int();
    Serial.println(returning_angle);
    returning_angle *= (4096.0 / 360.0); //CONVERSION
  }

  // POINTS
  total_samples = (float)(total_sampling_time_ms * timer_frequency) / (timer_count_between_samples * 1000.0);
  Serial.print("Total number of samples: ");
  Serial.println(total_samples);
  Serial.println();

  // TIMER
  Serial.print("Clock frequency: ");
  Serial.print(clock_frequency);
  Serial.println(" Hz\n");

  Serial.print("Prescaller: ");
  Serial.println(prescaller);
  Serial.println();

  Serial.print("Timer frequency: ");
  Serial.print(timer_frequency);
  Serial.println(" Hz\n");

  // SAMPLING
  Serial.print("Time between samples [Ts]: ");
  Serial.print(1000.0 * ((float) timer_count_between_samples) / ((float) timer_frequency));
  Serial.println(" ms\n");

  Serial.print("Sampling frequency [1/Ts]: ");
  Serial.print(timer_frequency / timer_count_between_samples);
  Serial.println(" Hz\n");


  // MEASSURE
  int16_t samples[total_samples];
//  int16_t samples2[total_samples];
  get_quanser_samples(move_clockwise, angle, return_with_power, returning_angle, samples);

  // INPUT
  double voltage_values [] = {-5.10, 5.10};
//  double voltage_values [] = {0.0, 0.0};
  uint16_t min_index_value = 0, last_index = 0;
  Serial.println("ARDUINO INPUT:");
  Serial.print("[");
  if(return_with_power){
    min_index_value = indexes_value_position[move_clockwise] > total_samples ? total_samples : indexes_value_position[move_clockwise];
    Serial.print(voltage_values[move_clockwise]);
    for(uint16_t k = 1; k < min_index_value; ++k){
      Serial.print(", ");
      Serial.print(voltage_values[move_clockwise]);
    }

    min_index_value = indexes_value_position[2] > total_samples ? total_samples : indexes_value_position[2];
    for(uint16_t k = indexes_value_position[move_clockwise]; k < min_index_value; ++k){
        Serial.print(", ");
        Serial.print(voltage_values[!move_clockwise]);
      }
  }
  else{
    min_index_value = indexes_value_position[2] > total_samples ? total_samples : indexes_value_position[2];
    Serial.print(voltage_values[move_clockwise]);
    for(uint16_t k = 1; k < min_index_value; ++k){
      Serial.print(", ");
      Serial.print(voltage_values[move_clockwise]);
    }
  }

  for(uint16_t k = min_index_value; k < total_samples; ++k){
      Serial.print(", ");
      Serial.print(0.0);
    }

  Serial.println("]");
  
//  Serial.println("ARDUINO MOTOR:");
//  Serial.println("ADC VALUE [MAX = 255]:");
//    Serial.print("[");
//    Serial.print(samples[0]);
//    for(uint16_t k = 1; k < total_samples; ++k){
//      Serial.print(", ");
//      Serial.print(samples2[k]);
//    }
//    Serial.println("]");

  Serial.println("ANGLE SYSTEM OUTPUT:");
  Serial.println("WITH ENCODER COUNTER VALUE [MAX = 4096]:");
  Serial.print("[");
  Serial.print(samples[0]);
  for(uint16_t k = 1; k < total_samples; ++k){
    Serial.print(", ");
    Serial.print(samples[k]);
  }
  Serial.println("]");
/*
  Serial.println("IN DEGREES:");
  Serial.print("[");
  double counter_to_degrees = 360.0 / 4096.0;
  Serial.print(samples[0] * counter_to_degrees);
  for(uint16_t k = 1; k < total_samples; ++k){
    Serial.print(", ");
    Serial.print(samples[k] * counter_to_degrees);
  }
  Serial.println("]");

  Serial.println("IN RADIANS:");
  Serial.print("[");
  double counter_to_radians = 2.0 * PI / 4096.0;
  Serial.print(samples[0] * counter_to_degrees);
  for(uint16_t k = 1; k < total_samples; ++k){
    Serial.print(", ");
    Serial.print(samples[k] * counter_to_radians);
  }
  Serial.println("]");*/
  delay(1000);
  return;
}

int16_t read_int(){
  while(Serial.available() <= 0){
    delay(100);
  }
  int16_t result = 0;
  char dato = 0;
  int16_t sign  = 1;
  while(Serial.available() > 0){
    dato = Serial.read();
    if (dato == '-'){
      sign *= -1;
    }
    else{
      result *= 10;
      result += dato - '0';
    }
  }
  result *= sign;
  return result;
}


static void get_quanser_samples(bool move_clockwise, int16_t angle, bool return_with_power, int16_t returning_angle, uint16_t *samples){
  while(encoder_get_counter_value() != 0){
    return_to_start_position();
  }
  Serial.println("Sampling Process Has Started");
  delay(1000);
  // RESTART INDEXES
  index = 0;
  indexes_value_position[0] = (uint16_t)(-1);
  indexes_value_position[1] = (uint16_t)(-1);
  indexes_value_position[2] = (uint16_t)(-1);
  not_finnished = true;


// MEDICION ENCODER
  bool release = false;
  int16_t encoder_position = 0;
//  Serial.println("Send a value greater than 1 to start meassuring");
//  Serial.println("Meanwhile you'll see the encoder angle");
//  while (!release){
//    if (encoder_get_counter_value() != encoder_position){
//    encoder_position = encoder_get_counter_value();
//    Serial.println(encoder_position * 360.0 / 4096.0 );
//    }
//    if (Serial.available() > 0){
//      release = read_int() >= 1;
//    }
//  }
  

  motor_move(move_clockwise, 255);
  TCNT5 = 0;

  if (move_clockwise){
    indexes_value_position[0] = index;
    while(encoder_get_counter_value() < angle){
      sampling(samples);
    }

    if(return_with_power){
      motor_move(!move_clockwise, 255);
      indexes_value_position[1] = index;
      while(encoder_get_counter_value() > returning_angle){
        sampling(samples);
      }
    }
  }
  else{
    indexes_value_position[1] = index;
    while(encoder_get_counter_value() > angle){
      sampling(samples);
    }
    if(return_with_power){
      motor_move(!move_clockwise, 255);
      indexes_value_position[0] = index;
      while(encoder_get_counter_value() < returning_angle){
        sampling(samples);
      }
    }
  }

  motors_stop();

  indexes_value_position[2] = index;
  while(index < total_samples){
    sampling(samples);
  }
  return;
}

static inline void sampling(uint16_t *samples){
  if (not_finnished){
  samples[index] = encoder_get_counter_value();
//  samples2[index] = analogRead(A0);
  ++index;
  local_delay(timer_count_between_samples);
  }
  not_finnished = index < total_samples;
}


static inline void local_delay(uint16_t local_delay){
  while(TCNT5 < local_delay){
    // WAIT FOR
  }
  TCNT5 = 0;
}



// CALIBRATION OF ZERO
static void return_to_start_position(){
  if(encoder_get_counter_value() == 0){
    return;
  }
  motor_move(true, 255);
  delay(10);
  //Position must be equal to start_position and motors should be off
  //START CONDITIONS
  motors_stop();
  delay(100);
  
  int16_t angle = encoder_get_counter_value();
  int16_t abs_angle = angle > 0 ? angle : -angle;
  int16_t angle_max_power = 4096;

  bool starting_angle_turn = angle < 0;
  bool new_angle_turn = starting_angle_turn;
  uint16_t max_power = 255;  // 5 v
  uint16_t min_power = 20; // 0.25 v
  uint16_t power = 0;

  while (angle != 0){
    angle = encoder_get_counter_value();
    starting_angle_turn = angle < 0;
    new_angle_turn = starting_angle_turn;

    abs_angle = starting_angle_turn ? angle : -angle;
    power = (abs_angle >= angle_max_power) ? max_power : (float)(max_power) * (abs_angle / angle_max_power);
    power = power >= min_power ? power : min_power;

    motor_move(starting_angle_turn, power);
    while ((starting_angle_turn == new_angle_turn) && (angle != 0)){
      angle = encoder_get_counter_value();
      new_angle_turn = angle < 0;
    }
    
    motors_stop();
    angle = encoder_get_counter_value();
    delay (100);
    while (angle != encoder_get_counter_value()){
      angle = encoder_get_counter_value();
      delay (100);
    }
    delay(100);
    angle = encoder_get_counter_value();
  }
  motors_stop();
  Serial.println("System has return to start position");
  delay(1000);

  return true;
}


/*SET TIMER*/
bool setup_quanser_samples(bool active_noise_canceler, uint16_t prescaller_value, float desired_time_between_samples_in_microseconds){
  // MASK INTERRUPTS
  TIMSK5 = 0;
  // TIME COUNTER REGESTER
  TCCR5A = 0;
  TCCR5B = active_noise_canceler ? 0b11000000 : 0b00000000;
  OCR5A = (uint16_t)(-1);

  bool setup = false;

  uint16_t prescaller_values[6] = {0,1,8,64,256,1024};

  for (uint8_t k = 0; k < 6; ++k){
    if (prescaller_value == prescaller_values[k]){
      TCCR5B |= k;
      prescaller = prescaller_value;
    }
  }
  timer_frequency = clock_frequency / (double) prescaller;
  max_time_sample = (1 << 16) / timer_frequency;
  TCNT5 = 0;
  timer_count_between_samples = desired_time_between_samples_in_microseconds * 16 / prescaller_value;
  return true;
}
