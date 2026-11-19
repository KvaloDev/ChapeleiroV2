#include "bracosSetup.h"
#include "config.h"
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

volatile long ticksOmbroEsq = 0;
volatile long ticksOmbroDir = 0;
volatile float anguloSpikeGraus = 0.0f;

// Lookup table em código Gray: rejeita transições simultâneas impossíveis causadas por ruído elétrico.
constexpr int8_t kTabelaQuadBraco[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
static volatile uint8_t estOmbroE = 0, estOmbroD = 0;
static volatile unsigned long ultMsE = 0, ultMsD = 0;

void IRAM_ATTR atualizarQuadBraco(uint8_t pinA, uint8_t pinB, volatile uint8_t& est, volatile unsigned long& ult, volatile long& ticks, int dir) {
    unsigned long agora = micros();
    // Picos induzidos pelo chaveamento PWM da ponte H duram menos de 50 µs.
    if (agora - ult < 50) return;

    uint8_t atual = (digitalRead(pinA) << 1) | digitalRead(pinB);
    ticks += kTabelaQuadBraco[(est << 2) | atual] * dir;
    est = atual;
    ult = agora;
}

void IRAM_ATTR isrOmbroEsq() { atualizarQuadBraco(kPinEncOmbroEsqA, kPinEncOmbroEsqB, estOmbroE, ultMsE, ticksOmbroEsq, kDirEncOmbroEsq); }
void IRAM_ATTR isrOmbroDir() { atualizarQuadBraco(kPinEncOmbroDirA, kPinEncOmbroDirB, estOmbroD, ultMsD, ticksOmbroDir, kDirEncOmbroDir); }

bool decodificarPacotePybricks(const std::string& dados, float& anguloSaida) {
    if (dados.size() < 4) return false;
    const uint8_t* d = reinterpret_cast<const uint8_t*>(dados.data());

    // Filtra apenas pacotes com o Company ID oficial da LEGO (0x0397 em little-endian).
    if (d[0] != 0x97 || d[1] != 0x03) return false;

    // O offset do canal varia conforme a presença do byte de subtipo entre revisões do firmware Pybricks.
    size_t idxCanal = 0;
    for (size_t k = 2; k < dados.size() && k <= 4; k++) {
        if (d[k] == kCanalSpikeGyro) {
            idxCanal = k;
            break;
        }
    }
    if (idxCanal == 0) return false;

    size_t i = idxCanal + 1;
    while (i < dados.size() && d[i] == 0x00) i++;
    if (i >= dados.size()) return false;

    uint8_t header = d[i++];
    uint8_t tipo = header >> 5;
    uint8_t bytes = header & 0x1F;

    // No protocolo Pybricks, o tipo 3 identifica inteiros com sinal (little-endian).
    if (tipo != 3 || i + bytes > dados.size()) return false;

    int32_t valor = 0;
    if (bytes == 1) valor = static_cast<int8_t>(d[i]);
    else if (bytes == 2) valor = static_cast<int16_t>(d[i] | (d[i + 1] << 8));
    else if (bytes == 4) valor = static_cast<int32_t>(d[i] | (d[i + 1] << 8) | (d[i + 2] << 16) | (d[i + 3] << 24));
    else return false;

    // O hub transmite o ângulo multiplicado por 10 para preservar 1 casa decimal sem overhead de float.
    anguloSaida = static_cast<float>(valor) / 10.0f;
    return true;
}

class SpikeBleCallback : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice dispositivo) override {
        if (!dispositivo.haveManufacturerData()) return;
        float anguloLido = 0.0f;
        if (decodificarPacotePybricks(dispositivo.getManufacturerData(), anguloLido)) {
            anguloSpikeGraus = anguloLido;
        }
    }
};

void BracosSetup::inicializarHardware() {
    Serial2.begin(kBaudSerial2, SERIAL_8N1, kPinRxSerial2, kPinTxSerial2);

    // Ignora pinos marcados com 255 enquanto o mapeamento físico estiver pendente.
    if (kPinOmbroEsqRpwm != 255) { pinMode(kPinOmbroEsqRpwm, OUTPUT); pinMode(kPinOmbroEsqLpwm, OUTPUT); }
    if (kPinOmbroDirRpwm != 255) { pinMode(kPinOmbroDirRpwm, OUTPUT); pinMode(kPinOmbroDirLpwm, OUTPUT); }

    if (kPinEncOmbroEsqA != 255 && kPinEncOmbroEsqB != 255) {
        pinMode(kPinEncOmbroEsqA, INPUT_PULLUP);
        pinMode(kPinEncOmbroEsqB, INPUT_PULLUP);
        estOmbroE = (digitalRead(kPinEncOmbroEsqA) << 1) | digitalRead(kPinEncOmbroEsqB);
        attachInterrupt(digitalPinToInterrupt(kPinEncOmbroEsqA), isrOmbroEsq, CHANGE);
        attachInterrupt(digitalPinToInterrupt(kPinEncOmbroEsqB), isrOmbroEsq, CHANGE);
    }

    if (kPinEncOmbroDirA != 255 && kPinEncOmbroDirB != 255) {
        pinMode(kPinEncOmbroDirA, INPUT_PULLUP);
        pinMode(kPinEncOmbroDirB, INPUT_PULLUP);
        estOmbroD = (digitalRead(kPinEncOmbroDirA) << 1) | digitalRead(kPinEncOmbroDirB);
        attachInterrupt(digitalPinToInterrupt(kPinEncOmbroDirA), isrOmbroDir, CHANGE);
        attachInterrupt(digitalPinToInterrupt(kPinEncOmbroDirB), isrOmbroDir, CHANGE);
    }
}

void BracosSetup::inicializarBleSpike() {
    BLEDevice::init("");
    BLEScan* scanner = BLEDevice::getScan();
    // Aceitar duplicatas é obrigatório porque o hub reutiliza o mesmo endereço MAC em cada frame de telemetria.
    scanner->setAdvertisedDeviceCallbacks(new SpikeBleCallback(), true);
    scanner->setActiveScan(false);
    // Sem Wi-Fi nesta placa, o rádio BLE opera com ciclo de 95% para latência mínima no giroscópio.
    scanner->setInterval(100);
    scanner->setWindow(95);
    scanner->start(0, nullptr, false);
}