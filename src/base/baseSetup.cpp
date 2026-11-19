#include "baseSetup.h"
#include "baseFuncoes.h"
#include "baseCenas.h"
#include "config.h"
#include "webPage.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "esp_bt.h"

// Usa volatile para impedir cache em registrador, já que os valores mudam em ISR ou via UART.
volatile long ticksM1 = 0, ticksM2 = 0, ticksM3 = 0, ticksM4 = 0;
volatile float anguloSpikeGraus = 0.0f;

// Instâncias estáticas globais para que vivam por toda a execução sem alocação dinâmica no heap.
static AsyncWebServer server(80);
static AsyncWebSocket ws("/ws");

// Mutex e fila circular fixa para transferir comandos da task AsyncTCP para o loop() sem race condition.
static portMUX_TYPE muxFilaCmd = portMUX_INITIALIZER_UNLOCKED;
constexpr size_t kTamFilaCmd = 8;
static char filaCmds[kTamFilaCmd][32];
static volatile uint8_t filaHead = 0;
static volatile uint8_t filaTail = 0;

// Ponteiros sem posse (non-owning): apontam para os objetos globais de main.cpp que nunca são destruídos.
static BaseFuncoes* ptrFuncoes = nullptr;
static BaseCenas* ptrCenas = nullptr;

// Tabela em código Gray para rejeitar transições simultâneas impossíveis causadas por ruído elétrico.
constexpr int8_t kTabelaQuad[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
// Usa volatile porque estado e timestamp são lidos e alterados dentro das interrupções de hardware.
static volatile uint8_t estM1 = 0, estM2 = 0, estM3 = 0, estM4 = 0;
static volatile unsigned long ultMsM1 = 0, ultMsM2 = 0, ultMsM3 = 0, ultMsM4 = 0;

// Mantém a função na RAM interna (IRAM_ATTR) para não causar crash quando a Flash estiver ocupada com Wi-Fi.
void IRAM_ATTR atualizarQuadratura(uint8_t pinA, uint8_t pinB, volatile uint8_t& est, volatile unsigned long& ult, volatile long& ticks, int dir) {
    unsigned long agora = micros();
    // Ignora pulsos menores que 50 µs porque picos induzidos pelo chaveamento PWM da BTS7960 são mais curtos que isso.
    if (agora - ult < 50) return;

    uint8_t atual = (digitalRead(pinA) << 1) | digitalRead(pinB);
    ticks += kTabelaQuad[(est << 2) | atual] * dir;
    est = atual;
    ult = agora;
}

// Exige IRAM_ATTR porque o ESP32 dispara interrupções de GPIO mesmo durante operações de escrita na Flash.
void IRAM_ATTR isrM1() { atualizarQuadratura(kPinEncM1A, kPinEncM1B, estM1, ultMsM1, ticksM1, kDirEncM1); }
void IRAM_ATTR isrM2() { atualizarQuadratura(kPinEncM2A, kPinEncM2B, estM2, ultMsM2, ticksM2, kDirEncM2); }
void IRAM_ATTR isrM3() { atualizarQuadratura(kPinEncM3A, kPinEncM3B, estM3, ultMsM3, ticksM3, kDirEncM3); }
void IRAM_ATTR isrM4() { atualizarQuadratura(kPinEncM4A, kPinEncM4B, estM4, ultMsM4, ticksM4, kDirEncM4); }

void BaseSetup::inicializarHardware() {
    Serial2.begin(kBaudSerial2, SERIAL_8N1, kPinRxSerial2, kPinTxSerial2);
    // Reduz o timeout de 1000 ms para 5 ms a fim de evitar que pacotes incompletos congelem o loop() e derrubem o Wi-Fi.
    Serial2.setTimeout(5);

    pinMode(kPinM1F, OUTPUT); pinMode(kPinM1T, OUTPUT);
    pinMode(kPinM2F, OUTPUT); pinMode(kPinM2T, OUTPUT);
    pinMode(kPinM3F, OUTPUT); pinMode(kPinM3T, OUTPUT);
    pinMode(kPinM4F, OUTPUT); pinMode(kPinM4T, OUTPUT);

    // Usa INPUT simples nos pinos 34 a 39 porque esses GPIOs são somente entrada e não possuem pull-up interno no silício.
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
    ptrFuncoes = &funcoes;
    ptrCenas = &cenas;

    // Desliga o controlador Bluetooth e libera ~70 KB de RAM porque o BLE roda apenas na ESP32 dos braços.
    btStop();
    esp_bt_controller_mem_release(ESP_BT_MODE_BTDM);

    // Desabilita persistência e apaga credenciais da NVS para impedir que o modo STA varra canais e faça o AP sumir.
    WiFi.persistent(false);
    WiFi.disconnect(true, true);
    WiFi.mode(WIFI_OFF);
    delay(100);
    WiFi.mode(WIFI_AP);

    // Fixa o canal 6 com potência de 15 dBm para reduzir interferência e evitar aquecimento do estágio de RF.
    WiFi.softAP(kWifiSsid, kWifiPass, 6, 0, 4);
    WiFi.setTxPower(WIFI_POWER_15dBm);
    // Desativa o modem-sleep porque a economia de energia derruba WebSockets e gera picos de ruído nos pinos 36 e 39.
    WiFi.setSleep(false);

    // Ponteiros srv, client, arg e data pertencem ao AsyncWebServer e só são válidos durante a execução deste callback.
    ws.onEvent([](AsyncWebSocket* srv, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
        // Descarta pacotes inválidos ou maiores que o buffer fixo para evitar estouro de memória.
        if (type != WS_EVT_DATA || len == 0 || len >= 32) return;

        // Enfileira em seção crítica porque este callback roda na task AsyncTCP em paralelo com o loop().
        portENTER_CRITICAL(&muxFilaCmd);
        uint8_t proximo = (filaHead + 1) % kTamFilaCmd;
        if (proximo != filaTail) {
            memcpy(filaCmds[filaHead], data, len);
            filaCmds[filaHead][len] = '\0';
            filaHead = proximo;
        }
        portEXIT_CRITICAL(&muxFilaCmd);
    });

    server.addHandler(&ws);
    // O ponteiro req pertence ao servidor assíncrono e é liberado internamente logo após o envio da resposta.
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
        // Usa send_P para transmitir o HTML direto da Flash (PROGMEM) sem copiar 24 KB para a SRAM.
        req->send_P(200, "text/html", kIndexHtml);
    });
    server.begin();
}

