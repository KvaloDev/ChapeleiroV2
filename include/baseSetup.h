#pragma once
#include <Arduino.h>

class BaseFuncoes;
class BaseCenas;

extern volatile long ticksM1;
extern volatile long ticksM2;
extern volatile long ticksM3;
extern volatile long ticksM4;
extern volatile float anguloSpikeGraus;

class BaseSetup {
public:
    /**
     * @brief Configura pinos dos motores, interrupções de quadratura e Serial2 (16/17).
     */
    void inicializarHardware();

    /**
     * @brief Inicia varredura BLE passiva para receber o ângulo IMU do SPIKE Prime.
     */
    void inicializarBleSpike();

    /**
     * @brief Inicia Access Point Wi-Fi e servidor WebSocket para calibração.
     */
    void inicializarRedeEInterface(BaseFuncoes& funcoes, BaseCenas& cenas);

    /**
     * @brief Mantém clientes WebSocket limpos e envia telemetria a 10 Hz.
     */
    void atualizarRede(const BaseFuncoes& funcoes);

private:
    unsigned long ultimoEnvioTelemetriaMs_ = 0;
};