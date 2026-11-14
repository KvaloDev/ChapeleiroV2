#pragma once
#include <Arduino.h>

enum class ModoTracao { Parado, Manual, Run, Strafe, DiagonalDir, DiagonalEsq, Turn };

class BaseFuncoes {
public:
    void run(float distanciaMm, int velocidade);
    void strafe(float distanciaMm, int velocidade);
    void diagonal(float distanciaMm, bool isDireita, int velocidade);
    void turn(float graus, int velocidade);

    void moverOmbroEsq(float graus, int velocidade);
    void moverOmbroDir(float graus, int velocidade);
    void moverCotoveloEsq(float graus);
    void moverCotoveloDir(float graus);

    void parar();
    void pararBrusco();
    void zerarGyro();
    void processarComandoManual(const String& cmd);
    void processarSerial2();
    void atualizarCinematica();

    bool isMovendo() const { return modoAtual_ != ModoTracao::Parado; }
    int getVelocidadeManual() const { return velocidadeManual_; }
    long getTicksOmbroEsq() const { return ticksOmbroEsq_; }
    long getTicksOmbroDir() const { return ticksOmbroDir_; }
    float getGyroRelativo() const;

private:
    void definirAlvoMecanum_(int fl, int fr, int rl, int rr);
    void aplicarPwmRoda_(uint8_t pinF, uint8_t pinT, int pwm);
    void acionarMecanum_(int fl, int fr, int rl, int rr);
    int calcularPassoRampa_(int atual, int alvo);

    ModoTracao modoAtual_ = ModoTracao::Parado;
    String ultimoCmdManual_ = "stop";
    int velocidadeManual_ = 120;

    int pwmAtualFL_ = 0, pwmAtualFR_ = 0, pwmAtualRL_ = 0, pwmAtualRR_ = 0;
    int pwmAlvoFL_ = 0, pwmAlvoFR_ = 0, pwmAlvoRL_ = 0, pwmAlvoRR_ = 0;
    unsigned long ultimoPassoRampaMs_ = 0;

    long inicioM1_ = 0;
    float alvoTicks_ = 0.0f;
    float alvoAngulo_ = 0.0f;
    float offsetGyro_ = 0.0f;
    int velAlvo_ = 0;

    long ticksOmbroEsq_ = 0;
    long ticksOmbroDir_ = 0;
};