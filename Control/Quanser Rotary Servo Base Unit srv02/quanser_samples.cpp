#include <quanser_motor.h>
#include <quadrature_encoder.h>

//LOCAL TIMER VARIABLES
volatile uint16_t *timer, *OCRA;
volatile uint16_t *timer_reg_a,*timer_reg_b;
bool noise_canceler;

double clock_frequency = 16e6;

uint8_t prescaller = 1;
double timer_frequency = clock_frequency / prescaller;

uint16_t points = 1;
uint16_t timer_value_between_samples = 1;


int16_t start_position = 0;
void return_to_start_position();
bool set_start_position(uint16_t start_position);
inline void delay(uint16_t delay);
uint16_t time_to_position(uint16_t initial_time);

inline void get_motor_generated_samples(bool move_clockwise, uint16_t points_samples[] ,uint16_t points, uint16_t time_between_samples);
void set_prescaller_mask_value();
void get_input_pulse_width();


void set_sample_points(uint16_t points){
  if(points > 0){
      this->points = points;
  }
  else{
    Serial.println("Error, please enter a positive points value")
  }

}
void activate_sample_timer(){
  set_prescaller_mask_value();
}
void deactivate_sample_timer(){
  *timer_reg_b = 0;
}

void set_prescaller_mask_value(){
  TIMSK &= 0b00110111;
  *timer_reg_a = 0;
  *timer_reg_b = noise_canceler ? 0b11000000 : 0b00000000;
  uint8_t prescaller_values[6] = [0,1,8,64,256,1024];
  for (uint8_t k = 0; k < 6; ++k){
    if (prescaller == prescaller_value[k]){
      *timer_reg_b |= k;
    }
  }
  *timer = 0;
}

void set_sample_timer(double clock_frequency, uint8_t prescaller, uint16_t &timer,uint16_t &OCRA,uint8_t &timer_reg_a, uint8_t &timer_reg_b, bool noise_canceler){
  this->clock_frequency = clock_frequency;
  this->prescaller = prescaller;
  this->timer = &timer;
  this->OCRA = &OCRA;
  this->timer_reg_a = &timer_reg_a;
  this->timer_reg_b = &timer_reg_b;
  this->noise_canceler = noise_canceler;
  set_prescaller_mask_value();
}


void get_input_pulse_width(){
  Serial.println("Waiting for commands to start");
  Serial.println("Enter the pulse width in seconds");
  double pulse_width = Serial.read();
  timer_frequency = clock_frequency / prescaller;
  timer_value_between_samples = (pulse_width * timer_frequency) / points;
  time_between_samples = timer_value_between_samples / timer_frequency;
  pulse_width = points * time_between_samples;
  Serial.printf("Clock frequency %f Hz\n",clock_frequency);
  Serial.printf("Prescaller %d\n",prescaller);
  Serial.printf("Timer frequency %d\n",timer_frequency);
  Serial.printf("Time between samples %f seconds\n",time_between_samples);
  Serial.printf("Clock value between samples %d\n",timer_value_between_samples);
  Serial.printf("The pulse generated will have a width of %f seconds \n",pulse_width);
}


bool set_start_position(int16_t start_position){
  this->start_position = start_position;
  return true;
}

inline void delay(uint16_t delay){
  while((*timer) < delay){
    // WAIT FOR
  }
  *timer = 0;
}

uint16_t time_to_position(uint16_t initial_time){
  motors_stop();
  uint16_t time_to_position;
  *timer = 0;
  motor_move((get_encoder_value() - start_position) > 0);
  while ((initial_turn != ((get_encoder_value()-start_position) > 0)) && (*timer < initial_time)){
      //WAIT TILL CROSS THE POSITION
  }
  time_to_position = *timer;
  motors_stop();
  return time_to_position
}

void return_to_start_position(){
  //Position must be equal to start_position and motors should be off
  //START CONDITIONS
  motors_stop();
  delay(10 * timer_value_between_samples);
  if (get_encoder_value() == start_position){
    return;
  }
  //FIND THE TIME TO REACH THE POSITION
  uint16_t time_to_zero = time_to_position((uint16_t)-1);
  uint16_t previous_time_to_zero = initial_time_to_zero;
  uint16_t wait_time_to_zero;
  
//USE THE INITIAL TIME TO REACH THE DESIRED Position
  while(get_encoder_value() != position){
    wait_time_to_zero = (time_to_zero + previous_time_to_zero) >> 1;
    previous_time_to_zero = time_to_zero;
    time_to_zero = time_to_position(wait_time_to_zero);
  }
}

inline void get_motor_generated_samples(bool move_clockwise, uint16_t points_samples[]){
  //motors_stop();
  //Serial.println("Put the bar in zero position and press ENTER when is ready");
  //Serial.read();
  return_to_start_position();
  uint16_t index = 0;
  *timer = 0;
  motor_move(move_clockwise);
  while (index < points){
    points_samples[index] = get_encoder_value();
    ++index;
    delay(timer_value_between_samples);
  }
  motors_stop();
}

void get_quanser_samples(){
  get_input_pulse_width();
  points_samples[points];

  Serial.println("Clockwise Samples");
  get_motor_generated_samples(true, points_samples);
  for (unsigned long int index = 0; index < points; ++index){
    Serial.printf("%d ",points_samples[index]);
  }
  Serial.println();

  Serial.println("Counterclockwise Samples");
  get_motor_generated_samples(false, points_samples);
  for (unsigned long int index = 0; index < points; ++index){
    Serial.printf("%d ",points_samples[index]);
  }
  Serial.println();
}
