#include "bracosFuncoes.h"
#include "bracosSetup.h"
#include "bracosCenas.h"
#include "config.h"

extern BracosSetup setupBracos;

namespace {
// Canais LEDC altos (8 a 11) isolados dos timers 0-3 usados pela biblioteca ESP32Servo.
constexpr uint8_t kChOmbroEsqR = 8;
constexpr uint8_t kChOmbroEsqL = 9;
constexpr uint8_t kChOmbroDirR = 10;
constexpr uint8_t kChOmbroDirL = 11;
constexpr uint32_t kFreqPwmOmbroHz = 1000;
constexpr uint8_t kResPwmOmbroBits = 8;

// Calibração medida fisicamente no robô: ~340 ticks em 90 graus (3.78 ticks/grau).
constexpr float kTicksPorGrauCalibrado = 5.2f;
constexpr long kToleranciaTicksOmbro = 5;
constexpr float kKpOmbro = 3.5f;
constexpr int kPwmMinOmbroCalibrado = 20;

bool pwmOmbrosConfigurado = false;

void garantirPwmOmbrosIsolado() {
    if (pwmOmbrosConfigurado) return;
    if (kPinOmbroEsqRpwm == 255 || kPinOmbroEsqLpwm == 255 ||
        kPinOmbroDirRpwm == 255 || kPinOmbroDirLpwm == 255) return;

    ledcSetup(kChOmbroEsqR, kFreqPwmOmbroHz, kResPwmOmbroBits);
    ledcSetup(kChOmbroEsqL, kFreqPwmOmbroHz, kResPwmOmbroBits);
    ledcSetup(kChOmbroDirR, kFreqPwmOmbroHz, kResPwmOmbroBits);
    ledcSetup(kChOmbroDirL, kFreqPwmOmbroHz, kResPwmOmbroBits);

    ledcAttachPin(kPinOmbroEsqRpwm, kChOmbroEsqR);
    ledcAttachPin(kPinOmbroEsqLpwm, kChOmbroEsqL);
    ledcAttachPin(kPinOmbroDirRpwm, kChOmbroDirR);
    ledcAttachPin(kPinOmbroDirLpwm, kChOmbroDirL);

    ledcWrite(kChOmbroEsqR, 0);
    ledcWrite(kChOmbroEsqL, 0);
    ledcWrite(kChOmbroDirR, 0);
    ledcWrite(kChOmbroDirL, 0);
    pwmOmbrosConfigurado = true;
}
}  // namespace

void BracosFuncoes::inicializarServos() {
    garantirPwmOmbrosIsolado();
    if (kPinServoCotoveloEsq == 255 || kPinServoCotoveloDir == 255) return;

    // Reserva timers específicos para os servos não colidirem com o PWM das pontes H BTS7960.
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);

    servoCotoveloEsq_.writeMicroseconds(1500);
    servoCotoveloDir_.writeMicroseconds(1500);
    servoCotoveloEsq_.attach(kPinServoCotoveloEsq, 500, 2500);
    servoCotoveloDir_.attach(kPinServoCotoveloDir, 500, 2500);
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
    garantirPwmOmbrosIsolado();

    uint8_t chR = (pinR == kPinOmbroEsqRpwm) ? kChOmbroEsqR : kChOmbroDirR;
    uint8_t chL = (pinL == kPinOmbroEsqLpwm) ? kChOmbroEsqL : kChOmbroDirL;

    if (pwm > 0) {
        ledcWrite(chR, pwm);
        ledcWrite(chL, 0);
    } else if (pwm < 0) {
        ledcWrite(chR, 0);
        ledcWrite(chL, abs(pwm));
    } else {
        ledcWrite(chR, 0);
        ledcWrite(chL, 0);
    }
}

void BracosFuncoes::pararOmbros() {
    isOmbroEsqAtivo_ = false;
    isOmbroDirAtivo_ = false;
    isManualOmbroAtivo_ = false;
    aplicarPwmOmbro_(kPinOmbroEsqRpwm, kPinOmbroEsqLpwm, 0);
    aplicarPwmOmbro_(kPinOmbroDirRpwm, kPinOmbroDirLpwm, 0);
}

