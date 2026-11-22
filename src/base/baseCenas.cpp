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

    // Quando finalizar a gravação no painel, substitua os blocos abaixo pelo "CÓDIGO COMPLETO UNIFICADO".
    if (passoAtual_ == 0 && decorridoMs >= 12763) {
        funcoes_.turnTo(-90.0f, 100);
        passoAtual_++;
    }
    if (passoAtual_ == 1 && decorridoMs >= 13796) {
        funcoes_.run(1844.2f, 98);
        passoAtual_++;
    }
    if (passoAtual_ == 2 && decorridoMs >= 17830) {
        funcoes_.turn(-135.0f, 100);
        passoAtual_++;
    }
    if (passoAtual_ == 3 && decorridoMs >= 20335) {
        funcoes_.turnTo(180.0f, 90);
        passoAtual_++;
    }
    if (passoAtual_ == 4 && decorridoMs >= 21753) {
        funcoes_.run(1622.4f, 90);
        passoAtual_++;
    }
    if (passoAtual_ == 5 && decorridoMs >= 25551) {
        funcoes_.turn(-135.0f, 90);
        passoAtual_++;
    }
}