#pragma once
#include "bracosFuncoes.h"

class BracosCenas {
public:
    BracosCenas(BracosFuncoes& funcoesBracos);
    void iniciarCena(int idCena);
    void atualizarCenas();

private:
    BracosFuncoes& funcoes_;
    bool isCenaAtiva_ = false;
    int idCenaAtual_ = 0;
    int passoAtual_ = 0;
    unsigned long inicioCenaMs_ = 0;
};