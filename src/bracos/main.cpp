#include <Arduino.h>
#include "bracosSetup.h"
#include "bracosFuncoes.h"
#include "bracosCenas.h"

BracosSetup setupBracos;
BracosFuncoes funcoesBracos;
BracosCenas cenasBracos(funcoesBracos);

void setup() {
    Serial.begin(115200);
    setupBracos.inicializarHardware();
    funcoesBracos.inicializarServos();
    setupBracos.inicializarBleSpike();
}

void loop() {
    funcoesBracos.processarSerial2(cenasBracos);
    cenasBracos.atualizarCenas();
    funcoesBracos.atualizarCinematica();
    funcoesBracos.enviarTelemetriaSerial2();
}