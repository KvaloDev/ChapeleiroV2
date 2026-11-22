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

    /** @brief Transmite um inteiro Pybricks (ex: 2565) via BLE no canal escolhido para os SPIKEs do cenário. */
    void transmitirCodigoSpike(int16_t valor, uint8_t canal, uint16_t duracaoMs = 1500);

    /** @brief Gerencia o início e o encerramento da transmissão BLE sem bloquear o loop. */
    void atualizarTransmissaoBle();
};