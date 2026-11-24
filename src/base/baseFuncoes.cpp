#include "baseFuncoes.h"
#include "baseSetup.h"
#include "config.h"
#include <cmath>

namespace {
// Mantém a referência angular absoluta para giros relativos consecutivos sem acumular erro da zona morta.
float sAlvoAbsolutoGyro = 0.0f;
bool sGyroAlteradoManualmente = false;

// Controle PI de rumo em movimentos lineares (run/strafe/diagonal) para eliminar desvio por atrito assimétrico.
float sRumoAlvoLinear = 0.0f;
float sIntegralRumoLinear = 0.0f;
unsigned long sUltimoCalculoRumoMs = 0;
constexpr float kKpRumoLinear = 4.5f;
constexpr float kKiRumoLinear = 2.2f;
constexpr float kMaxIntegralRumoPwm = 30.0f;
constexpr int kMaxCorrecaoRumoPwm = 45;

// Ganho proporcional da desaceleração linear (~30 cm antes do alvo a PWM 120) para evitar balanço na parada.
constexpr float kGanhoDesacelLinear = 0.24f;

// Rampa ágil exclusiva para giros (Turn/TurnTo) para vencer o atrito lateral rápido sem afetar a suavidade do Run.
constexpr float kPassoAcelTurnPwm = 45.0f;
constexpr float kPassoFreioTurnPwm = 50.0f;
}  // namespace

float BaseFuncoes::getGyroRelativo() const {
    // Subtrai offsetGyro_ e aplica kDirGyroSpike para alinhar o zero e o sentido positivo do hub ao referencial do chassi.
    return (anguloSpikeGraus - offsetGyro_) * kDirGyroSpike;
}

void BaseFuncoes::zerarGyro() {
    offsetGyro_ = anguloSpikeGraus;
    sAlvoAbsolutoGyro = 0.0f;
    sRumoAlvoLinear = 0.0f;
    sIntegralRumoLinear = 0.0f;
    sGyroAlteradoManualmente = false;
}

