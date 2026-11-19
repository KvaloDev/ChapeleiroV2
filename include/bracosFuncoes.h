#pragma once
#include <Arduino.h>

enum class ModoTracao { Parado, Manual, Run, Strafe, DiagonalDir, DiagonalEsq, Turn };

// Gerencia a cinemática mecanum, rampas de aceleração e a ponte UART com a ESP32 dos braços.
// Não é thread-safe: todos os métodos devem ser chamados exclusivamente a partir da thread do loop().
class BaseFuncoes {
public:
    void run(float distanciaMm, int velocidade);
    void strafe(float distanciaMm, int velocidade);
    void diagonal(float distanciaMm, bool isDireita, int velocidade);
    // Soma graus à orientação atual para realizar uma rotação relativa ao ponto de partida.
    void turn(float graus, int velocidade);
    // Define anguloAlvo diretamente como destino absoluto em relação ao zero calibrado do giroscópio.
    void turnTo(float anguloAlvo, int velocidade);

    // Delega o controle dos atuadores superiores via Serial2 porque os drivers ficam na segunda ESP32.
    void moverOmbroEsq(float graus, int velocidade);
    void moverOmbroDir(float graus, int velocidade);
    void moverCotoveloEsq(float graus);
    void moverCotoveloDir(float graus);

    // Reduz a velocidade gradualmente pela rampa; usar pararBrusco() quando precisar travar no alvo exato.
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
    int calcularPassoRampa_(int atual, int alvo);

    ModoTracao modoAtual_ = ModoTracao::Parado;
    String ultimoCmdManual_ = "stop";
    int velocidadeManual_ = 120;
    bool fieldOriented_ = true;

    int pwmAtualFL_ = 0, pwmAtualFR_ = 0, pwmAtualRL_ = 0, pwmAtualRR_ = 0;
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