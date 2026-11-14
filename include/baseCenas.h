#pragma once
#include "baseFuncoes.h"

class BaseCenas {
public:
    BaseCenas(BaseFuncoes& funcoesBase);

    /**
     * @brief Inicia uma coreografia assíncrona e avisa a ESP32 dos braços via Serial2.
     */
    void iniciarCena(int idCena);

    /**
     * @brief Avança os passos da cena conforme o cronômetro sem bloquear o loop.
     */
    void atualizarCenas();

private:
    BaseFuncoes& funcoes_;
    bool isCenaAtiva_ = false;
    int idCenaAtual_ = 0;
    int passoAtual_ = 0;
    unsigned long inicioCenaMs_ = 0;
};