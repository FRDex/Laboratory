#ifndef __QUANSER_SAMPLES_H_INCLUDED__
#define __QUANSER_SAMPLES_H_INCLUDED__
//EXTERNAL LIBRARIES
#include <Arduino.h>
#include "quanser_motor.h"
#include "quadrature_encoder.h"

/*SAMPLES*/
void generate_quanser_samples();

/*TIMER SETUP*/
bool setup_quanser_samples(bool active_noise_canceler = false, uint16_t prescaller_value = 1, float desired_time_between_samples_in_microseconds = 2000);

#endif
