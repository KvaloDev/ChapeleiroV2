#include "bracosSetup.h"
#include "config.h"
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEAdvertising.h>
#include <esp_bt.h>

static volatile bool sStartPendente = false;

volatile long ticksOmbroEsq = 0;
volatile long ticksOmbroDir = 0;
volatile float anguloSpikeGraus = 0.0f;

constexpr int8_t kTabelaQuadBraco[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
static volatile uint8_t estOmbroE = 0, estOmbroD = 0;
static volatile unsigned long ultMsE = 0, ultMsD = 0;

// ---------------------------------------------------------------------------
// Estado do TX BLE
// ---------------------------------------------------------------------------
static BLEAdvertising* sAdv = nullptr;
static bool sAguardandoStartAdv = false;   // pacote carregado, esperando para dar start
static unsigned long sMomentoStartAdvMs = 0;
static bool sTransmitindo = false;         // advertising ativo
static bool sTransmissaoInfinita = false;  // duracaoMs == 0
static unsigned long sFimTransmissaoMs = 0;
static uint16_t sDuracaoPendenteMs = 0;

// Tipos do formato Pybricks broadcast/observe (pybricks/technical-info).
constexpr uint8_t kPbTipoSingleObject = 0;
constexpr uint8_t kPbTipoInt = 3;

void IRAM_ATTR atualizarQuadBraco(uint8_t pinA, uint8_t pinB, volatile uint8_t& est, volatile unsigned long& ult, volatile long& ticks, int dir) {
    unsigned long agora = micros();
    if (agora - ult < 50) return;

    uint8_t atual = (digitalRead(pinA) << 1) | digitalRead(pinB);
    ticks += kTabelaQuadBraco[(est << 2) | atual] * dir;
    est = atual;
    ult = agora;
}

void IRAM_ATTR isrOmbroEsq() { atualizarQuadBraco(kPinEncOmbroEsqA, kPinEncOmbroEsqB, estOmbroE, ultMsE, ticksOmbroEsq, kDirEncOmbroEsq); }
void IRAM_ATTR isrOmbroDir() { atualizarQuadBraco(kPinEncOmbroDirA, kPinEncOmbroDirB, estOmbroD, ultMsD, ticksOmbroDir, kDirEncOmbroDir); }

// ---------------------------------------------------------------------------
// RX: decodifica pacote Pybricks vindo do SPIKE (inalterado)
// ---------------------------------------------------------------------------
bool decodificarPacotePybricks(const std::string& dados, float& anguloSaida) {
    if (dados.size() < 4) return false;
    const uint8_t* d = reinterpret_cast<const uint8_t*>(dados.data());

    if (d[0] != 0x97 || d[1] != 0x03) return false;

    size_t idxCanal = 0;
    for (size_t k = 2; k < dados.size() && k <= 4; k++) {
        if (d[k] == kCanalSpikeGyro) {
            idxCanal = k;
            break;
        }
    }
    if (idxCanal == 0) return false;

    size_t i = idxCanal + 1;
    while (i < dados.size() && d[i] == 0x00) i++;  // pula o header SINGLE_OBJECT (0x00)
    if (i >= dados.size()) return false;

    uint8_t header = d[i++];
    uint8_t tipo = header >> 5;
    uint8_t bytes = header & 0x1F;

    if (tipo != 3 || i + bytes > dados.size()) return false;

    int32_t valor = 0;
    if (bytes == 1) valor = static_cast<int8_t>(d[i]);
    else if (bytes == 2) valor = static_cast<int16_t>(d[i] | (d[i + 1] << 8));
    else if (bytes == 4) valor = static_cast<int32_t>(d[i] | (d[i + 1] << 8) | (d[i + 2] << 16) | (d[i + 3] << 24));
    else return false;

    anguloSaida = static_cast<float>(valor) / 10.0f;
    return true;
}

class SpikeBleCallback : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice dispositivo) override {
        if (!dispositivo.haveManufacturerData()) return;
        float anguloLido = 0.0f;
        if (decodificarPacotePybricks(dispositivo.getManufacturerData(), anguloLido)) {
            if (anguloLido >= 2500.0f) {
                anguloSpikeGraus = 0.0f;
                static unsigned long ultimoStartMs = 0;
                if (millis() - ultimoStartMs > 1000) {
                    ultimoStartMs = millis();
                    sStartPendente = true;
                }
                return;
            }
            anguloSpikeGraus = anguloLido;
        }
    }
};

