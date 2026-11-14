#include "baseFuncoes.h"
#include "baseSetup.h"
#include "config.h"

float BaseFuncoes::getGyroRelativo() const {
    return (anguloSpikeGraus - offsetGyro_) * kDirGyroSpike;
}

void BaseFuncoes::zerarGyro() {
    offsetGyro_ = anguloSpikeGraus;
}

void BaseFuncoes::aplicarPwmRoda_(uint8_t pinF, uint8_t pinT, int pwm) {
    if (pwm > 0) {
        analogWrite(pinF, pwm); analogWrite(pinT, 0);
    } else if (pwm < 0) {
        analogWrite(pinF, 0); analogWrite(pinT, abs(pwm));
    } else {
        analogWrite(pinF, 0); analogWrite(pinT, 0);
    }
}

void BaseFuncoes::acionarMecanum_(int fl, int fr, int rl, int rr) {
    aplicarPwmRoda_(kPinM1F, kPinM1T, fl);
    aplicarPwmRoda_(kPinM2F, kPinM2T, fr);
    aplicarPwmRoda_(kPinM3F, kPinM3T, rl);
    aplicarPwmRoda_(kPinM4F, kPinM4T, rr);
}

void BaseFuncoes::definirAlvoMecanum_(int fl, int fr, int rl, int rr) {
    pwmAlvoFL_ = fl;
    pwmAlvoFR_ = fr;
    pwmAlvoRL_ = rl;
    pwmAlvoRR_ = rr;
}

void BaseFuncoes::parar() {
    definirAlvoMecanum_(0, 0, 0, 0);
    ultimoCmdManual_ = "stop";

    if (pwmAtualFL_ == 0 && pwmAtualFR_ == 0 && pwmAtualRL_ == 0 && pwmAtualRR_ == 0) {
        modoAtual_ = ModoTracao::Parado;
    } else {
        // Mantém o loop ativo para desacelerar em degraus até zero.
        modoAtual_ = ModoTracao::Manual;
    }
}

void BaseFuncoes::pararBrusco() {
    definirAlvoMecanum_(0, 0, 0, 0);
    pwmAtualFL_ = pwmAtualFR_ = pwmAtualRL_ = pwmAtualRR_ = 0;
    acionarMecanum_(0, 0, 0, 0);
    ultimoCmdManual_ = "stop";
    modoAtual_ = ModoTracao::Parado;
}

void BaseFuncoes::run(float distanciaMm, int velocidade) {
    noInterrupts(); inicioM1_ = ticksM1; interrupts();
    alvoTicks_ = distanciaMm * kTicksPorMmBase;
    velAlvo_ = constrain(velocidade, kPwmMinBase, kPwmMax);
    modoAtual_ = ModoTracao::Run;
}

void BaseFuncoes::strafe(float distanciaMm, int velocidade) {
    noInterrupts(); inicioM1_ = ticksM1; interrupts();
    alvoTicks_ = distanciaMm * kTicksPorMmBase;
    velAlvo_ = constrain(velocidade, kPwmMinBase, kPwmMax);
    modoAtual_ = ModoTracao::Strafe;
}

void BaseFuncoes::diagonal(float distanciaMm, bool isDireita, int velocidade) {
    noInterrupts(); inicioM1_ = ticksM1; interrupts();
    alvoTicks_ = distanciaMm * kTicksPorMmBase;
    velAlvo_ = constrain(velocidade, kPwmMinBase, kPwmMax);
    modoAtual_ = isDireita ? ModoTracao::DiagonalDir : ModoTracao::DiagonalEsq;
}

void BaseFuncoes::turn(float graus, int velocidade) {
    alvoAngulo_ = getGyroRelativo() + graus;
    velAlvo_ = constrain(velocidade, kPwmMinBase, kPwmMax);
    modoAtual_ = ModoTracao::Turn;
}

void BaseFuncoes::moverOmbroEsq(float graus, int velocidade) {
    Serial2.printf("OMB_POS_E:%.1f,%d\n", graus, velocidade);
}

void BaseFuncoes::moverOmbroDir(float graus, int velocidade) {
    Serial2.printf("OMB_POS_D:%.1f,%d\n", graus, velocidade);
}

void BaseFuncoes::moverCotoveloEsq(float graus) {
    Serial2.printf("SRV_POS_E:%.1f\n", graus);
}

void BaseFuncoes::moverCotoveloDir(float graus) {
    Serial2.printf("SRV_POS_D:%.1f\n", graus);
}

