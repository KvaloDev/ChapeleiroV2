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
    /** @brief Configura pinos da ponte H, Serial2 e interrupções dos encoders. */
    void inicializarHardware();

    /** @brief Sobe o SoftAP Wi-Fi e registra as rotas HTTP e WebSocket do painel. */
    void inicializarRedeEInterface(BaseFuncoes& funcoes, BaseCenas& cenas);

    /** @brief Transmite telemetria de encoders e giroscópio ao painel conectado. */
    void atualizarRede(const BaseFuncoes& funcoes);

private:
    unsigned long ultimoEnvioTelemetriaMs_ = 0;
};