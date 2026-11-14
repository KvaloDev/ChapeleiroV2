#pragma once
#include <Arduino.h>
#include <ESP32Servo.h>

class BracosCenas;

class BracosFuncoes {
public:
    void inicializarServos();
    void moverOmbroEsq(float graus, int velocidade);
    void moverOmbroDir(float graus, int velocidade);
    void moverCotoveloEsq(float graus);
    void moverCotoveloDir(float graus);

    void pararOmbros();
    void processarSerial2(BracosCenas& cenas);
    void atualizarCinematica();
    void enviarTelemetriaSerial2();

private:
    float limitarAngulo_(float valor, float minimo, float maximo, const char* nomeJunta);
    void aplicarPwmOmbro_(uint8_t pinR, uint8_t pinL, int pwm);

    Servo servoCotoveloEsq_;
    Servo servoCotoveloDir_;

    bool isOmbroEsqAtivo_ = false;
    bool isOmbroDirAtivo_ = false;
    long alvoTicksEsq_ = 0;
    long alvoTicksDir_ = 0;
    int velOmbroEsq_ = 150;
    int velOmbroDir_ = 150;
    int velManualOmbro_ = 130;

    unsigned long ultimoEnvioTelemetriaMs_ = 0;
};