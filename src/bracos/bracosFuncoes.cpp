#include "bracosFuncoes.h"
#include "bracosSetup.h"
#include "bracosCenas.h"
#include "config.h"

void BracosFuncoes::inicializarServos() {
    if (kPinServoCotoveloEsq == 255 || kPinServoCotoveloDir == 255) return;

    servoCotoveloEsq_.write(static_cast<int>(kCotoveloEsqMinGraus));
    servoCotoveloDir_.write(static_cast<int>(kCotoveloDirMinGraus));
    servoCotoveloEsq_.attach(kPinServoCotoveloEsq);
    servoCotoveloDir_.attach(kPinServoCotoveloDir);
}

float BracosFuncoes::limitarAngulo_(float valor, float minimo, float maximo, const char* nomeJunta) {
    if (valor < minimo) {
        Serial.printf("[CLAMP] %s limitado de %.1f para %.1f graus\n", nomeJunta, valor, minimo);
        return minimo;
    }
    if (valor > maximo) {
        Serial.printf("[CLAMP] %s limitado de %.1f para %.1f graus\n", nomeJunta, valor, maximo);
        return maximo;
    }
    return valor;
}

void BracosFuncoes::aplicarPwmOmbro_(uint8_t pinR, uint8_t pinL, int pwm) {
    if (pinR == 255 || pinL == 255) return;
    if (pwm > 0) {
        analogWrite(pinR, pwm); analogWrite(pinL, 0);
    } else if (pwm < 0) {
        analogWrite(pinR, 0); analogWrite(pinL, abs(pwm));
    } else {
        analogWrite(pinR, 0); analogWrite(pinL, 0);
    }
}

void BracosFuncoes::pararOmbros() {
    isOmbroEsqAtivo_ = false;
    isOmbroDirAtivo_ = false;
    aplicarPwmOmbro_(kPinOmbroEsqRpwm, kPinOmbroEsqLpwm, 0);
    aplicarPwmOmbro_(kPinOmbroDirRpwm, kPinOmbroDirLpwm, 0);
}

void BracosFuncoes::moverOmbroEsq(float graus, int velocidade) {
    float seguro = limitarAngulo_(graus, kOmbroMinGraus, kOmbroMaxGraus, "OmbroEsq");
    alvoTicksEsq_ = static_cast<long>(seguro * kTicksPorGrauOmbro);
    velOmbroEsq_ = constrain(velocidade, kPwmMinOmbro, kPwmMax);
    isOmbroEsqAtivo_ = true;
}

void BracosFuncoes::moverOmbroDir(float graus, int velocidade) {
    float seguro = limitarAngulo_(graus, kOmbroMinGraus, kOmbroMaxGraus, "OmbroDir");
    alvoTicksDir_ = static_cast<long>(seguro * kTicksPorGrauOmbro);
    velOmbroDir_ = constrain(velocidade, kPwmMinOmbro, kPwmMax);
    isOmbroDirAtivo_ = true;
}

void BracosFuncoes::moverCotoveloEsq(float graus) {
    float seguro = limitarAngulo_(graus, kCotoveloEsqMinGraus, kCotoveloEsqMaxGraus, "CotoveloEsq");
    if (kPinServoCotoveloEsq != 255) servoCotoveloEsq_.write(static_cast<int>(seguro));
}

void BracosFuncoes::moverCotoveloDir(float graus) {
    float seguro = limitarAngulo_(graus, kCotoveloDirMinGraus, kCotoveloDirMaxGraus, "CotoveloDir");
    if (kPinServoCotoveloDir != 255) servoCotoveloDir_.write(static_cast<int>(seguro));
}

