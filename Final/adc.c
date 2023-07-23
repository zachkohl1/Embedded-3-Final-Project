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

//Sequencer data
int readings[2];

void adc_init(void){
	adc_set_mode_run_continuously(MODULAR_ADC_0_SEQUENCER_CSR_BASE);
	adc_start(MODULAR_ADC_0_SEQUENCER_CSR_BASE);
}

////Returns the last 2 conversions
int* adc_read(void){
	alt_adc_word_read(MODULAR_ADC_0_SAMPLE_STORE_CSR_BASE,readings,2);

	//Make 8-bit data
	for(int i = 0; i <= 1; i++){
		readings[i] = readings[i] >> 4;
	}
	return readings;
}
