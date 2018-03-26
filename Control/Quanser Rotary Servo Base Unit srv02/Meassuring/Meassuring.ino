#include <quanser_motor.h>
#include <quadrature_encoder.h>
#include <quanser_samples.h>

#define ENCODER_CW_PIN 2
#define ENCODER_CCW_PIN 3
#define MOTOR_CW_PIN 4
#define MOTOR_CCW_PIN 5

void setup() {
    Serial.begin(9600);

    if(motor_set_pins(MOTOR_CW_PIN, MOTOR_CCW_PIN)){Serial.println("Motors is set")}
    else{Serial.println("Error setting up the motors")}

    if(encoder_set(ENCODER_CW_PIN, ENCODER_CCW_PIN)){Serial.println("Encoder is set")}
    else{Serial.println("Error setting up the encoder")}

    if(set_sample_timer_1(16e6, 8, false)){Serial.println("Timer is set")}
    else{Serial.println("Error setting up the timer")}

}

void loop() {
    get_quanser_samples();
}