void BaseSetup::atualizarRede(const BaseFuncoes& funcoes) {
    // Consome a fila dentro do loop() principal para evitar acesso concorrente a objetos String e Serial2.
    while (filaTail != filaHead) {
        char cmdBuf[32];
        portENTER_CRITICAL(&muxFilaCmd);
        strcpy(cmdBuf, filaCmds[filaTail]);
        filaTail = (filaTail + 1) % kTamFilaCmd;
        portEXIT_CRITICAL(&muxFilaCmd);

        String cmd(cmdBuf);
        if (cmd.startsWith("cena_") && ptrCenas != nullptr) {
            ptrCenas->iniciarCena(cmd.substring(5).toInt());
        } else if (ptrFuncoes != nullptr) {
            ptrFuncoes->processarComandoManual(cmd);
        }
    }

    // Libera descritores de clientes desconectados para não esgotar o limite de sockets do servidor.
    ws.cleanupClients();
    // Aborta a montagem de telemetria quando não há navegadores conectados para poupar CPU e buffers TCP.
    if (ws.count() == 0) return;

    // Limita o envio a 12,5 Hz (80 ms) para manter o painel responsivo sem saturar a fila de pacotes do Wi-Fi.
    if (millis() - ultimoEnvioTelemetriaMs_ < 80) return;
    ultimoEnvioTelemetriaMs_ = millis();

    // Desabilita interrupções brevemente para copiar os contadores volatile de 32 bits de forma atômica.
    noInterrupts();
    long c1 = ticksM1, c2 = ticksM2, c3 = ticksM3, c4 = ticksM4;
    interrupts();

    // Formata em buffer estático na stack em vez de concatenar Strings para evitar fragmentação da Heap.
    char pacote[96];
    snprintf(pacote, sizeof(pacote), "DAT:%ld,%ld,%ld,%ld,%d,%ld,%ld,%.1f",
             c1, c2, c3, c4,
             funcoes.getVelocidadeManual(),
             funcoes.getTicksOmbroEsq(),
             funcoes.getTicksOmbroDir(),
             funcoes.getGyroRelativo());
    ws.textAll(pacote);
}