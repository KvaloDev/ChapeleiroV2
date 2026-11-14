#include "baseSetup.h"
#include "baseFuncoes.h"
#include "baseCenas.h"
#include "config.h"
#include "webPage.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

volatile long ticksM1 = 0, ticksM2 = 0, ticksM3 = 0, ticksM4 = 0;
volatile float anguloSpikeGraus = 0.0f;

static AsyncWebServer server(80);
static AsyncWebSocket ws("/ws");

// Lookup table em código Gray: rejeita transições simultâneas impossíveis causadas por ruído elétrico.
constexpr int8_t kTabelaQuad[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
static volatile uint8_t estM1 = 0, estM2 = 0, estM3 = 0, estM4 = 0;
static volatile unsigned long ultMsM1 = 0, ultMsM2 = 0, ultMsM3 = 0, ultMsM4 = 0;

void IRAM_ATTR atualizarQuadratura(uint8_t pinA, uint8_t pinB, volatile uint8_t& est, volatile unsigned long& ult, volatile long& ticks, int dir) {
    unsigned long agora = micros();
    // Picos induzidos pelo chaveamento PWM da ponte H duram menos de 50 us.
    if (agora - ult < 50) return;

    uint8_t atual = (digitalRead(pinA) << 1) | digitalRead(pinB);
    ticks += kTabelaQuad[(est << 2) | atual] * dir;
    est = atual;
    ult = agora;
}

void IRAM_ATTR isrM1() { atualizarQuadratura(kPinEncM1A, kPinEncM1B, estM1, ultMsM1, ticksM1, kDirEncM1); }
void IRAM_ATTR isrM2() { atualizarQuadratura(kPinEncM2A, kPinEncM2B, estM2, ultMsM2, ticksM2, kDirEncM2); }
void IRAM_ATTR isrM3() { atualizarQuadratura(kPinEncM3A, kPinEncM3B, estM3, ultMsM3, ticksM3, kDirEncM3); }
void IRAM_ATTR isrM4() { atualizarQuadratura(kPinEncM4A, kPinEncM4B, estM4, ultMsM4, ticksM4, kDirEncM4); }

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
    if (bytes == 1) {
        valor = static_cast<int8_t>(d[i]);
    } else if (bytes == 2) {
        valor = static_cast<int16_t>(d[i] | (d[i + 1] << 8));
    } else if (bytes == 4) {
        valor = static_cast<int32_t>(d[i] | (d[i + 1] << 8) | (d[i + 2] << 16) | (d[i + 3] << 24));
    } else {
        return false;
    }

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

void BaseSetup::inicializarHardware() {
    Serial2.begin(kBaudSerial2, SERIAL_8N1, kPinRxSerial2, kPinTxSerial2);

    pinMode(kPinM1F, OUTPUT); pinMode(kPinM1T, OUTPUT);
    pinMode(kPinM2F, OUTPUT); pinMode(kPinM2T, OUTPUT);
    pinMode(kPinM3F, OUTPUT); pinMode(kPinM3T, OUTPUT);
    pinMode(kPinM4F, OUTPUT); pinMode(kPinM4T, OUTPUT);

    // Pinos 34 a 39 são somente entrada e não possuem pull-up interno no silício do ESP32.
    pinMode(kPinEncM1A, INPUT); pinMode(kPinEncM1B, INPUT);
    pinMode(kPinEncM2A, INPUT); pinMode(kPinEncM2B, INPUT);
    pinMode(kPinEncM3A, INPUT_PULLUP); pinMode(kPinEncM3B, INPUT_PULLUP);
    pinMode(kPinEncM4A, INPUT_PULLUP); pinMode(kPinEncM4B, INPUT_PULLUP);

    estM1 = (digitalRead(kPinEncM1A) << 1) | digitalRead(kPinEncM1B);
    estM2 = (digitalRead(kPinEncM2A) << 1) | digitalRead(kPinEncM2B);
    estM3 = (digitalRead(kPinEncM3A) << 1) | digitalRead(kPinEncM3B);
    estM4 = (digitalRead(kPinEncM4A) << 1) | digitalRead(kPinEncM4B);

    attachInterrupt(digitalPinToInterrupt(kPinEncM1A), isrM1, CHANGE);
    attachInterrupt(digitalPinToInterrupt(kPinEncM1B), isrM1, CHANGE);
    attachInterrupt(digitalPinToInterrupt(kPinEncM2A), isrM2, CHANGE);
    attachInterrupt(digitalPinToInterrupt(kPinEncM2B), isrM2, CHANGE);
    attachInterrupt(digitalPinToInterrupt(kPinEncM3A), isrM3, CHANGE);
    attachInterrupt(digitalPinToInterrupt(kPinEncM3B), isrM3, CHANGE);
    attachInterrupt(digitalPinToInterrupt(kPinEncM4A), isrM4, CHANGE);
    attachInterrupt(digitalPinToInterrupt(kPinEncM4B), isrM4, CHANGE);
}

void BaseSetup::inicializarRedeEInterface(BaseFuncoes& funcoes, BaseCenas& cenas) {
    // Evita desgaste da Flash e previne conflito com credenciais corrompidas na partição NVS.
    WiFi.persistent(false);
    WiFi.disconnect(true);
    delay(100);
    WiFi.mode(WIFI_AP);

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        if (event == ARDUINO_EVENT_WIFI_AP_STACONNECTED) {
            Serial.println("[WIFI] Dispositivo autenticado com sucesso!");
        } else if (event == ARDUINO_EVENT_WIFI_AP_STADISCONNECTED) {
            Serial.printf("[WIFI] Dispositivo desconectado (AID: %d)\n", info.wifi_ap_stadisconnected.aid);
        }
    });

    // Canal 6 reduz colisão com canais adjacentes e 15 dBm evita picos de corrente no handshake WPA2.
    WiFi.softAP(kWifiSsid, kWifiPass, 6, 0, 4);
    WiFi.setTxPower(WIFI_POWER_15dBm);
    // Desativa o modem-sleep para evitar picos de consumo no rádio que geram falso disparo nos pinos 36 e 39.
    WiFi.setSleep(false);

    ws.onEvent([&funcoes, &cenas](AsyncWebSocket* srv, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
        if (type != WS_EVT_DATA) return;
        String cmd;
        cmd.reserve(len);
        for (size_t i = 0; i < len; i++) cmd += static_cast<char>(data[i]);

        if (cmd.startsWith("cena_")) {
            cenas.iniciarCena(cmd.substring(5).toInt());
        } else {
            funcoes.processarComandoManual(cmd);
        }
    });

    server.addHandler(&ws);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send_P(200, "text/html", kIndexHtml);
    });
    server.begin();
    Serial.println("[WIFI] AP ChapeleiroV2 iniciado no canal 6.");
}