void BracosFuncoes::moverOmbroEsq(float graus, int velocidade) {
    float seguro = limitarAngulo_(graus, kOmbroMinGraus, kOmbroMaxGraus, "OmbroEsq");
    alvoTicksEsq_ = lroundf(seguro * kTicksPorGrauCalibrado);
    velOmbroEsq_ = constrain(velocidade, kPwmMinOmbroCalibrado, kPwmMax);
    isManualOmbroAtivo_ = false;
    isOmbroEsqAtivo_ = true;
}

void BracosFuncoes::moverOmbroDir(float graus, int velocidade) {
    float seguro = limitarAngulo_(graus, kOmbroMinGraus, kOmbroMaxGraus, "OmbroDir");
    alvoTicksDir_ = lroundf(seguro * kTicksPorGrauCalibrado);
    velOmbroDir_ = constrain(velocidade, kPwmMinOmbroCalibrado, kPwmMax);
    isManualOmbroAtivo_ = false;
    isOmbroDirAtivo_ = true;
}

void BracosFuncoes::moverCotoveloEsq(float graus) {
    if (kPinServoCotoveloEsq == 255) return;
    int posIntuitiva = constrain(static_cast<int>(graus), 0, 120);
    int anguloReal = map(posIntuitiva, 0, 120, static_cast<int>(kCotoveloEsqMinGraus), static_cast<int>(kCotoveloEsqMaxGraus));
    servoCotoveloEsq_.writeMicroseconds(map(anguloReal, 0, 120, 500, 2500));
}

void BracosFuncoes::moverCotoveloDir(float graus) {
    if (kPinServoCotoveloDir == 255) return;
    int posIntuitiva = constrain(static_cast<int>(graus), 0, 120);
    int anguloReal = map(posIntuitiva, 0, 120, static_cast<int>(kCotoveloDirMaxGraus), static_cast<int>(kCotoveloDirMinGraus));
    servoCotoveloDir_.writeMicroseconds(map(anguloReal, 0, 120, 500, 2500));
}

void BracosFuncoes::processarSerial2(BracosCenas& cenas) {
    while (Serial2.available()) {
        String cmd = Serial2.readStringUntil('\n');
        cmd.trim();
        if (cmd.length() == 0) continue;

        if (cmd.startsWith("CENA:")) {
            cenas.iniciarCena(cmd.substring(5).toInt());
        } else if (cmd.startsWith("BLE_TX:")) {
            int virgula = cmd.indexOf(',');
            int16_t codigo = (virgula > 7)
                ? static_cast<int16_t>(cmd.substring(7, virgula).toInt())
                : static_cast<int16_t>(cmd.substring(7).toInt());
            uint8_t canal = (virgula > 7)
                ? static_cast<uint8_t>(constrain(cmd.substring(virgula + 1).toInt(), 0, 255))
                : kCanalSpikeCenario;
            setupBracos.transmitirCodigoSpike(codigo, canal, 1500);
        } else if (cmd.startsWith("vel_braco:")) {
            velManualOmbro_ = constrain(cmd.substring(10).toInt(), kPwmMinOmbroCalibrado, kPwmMax);
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
        } else if (cmd == "be_up" || cmd == "BE_UP") {
            isOmbroEsqAtivo_ = false;
            isManualOmbroAtivo_ = true;
            ultimoCmdManualBracoMs_ = millis();
            aplicarPwmOmbro_(kPinOmbroEsqRpwm, kPinOmbroEsqLpwm, velManualOmbro_);
        } else if (cmd == "be_dwn" || cmd == "BE_DWN") {
            isOmbroEsqAtivo_ = false;
            isManualOmbroAtivo_ = true;
            ultimoCmdManualBracoMs_ = millis();
            aplicarPwmOmbro_(kPinOmbroEsqRpwm, kPinOmbroEsqLpwm, -velManualOmbro_);
        } else if (cmd == "bd_up" || cmd == "BD_UP") {
            isOmbroDirAtivo_ = false;
            isManualOmbroAtivo_ = true;
            ultimoCmdManualBracoMs_ = millis();
            aplicarPwmOmbro_(kPinOmbroDirRpwm, kPinOmbroDirLpwm, velManualOmbro_);
        } else if (cmd == "bd_dwn" || cmd == "BD_DWN") {
            isOmbroDirAtivo_ = false;
            isManualOmbroAtivo_ = true;
            ultimoCmdManualBracoMs_ = millis();
            aplicarPwmOmbro_(kPinOmbroDirRpwm, kPinOmbroDirLpwm, -velManualOmbro_);
        } else if (cmd == "BE_STP" || cmd == "be_stp") {
            isOmbroEsqAtivo_ = false;
            aplicarPwmOmbro_(kPinOmbroEsqRpwm, kPinOmbroEsqLpwm, 0);
        } else if (cmd == "BD_STP" || cmd == "bd_stp") {
            isOmbroDirAtivo_ = false;
            aplicarPwmOmbro_(kPinOmbroDirRpwm, kPinOmbroDirLpwm, 0);
        }
    }
}