void BaseFuncoes::processarComandoManual(const String& cmd) {
    if (cmd == "zero_gyro") { zerarGyro(); return; }

    if (cmd.startsWith("vel:")) {
        velocidadeManual_ = constrain(cmd.substring(4).toInt(), 0, kPwmMax);
        if (modoAtual_ == ModoTracao::Manual && ultimoCmdManual_ != "stop") {
            processarComandoManual(ultimoCmdManual_);
        }
        return;
    }

    if (cmd.startsWith("vel_braco:")) {
        Serial2.println(cmd);
        return;
    }

    if (cmd.startsWith("turn:")) {
        int virgula = cmd.indexOf(',');
        if (virgula > 0) {
            turn(cmd.substring(5, virgula).toFloat(), cmd.substring(virgula + 1).toInt());
        } else {
            turn(cmd.substring(5).toFloat(), velocidadeManual_);
        }
        return;
    }

    if (cmd.startsWith("srv_") || cmd.startsWith("be_") || cmd.startsWith("bd_") || cmd.startsWith("OMB_")) {
        Serial2.println(cmd);
        return;
    }

    if (cmd == "stop") {
        parar();
        Serial2.println("BE_STP");
        Serial2.println("BD_STP");
        return;
    }

    modoAtual_ = ModoTracao::Manual;
    ultimoCmdManual_ = cmd;
    int p = velocidadeManual_;

    if (cmd == "w") definirAlvoMecanum_(p, p, p, p);
    else if (cmd == "s") definirAlvoMecanum_(-p, -p, -p, -p);
    else if (cmd == "a") definirAlvoMecanum_(-p, p, p, -p);
    else if (cmd == "d") definirAlvoMecanum_(p, -p, -p, p);
    else if (cmd == "wd") definirAlvoMecanum_(p, 0, 0, p);
    else if (cmd == "wa") definirAlvoMecanum_(0, p, p, 0);
    else if (cmd == "sd") definirAlvoMecanum_(0, -p, -p, 0);
    else if (cmd == "sa") definirAlvoMecanum_(-p, 0, 0, -p);
    else if (cmd == "q") definirAlvoMecanum_(-p, p, -p, p);
    else if (cmd == "e") definirAlvoMecanum_(p, -p, p, -p);
}

void BaseFuncoes::processarSerial2() {
    if (!Serial2.available()) return;
    String msg = Serial2.readStringUntil('\n');
    msg.trim();
    if (!msg.startsWith("ENC:")) return;

    int virgula = msg.indexOf(',');
    if (virgula > 0) {
        ticksOmbroEsq_ = msg.substring(4, virgula).toInt();
        ticksOmbroDir_ = msg.substring(virgula + 1).toInt();
    }
}

int BaseFuncoes::calcularPassoRampa_(int atual, int alvo) {
    if (atual == alvo) return alvo;

    // Desacelera até zero antes de inverter o sentido de rotação do motor.
    if (atual > 0 && alvo < 0) return max(0, atual - kPassoRampaPwm);
    if (atual < 0 && alvo > 0) return min(0, atual + kPassoRampaPwm);

    if (atual == 0) {
        if (alvo > 0) return min(alvo, kPwmInicialRampa);
        return max(alvo, -kPwmInicialRampa);
    }

    if (atual < alvo) return min(alvo, atual + kPassoRampaPwm);
    return max(alvo, atual - kPassoRampaPwm);
}

void BaseFuncoes::atualizarCinematica() {
    if (modoAtual_ == ModoTracao::Turn) {
        float erroGraus = alvoAngulo_ - getGyroRelativo();
        if (abs(erroGraus) <= 2.0f) {
            pararBrusco();
            return;
        }

        int pwmProporcional = constrain(static_cast<int>(abs(erroGraus) * 2.2f), kPwmMinBase, velAlvo_);
        int pwm = (erroGraus > 0) ? pwmProporcional : -pwmProporcional;

        // Zera a inércia da rampa imediatamente se ultrapassar o alvo para corrigir sem oscilar.
        if ((pwm > 0 && pwmAtualFL_ < 0) || (pwm < 0 && pwmAtualFL_ > 0)) {
            pwmAtualFL_ = pwmAtualFR_ = pwmAtualRL_ = pwmAtualRR_ = 0;
        }
        definirAlvoMecanum_(pwm, -pwm, pwm, -pwm);
    } else if (modoAtual_ != ModoTracao::Manual && modoAtual_ != ModoTracao::Parado) {
        noInterrupts();
        long atualM1 = ticksM1;
        interrupts();

        long erroTicks = abs(static_cast<long>(alvoTicks_)) - abs(atualM1 - inicioM1_);
        if (erroTicks <= 5) {
            pararBrusco();
            return;
        }

        int pwm = constrain(static_cast<int>(erroTicks * 1.5f), kPwmMinBase, velAlvo_);
        if (alvoTicks_ < 0) pwm = -pwm;

        if (modoAtual_ == ModoTracao::Run) definirAlvoMecanum_(pwm, pwm, pwm, pwm);
        else if (modoAtual_ == ModoTracao::Strafe) definirAlvoMecanum_(pwm, -pwm, -pwm, pwm);
        else if (modoAtual_ == ModoTracao::DiagonalDir) definirAlvoMecanum_(pwm, 0, 0, pwm);
        else if (modoAtual_ == ModoTracao::DiagonalEsq) definirAlvoMecanum_(0, pwm, pwm, 0);
    }

    if (modoAtual_ == ModoTracao::Parado) return;

    unsigned long agora = millis();
    if (agora - ultimoPassoRampaMs_ < kIntervaloRampaMs) return;
    ultimoPassoRampaMs_ = agora;

    pwmAtualFL_ = calcularPassoRampa_(pwmAtualFL_, pwmAlvoFL_);
    pwmAtualFR_ = calcularPassoRampa_(pwmAtualFR_, pwmAlvoFR_);
    pwmAtualRL_ = calcularPassoRampa_(pwmAtualRL_, pwmAlvoRL_);
    pwmAtualRR_ = calcularPassoRampa_(pwmAtualRR_, pwmAlvoRR_);

    acionarMecanum_(pwmAtualFL_, pwmAtualFR_, pwmAtualRL_, pwmAtualRR_);

    if (modoAtual_ == ModoTracao::Manual &&
        pwmAlvoFL_ == 0 && pwmAlvoFR_ == 0 && pwmAlvoRL_ == 0 && pwmAlvoRR_ == 0 &&
        pwmAtualFL_ == 0 && pwmAtualFR_ == 0 && pwmAtualRL_ == 0 && pwmAtualRR_ == 0) {
        modoAtual_ = ModoTracao::Parado;
    }
}