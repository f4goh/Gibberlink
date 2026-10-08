/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Dds.cpp
 * Author: ale
 * 
 * Created on 28 août 2025, 07:55
 */

#include "Dds.h"

Dds::Dds() {
}

Dds::Dds(const Dds& orig) {
}

Dds::~Dds() {
}

void Dds::coreUnSetup() {
  uint16_t t;
  uint32_t delta;
  PIO pio = pio0;
  gpio_init(0);
  gpio_set_dir(0, GPIO_OUT);
  pio_gpio_init(pio0, 15);         
  uint offset = pio_add_program(pio0, &vfo_program);         
  pio_sm_set_consecutive_pindirs(pio0, 0, 15, 1, true);          
  pio_sm_config c = vfo_program_get_default_config(offset);
  sm_config_set_set_pins(&c, 15, 1);                  
  pio_sm_init(pio0, 0, offset, &c);       
  pio_sm_set_enabled(pio0, 0, true);
  periods = 200 << 24;  //dds off
  for(;;){
    t = (periods+delta) >> 24;  
    pio_sm_put_blocking(pio0, 0, t);
    delta += periods-(t << 24);      
  }

}


void Dds::setFreqGG(uint32_t freq){
      uint64_t ratio = (uint64_t) CLK_KHZ * 1000LL * (1 << 24) / (uint64_t) freq;
    periods = (uint32_t) ratio;
    uint32_t dwsfr = GG_TONE_SPACING * (periods / freq) / 100L; //46.87 Hz de shift

    ggfr[0] = periods - (10 << 24);
    ggfr[1] = ggfr[0] - dwsfr;
    ggfr[2] = ggfr[1] - dwsfr;
    ggfr[3] = ggfr[2] - dwsfr;
    ggfr[4] = ggfr[3] - dwsfr;
    ggfr[5] = ggfr[4] - dwsfr;
    ggfr[6] = ggfr[5] - dwsfr;
    ggfr[7] = ggfr[6] - dwsfr;
    ggfr[8] = ggfr[7] - dwsfr;
    ggfr[9] = ggfr[8] - dwsfr;
    ggfr[10] = ggfr[9] - dwsfr;
    ggfr[11] = ggfr[10] - dwsfr;
    ggfr[12] = ggfr[11] - dwsfr;
    ggfr[13] = ggfr[12] - dwsfr;
    ggfr[14] = ggfr[13] - dwsfr;
    ggfr[15] = ggfr[14] - dwsfr;
}
   
void Dds::setGGTone(uint8_t n){
    periods = ggfr[n];
}
   
void Dds::stopTx(){
    periods = 200 << 24; //dds off
}
