#pragma once
#include <Arduino.h>

extern volatile long ticksOmbroEsq;
extern volatile long ticksOmbroDir;
extern volatile float anguloSpikeGraus;

class BracosSetup {
public:
    /** @brief Configura ponte H, Serial2 e interrupções dos encoders dos ombros. */
    void inicializarHardware();

    /** @brief Inicia scanner BLE passivo para ler telemetria do giroscópio SPIKE Prime. */
    void inicializarBleSpike();
};