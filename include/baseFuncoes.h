// Evita inclusão múltipla deste cabeçalho na mesma unidade de compilação para prevenir erros de redefinição.
#pragma once
#include <Arduino.h>
#include "config.h"

enum class ModoTracao { Parado, Manual, Run, Strafe, DiagonalDir, DiagonalEsq, Turn, MovGirando };

// Gerencia a cinemática mecanum, rampas de aceleração/frenagem e a ponte UART com a ESP32 dos braços.
// Não é thread-safe: todos os métodos devem ser chamados exclusivamente a partir da thread do loop().
class BaseFuncoes {
public:
    void run(float distanciaMm, int velocidade);
    void strafe(float distanciaMm, int velocidade);
    void diagonal(float distanciaMm, bool isDireita, int velocidade);
    // Soma graus à orientação atual para realizar uma rotação relativa ao ponto de partida.
    void turn(float graus, int velocidade);
    // Orienta o chassi na direção absoluta equivalente (módulo 360°) pelo menor caminho angular (<= 180°).
    void turnTo(float anguloAlvo, int velocidade);
    // Desloca o chassi na direção (xCampo, yCampo) enquanto rotaciona até completar anguloGraus pelo giroscópio.
    void moverGirando(float xCampo, float yCampo, float anguloGraus, int velocidade);

    // Delega o controle dos atuadores superiores via Serial2 porque os drivers ficam na segunda ESP32.
    void moverOmbroEsq(float graus, int velocidade);
    void moverOmbroDir(float graus, int velocidade);
    void moverCotoveloEsq(float graus);
    void moverCotoveloDir(float graus);

    // Desacelera usando a rampa de frenagem; usar pararBrusco() quando precisar cortar energia instantaneamente.
    void parar();
    void pararBrusco();
    // Salva a leitura atual do SPIKE como offset para definir o 0° lógico sem precisar reiniciar o hub.
    void zerarGyro();
    void processarComandoManual(const String& cmd);
    void processarSerial2();
    // Precisa rodar continuamente no loop() para atualizar as malhas fechadas e aplicar os degraus da rampa.
    void atualizarCinematica();

    bool isMovendo() const { return modoAtual_ != ModoTracao::Parado; }
    bool isFieldOriented() const { return fieldOriented_; }
    int getVelocidadeManual() const { return velocidadeManual_; }
    long getTicksOmbroEsq() const { return ticksOmbroEsq_; }
    long getTicksOmbroDir() const { return ticksOmbroDir_; }
    float getGyroRelativo() const;

private:
    void atualizarAlvoManualMecanum_();
    void definirAlvoMecanum_(int fl, int fr, int rl, int rr);
    void aplicarPwmRoda_(uint8_t pinF, uint8_t pinT, int pwm);
    void acionarMecanum_(int fl, int fr, int rl, int rr);
    float calcularPassoRampa_(float atual, float alvo);

    ModoTracao modoAtual_ = ModoTracao::Parado;
    String ultimoCmdManual_ = "stop";
    int velocidadeManual_ = 120;
    bool fieldOriented_ = true;

    // Componentes normalizados (-1.0 a 1.0) para permitir translação e rotação simultâneas no modo manual.
    float manualX_ = 0.0f;
    float manualY_ = 0.0f;
    float manualRot_ = 0.0f;

    // Vetor linear armazenado para manter a direção de translação durante a rotina autônoma moverGirando().
    float autoX_ = 0.0f;
    float autoY_ = 0.0f;

    // Expressos em variação de PWM por 100 ms para permitir rampas fracionárias suaves a cada 20 ms.
    int passoAcelPwm_ = kPassoAcelPwm;
    int passoFreioPwm_ = kPassoFreioPwm;

    // Usa float no PWM atual para acumular incrementos menores que 1 PWM por ciclo de 20 ms sem perder precisão.
    float pwmAtualFL_ = 0.0f, pwmAtualFR_ = 0.0f, pwmAtualRL_ = 0.0f, pwmAtualRR_ = 0.0f;
    int pwmAlvoFL_ = 0, pwmAlvoFR_ = 0, pwmAlvoRL_ = 0, pwmAlvoRR_ = 0;
    unsigned long ultimoPassoRampaMs_ = 0;

    // Usa apenas o encoder M1 como referência linear para simplificar o controle de distância nas malhas fechadas.
    long inicioM1_ = 0;
    float alvoTicks_ = 0.0f;
    float alvoAngulo_ = 0.0f;
    float offsetGyro_ = 0.0f;
    int velAlvo_ = 0;
    // Registra o início do giro para abortar por segurança caso a leitura do SPIKE falhe no meio da rotação.
    unsigned long inicioTurnMs_ = 0;

    long ticksOmbroEsq_ = 0;
    long ticksOmbroDir_ = 0;
};