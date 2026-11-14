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
    if (!isCenaAtiva_) return;
    unsigned long decorridoMs = millis() - inicioCenaMs_;

    if (idCenaAtual_ == 1) {
        // Exemplo de chamada direta ou colada do Roteirizador Web:
        if (passoAtual_ == 0 && decorridoMs >= 1000) {
            funcoes_.run(600.0f, 180);
            funcoes_.moverOmbroEsq(45.0f, 150);
            passoAtual_++;
        } else if (passoAtual_ == 1 && decorridoMs >= 4500) {
            funcoes_.turn(90.0f, 140);
            funcoes_.moverCotoveloEsq(110.0f);
            passoAtual_++;
        } else if (passoAtual_ == 2 && decorridoMs >= 7500) {
            isCenaAtiva_ = false;
        }
    }
}