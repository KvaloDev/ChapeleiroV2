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

    if (passoAtual_ == 0 && decorridoMs >= 5900) {
    funcoes_.moverCotoveloEsq(50);
    funcoes_.moverCotoveloDir(50);
    passoAtual_++;
}

if (passoAtual_ == 1 && decorridoMs >= 7600) {
    funcoes_.moverCotoveloEsq(100);
    funcoes_.moverCotoveloDir(100);
    passoAtual_++;
}

if (passoAtual_ == 2 && decorridoMs >= 8600) {
    funcoes_.moverCotoveloEsq(0);
    funcoes_.moverCotoveloDir(0);
    passoAtual_++;
}

if (passoAtual_ == 3 && decorridoMs >= 18500) {
    funcoes_.moverCotoveloEsq(50);
    funcoes_.moverCotoveloDir(50);
    passoAtual_++;
}

if (passoAtual_ == 4 && decorridoMs >= 19500) {
    funcoes_.moverCotoveloEsq(100);
    funcoes_.moverCotoveloDir(100);
    passoAtual_++;
}

if (passoAtual_ == 5 && decorridoMs >= 20300) {
    funcoes_.moverCotoveloEsq(0);
    funcoes_.moverCotoveloDir(0);
    passoAtual_++;
}

if (passoAtual_ == 6 && decorridoMs >= 83800) {
    funcoes_.moverCotoveloEsq(45);
    funcoes_.moverCotoveloDir(45);
    passoAtual_++;
}

if (passoAtual_ == 7 && decorridoMs >= 85800) {
    funcoes_.moverCotoveloEsq(50);
    funcoes_.moverCotoveloDir(50);
    passoAtual_++;
}

if (passoAtual_ == 8 && decorridoMs >= 87000) {
    funcoes_.moverCotoveloEsq(100);
    funcoes_.moverCotoveloDir(100);
    passoAtual_++;
}

if (passoAtual_ == 9 && decorridoMs >= 88000) {
    funcoes_.moverCotoveloEsq(80);
    funcoes_.moverCotoveloDir(80);
    passoAtual_++;
}

if (passoAtual_ == 10 && decorridoMs >= 90500) {
    funcoes_.moverOmbroEsq(50,45);
    funcoes_.moverCotoveloEsq(35);
    funcoes_.moverCotoveloDir(0);
    passoAtual_++;
}

if (passoAtual_ == 11 && decorridoMs > 92600) {
    funcoes_.moverOmbroEsq(0, 45);
    funcoes_.moverCotoveloEsq(60);
    funcoes_.moverCotoveloDir(60);
    passoAtual_++;
}

if (passoAtual_ == 12 && decorridoMs > 93600) {
    funcoes_.moverCotoveloDir(0);
    funcoes_.moverCotoveloEsq(0);
    passoAtual_++;
}

if (passoAtual_ == 13 && decorridoMs > 159500) {
    funcoes_.moverOmbroEsq(80, 45);
    funcoes_.moverCotoveloEsq(45);
    passoAtual_++;
}

if (passoAtual_ == 14 && decorridoMs > 203500) {
    funcoes_.moverOmbroEsq(0, 45);
    funcoes_.moverCotoveloDir(95);
    funcoes_.moverCotoveloEsq(95);
    passoAtual_++;
}

if (passoAtual_ == 15 && decorridoMs > 206200) {
    funcoes_.moverCotoveloDir(85);
    funcoes_.moverCotoveloEsq(85);
    passoAtual_++;
}

if (passoAtual_ == 16 && decorridoMs > 207400) {
    funcoes_.moverCotoveloDir(70);
    funcoes_.moverCotoveloEsq(70);
    passoAtual_++;
}

if (passoAtual_ == 17 && decorridoMs > 209000) {
    funcoes_.moverCotoveloDir(70);
    funcoes_.moverCotoveloEsq(70);
    passoAtual_++;
}

if (passoAtual_ == 18 && decorridoMs > 210000) {
    funcoes_.moverCotoveloDir(70);
    funcoes_.moverCotoveloEsq(70);
    passoAtual_++;
}

}