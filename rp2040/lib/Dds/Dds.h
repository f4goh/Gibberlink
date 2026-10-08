/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Dds.h
 * Author: ale
 *
 * Created on 28 août 2025, 07:55
 */

#ifndef DDS_H
#define DDS_H

#include <Arduino.h>
#include "vfo2.pio.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"


//#define CLK_KHZ  125000L
#define CLK_KHZ  180000L


#define GG_TONE_SPACING        4687L  //46.87 Hz


class Dds {
public:
    Dds();
    Dds(const Dds& orig);
    virtual ~Dds();
    
   
    void coreUnSetup();
 
    
    void setFreqGG(uint32_t freq);
    
    void setGGTone(uint8_t n);
   
    void stopTx();
    
    uint32_t ggfr[16];  //16-FSK
    
     
    volatile uint32_t periods;  //pas de setter en lien avec les tables pour le moment mais il faudrait le mettre en private
    
private:
};

#endif // DDS_H