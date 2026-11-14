#pragma once
#include <Arduino.h>

extern volatile long ticksOmbroEsq;
extern volatile long ticksOmbroDir;

class BracosSetup {
public:
    /**
     * @brief Inicializa Serial2 (16/17), pontes H dos ombros e encoders de quadratura.
     */
    void inicializarHardware();
};