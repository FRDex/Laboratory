#ifndef __quanser_samples_H_INCLUDED__
#define __quanser_samples_H_INCLUDED__

void get_quanser_samples();

void set_sample_timer(double clock_frequency, uint8_t prescaller, uint16_t &timer,uint16_t &OCRA,uint8_t &timer_reg_a, uint8_t &timer_reg_b, bool noise_canceler);
void set_sample_points(uint16_t points);
void activate_sample_timer();
void deactivate_sample_timer();

#endif