void BracosFuncoes::atualizarCinematica() {
    if (isManualOmbroAtivo_ && (millis() - ultimoCmdManualBracoMs_ > 300)) {
        pararOmbros();
    }

    noInterrupts();
    long atualEsq = ticksOmbroEsq;
    long atualDir = ticksOmbroDir;
    interrupts();

    const long minTicks = lroundf(kOmbroMinGraus * kTicksPorGrauCalibrado);
    const long maxTicks = lroundf(kOmbroMaxGraus * kTicksPorGrauCalibrado);

    // Atualiza os dois ombros em paralelo e corta o PWM assim que atingem o alvo.
    if (isOmbroEsqAtivo_) {
        long erroEsq = alvoTicksEsq_ - atualEsq;
        bool atingiuFimCursoEsq = (erroEsq > 0 && atualEsq >= maxTicks) || (erroEsq < 0 && atualEsq <= minTicks);

        if (abs(erroEsq) <= kToleranciaTicksOmbro || atingiuFimCursoEsq) {
            isOmbroEsqAtivo_ = false;
            aplicarPwmOmbro_(kPinOmbroEsqRpwm, kPinOmbroEsqLpwm, 0);
        } else {
            int pwmMinEfetivo = min(kPwmMinOmbroCalibrado, velOmbroEsq_);
            int pwm = constrain(static_cast<int>(abs(erroEsq) * kKpOmbro), pwmMinEfetivo, velOmbroEsq_);
            aplicarPwmOmbro_(kPinOmbroEsqRpwm, kPinOmbroEsqLpwm, (erroEsq > 0) ? pwm : -pwm);
        }
    }

    if (isOmbroDirAtivo_) {
        long erroDir = alvoTicksDir_ - atualDir;
        bool atingiuFimCursoDir = (erroDir > 0 && atualDir >= maxTicks) || (erroDir < 0 && atualDir <= minTicks);

        if (abs(erroDir) <= kToleranciaTicksOmbro || atingiuFimCursoDir) {
            isOmbroDirAtivo_ = false;
            aplicarPwmOmbro_(kPinOmbroDirRpwm, kPinOmbroDirLpwm, 0);
        } else {
            int pwmMinEfetivo = min(kPwmMinOmbroCalibrado, velOmbroDir_);
            int pwm = constrain(static_cast<int>(abs(erroDir) * kKpOmbro), pwmMinEfetivo, velOmbroDir_);
            aplicarPwmOmbro_(kPinOmbroDirRpwm, kPinOmbroDirLpwm, (erroDir > 0) ? pwm : -pwm);
        }
    }
}

void BracosFuncoes::enviarTelemetriaSerial2() {
    if (millis() - ultimoEnvioTelemetriaMs_ < 20) return;
    ultimoEnvioTelemetriaMs_ = millis();

    noInterrupts();
    long e = ticksOmbroEsq, d = ticksOmbroDir;
    interrupts();
    float gyro = anguloSpikeGraus;

    Serial2.printf("ENC:%ld,%ld,%.1f\n", e, d, gyro);
}