#pragma once
#include <Arduino.h>

// Em competicao, desativa o Wi-Fi para evitar interferencia RF e executa a cena autonomamente.
constexpr bool kModoDesenvolvimento = true;

constexpr uint8_t kPinRxSerial2 = 16;
constexpr uint8_t kPinTxSerial2 = 17;
constexpr uint32_t kBaudSerial2 = 115200;

// --- PARAMETROS DA BASE ---
constexpr float kTicksPorMmBase = 1.65f;
constexpr int kPwmMax = 255;
constexpr int kPwmMinBase = 15;
constexpr uint8_t kCanalSpikeGyro = 8;

// Inverter para -1 caso girar o robo no sentido horario diminua o angulo lido pelo hub.
constexpr int kDirGyroSpike = 1;

// Rampa trapezoidal aplicada a qualquer velocidade para evitar patinagem e trancos mecanicos.
constexpr int kPwmInicialRampa = 15;
constexpr int kPassoRampaPwm = 5;
constexpr unsigned long kIntervaloRampaMs = 100;

// Padroniza incremento positivo nos 4 encoders durante deslocamento frontal.
constexpr int kDirEncM1 = 1;
constexpr int kDirEncM2 = 1;
constexpr int kDirEncM3 = 1;
constexpr int kDirEncM4 = 1;

constexpr uint8_t kPinM1F = 25, kPinM1T = 26;
constexpr uint8_t kPinM2F = 33, kPinM2T = 32;
constexpr uint8_t kPinM3F = 18, kPinM3T = 19;
constexpr uint8_t kPinM4F = 22, kPinM4T = 21;

constexpr uint8_t kPinEncM1A = 36, kPinEncM1B = 39;
constexpr uint8_t kPinEncM2A = 34, kPinEncM2B = 35;
constexpr uint8_t kPinEncM3A = 27, kPinEncM3B = 13;
constexpr uint8_t kPinEncM4A = 23, kPinEncM4B = 4;

constexpr char kWifiSsid[] = "ChapeleiroV2";
constexpr char kWifiPass[] = "tucunarobotica";

// --- PARAMETROS DOS BRACOS ---
constexpr float kTicksPorGrauOmbro = 26.49f;
constexpr int kPwmMinOmbro = 50;

// Limites fisicos de operacao para evitar colisao estrutural dos membros superiores.
constexpr float kOmbroMinGraus = 0.0f;
constexpr float kOmbroMaxGraus = 90.0f;
constexpr float kCotoveloEsqMinGraus = 75.0f;
constexpr float kCotoveloEsqMaxGraus = 120.0f;
constexpr float kCotoveloDirMinGraus = 0.0f;
constexpr float kCotoveloDirMaxGraus = 50.0f;

constexpr int kDirEncOmbroEsq = 1;
constexpr int kDirEncOmbroDir = 1;

constexpr uint8_t kPinOmbroEsqRpwm = 255;
constexpr uint8_t kPinOmbroEsqLpwm = 255;
constexpr uint8_t kPinOmbroDirRpwm = 255;
constexpr uint8_t kPinOmbroDirLpwm = 255;

constexpr uint8_t kPinEncOmbroEsqA = 255;
constexpr uint8_t kPinEncOmbroEsqB = 255;
constexpr uint8_t kPinEncOmbroDirA = 255;
constexpr uint8_t kPinEncOmbroDirB = 255;

constexpr uint8_t kPinServoCotoveloEsq = 255;
constexpr uint8_t kPinServoCotoveloDir = 255;