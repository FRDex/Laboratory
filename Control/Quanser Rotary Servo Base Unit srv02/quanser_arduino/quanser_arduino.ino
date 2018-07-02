#include <Stdint.h>
#include "quanser_motor.h"
#include "tachometer.h"
#include "potentiometer.h"
#include "quadrature_encoder.h"
#include "quanser_samples.h"
#include "differentiation.h"
#include "integral.h"

uint8_t motor_clockwise_pin = 1, motor_counterclockwise_pin = 2;
uint8_t tachometer_pin = 3;
uint8_t potentiometer_pin = 4;
uint8_t encoder_clockwise_pin = 0, encoder_counterclockwise_pin = 0;


void setup() {
  Serial.begin(9600);

  Serial.println("Motor Setup is correct?");
  Serial.println(motor_set_pins(motor_clockwise_pin, motor_counterclockwise_pin));

    Serial.println("Tachometer Setup is correct?");
    Serial.println(tachometer_set(tachometer_pin));
  
    Serial.println("Potentiometer Setup is correct?");
    Serial.println(potentiometer_set(potentiometer_pin));
  
    Serial.println("Potentiometer Setup is correct?");
    Serial.println(potentiometer_set(potentiometer_pin));
  
    Serial.println("Encoder Setup is correct?");
    Serial.println(encoder_set(encoder_clockwise_pin, encoder_counterclockwise_pin));


}

void loop() {
  // put your main code here, to run repeatedly:

}

