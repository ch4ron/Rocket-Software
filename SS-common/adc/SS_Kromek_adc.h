/*
 * SS_Kromek_adc.h
 *
 *  Created on: Dec 24, 2019
 *      Author: maciek
 */

#ifndef SS_KROMEK_ADC_H_
#define SS_KROMEK_ADC_H_


void SS_adc_init();
/* Note that channelId is equal to channel's rank, counting from 1 (dependent on CubeMX configuration) not ADC Channel */
void SS_adc_add_measurement(float *value, float (*fun)(uint16_t, float), int rankId, int adc);
void print_meas(void);

#endif /* SS_KROMEK_ADC_H_ */