bool BaseFuncoes::consumirStartSpike() {
    if (!startSpikePendente_) return false;
    startSpikePendente_ = false;
    return true;
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

    // Aceita vetor contínuo "mov" para combinar translação e giro ao mesmo tempo, mantendo atalhos legados.
    if (ultimoCmdManual_ == "mov") {
        xCampo = manualX_;
        yCampo = manualY_;
        rot = manualRot_;
    } else if (ultimoCmdManual_ == "w") { yCampo = 1.0f; }
    else if (ultimoCmdManual_ == "s") { yCampo = -1.0f; }
    else if (ultimoCmdManual_ == "d") { xCampo = 1.0f; }
    else if (ultimoCmdManual_ == "a") { xCampo = -1.0f; }
    // Usa 0.7071 (sen/cos de 45°) nas diagonais para que o vetor resultante tenha norma unitária igual aos eixos puros.
    else if (ultimoCmdManual_ == "wd") { xCampo = 0.7071f; yCampo = 0.7071f; }
    else if (ultimoCmdManual_ == "wa") { xCampo = -0.7071f; yCampo = 0.7071f; }
    else if (ultimoCmdManual_ == "sd") { xCampo = 0.7071f; yCampo = -0.7071f; }
    else if (ultimoCmdManual_ == "sa") { xCampo = -0.7071f; yCampo = -0.7071f; }
    else if (ultimoCmdManual_ == "q") { rot = -1.0f; }
    else if (ultimoCmdManual_ == "e") { rot = 1.0f; }

    // Sincroniza a âncora absoluta no próximo giro relativo se o piloto rotacionou o robô manualmente.
    if (fabsf(rot) > 0.01f) {
        sGyroAlteradoManualmente = true;
    }

    float xRobo = xCampo;
    float yRobo = yCampo;

    if (fieldOriented_ && (xCampo != 0.0f || yCampo != 0.0f)) {
        float rad = getGyroRelativo() * DEG_TO_RAD;
        float cosA = cosf(rad);
        float sinA = sinf(rad);
        // Aplica matriz de rotação 2D para converter a direção da arena no referencial local enquanto o robô gira.
        xRobo = xCampo * cosA - yCampo * sinA;
        yRobo = xCampo * sinA + yCampo * cosA;
    }

    float fl = yRobo + xRobo + rot;
    float fr = yRobo - xRobo - rot;
    float rl = yRobo - xRobo + rot;
    float rr = yRobo + xRobo - rot;

    // Divide pelo maior módulo quando excede 1.0 para preservar a proporção entre translação e rotação sem saturar.
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

void BaseFuncoes::pararBase() {
    definirAlvoMecanum_(0, 0, 0, 0);
    manualX_ = manualY_ = manualRot_ = 0.0f;
    ultimoCmdManual_ = "stop";

    if (fabsf(pwmAtualFL_) < 0.5f && fabsf(pwmAtualFR_) < 0.5f &&
        fabsf(pwmAtualRL_) < 0.5f && fabsf(pwmAtualRR_) < 0.5f) {
        pwmAtualFL_ = pwmAtualFR_ = pwmAtualRL_ = pwmAtualRR_ = 0.0f;
        modoAtual_ = ModoTracao::Parado;
    } else {
        // Mantém o modo Manual ativo até que a rampa de frenagem reduza o PWM atual das rodas a zero.
        modoAtual_ = ModoTracao::Manual;
    }
}

void BaseFuncoes::parar() {
    pararBase();
}

void BaseFuncoes::pararBrusco() {
    // Trava as rodas no alvo de giro assim que a desaceleração final atinge o piso mínimo.
    definirAlvoMecanum_(0, 0, 0, 0);
    manualX_ = manualY_ = manualRot_ = 0.0f;
    pwmAtualFL_ = pwmAtualFR_ = pwmAtualRL_ = pwmAtualRR_ = 0.0f;
    acionarMecanum_(0, 0, 0, 0);
    ultimoCmdManual_ = "stop";
    modoAtual_ = ModoTracao::Parado;
}

void BaseFuncoes::run(float distanciaMm, int velocidade) {
    // Isola a leitura do contador volatile com noInterrupts() para evitar leitura corrompida pela ISR.
    noInterrupts(); inicioM1_ = ticksM1; interrupts();
    alvoTicks_ = distanciaMm * kTicksPorMmBase;
    velAlvo_ = constrain(velocidade, kPwmMinBase, kPwmMax);
    sRumoAlvoLinear = getGyroRelativo();
    sIntegralRumoLinear = 0.0f;
    sUltimoCalculoRumoMs = millis();
    modoAtual_ = ModoTracao::Run;
}

void BaseFuncoes::strafe(float distanciaMm, int velocidade) {
    noInterrupts(); inicioM1_ = ticksM1; interrupts();
    alvoTicks_ = distanciaMm * kTicksPorMmBase;
    velAlvo_ = constrain(velocidade, kPwmMinBase, kPwmMax);
    sRumoAlvoLinear = getGyroRelativo();
    sIntegralRumoLinear = 0.0f;
    sUltimoCalculoRumoMs = millis();
    modoAtual_ = ModoTracao::Strafe;
}

void BaseFuncoes::diagonal(float distanciaMm, bool isDireita, int velocidade) {
    noInterrupts(); inicioM1_ = ticksM1; interrupts();
    alvoTicks_ = distanciaMm * kTicksPorMmBase;
    velAlvo_ = constrain(velocidade, kPwmMinBase, kPwmMax);
    sRumoAlvoLinear = getGyroRelativo();
    sIntegralRumoLinear = 0.0f;
    sUltimoCalculoRumoMs = millis();
    modoAtual_ = isDireita ? ModoTracao::DiagonalDir : ModoTracao::DiagonalEsq;
}

void BaseFuncoes::turn(float graus, int velocidade) {
    if (sGyroAlteradoManualmente) {
        sAlvoAbsolutoGyro = getGyroRelativo();
        sGyroAlteradoManualmente = false;
    }

    sAlvoAbsolutoGyro += graus;
    float atual = getGyroRelativo();

    // Calcula o menor deslocamento modular até a orientação absoluta desejada.
    float diferenca = fmodf(sAlvoAbsolutoGyro - atual, 360.0f);
    if (diferenca > 180.0f)  diferenca -= 360.0f;
    if (diferenca < -180.0f) diferenca += 360.0f;

    // Preserva o sentido escolhido (RB = direita/+, LB = esquerda/-) mesmo em giros de 180° ou maiores.
    if (graus > 0.0f && diferenca < -2.0f) {
        diferenca += 360.0f;
    } else if (graus < 0.0f && diferenca > 2.0f) {
        diferenca -= 360.0f;
    }

    alvoAngulo_ = atual + diferenca;
    velAlvo_ = constrain(velocidade, kPwmMinBase, kPwmMax);
    inicioTurnMs_ = millis();
    modoAtual_ = ModoTracao::Turn;
}

void BaseFuncoes::turnTo(float anguloAlvo, int velocidade) {
    sAlvoAbsolutoGyro = anguloAlvo;
    sGyroAlteradoManualmente = false;

    float atual = getGyroRelativo();
    float diferenca = fmodf(anguloAlvo - atual, 360.0f);

    // Normaliza a diferença para [-180°, +180°] a fim de girar sempre pelo menor arco angular (módulo 360°).
    if (diferenca > 180.0f)  diferenca -= 360.0f;
    if (diferenca < -180.0f) diferenca += 360.0f;

    // Soma o menor deslocamento à leitura contínua atual para evitar saltos de referência ao cruzar 0°/360°.
    alvoAngulo_ = atual + diferenca;
    velAlvo_ = constrain(velocidade, kPwmMinBase, kPwmMax);
    inicioTurnMs_ = millis();
    modoAtual_ = ModoTracao::Turn;
}

void BaseFuncoes::moverGirando(float xCampo, float yCampo, float anguloGraus, int velocidade) {
    autoX_ = constrain(xCampo, -1.0f, 1.0f);
    autoY_ = constrain(yCampo, -1.0f, 1.0f);
    alvoAngulo_ = getGyroRelativo() + anguloGraus;
    sAlvoAbsolutoGyro = alvoAngulo_;
    sGyroAlteradoManualmente = false;
    velAlvo_ = constrain(velocidade, kPwmMinBase, kPwmMax);
    inicioTurnMs_ = millis();
    modoAtual_ = ModoTracao::MovGirando;
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

void BaseFuncoes::enviarCodigoCenario(int codigo, int canal) {
    int canalSeguro = constrain(canal, 0, 255);
    Serial2.printf("BLE_TX:%d,%d\n", codigo, canalSeguro);
}

void BaseFuncoes::processarComandoManual(const String& cmd) {
    if (cmd == "zero_gyro") { zerarGyro(); return; }

    if (cmd.startsWith("ble_tx:")) {
        int virgula = cmd.indexOf(',');
        if (virgula > 7) {
            int codigo = cmd.substring(7, virgula).toInt();
            int canal = cmd.substring(virgula + 1).toInt();
            enviarCodigoCenario(codigo, canal);
        } else {
            enviarCodigoCenario(cmd.substring(7).toInt(), kCanalSpikeCenario);
        }
        return;
    }

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

    // Permite ajustar ao vivo a suavidade da arrancada e da frenagem (em PWM por 100 ms) sem regravar o código.
    if (cmd.startsWith("acel:")) {
        passoAcelPwm_ = constrain(cmd.substring(5).toInt(), 1, kPwmMax);
        return;
    }

    if (cmd.startsWith("freio:")) {
        passoFreioPwm_ = constrain(cmd.substring(6).toInt(), 1, kPwmMax);
        return;
    }

    if (cmd.startsWith("vel_braco:")) {
        Serial2.println(cmd);
        return;
    }

    if (cmd.startsWith("mov:")) {
        int v1 = cmd.indexOf(',');
        int v2 = cmd.indexOf(',', v1 + 1);
        if (v1 > 4 && v2 > v1) {
            manualX_ = constrain(cmd.substring(4, v1).toFloat(), -1.0f, 1.0f);
            manualY_ = constrain(cmd.substring(v1 + 1, v2).toFloat(), -1.0f, 1.0f);
            manualRot_ = constrain(cmd.substring(v2 + 1).toFloat(), -1.0f, 1.0f);
            modoAtual_ = ModoTracao::Manual;
            ultimoCmdManual_ = "mov";
            atualizarAlvoManualMecanum_();
        }
        return;
    }

    if (cmd.startsWith("run:")) {
        int virgula = cmd.indexOf(',');
        if (virgula > 0) {
            run(cmd.substring(4, virgula).toFloat(), cmd.substring(virgula + 1).toInt());
        } else {
            run(cmd.substring(4).toFloat(), velocidadeManual_);
        }
        return;
    }

    if (cmd.startsWith("strafe:")) {
        int virgula = cmd.indexOf(',');
        if (virgula > 0) {
            strafe(cmd.substring(7, virgula).toFloat(), cmd.substring(virgula + 1).toInt());
        } else {
            strafe(cmd.substring(7).toFloat(), velocidadeManual_);
        }
        return;
    }

    if (cmd.startsWith("diag:")) {
        int v1 = cmd.indexOf(',');
        int v2 = cmd.indexOf(',', v1 + 1);
        if (v1 > 5 && v2 > v1) {
            diagonal(cmd.substring(5, v1).toFloat(), cmd.substring(v1 + 1, v2).toInt() == 1, cmd.substring(v2 + 1).toInt());
        }
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
    if (cmd.startsWith("srv_") || cmd.startsWith("SRV_") || cmd.startsWith("be_") || cmd.startsWith("bd_") ||
        cmd.startsWith("OMB_") || cmd == "BE_STP" || cmd == "BD_STP") {
        Serial2.println(cmd);
        return;
    }

    // Para apenas as rodas sem interferir nos braços durante gravação por cima ou troca de direção.
    if (cmd == "stop_base") {
        pararBase();
        return;
    }

    // Parada geral de emergência: interrompe rodas e braços.
    if (cmd == "stop") {
        pararBase();
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
    static float sUltimoAnguloBruto = 0.0f;
    static bool sFiltroGyroAtivo = false;

    // Esvazia todo o buffer RX a cada ciclo para descartar leituras antigas e usar sempre o ângulo mais recente.
    while (Serial2.available()) {
        String msg = Serial2.readStringUntil('\n');
        msg.trim();
        if (msg == "START") {
            startSpikePendente_ = true;
            sFiltroGyroAtivo = false;
            continue;
        }
        if (!msg.startsWith("ENC:")) continue;

        int v1 = msg.indexOf(',');
        int v2 = msg.indexOf(',', v1 + 1);
        if (v1 > 4 && v2 > v1 + 1 && static_cast<int>(msg.length()) > v2 + 1) {
            ticksOmbroEsq_ = msg.substring(4, v1).toInt();
            ticksOmbroDir_ = msg.substring(v1 + 1, v2).toInt();
            float novoAngulo = msg.substring(v2 + 1).toFloat();

            // Ignora picos isolados causados por ruído elétrico dos motores na UART; aceita se repetir no pacote seguinte.
            if (!sFiltroGyroAtivo || fabsf(novoAngulo - anguloSpikeGraus) <= 30.0f || fabsf(novoAngulo - sUltimoAnguloBruto) <= 5.0f) {
                anguloSpikeGraus = novoAngulo;
                sFiltroGyroAtivo = true;
            }
            sUltimoAnguloBruto = novoAngulo;
        }
    }
}

float BaseFuncoes::calcularPassoRampa_(float atual, float alvo) {
    if (fabsf(alvo - atual) <= 0.01f) return alvo;

    // Converte a taxa definida por 100 ms para o incremento proporcional ao ciclo atual de kIntervaloRampaMs.
    float escala = static_cast<float>(kIntervaloRampaMs) / 100.0f;
    bool isGiro = (modoAtual_ == ModoTracao::Turn);
    float passoAcel = (isGiro ? kPassoAcelTurnPwm : static_cast<float>(passoAcelPwm_)) * escala;
    float passoFreio = (isGiro ? kPassoFreioTurnPwm : static_cast<float>(passoFreioPwm_)) * escala;

    // Desacelera suavemente até zero antes de inverter a polaridade para evitar trancos na ponte H BTS7960.
    if (atual > 0.0f && alvo < 0.0f) return max(0.0f, atual - passoFreio);
    if (atual < 0.0f && alvo > 0.0f) return min(0.0f, atual + passoFreio);

    // Parte direto de kPwmInicialRampa ao sair do repouso para vencer a zona morta mecânica sem atraso.
    if (fabsf(atual) < 0.5f) {
        if (alvo > 0.0f) return min(alvo, static_cast<float>(kPwmInicialRampa));
        return max(alvo, -static_cast<float>(kPwmInicialRampa));
    }

    // Reduz o PWM acompanhando a desaceleração sem travar a curva de aproximação.
    if (fabsf(alvo) < fabsf(atual)) {
        if (atual > 0.0f) return max(alvo, atual - passoFreio);
        return min(alvo, atual + passoFreio);
    }

    // Usa passoAcel ao ganhar velocidade para evitar patinagem das rodas e picos de corrente.
    if (atual < alvo) return min(alvo, atual + passoAcel);
    return max(alvo, atual - passoAcel);
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
        if (abs(erroGraus) <= 2.0f) {
            pararBrusco();
            return;
        }

        // Gira na velocidade máxima até ~35° do alvo, desacelera até kPwmMinBase e trava na posição.
        int pwm = constrain(static_cast<int>(abs(erroGraus) * 2.6f), kPwmMinBase, velAlvo_);
        if (erroGraus < 0) pwm = -pwm;
        definirAlvoMecanum_(pwm, -pwm, pwm, -pwm);
    } else if (modoAtual_ == ModoTracao::MovGirando) {
        // Interrompe após 5 s para impedir deslocamento infinito caso o giroscópio pare de atualizar via UART.
        if (millis() - inicioTurnMs_ > 5000) {
            pararBrusco();
            return;
        }

        float erroGraus = alvoAngulo_ - getGyroRelativo();
        if (abs(erroGraus) <= 2.0f) {
            pararBrusco();
            return;
        }

        float xRobo = autoX_;
        float yRobo = autoY_;
        if (fieldOriented_ && (autoX_ != 0.0f || autoY_ != 0.0f)) {
            float rad = getGyroRelativo() * DEG_TO_RAD;
            float cosA = cosf(rad);
            float sinA = sinf(rad);
            // Atualiza o referencial local continuamente porque a orientação do chassi muda enquanto gira.
            xRobo = autoX_ * cosA - autoY_ * sinA;
            yRobo = autoX_ * sinA + autoY_ * cosA;
        }

        // Mantém piso mínimo de 0.35 na componente de giro para concluir o ângulo final sem perder tração.
        float magRot = constrain(fabsf(erroGraus) / 45.0f, 0.35f, 1.0f);
        float rot = (erroGraus > 0.0f) ? magRot : -magRot;

        float fl = yRobo + xRobo + rot;
        float fr = yRobo - xRobo - rot;
        float rl = yRobo - xRobo + rot;
        float rr = yRobo + xRobo - rot;

        // Normaliza pelo maior módulo para manter translação e rotação proporcionais dentro de velAlvo_.
        float maxMag = max(max(fabsf(fl), fabsf(fr)), max(fabsf(rl), fabsf(rr)));
        if (maxMag > 1.0f) {
            fl /= maxMag;
            fr /= maxMag;
            rl /= maxMag;
            rr /= maxMag;
        }

        definirAlvoMecanum_(
            static_cast<int>(roundf(fl * velAlvo_)),
            static_cast<int>(roundf(fr * velAlvo_)),
            static_cast<int>(roundf(rl * velAlvo_)),
            static_cast<int>(roundf(rr * velAlvo_))
        );
    } else if (modoAtual_ != ModoTracao::Manual && modoAtual_ != ModoTracao::Parado) {
        noInterrupts();
        long atualM1 = ticksM1;
        interrupts();

        long erroTicks = abs(static_cast<long>(alvoTicks_)) - abs(atualM1 - inicioM1_);
        // Inicia a rampa final para zero ~7 mm (12 ticks) antes do alvo para parar sem tranco na estrutura.
        if (erroTicks <= 12) {
            pararBase();
            return;
        }

        // Desacelera progressivamente com kGanhoDesacelLinear até kPwmMinBase antes da rampa final de parada.
        int pwm = constrain(static_cast<int>(erroTicks * kGanhoDesacelLinear), kPwmMinBase, velAlvo_);
        if (alvoTicks_ < 0) pwm = -pwm;

        unsigned long agoraRumo = millis();
        float dtRumo = (sUltimoCalculoRumoMs > 0) ? min((agoraRumo - sUltimoCalculoRumoMs) / 1000.0f, 0.1f) : 0.02f;
        sUltimoCalculoRumoMs = agoraRumo;

        // Controle PI de rumo via giroscópio: Kp corrige desvios rápidos e Ki elimina a tendência contínua para um lado.
        float erroRumo = sRumoAlvoLinear - getGyroRelativo();
        if (fabsf(erroRumo) > 0.3f) {
            sIntegralRumoLinear = constrain(
                sIntegralRumoLinear + (erroRumo * kKiRumoLinear * dtRumo),
                -kMaxIntegralRumoPwm,
                kMaxIntegralRumoPwm
            );
        }
        int corr = constrain(
            static_cast<int>(roundf((erroRumo * kKpRumoLinear) + sIntegralRumoLinear)),
            -kMaxCorrecaoRumoPwm,
            kMaxCorrecaoRumoPwm
        );

        int fl = 0, fr = 0, rl = 0, rr = 0;
        if (modoAtual_ == ModoTracao::Run) {
            fl = pwm + corr;
            fr = pwm - corr;
            rl = pwm + corr;
            rr = pwm - corr;
        } else if (modoAtual_ == ModoTracao::Strafe) {
            fl = pwm + corr;
            fr = -pwm - corr;
            rl = -pwm + corr;
            rr = pwm - corr;
        } else if (modoAtual_ == ModoTracao::DiagonalDir) {
            // Mantém o par diagonal secundário parado porque os roletes a 45° geram resultante diagonal.
            fl = pwm + corr;
            rr = pwm - corr;
        } else if (modoAtual_ == ModoTracao::DiagonalEsq) {
            fr = pwm - corr;
            rl = pwm + corr;
        }

        definirAlvoMecanum_(
            constrain(fl, -velAlvo_, velAlvo_),
            constrain(fr, -velAlvo_, velAlvo_),
            constrain(rl, -velAlvo_, velAlvo_),
            constrain(rr, -velAlvo_, velAlvo_)
        );
    }

    if (modoAtual_ == ModoTracao::Parado) return;

    unsigned long agora = millis();
    if (agora - ultimoPassoRampaMs_ < kIntervaloRampaMs) return;
    ultimoPassoRampaMs_ = agora;

    pwmAtualFL_ = calcularPassoRampa_(pwmAtualFL_, pwmAlvoFL_);
    pwmAtualFR_ = calcularPassoRampa_(pwmAtualFR_, pwmAlvoFR_);
    pwmAtualRL_ = calcularPassoRampa_(pwmAtualRL_, pwmAlvoRL_);
    pwmAtualRR_ = calcularPassoRampa_(pwmAtualRR_, pwmAlvoRR_);

    acionarMecanum_(
        static_cast<int>(roundf(pwmAtualFL_)),
        static_cast<int>(roundf(pwmAtualFR_)),
        static_cast<int>(roundf(pwmAtualRL_)),
        static_cast<int>(roundf(pwmAtualRR_))
    );

    if (modoAtual_ == ModoTracao::Manual &&
        pwmAlvoFL_ == 0 && pwmAlvoFR_ == 0 && pwmAlvoRL_ == 0 && pwmAlvoRR_ == 0 &&
        fabsf(pwmAtualFL_) < 0.5f && fabsf(pwmAtualFR_) < 0.5f &&
        fabsf(pwmAtualRL_) < 0.5f && fabsf(pwmAtualRR_) < 0.5f) {
        pwmAtualFL_ = pwmAtualFR_ = pwmAtualRL_ = pwmAtualRR_ = 0.0f;
        modoAtual_ = ModoTracao::Parado;
    }
}