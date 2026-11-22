#include "baseSetup.h"
#include "baseFuncoes.h"
#include "baseCenas.h"
#include "config.h"
#include "webPage.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

volatile long ticksM1 = 0;
volatile long ticksM2 = 0;
volatile long ticksM3 = 0;
volatile long ticksM4 = 0;
volatile float anguloSpikeGraus = 0.0f;

namespace {
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

BaseFuncoes* sFuncoesPtr = nullptr;
BaseCenas* sCenasPtr = nullptr;
unsigned long sUltimoEnvioWsMs = 0;

// Tabela de quadratura em código Gray para filtrar transições inválidas por ruído.
constexpr int8_t kTabelaQuadBase[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
volatile uint8_t sEstM1 = 0, sEstM2 = 0, sEstM3 = 0, sEstM4 = 0;

void IRAM_ATTR atualizarQuadBase(uint8_t pinA, uint8_t pinB, volatile uint8_t& est, volatile long& ticks, int dir) {
    uint8_t atual = (digitalRead(pinA) << 1) | digitalRead(pinB);
    ticks += kTabelaQuadBase[(est << 2) | atual] * dir;
    est = atual;
}

void IRAM_ATTR isrM1() { atualizarQuadBase(kPinEncM1A, kPinEncM1B, sEstM1, ticksM1, kDirEncM1); }
void IRAM_ATTR isrM2() { atualizarQuadBase(kPinEncM2A, kPinEncM2B, sEstM2, ticksM2, kDirEncM2); }
void IRAM_ATTR isrM3() { atualizarQuadBase(kPinEncM3A, kPinEncM3B, sEstM3, ticksM3, kDirEncM3); }
void IRAM_ATTR isrM4() { atualizarQuadBase(kPinEncM4A, kPinEncM4B, sEstM4, ticksM4, kDirEncM4); }

void configurarPinoEncoder(uint8_t pin) {
    // Pinos 34 a 39 do ESP32 são somente entrada e não possuem pull-up interno.
    pinMode(pin, (pin >= 34) ? INPUT : INPUT_PULLUP);
}

void onWsEvent(AsyncWebSocket* servidor, AsyncWebSocketClient* cliente, AwsEventType tipo,
               void* arg, uint8_t* data, size_t len) {
    if (tipo == WS_EVT_DISCONNECT) {
        if (sFuncoesPtr) sFuncoesPtr->parar();
        return;
    }
    if (tipo != WS_EVT_DATA) return;

    AwsFrameInfo* info = reinterpret_cast<AwsFrameInfo*>(arg);
    if (!info->final || info->index != 0 || info->len != len || info->opcode != WS_TEXT) return;

    String cmd;
    cmd.reserve(len + 1);
    for (size_t i = 0; i < len; i++) {
        cmd += static_cast<char>(data[i]);
    }
    cmd.trim();
    if (cmd.length() == 0) return;

    if (cmd == "cena_1") {
        if (sCenasPtr) sCenasPtr->iniciarCena(1);
    } else if (sFuncoesPtr) {
        sFuncoesPtr->processarComandoManual(cmd);
    }
}
}  // namespace

void BaseSetup::inicializarHardware() {
    Serial2.begin(kBaudSerial2, SERIAL_8N1, kPinRxSerial2, kPinTxSerial2);

    const uint8_t pinosMotores[] = {
        kPinM1F, kPinM1T, kPinM2F, kPinM2T,
        kPinM3F, kPinM3T, kPinM4F, kPinM4T
    };
    for (uint8_t pin : pinosMotores) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }

    configurarPinoEncoder(kPinEncM1A); configurarPinoEncoder(kPinEncM1B);
    configurarPinoEncoder(kPinEncM2A); configurarPinoEncoder(kPinEncM2B);
    configurarPinoEncoder(kPinEncM3A); configurarPinoEncoder(kPinEncM3B);
    configurarPinoEncoder(kPinEncM4A); configurarPinoEncoder(kPinEncM4B);

    sEstM1 = (digitalRead(kPinEncM1A) << 1) | digitalRead(kPinEncM1B);
    sEstM2 = (digitalRead(kPinEncM2A) << 1) | digitalRead(kPinEncM2B);
    sEstM3 = (digitalRead(kPinEncM3A) << 1) | digitalRead(kPinEncM3B);
    sEstM4 = (digitalRead(kPinEncM4A) << 1) | digitalRead(kPinEncM4B);

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
    sFuncoesPtr = &funcoes;
    sCenasPtr = &cenas;

    WiFi.mode(WIFI_AP);
    WiFi.softAP(kWifiSsid, kWifiPass);

    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send_P(200, "text/html", kIndexHtml);
    });

    server.begin();
    Serial.print("Painel ativo em: http://");
    Serial.println(WiFi.softAPIP());
}

void BaseSetup::atualizarRede(const BaseFuncoes& funcoes) {
    ws.cleanupClients();
    if (ws.count() == 0) return;

    unsigned long agora = millis();
    if (agora - sUltimoEnvioWsMs < 50) return;
    sUltimoEnvioWsMs = agora;

    noInterrupts();
    long m1 = ticksM1, m2 = ticksM2, m3 = ticksM3, m4 = ticksM4;
    interrupts();

    char payload[96];
    snprintf(payload, sizeof(payload), "DAT:%ld,%ld,%ld,%ld,%d,%ld,%ld,%.1f",
             m1, m2, m3, m4,
             funcoes.isMovendo() ? 1 : 0,
             funcoes.getTicksOmbroEsq(),
             funcoes.getTicksOmbroDir(),
             funcoes.getGyroRelativo());

    ws.textAll(payload);
}