void BracosFuncoes::processarSerial2(BracosCenas& cenas) {
    if (!Serial2.available()) return;
    String cmd = Serial2.readStringUntil('\n');
    cmd.trim();

    if (cmd.startsWith("CENA:")) {
        cenas.iniciarCena(cmd.substring(5).toInt());
    } else if (cmd.startsWith("vel_braco:")) {
        velManualOmbro_ = constrain(cmd.substring(10).toInt(), kPwmMinOmbro, kPwmMax);
    } else if (cmd.startsWith("SRV_POS_E:") || cmd.startsWith("srv_pos_e:")) {
        moverCotoveloEsq(cmd.substring(10).toFloat());
    } else if (cmd.startsWith("SRV_POS_D:") || cmd.startsWith("srv_pos_d:")) {
        moverCotoveloDir(cmd.substring(10).toFloat());
    } else if (cmd.startsWith("OMB_POS_E:")) {
        int virgula = cmd.indexOf(',');
        moverOmbroEsq(cmd.substring(10, virgula).toFloat(), cmd.substring(virgula + 1).toInt());
    } else if (cmd.startsWith("OMB_POS_D:")) {
        int virgula = cmd.indexOf(',');
        moverOmbroDir(cmd.substring(10, virgula).toFloat(), cmd.substring(virgula + 1).toInt());
    } else if (cmd == "be_up") {
        isOmbroEsqAtivo_ = false;
        aplicarPwmOmbro_(kPinOmbroEsqRpwm, kPinOmbroEsqLpwm, velManualOmbro_);
    } else if (cmd == "be_dwn") {
        isOmbroEsqAtivo_ = false;
        aplicarPwmOmbro_(kPinOmbroEsqRpwm, kPinOmbroEsqLpwm, -velManualOmbro_);
    } else if (cmd == "bd_up") {
        isOmbroDirAtivo_ = false;
        aplicarPwmOmbro_(kPinOmbroDirRpwm, kPinOmbroDirLpwm, velManualOmbro_);
    } else if (cmd == "bd_dwn") {
        isOmbroDirAtivo_ = false;
        aplicarPwmOmbro_(kPinOmbroDirRpwm, kPinOmbroDirLpwm, -velManualOmbro_);
    } else if (cmd == "BE_STP" || cmd == "BD_STP") {
        pararOmbros();
    }
}

void BracosFuncoes::atualizarCinematica() {
    noInterrupts();
    long atualEsq = ticksOmbroEsq;
    long atualDir = ticksOmbroDir;
    interrupts();

    long maxTicks = static_cast<long>(kOmbroMaxGraus * kTicksPorGrauOmbro);
    if (atualEsq >= maxTicks || atualEsq < 0) aplicarPwmOmbro_(kPinOmbroEsqRpwm, kPinOmbroEsqLpwm, 0);
    if (atualDir >= maxTicks || atualDir < 0) aplicarPwmOmbro_(kPinOmbroDirRpwm, kPinOmbroDirLpwm, 0);

    if (isOmbroEsqAtivo_) {
        long erro = alvoTicksEsq_ - atualEsq;
        if (abs(erro) <= 10) {
            isOmbroEsqAtivo_ = false;
            aplicarPwmOmbro_(kPinOmbroEsqRpwm, kPinOmbroEsqLpwm, 0);
        } else {
            int pwm = constrain(static_cast<int>(abs(erro) * 0.4f), kPwmMinOmbro, velOmbroEsq_);
            aplicarPwmOmbro_(kPinOmbroEsqRpwm, kPinOmbroEsqLpwm, (erro > 0) ? pwm : -pwm);
        }
    }

    if (isOmbroDirAtivo_) {
        long erro = alvoTicksDir_ - atualDir;
        if (abs(erro) <= 10) {
            isOmbroDirAtivo_ = false;
            aplicarPwmOmbro_(kPinOmbroDirRpwm, kPinOmbroDirLpwm, 0);
        } else {
            int pwm = constrain(static_cast<int>(abs(erro) * 0.4f), kPwmMinOmbro, velOmbroDir_);
            aplicarPwmOmbro_(kPinOmbroDirRpwm, kPinOmbroDirLpwm, (erro > 0) ? pwm : -pwm);
        }
    }
}

void BracosFuncoes::enviarTelemetriaSerial2() {
    if (millis() - ultimoEnvioTelemetriaMs_ < 100) return;
    ultimoEnvioTelemetriaMs_ = millis();

    noInterrupts();
    long e = ticksOmbroEsq, d = ticksOmbroDir;
    interrupts();

    Serial2.printf("ENC:%ld,%ld\n", e, d);
}