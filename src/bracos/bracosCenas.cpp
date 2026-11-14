#include "bracosCenas.h"
#include <Arduino.h>

BracosCenas::BracosCenas(BracosFuncoes& funcoesBracos) : funcoes_(funcoesBracos) {}

void BracosCenas::iniciarCena(int idCena) {
    idCenaAtual_ = idCena;
    passoAtual_ = 0;
    inicioCenaMs_ = millis();
    isCenaAtiva_ = true;
}

void BracosCenas::atualizarCenas() {
    if (!isCenaAtiva_) return;
    unsigned long decorridoMs = millis() - inicioCenaMs_;

    if (idCenaAtual_ == 1) {
        if (passoAtual_ == 0 && decorridoMs >= 1000) {
            funcoes_.moverOmbroDir(45.0f, 150);
            passoAtual_++;
        } else if (passoAtual_ == 1 && decorridoMs >= 5000) {
            isCenaAtiva_ = false;
        }
    }
}