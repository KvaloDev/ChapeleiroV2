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


//Parte 1

if (passoAtual_ == 0 && decorridoMs >= 377) {
    funcoes_.run(1700.0f, 90);
    passoAtual_++;
}
if (passoAtual_ == 1 && decorridoMs >= 4043) {
    funcoes_.turn(-135.0f, 100);
    passoAtual_++;
}

//Parte 2 

if (passoAtual_ == 2 && decorridoMs >= 11932) {
    funcoes_.turnTo(-90.0f, 100);
    passoAtual_++;
}
if (passoAtual_ == 3 && decorridoMs >= 13000) {
    funcoes_.run(1600.0f, 95);
    passoAtual_++;
}
if (passoAtual_ == 4 && decorridoMs >= 16315) {
    funcoes_.turnTo(180.0f, 100);
    passoAtual_++;
}
if (passoAtual_ == 5 && decorridoMs >= 23764) {
    funcoes_.turnTo(90.0f, 100);
    passoAtual_++;
}
if (passoAtual_ == 6 && decorridoMs >= 26781) {
    funcoes_.turn(33.5f, 100);
    passoAtual_++;
}
if (passoAtual_ == 7 && decorridoMs >= 28249) {
    funcoes_.strafe(-920.0f, 70);
    passoAtual_++;
}
if (passoAtual_ == 8 && decorridoMs >= 31479) {
    funcoes_.turn(5.1f, 90);
    passoAtual_++;
}

}