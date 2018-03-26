#ifndef __QUANSER_SAMPLES_H_INCLUDED__
#define __QUANSER_SAMPLES_H_INCLUDED__

/*SAMPLES*/
void get_quanser_samples();
/*TIMER SETUP*/
void set_sample_timer(double clock_frequency, uint8_t prescaller_value, uint16_t &timer_ref,uint16_t &OCRA_ref, uint8_t &TIMSK_ref,uint8_t &timer_reg_a_ref, uint8_t &timer_reg_b_ref, bool active_noise_canceler);

#endif
