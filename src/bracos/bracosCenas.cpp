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

}