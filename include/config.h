#pragma once
#include <Arduino.h>

// true = liga o Wi-Fi e o painel de gravação | false = desliga o Wi-Fi para apresentação.
constexpr bool kModoDesenvolvimento = true;

constexpr uint8_t kPinRxSerial2 = 16;
constexpr uint8_t kPinTxSerial2 = 17;
constexpr uint32_t kBaudSerial2 = 115200;

// Fator calibrado empiricamente (pulsos de quadratura por mm) para converter distância física em ticks.
constexpr float kTicksPorMmBase = 1.65f;
constexpr int kPwmMax = 255;
// Piso de PWM para vencer a zona morta e o atrito estático dos motores Yellow Jacket.
constexpr int kPwmMinBase = 28;

// Canais BLE Pybricks: canal 8 recebe o giroscópio do SPIKE do Chapeleiro; canal 10 envia o código 2565 ao cenário.
constexpr uint8_t kCanalSpikeGyro = 8;
constexpr uint8_t kCanalSpikeCenario = 120;

// Colocar -1 caso girar o robô no sentido horário diminua o ângulo lido pelo hub.
constexpr int kDirGyroSpike = 1;

// Atualiza a rampa a cada 20 ms (50 Hz) para suavizar a transição, usando passos expressos em PWM por 100 ms.
constexpr int kPwmInicialRampa = 28;
constexpr int kPassoAcelPwm = 8;
constexpr int kPassoFreioPwm = 8;
constexpr int kPassoRampaPwm = kPassoAcelPwm;
constexpr unsigned long kIntervaloRampaMs = 20;

// Padroniza incremento positivo nos 4 encoders durante deslocamento frontal.
constexpr int kDirEncM1 = 1;
constexpr int kDirEncM2 = 1;
constexpr int kDirEncM3 = 1;
constexpr int kDirEncM4 = 1;

constexpr uint8_t kPinM1F = 25, kPinM1T = 26;
constexpr uint8_t kPinM2F = 33, kPinM2T = 32;
constexpr uint8_t kPinM3F = 18, kPinM3T = 19;
constexpr uint8_t kPinM4F = 22, kPinM4T = 21;

// Pinos 34 a 39 são somente entrada no silício do ESP32 e exigem resistor pull-up externo no hardware.
constexpr uint8_t kPinEncM1A = 36, kPinEncM1B = 39;
constexpr uint8_t kPinEncM2A = 34, kPinEncM2B = 35;
constexpr uint8_t kPinEncM3A = 27, kPinEncM3B = 13;
constexpr uint8_t kPinEncM4A = 23, kPinEncM4B = 4;

constexpr char kWifiSsid[] = "ChapeleiroV2";
constexpr char kWifiPass[] = "tucunarobotica";

// Constantes abaixo pertencem à ESP32 dos braços porque ambas as placas compartilham este cabeçalho.
constexpr float kTicksPorGrauOmbro = 3.78f;
constexpr int kPwmMinOmbro = 50;

// Limites físicos de operação para evitar colisão estrutural dos membros superiores com o chassi.
constexpr float kOmbroMinGraus = 0.0f;
constexpr float kOmbroMaxGraus = 90.0f;
constexpr float kCotoveloEsqMinGraus = 75.0f;
constexpr float kCotoveloEsqMaxGraus = 120.0f;
constexpr float kCotoveloDirMinGraus = 0.0f;
constexpr float kCotoveloDirMaxGraus = 50.0f;

// Ombro esquerdo sobe no sentido correto, mas o encoder estava contando invertido na montagem física.
constexpr int kDirEncOmbroEsq = -1;
constexpr int kDirEncOmbroDir = 1;

// Ombro direito voltou para 25/26 para manter a convenção de subir ao pressionar SOBE.
constexpr uint8_t kPinOmbroEsqRpwm = 33;
constexpr uint8_t kPinOmbroEsqLpwm = 32;
constexpr uint8_t kPinOmbroDirRpwm = 25;
constexpr uint8_t kPinOmbroDirLpwm = 26;

constexpr uint8_t kPinEncOmbroEsqA = 22;
constexpr uint8_t kPinEncOmbroEsqB = 21;
constexpr uint8_t kPinEncOmbroDirA = 19;
constexpr uint8_t kPinEncOmbroDirB = 18;

constexpr uint8_t kPinServoCotoveloEsq = 4;
constexpr uint8_t kPinServoCotoveloDir = 27;