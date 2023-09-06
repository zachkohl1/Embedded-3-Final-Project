/**
 ******************************************************************************
 * @file    acd.c
 * @author  Zachary Kohlman
 * @email	 kohlmanz@msoe.edu
 * @version V1.0
 * @brief   Contains the API for the ADC.
 ******************************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stddef.h>

#include "altera_avalon_pio_regs.h"
#include "system.h"
#include "alt_types.h"
#include "altera_modular_adc.h"
#include "adc.h"

// This array stores the last two readings from the ADC.
int readings[2];

/**
 * This function initializes the ADC to run continuously.
 * It sets the ADC mode to run continuously and starts the ADC.
 */
void adc_init(void){
	adc_set_mode_run_continuously(MODULAR_ADC_0_SEQUENCER_CSR_BASE);
	adc_start(MODULAR_ADC_0_SEQUENCER_CSR_BASE);
}

/**
 * This function reads the last 2 conversions from the ADC.
 * It reads the data from the ADC, converts it to 8-bit data and returns the readings.
 * @return readings: An array of the last 2 ADC readings.
 */
int* adc_read(void){
	alt_adc_word_read(MODULAR_ADC_0_SAMPLE_STORE_CSR_BASE,readings,2);

	// This loop converts the ADC readings to 8-bit data.
	for(int i = 0; i <= 1; i++){
		readings[i] = readings[i] >> 4;
	}
	return readings;
}