void BaseSetup::inicializarBleSpike() {
    BLEDevice::init("");
    BLEScan* scanner = BLEDevice::getScan();
    // Aceitar duplicatas é obrigatório porque o hub reutiliza o mesmo endereço MAC em cada frame de telemetria.
    scanner->setAdvertisedDeviceCallbacks(new SpikeBleCallback(), true);
    // Varredura passiva evita envio de SCAN_REQ, economizando tempo de antena para o SoftAP Wi-Fi.
    scanner->setActiveScan(false);
    // Ciclo curto (45/80 ms) reduz o intervalo cego máximo para 35 ms sem derrubar os beacons do Wi-Fi.
    scanner->setInterval(80);
    scanner->setWindow(45);
    scanner->start(0, nullptr, false);
}

void BaseSetup::atualizarRede(const BaseFuncoes& funcoes) {
    ws.cleanupClients();
    // Atualiza a telemetria a 20 Hz para manter a resposta visual do giroscópio fluida sem saturar o TCP.
    if (millis() - ultimoEnvioTelemetriaMs_ < 50) return;
    ultimoEnvioTelemetriaMs_ = millis();

    noInterrupts();
    long c1 = ticksM1, c2 = ticksM2, c3 = ticksM3, c4 = ticksM4;
    interrupts();

    String pacote = "DAT:" + String(c1) + "," + String(c2) + "," + String(c3) + "," + String(c4) + "," +
                    String(funcoes.getVelocidadeManual()) + "," +
                    String(funcoes.getTicksOmbroEsq()) + "," + String(funcoes.getTicksOmbroDir()) + "," +
                    String(funcoes.getGyroRelativo(), 1);
    ws.textAll(pacote);
}