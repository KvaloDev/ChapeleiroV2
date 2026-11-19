#include "baseFuncoes.h"
#include "baseSetup.h"
#include "config.h"
#include <cmath>

float BaseFuncoes::getGyroRelativo() const {
    // Subtrai offsetGyro_ e aplica kDirGyroSpike para alinhar o zero e o sentido positivo do hub ao referencial do chassi.
    return (anguloSpikeGraus - offsetGyro_) * kDirGyroSpike;
}

void BaseFuncoes::zerarGyro() {
    offsetGyro_ = anguloSpikeGraus;
}

void BaseFuncoes::aplicarPwmRoda_(uint8_t pinF, uint8_t pinT, int pwm) {
    // Mantém um canal em 0 enquanto modula o oposto para evitar condução simultânea na ponte H BTS7960.
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

void BaseFuncoes::atualizarAlvoManualMecanum_() {
    if (ultimoCmdManual_ == "stop") {
        definirAlvoMecanum_(0, 0, 0, 0);
        return;
    }

    float xCampo = 0.0f;
    float yCampo = 0.0f;
    float rot = 0.0f;

    // Usa 0.7071 (sen/cos de 45°) nas diagonais para que o vetor resultante tenha norma unitária igual aos eixos puros.
    if (ultimoCmdManual_ == "w") { yCampo = 1.0f; }
    else if (ultimoCmdManual_ == "s") { yCampo = -1.0f; }
    else if (ultimoCmdManual_ == "d") { xCampo = 1.0f; }
    else if (ultimoCmdManual_ == "a") { xCampo = -1.0f; }
    else if (ultimoCmdManual_ == "wd") { xCampo = 0.7071f; yCampo = 0.7071f; }
    else if (ultimoCmdManual_ == "wa") { xCampo = -0.7071f; yCampo = 0.7071f; }
    else if (ultimoCmdManual_ == "sd") { xCampo = 0.7071f; yCampo = -0.7071f; }
    else if (ultimoCmdManual_ == "sa") { xCampo = -0.7071f; yCampo = -0.7071f; }
    else if (ultimoCmdManual_ == "q") { rot = -1.0f; }
    else if (ultimoCmdManual_ == "e") { rot = 1.0f; }

    float xRobo = xCampo;
    float yRobo = yCampo;

    if (fieldOriented_ && (xCampo != 0.0f || yCampo != 0.0f)) {
        float rad = getGyroRelativo() * DEG_TO_RAD;
        float cosA = cosf(rad);
        float sinA = sinf(rad);
        // Aplica matriz de rotação 2D para converter a direção da arena no referencial local do robô.
        xRobo = xCampo * cosA - yCampo * sinA;
        yRobo = xCampo * sinA + yCampo * cosA;
    }

    float fl = yRobo + xRobo + rot;
    float fr = yRobo - xRobo - rot;
    float rl = yRobo - xRobo + rot;
    float rr = yRobo + xRobo - rot;

    // Divide pelo maior módulo quando excede 1.0 para preservar a direção do movimento sem saturar o PWM.
    float maxMag = max(max(fabsf(fl), fabsf(fr)), max(fabsf(rl), fabsf(rr)));
    if (maxMag > 1.0f) {
        fl /= maxMag;
        fr /= maxMag;
        rl /= maxMag;
        rr /= maxMag;
    }

    int p = velocidadeManual_;
    definirAlvoMecanum_(
        static_cast<int>(roundf(fl * p)),
        static_cast<int>(roundf(fr * p)),
        static_cast<int>(roundf(rl * p)),
        static_cast<int>(roundf(rr * p))
    );
}

void BaseFuncoes::parar() {
    definirAlvoMecanum_(0, 0, 0, 0);
    ultimoCmdManual_ = "stop";

    if (pwmAtualFL_ == 0 && pwmAtualFR_ == 0 && pwmAtualRL_ == 0 && pwmAtualRR_ == 0) {
        modoAtual_ = ModoTracao::Parado;
    } else {
        // Mantém o modo Manual ativo até que a rampa reduza gradualmente o PWM atual das rodas a zero.
        modoAtual_ = ModoTracao::Manual;
    }
}

void BaseFuncoes::pararBrusco() {
    // Zera os PWMs imediatamente sem rampa para impedir que a inércia faça o robô passar do alvo autônomo.
    definirAlvoMecanum_(0, 0, 0, 0);
    pwmAtualFL_ = pwmAtualFR_ = pwmAtualRL_ = pwmAtualRR_ = 0;
    acionarMecanum_(0, 0, 0, 0);
    ultimoCmdManual_ = "stop";
    modoAtual_ = ModoTracao::Parado;
}

void BaseFuncoes::run(float distanciaMm, int velocidade) {
    // Isola a leitura do contador volatile com noInterrupts() para evitar leitura corrompida pela ISR.
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
    inicioTurnMs_ = millis();
    modoAtual_ = ModoTracao::Turn;
}

void BaseFuncoes::turnTo(float anguloAlvo, int velocidade) {
    float atual = getGyroRelativo();
    float diferenca = anguloAlvo - atual;

    // Normaliza a diferença para [-180°, +180°] a fim de girar sempre pelo menor arco angular (módulo 360°).
    while (diferenca > 180.0f)  diferenca -= 360.0f;
    while (diferenca < -180.0f) diferenca += 360.0f;

    // Soma o menor deslocamento à leitura contínua atual para evitar saltos de referência ao cruzar 0°/360°.
    alvoAngulo_ = atual + diferenca;
    velAlvo_ = constrain(velocidade, kPwmMinBase, kPwmMax);
    inicioTurnMs_ = millis();
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

    if (cmd.startsWith("fod:")) {
        fieldOriented_ = (cmd.substring(4).toInt() == 1);
        if (modoAtual_ == ModoTracao::Manual) atualizarAlvoManualMecanum_();
        return;
    }

    if (cmd.startsWith("vel:")) {
        velocidadeManual_ = constrain(cmd.substring(4).toInt(), 0, kPwmMax);
        if (modoAtual_ == ModoTracao::Manual && ultimoCmdManual_ != "stop") {
            atualizarAlvoManualMecanum_();
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

    if (cmd.startsWith("turn_to:")) {
        int virgula = cmd.indexOf(',');
        if (virgula > 0) {
            turnTo(cmd.substring(8, virgula).toFloat(), cmd.substring(virgula + 1).toInt());
        } else {
            turnTo(cmd.substring(8).toFloat(), velocidadeManual_);
        }
        return;
    }

    // Encaminha comandos dos braços e servos direto pela UART porque esses periféricos estão ligados na outra ESP32.
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

    // Efeito colateral: qualquer texto não reconhecido cai aqui e zera o vetor manual por segurança.
    modoAtual_ = ModoTracao::Manual;
    ultimoCmdManual_ = cmd;
    atualizarAlvoManualMecanum_();
}

void BaseFuncoes::processarSerial2() {
    // Esvazia todo o buffer RX a cada ciclo para descartar leituras antigas e usar sempre o ângulo mais recente.
    while (Serial2.available()) {
        String msg = Serial2.readStringUntil('\n');
        msg.trim();
        if (!msg.startsWith("ENC:")) continue;

        int v1 = msg.indexOf(',');
        int v2 = msg.indexOf(',', v1 + 1);
        if (v1 > 0 && v2 > v1) {
            ticksOmbroEsq_ = msg.substring(4, v1).toInt();
            ticksOmbroDir_ = msg.substring(v1 + 1, v2).toInt();
            anguloSpikeGraus = msg.substring(v2 + 1).toFloat();
        }
    }
}

int BaseFuncoes::calcularPassoRampa_(int atual, int alvo) {
    if (atual == alvo) return alvo;

    // Reduz a velocidade até zero antes de inverter a polaridade para proteger as caixas de redução dos motores.
    if (atual > 0 && alvo < 0) return max(0, atual - kPassoRampaPwm);
    if (atual < 0 && alvo > 0) return min(0, atual + kPassoRampaPwm);

    // Parte direto de kPwmInicialRampa ao sair do repouso para vencer a zona morta mecânica sem atraso.
    if (atual == 0) {
        if (alvo > 0) return min(alvo, kPwmInicialRampa);
        return max(alvo, -kPwmInicialRampa);
    }

    if (atual < alvo) return min(alvo, atual + kPassoRampaPwm);
    return max(alvo, atual - kPassoRampaPwm);
}

void BaseFuncoes::atualizarCinematica() {
    if (modoAtual_ == ModoTracao::Manual && ultimoCmdManual_ != "stop") {
        atualizarAlvoManualMecanum_();
    } else if (modoAtual_ == ModoTracao::Turn) {
        // Interrompe o giro após 5 s para impedir rotação infinita caso a ESP32 dos braços pare de enviar o giroscópio.
        if (millis() - inicioTurnMs_ > 5000) {
            pararBrusco();
            return;
        }

        float erroGraus = alvoAngulo_ - getGyroRelativo();
        // Usa banda morta de +-2° para encerrar o giro sem ficar oscilando em torno do ponto de parada.
        if (abs(erroGraus) <= 2.0f) {
            pararBrusco();
            return;
        }

        // Garante piso mínimo de PWM maior no giro para vencer o arrasto lateral dos roletes mecanum perto do alvo.
        int pisoGiro = min(velAlvo_, max(kPwmMinBase, 55));
        int pwmProporcional = constrain(static_cast<int>(abs(erroGraus) * 2.2f), pisoGiro, velAlvo_);
        int pwm = (erroGraus > 0) ? pwmProporcional : -pwmProporcional;

        // Zera a inércia da rampa ao inverter o sinal do erro para corrigir eventuais ultrapassagens imediatamente.
        if ((pwm > 0 && pwmAtualFL_ < 0) || (pwm < 0 && pwmAtualFL_ > 0)) {
            pwmAtualFL_ = pwmAtualFR_ = pwmAtualRL_ = pwmAtualRR_ = 0;
        }
        definirAlvoMecanum_(pwm, -pwm, pwm, -pwm);
    } else if (modoAtual_ != ModoTracao::Manual && modoAtual_ != ModoTracao::Parado) {
        noInterrupts();
        long atualM1 = ticksM1;
        interrupts();

        long erroTicks = abs(static_cast<long>(alvoTicks_)) - abs(atualM1 - inicioM1_);
        // Encerra o deslocamento 5 ticks (~3 mm) antes do fim para compensar o deslizamento residual das rodas.
        if (erroTicks <= 5) {
            pararBrusco();
            return;
        }

        int pwm = constrain(static_cast<int>(erroTicks * 1.5f), kPwmMinBase, velAlvo_);
        if (alvoTicks_ < 0) pwm = -pwm;

        // Mantém um par diagonal parado no movimento oblíquo porque os roletes a 45° geram resultante diagonal.
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