// ---------------------------------------------------------------------------
// TX: monta o pacote EXATAMENTE como hub.ble.broadcast(valor) do Pybricks
//
//   97 03            company ID LEGO (0x0397, little-endian)
//   <canal>          canal 0..255
//   00               header SINGLE_OBJECT  <-- obrigatorio para valor unico
//   <(3<<5)|n>       header INT com n bytes (n = 1, 2 ou 4)
//   <valor LE>       n bytes, complemento de 2, little-endian
//
// Sem o byte 00, o receptor interpreta o payload como TUPLA -> observe()
// devolve (2565,) em vez de 2565 e "(2565,) != 2565" nunca sai do while.
// ---------------------------------------------------------------------------
static std::string montarPacotePybricks(uint8_t canal, int16_t valor) {
    std::string d;
    d.push_back(static_cast<char>(0x97));
    d.push_back(static_cast<char>(0x03));
    d.push_back(static_cast<char>(canal));
    d.push_back(static_cast<char>((kPbTipoSingleObject << 5) | 0));

    if (valor >= -128 && valor <= 127) {
        d.push_back(static_cast<char>((kPbTipoInt << 5) | 1));
        d.push_back(static_cast<char>(valor & 0xFF));
    } else {
        d.push_back(static_cast<char>((kPbTipoInt << 5) | 2));
        d.push_back(static_cast<char>(valor & 0xFF));
        d.push_back(static_cast<char>((valor >> 8) & 0xFF));
    }
    return d;
}

void BracosSetup::inicializarHardware() {
    Serial2.begin(kBaudSerial2, SERIAL_8N1, kPinRxSerial2, kPinTxSerial2);

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

    // Mais potencia no anuncio (o robo e metalico e o SPIKE do cenario pode estar longe).
    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);
    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_P9);

    sAdv = BLEDevice::getAdvertising();
    sAdv->setAdvertisementType(ADV_TYPE_NONCONN_IND);
    sAdv->setScanResponse(false);
    sAdv->setMinInterval(0x20);  // 20 ms
    sAdv->setMaxInterval(0x30);  // 30 ms

    BLEScan* scanner = BLEDevice::getScan();
    scanner->setAdvertisedDeviceCallbacks(new SpikeBleCallback(), true);
    scanner->setActiveScan(false);
    scanner->setInterval(100);
    scanner->setWindow(85);
    scanner->start(0, nullptr, false);
}

void BracosSetup::transmitirCodigoSpike(int16_t valor, uint8_t canal, uint16_t duracaoMs) {
    if (!sAdv) return;

    const std::string dados = montarPacotePybricks(canal, valor);

    BLEAdvertisementData pacote;
    pacote.setManufacturerData(dados);

    // Para o anuncio atual, carrega o novo pacote e so da start depois de ~40 ms
    // (esp_ble_gap_config_adv_data_raw e assincrono).
    sAdv->stop();
    sTransmitindo = false;
    sAdv->setAdvertisementData(pacote);

    sDuracaoPendenteMs = duracaoMs;
    sMomentoStartAdvMs = millis() + 40;
    sAguardandoStartAdv = true;

    // Dump hex do AD structure que vai ao ar: 0x?? 0xFF + dados
    Serial.printf("[BLE TX] canal=%u valor=%d dur=%ums AD=[%02X FF", canal, valor, duracaoMs,
                  static_cast<unsigned>(dados.size() + 1));
    for (size_t i = 0; i < dados.size(); i++) {
        Serial.printf(" %02X", static_cast<uint8_t>(dados[i]));
    }
    Serial.println("]");
}

void BracosSetup::atualizarTransmissaoBle() {

    if (sStartPendente) {
        sStartPendente = false;
        Serial2.printf("ENC:%ld,%ld,0.0\n", ticksOmbroEsq, ticksOmbroDir);
        Serial2.println("START");
        Serial.println("[BLE RX] START enviado para Base");
    }

    const unsigned long agora = millis();

    if (sAguardandoStartAdv && static_cast<long>(agora - sMomentoStartAdvMs) >= 0) {
        sAguardandoStartAdv = false;
        if (sAdv) {
            sAdv->start();
            sTransmitindo = true;
            sTransmissaoInfinita = (sDuracaoPendenteMs == 0);
            sFimTransmissaoMs = agora + sDuracaoPendenteMs;
            Serial.println("[BLE TX] Broadcast ativo!");
        }
        return;
    }

    if (sTransmitindo && !sTransmissaoInfinita && static_cast<long>(agora - sFimTransmissaoMs) >= 0) {
        sTransmitindo = false;
        if (sAdv) sAdv->stop();
        Serial.println("[BLE TX] Broadcast encerrado.");
    }
}