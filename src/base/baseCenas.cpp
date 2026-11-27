#include "baseCenas.h"
#include <Arduino.h>

BaseCenas::BaseCenas(BaseFuncoes& funcoesBase) : funcoes_(funcoesBase) {}

void BaseCenas::iniciarCena(int idCena) {
    idCenaAtual_ = idCena;
    passoAtual_ = 0;
    inicioCenaMs_ = millis();
    isCenaAtiva_ = true;
    Serial2.printf("CENA:%d\n", idCena);
}

void BaseCenas::atualizarCenas() {
    // Dispara a cena sincronizada ao clicar na seta direita (>) do SPIKE Prime.
    if (funcoes_.consumirStartSpike()) {
        funcoes_.zerarGyro();
        iniciarCena(1);
    }

    if (!isCenaAtiva_) return;
    unsigned long decorridoMs = millis() - inicioCenaMs_;

/*
// [Parte 1]
if (passoAtual_ == 0 && decorridoMs >= 377) {
    funcoes_.run(1700.0f, 90);
    passoAtual_++;
}
// [Parte 1]
if (passoAtual_ == 1 && decorridoMs >= 4043) {
    funcoes_.turnTo(-135.0f, 90);
    passoAtual_++;
}
// [Parte 2]
if (passoAtual_ == 2 && decorridoMs >= 11850) {
    funcoes_.turnTo(-90.0f, 90);
    passoAtual_++;
}
// [Parte 2]
if (passoAtual_ == 3 && decorridoMs >= 13700) {
    funcoes_.run(1700.0f, 90);
    passoAtual_++;
}
// [Parte 2]
if (passoAtual_ == 4 && decorridoMs >= 17790) {
    funcoes_.turnTo(-180.0f, 90);
    passoAtual_++;
}
// [Parte 3]
if (passoAtual_ == 5 && decorridoMs >= 24000) {
    funcoes_.turnTo(90.0f, 90);
    passoAtual_++;
}
// [Parte 3]
if (passoAtual_ == 6 && decorridoMs >= 26500) {
    funcoes_.turnTo(-180.0f, 20);
    passoAtual_++;
}
// [Parte 3]
if (passoAtual_ == 7 && decorridoMs >= 30800) {
    funcoes_.strafe(-1400.0f, 90);
    passoAtual_++;
}
// [Parte 3]
if (passoAtual_ == 8 && decorridoMs >= 33900) {
    funcoes_.turnTo(-155.0f, 90);
    passoAtual_++;
}
// [Parte 4]
if (passoAtual_ == 9 && decorridoMs >= 71000) {
    funcoes_.enviarCodigoCenario(2565, 120);
    passoAtual_++;
}
// [Parte 4]
if (passoAtual_ == 10 && decorridoMs >= 81000) {
    funcoes_.turnTo(-145.0f, 80);
    passoAtual_++;
}
// [Parte 4]
if (passoAtual_ == 11 && decorridoMs >= 83000) {
    funcoes_.run(800.0f, 80);
    passoAtual_++;
}
// [Parte 5]
if (passoAtual_ == 12 && decorridoMs >= 116000) {
    funcoes_.turnTo(-180.0f, 90);
    passoAtual_++;
}
// [Parte 5]
if (passoAtual_ == 13 && decorridoMs >= 117000) {
    funcoes_.strafe(800.0f, 90);
    passoAtual_++;
}
// [Parte 5]
if (passoAtual_ == 14 && decorridoMs >= 121000) {
    funcoes_.run(1500.0f, 90);
    passoAtual_++;
}
// [Parte 6]
if (passoAtual_ == 15 && decorridoMs >= 149500) {
    funcoes_.turnTo(-0.0f, 95);
    passoAtual_++;
}
// [Parte 6]
if (passoAtual_ == 16 && decorridoMs >= 152200) {
    funcoes_.strafe(550.0f, 90);
    passoAtual_++;
}
// [Parte 6]
if (passoAtual_ == 17 && decorridoMs >= 153600) {
    funcoes_.turnTo(0.0f, 90);
    passoAtual_++;
}
*/

}