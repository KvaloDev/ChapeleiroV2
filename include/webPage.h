// Evita inclusão múltipla deste cabeçalho na mesma unidade de compilação para prevenir erros de redefinição.
#pragma once
#include <Arduino.h>

// Armazena o HTML na Flash (PROGMEM) para não consumir a SRAM necessária para a pilha TCP/IP do Wi-Fi.
const char kIndexHtml[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
  <title>Painel Chapeleiro - Equipe Tucuna</title>
  <style>
    :root { --bg: #050A15; --card: #0B1536; --btn: #16265C; --yellow: #FFCC00; --text: #FFF; --green: #00E676; --red: #E53935; }
    body { font-family: 'Segoe UI', sans-serif; text-align: center; background: var(--bg); color: var(--text); margin: 0; padding: 10px; user-select: none; }
    h1 { color: var(--yellow); margin: 4px 0 10px; font-style: italic; font-weight: 900; font-size: 22px; }
    h3 { color: var(--yellow); margin: 0 0 8px; font-size: 13px; text-transform: uppercase; letter-spacing: 0.5px; }
    .box { background: var(--card); border: 2px solid var(--yellow); border-radius: 8px; padding: 10px; margin: 8px auto; max-width: 680px; }
    .grid-2 { display: grid; grid-template-columns: 1fr 1fr; gap: 8px; max-width: 680px; margin: 8px auto; }
    .grid-3 { display: grid; grid-template-columns: repeat(3, 1fr); gap: 6px; max-width: 340px; margin: 10px auto; }
    .grid-4 { display: grid; grid-template-columns: repeat(4, 1fr); gap: 6px; margin-top: 6px; }
    button { background: var(--btn); color: var(--text); border: 2px solid var(--btn); padding: 11px 8px; font-weight: bold; border-radius: 6px; cursor: pointer; touch-action: none; font-size: 13px; }
    button:active, button.tecla-ativa { background: var(--yellow); color: var(--bg); border-color: var(--yellow); }
    button:disabled { opacity: 0.4; cursor: not-allowed; }
    .turn { border-color: var(--yellow); color: var(--yellow); }
    .telem { font-family: monospace; color: var(--yellow); font-size: 16px; font-weight: bold; }
    .gyro-big { font-size: 24px; color: var(--green); }
    input[type=number] { width: 64px; padding: 6px; background: var(--bg); color: var(--yellow); border: 1px solid var(--yellow); border-radius: 4px; text-align: center; font-weight: bold; font-size: 14px; }
    .preset-card { background: var(--bg); border: 1px solid var(--btn); border-radius: 6px; padding: 6px; }
    .preset-card.ativo { border: 2px solid var(--yellow); }
    .preset-card input { width: 54px; padding: 4px; margin-top: 4px; }
    .rec-box { width: 100%; height: 155px; background: #02050A; color: #00FF66; font-family: monospace; font-size: 12px; border: 1px solid var(--btn); padding: 8px; box-sizing: border-box; }
    .btn-rec { background: var(--red); border-color: var(--red); }
    .btn-rec.ativo { background: #555; border-color: #555; }
    .btn-cont { background: #0277BD; border-color: #29B6F6; color: #FFF; }
    .btn-overdub { background: #E65100; border-color: #FF9800; color: #FFF; }
    .btn-play { background: #1B5E20; border-color: var(--green); color: var(--green); }
    .medida-bar { display: flex; flex-wrap: wrap; gap: 6px; justify-content: center; margin-top: 8px; min-height: 28px; }
    .chip-medida { background: var(--bg); border: 1px solid var(--green); color: var(--green); padding: 4px 10px; border-radius: 14px; font-family: monospace; font-size: 13px; cursor: pointer; }
    .rampa-row { display: flex; gap: 8px; justify-content: center; align-items: center; font-size: 12px; color: #CCC; }
    .rampa-row input { width: 48px; padding: 4px; font-size: 12px; }
    .gamepad-status { font-size: 12px; margin-top: 8px; padding: 4px 8px; border-radius: 4px; background: var(--bg); color: #AAA; border: 1px solid var(--btn); }
    .gamepad-status.conectado { color: var(--green); border-color: var(--green); }
    .gamepad-status.digitando { color: var(--yellow); border-color: var(--yellow); background: #261E00; font-weight: bold; }
    .rec-info { font-size: 12px; color: #CCC; margin-top: 6px; font-family: monospace; }
  </style>
</head>
<body oncontextmenu="return false;">
  <h1>CHAPELEIRO V2 | TUCUNA</h1>

  <div class="grid-2">
    <div class="box" style="margin:0;">
      <h3>Giroscópio SPIKE (BLE)</h3>
      <span class="telem gyro-big" id="gyroDisp">0.0</span><span class="telem gyro-big">°</span>
      <br><br>
      <div style="display:flex; gap:5px; justify-content:center; flex-wrap:wrap;">
        <button onclick="sendCmd('zero_gyro')" style="border-color:var(--green); color:var(--green);">ZERAR (0°)</button>
        <button id="btnFod" onclick="toggleFod()" style="background:#1B5E20; border-color:var(--green); color:var(--green);">🧭 CAMPO: ON</button>
        <button onclick="sendCmd('cena_1')" style="border-color:var(--yellow);">CENA 1</button>
      </div>
      <div id="gamepadStatus" class="gamepad-status">🎮 Controle: Desconectado (aperte um botão)</div>
    </div>

    <div class="box" style="margin:0;">
      <h3>Força da Base e Rampa (PWM/100ms)</h3>
      <button onclick="alterarForcaBase(-10)">-10</button>
      <input type="number" id="forcaBase" value="120" min="0" max="255" onchange="aplicarForcaBase(this.value)">
      <button onclick="alterarForcaBase(10)">+10</button>
      <div class="rampa-row" style="margin-top:8px;">
        <span>Acel: <input type="number" id="passoAcel" value="8" min="1" max="255" oninput="aplicarRampa()" onkeydown="if(event.key==='Enter')this.blur()"></span>
        <span>Freio: <input type="number" id="passoFreio" value="15" min="1" max="255" oninput="aplicarRampa()" onkeydown="if(event.key==='Enter')this.blur()"></span>
      </div>
    </div>
  </div>

  <div class="box">
    <h3>Velocidades Rápidas (Segure ou aperte 1, 2, 3 ou 4)</h3>
    <div class="grid-4">
      <div class="preset-card" id="cardVel1" onclick="selecionarPreset(1)">
        <div><b>[1]</b> Lenta</div>
        <input type="number" id="preset1" value="55" min="0" max="255" onchange="atualizarPreset(1)">
      </div>
      <div class="preset-card ativo" id="cardVel2" onclick="selecionarPreset(2)">
        <div><b>[2]</b> Média</div>
        <input type="number" id="preset2" value="95" min="0" max="255" onchange="atualizarPreset(2)">
      </div>
      <div class="preset-card" id="cardVel3" onclick="selecionarPreset(3)">
        <div><b>[3]</b> Rápida</div>
        <input type="number" id="preset3" value="145" min="0" max="255" onchange="atualizarPreset(3)">
      </div>
      <div class="preset-card" id="cardVel4" onclick="selecionarPreset(4)">
        <div><b>[4]</b> Turbo</div>
        <input type="number" id="preset4" value="200" min="0" max="255" onchange="atualizarPreset(4)">
      </div>
    </div>
  </div>

  <div class="grid-2">
    <div class="box" style="margin:0;">
      <h3>Giro Preciso (RB/LB + Teclado ou Y/B/A/X)</h3>
      <input type="number" id="anguloGiro" value="90" min="1" max="360" step="1"> °
      <button class="turn" onclick="executarGiroSeta(-1)">↶ ESQ (LB)</button>
      <button class="turn" onclick="executarGiroSeta(1)">DIR (RB) ↷</button>
      <div style="margin-top:8px; padding-top:8px; border-top:1px solid var(--btn);">
        <input type="number" id="anguloAbsoluto" value="0" min="-360" max="360" step="1"> °
        <button onclick="executarGiroAbsoluto()" style="border-color:var(--green); color:var(--green);">IR PARA X°</button>
        <button onclick="executarGiroAbsoluto(0)" style="background:#1B5E20; border-color:var(--green); color:var(--green);">Y: 0°</button>
      </div>
    </div>

    <div class="box" style="margin:0;">
      <h3>Régua Digital (Tecla M)</h3>
      <span class="telem" id="medidaAtualDisp">0.0 mm</span>
      <br><br>
      <button id="btnMedir" onclick="toggleMedirDistancia()">📏 MEDIR (M)</button>
      <button onclick="limparMedidas()">LIMPAR BARRA</button>
    </div>
  </div>

  <div class="box" style="padding:6px 10px;">
    <div style="font-size:11px; color:#AAA;">MEDIDAS SALVAS (clique para copiar):</div>
    <div class="medida-bar" id="barraMedidas"></div>
  </div>

  <div class="grid-3">
    <button id="btn_wa" onmousedown="pressionarBotaoDir('wa')" onmouseup="soltarBotaoDir('wa')" ontouchstart="event.preventDefault();pressionarBotaoDir('wa')" ontouchend="soltarBotaoDir('wa')" ontouchcancel="soltarBotaoDir('wa')">↖ W+A</button>
    <button id="btn_w" onmousedown="pressionarBotaoDir('w')" onmouseup="soltarBotaoDir('w')" ontouchstart="event.preventDefault();pressionarBotaoDir('w')" ontouchend="soltarBotaoDir('w')" ontouchcancel="soltarBotaoDir('w')">▲ W</button>
    <button id="btn_wd" onmousedown="pressionarBotaoDir('wd')" onmouseup="soltarBotaoDir('wd')" ontouchstart="event.preventDefault();pressionarBotaoDir('wd')" ontouchend="soltarBotaoDir('wd')" ontouchcancel="soltarBotaoDir('wd')">↗ W+D</button>

    <button id="btn_a" onmousedown="pressionarBotaoDir('a')" onmouseup="soltarBotaoDir('a')" ontouchstart="event.preventDefault();pressionarBotaoDir('a')" ontouchend="soltarBotaoDir('a')" ontouchcancel="soltarBotaoDir('a')">◄ A</button>
    <button onmousedown="pararTudoManual()" style="background:#333;">■ STOP</button>
    <button id="btn_d" onmousedown="pressionarBotaoDir('d')" onmouseup="soltarBotaoDir('d')" ontouchstart="event.preventDefault();pressionarBotaoDir('d')" ontouchend="soltarBotaoDir('d')" ontouchcancel="soltarBotaoDir('d')">► D</button>

    <button id="btn_sa" onmousedown="pressionarBotaoDir('sa')" onmouseup="soltarBotaoDir('sa')" ontouchstart="event.preventDefault();pressionarBotaoDir('sa')" ontouchend="soltarBotaoDir('sa')" ontouchcancel="soltarBotaoDir('sa')">↙ S+A</button>
    <button id="btn_s" onmousedown="pressionarBotaoDir('s')" onmouseup="soltarBotaoDir('s')" ontouchstart="event.preventDefault();pressionarBotaoDir('s')" ontouchend="soltarBotaoDir('s')" ontouchcancel="soltarBotaoDir('s')">▼ S</button>
    <button id="btn_sd" onmousedown="pressionarBotaoDir('sd')" onmouseup="soltarBotaoDir('sd')" ontouchstart="event.preventDefault();pressionarBotaoDir('sd')" ontouchend="soltarBotaoDir('sd')" ontouchcancel="soltarBotaoDir('sd')">↘ S+D</button>
  </div>
  <div style="max-width:340px; margin:0 auto 10px; display:grid; grid-template-columns:1fr 1fr; gap:6px;">
    <button id="btn_q" class="turn" onmousedown="pressionarBotaoDir('q')" onmouseup="soltarBotaoDir('q')" ontouchstart="event.preventDefault();pressionarBotaoDir('q')" ontouchend="soltarBotaoDir('q')" ontouchcancel="soltarBotaoDir('q')">↶ GIRAR Q</button>
    <button id="btn_e" class="turn" onmousedown="pressionarBotaoDir('e')" onmouseup="soltarBotaoDir('e')" ontouchstart="event.preventDefault();pressionarBotaoDir('e')" ontouchend="soltarBotaoDir('e')" ontouchcancel="soltarBotaoDir('e')">↷ GIRAR E</button>
  </div>

  <div class="box">
    <h3>Força dos Braços (Ombros PWM)</h3>
    <input type="number" id="forcaBraco" value="130" min="50" max="255" onchange="aplicarForcaBraco(this.value)">
    <button onclick="aplicarForcaBraco(document.getElementById('forcaBraco').value)">APLICAR</button>
  </div>

  <div class="grid-2">
    <div class="box" style="margin:0;">
      <div>OMBRO ESQ: <span class="telem" id="encBE">0</span></div><br>
      <button onmousedown="iniciarBraco('be_up')" onmouseup="pararBraco()" onmouseleave="pararBraco()" ontouchstart="event.preventDefault();iniciarBraco('be_up')" ontouchend="pararBraco()" ontouchcancel="pararBraco()">▲ SOBE</button>
      <button onmousedown="iniciarBraco('be_dwn')" onmouseup="pararBraco()" onmouseleave="pararBraco()" ontouchstart="event.preventDefault();iniciarBraco('be_dwn')" ontouchend="pararBraco()" ontouchcancel="pararBraco()">▼ DESCE</button>
    </div>
    <div class="box" style="margin:0;">
      <div>OMBRO DIR: <span class="telem" id="encBD">0</span></div><br>
      <button onmousedown="iniciarBraco('bd_up')" onmouseup="pararBraco()" onmouseleave="pararBraco()" ontouchstart="event.preventDefault();iniciarBraco('bd_up')" ontouchend="pararBraco()" ontouchcancel="pararBraco()">▲ SOBE</button>
      <button onmousedown="iniciarBraco('bd_dwn')" onmouseup="pararBraco()" onmouseleave="pararBraco()" ontouchstart="event.preventDefault();iniciarBraco('bd_dwn')" ontouchend="pararBraco()" ontouchcancel="pararBraco()">▼ DESCE</button>
    </div>
    <div class="box" style="margin:0;">
      <div>COTOVELO ESQ (0-120)</div><br>
      <input type="number" id="srvE" value="0" min="0" max="120">
      <button onclick="sendCmd('srv_pos_e:' + document.getElementById('srvE').value)">SET</button>
      <div style="display:grid; grid-template-columns:repeat(3,1fr); gap:4px; margin-top:6px;">
        <button onclick="document.getElementById('srvE').value=0; sendCmd('srv_pos_e:0')">0</button>
        <button onclick="document.getElementById('srvE').value=60; sendCmd('srv_pos_e:60')">60</button>
        <button onclick="document.getElementById('srvE').value=120; sendCmd('srv_pos_e:120')">120</button>
      </div>
    </div>
    <div class="box" style="margin:0;">
      <div>COTOVELO DIR (0-120)</div><br>
      <input type="number" id="srvD" value="0" min="0" max="120">
      <button onclick="sendCmd('srv_pos_d:' + document.getElementById('srvD').value)">SET</button>
      <div style="display:grid; grid-template-columns:repeat(3,1fr); gap:4px; margin-top:6px;">
        <button onclick="document.getElementById('srvD').value=0; sendCmd('srv_pos_d:0')">0</button>
        <button onclick="document.getElementById('srvD').value=60; sendCmd('srv_pos_d:60')">60</button>
        <button onclick="document.getElementById('srvD').value=120; sendCmd('srv_pos_d:120')">120</button>
      </div>
    </div>
  </div>

  <div class="box">
    ENCODERS: M1:<span class="telem" id="enc1">0</span> | M2:<span class="telem" id="enc2">0</span> | M3:<span class="telem" id="enc3">0</span> | M4:<span class="telem" id="enc4">0</span>
  </div>

  <div class="box" style="text-align:left;">
    <div style="display:flex; gap:6px; flex-wrap:wrap; align-items:center;">
      <button id="btnRec" class="btn-rec" onclick="toggleRec('novo')">🔴 GRAVAR DO ZERO</button>
      <button id="btnContinuar" class="btn-cont" onclick="toggleRec('continuar')" disabled>⏭ CONTINUAR GRAVANDO</button>
      <button id="btnOverdub" class="btn-overdub" onclick="toggleRec('overdub')" disabled>➕ GRAVAR POR CIMA</button>
      <button id="btnDesfazer" onclick="desfazerUltimaParte()" disabled>↩ DESFAZER PARTE</button>
      <button class="btn-play" onclick="testarRoteiroGravado()">▶ TESTAR GRAVAÇÃO</button>
      <button onclick="limparRoteiro()">LIMPAR</button>
      <label style="font-size:12px; color:#CCC; cursor:pointer;">
        <input type="checkbox" id="chkReplayOverdub" checked> Reproduzir ao sobrepor
      </label>
    </div>
    <div id="recInfo" class="rec-info">⏱ Duração gravada: 0.0s | Partes: 0 | Passos: 0</div>
    <br>
    <textarea id="codeOut" class="rec-box" readonly></textarea>
  </div>

  <script>
    const ws = new WebSocket(`ws://${window.location.hostname}/ws`);
    // Fatores calibrados em config.h para converter pulsos de encoder em milímetros e graus de ombro.
    const kTicksPorMm = 1.65;
    const kTicksPorGrauOmbro = 26.49;

    let ticks = [0, 0, 0, 0];
    let ticksOmbroEsq = 0;
    let ticksOmbroDir = 0;
    let gyroAtual = 0;
    let fodAtivo = true;

    let gravando = false, inicioRec = 0, m1Start = 0, m2Start = 0, gyroStart = 0, cmdAtivo = "";
    let movX = 0, movY = 0, movRot = 0, movPicoMag = 0;
    let bracoAtivo = "", ticksBracoStart = 0;

    // Identifica cada segmento contínuo para substituir giros manuais em malha aberta por comandos absolutos no replay.
    let contadorSegmentos = 0;
    let segAtualId = 0;
    let segInicioTempo = 0;

    // Rastreia partes sequenciais e camadas sobrepostas para permitir gravar em etapas e desfazer a última parte.
    let duracaoTotalGravada = 0;
    let parteAtualId = 0;
    let historicoPartes = [];
    let passosGravados = [];
    let eventosGravados = [];
    let timersReplay = [];

    let medindo = false;
    let ticksInicioMedicao = [0, 0, 0, 0];

    const teclasAtivas = {};
    const botoesAtivos = {};
    let cmdManualAtual = "stop";
    let intervaloBraco = null;
    let comandoBracoAtivo = "";

    // Estado do controle Logitech FTC (Gamepad API).
    let gpNomeAtivo = "";
    let gpX = 0, gpY = 0, gpRot = 0;
    let gpDpadEsqAnterior = false;
    let gpDpadDirAnterior = false;
    let gpBtnAAnterior = false, gpBtnBAnterior = false, gpBtnXAnterior = false, gpBtnYAnterior = false;
    let gpLbAnterior = false, gpRbAnterior = false;
    let gpModoGiroBumper = null; // "RB" (horário) ou "LB" (anti-horário) enquanto o gatilho estiver segurado.
    let gpBufferAngulo = "";
    let gpUltimoEnvioMs = 0;
    let gpUltimoX = 0, gpUltimoY = 0, gpUltimoRot = 0;

    ws.onopen = () => {
      aplicarForcaBase(document.getElementById("forcaBase").value);
      aplicarRampa();
      aplicarForcaBraco(document.getElementById("forcaBraco").value);
      sendCmd(`fod:${fodAtivo ? 1 : 0}`);
    };

    ws.onmessage = (e) => {
      if (!e.data.startsWith("DAT:")) return;
      const d = e.data.substring(4).split(",");
      ticks = [parseInt(d[0]) || 0, parseInt(d[1]) || 0, parseInt(d[2]) || 0, parseInt(d[3]) || 0];
      ticksOmbroEsq = parseInt(d[5]) || 0;
      ticksOmbroDir = parseInt(d[6]) || 0;
      gyroAtual = parseFloat(d[7]) || 0;

      document.getElementById("enc1").innerText = ticks[0];
      document.getElementById("enc2").innerText = ticks[1];
      document.getElementById("enc3").innerText = ticks[2];
      document.getElementById("enc4").innerText = ticks[3];
      document.getElementById("encBE").innerText = ticksOmbroEsq;
      document.getElementById("encBD").innerText = ticksOmbroDir;
      document.getElementById("gyroDisp").innerText = gyroAtual.toFixed(1);

      if (medindo) {
        const distMm = calcularDistanciaMediaMm();
        document.getElementById("medidaAtualDisp").innerText = distMm.toFixed(1) + " mm";
      }
    };

    function normalizarAngulo180(ang) {
      let a = ang % 360.0;
      if (a > 180.0) a -= 360.0;
      if (a <= -180.0) a += 360.0;
      // Normaliza -180° para +180° por convenção de leitura no código gerado.
      if (Math.abs(a + 180.0) < 0.05) a = 180.0;
      if (Math.abs(a) < 0.05) a = 0.0;
      return a.toFixed(1);
    }

    function aplicarZonaMorta(valor, zonaMorta = 0.10) {
      if (Math.abs(valor) < zonaMorta) return 0;
      // Reescalona o curso útil [zonaMorta..1.0] para [0.0..1.0] para manter controle fino em baixa velocidade.
      const sinal = Math.sign(valor);
      return sinal * ((Math.abs(valor) - zonaMorta) / (1.0 - zonaMorta));
    }

    function atualizarStatusGamepad(gp) {
      const el = document.getElementById("gamepadStatus");
      if (!el) return;

      if (gpModoGiroBumper) {
        const sent = (gpModoGiroBumper === "RB") ? "HORÁRIO ↷" : "ANTI-HORÁRIO ↶";
        const valTexto = gpBufferAngulo.length > 0 ? gpBufferAngulo : "_";
        el.innerText = `🎯 ${gpModoGiroBumper} (${sent}): digite graus -> ${valTexto}°`;
        el.classList.remove("conectado");
        el.classList.add("digitando");
        return;
      }

      el.classList.remove("digitando");
      if (gp || gpNomeAtivo) {
        if (gp) gpNomeAtivo = gp.id.substring(0, 28);
        el.innerText = `🎮 Controle: ${gpNomeAtivo}`;
        el.classList.add("conectado");
      } else {
        el.innerText = "🎮 Controle: Desconectado (aperte um botão)";
        el.classList.remove("conectado");
      }
    }

    window.addEventListener("gamepadconnected", (e) => atualizarStatusGamepad(e.gamepad));
    window.addEventListener("gamepaddisconnected", () => {
      gpNomeAtivo = "";
      gpX = gpY = gpRot = 0;
      gpDpadEsqAnterior = gpDpadDirAnterior = false;
      gpBtnAAnterior = gpBtnBAnterior = gpBtnXAnterior = gpBtnYAnterior = false;
      gpLbAnterior = gpRbAnterior = false;
      gpModoGiroBumper = null;
      gpBufferAngulo = "";
      atualizarStatusGamepad(null);
      sincronizarMovimentoManual();
    });

    function lerGamepadLoop() {
      const lista = navigator.getGamepads ? navigator.getGamepads() : [];
      let gp = null;
      for (let i = 0; i < lista.length; i++) {
        if (lista[i] && lista[i].connected) { gp = lista[i]; break; }
      }

      if (gp) {
        if (!gpNomeAtivo) atualizarStatusGamepad(gp);

        // Analógico esquerdo controla translação (eixo Y invertido no padrão Gamepad API: cima = -1).
        let lx = aplicarZonaMorta(gp.axes[0] || 0);
        let ly = aplicarZonaMorta(-(gp.axes[1] || 0));
        const magL = Math.hypot(lx, ly);
        if (magL > 1.0) {
          lx /= magL;
          ly /= magL;
        }

        // Analógico direito (eixo horizontal) controla rotação no próprio eixo simultaneamente.
        const rx = aplicarZonaMorta(gp.axes[2] || 0);

        gpX = lx;
        gpY = ly;
        gpRot = rx;

        // Botões A (0), B (1), X (2), Y (3): alinhamento absoluto rápido pelo giroscópio.
        const btnA = !!(gp.buttons[0] && gp.buttons[0].pressed);
        const btnB = !!(gp.buttons[1] && gp.buttons[1].pressed);
        const btnX = !!(gp.buttons[2] && gp.buttons[2].pressed);
        const btnY = !!(gp.buttons[3] && gp.buttons[3].pressed);

        if (btnY && !gpBtnYAnterior) executarGiroAbsoluto(0);
        if (btnB && !gpBtnBAnterior) executarGiroAbsoluto(90);
        if (btnA && !gpBtnAAnterior) executarGiroAbsoluto(180);
        if (btnX && !gpBtnXAnterior) executarGiroAbsoluto(-90);

        gpBtnAAnterior = btnA;
        gpBtnBAnterior = btnB;
        gpBtnXAnterior = btnX;
        gpBtnYAnterior = btnY;

        // Botões LB (4) e RB (5): segure, digite o ângulo no teclado do notebook e solte para girar.
        const btnLb = !!(gp.buttons[4] && gp.buttons[4].pressed);
        const btnRb = !!(gp.buttons[5] && gp.buttons[5].pressed);

        if (btnRb && !gpRbAnterior && !gpModoGiroBumper) {
          gpModoGiroBumper = "RB";
          gpBufferAngulo = "";
          atualizarStatusGamepad(gp);
        } else if (!btnRb && gpRbAnterior && gpModoGiroBumper === "RB") {
          finalizarGiroBumper(1, gp);
        }

        if (btnLb && !gpLbAnterior && !gpModoGiroBumper) {
          gpModoGiroBumper = "LB";
          gpBufferAngulo = "";
          atualizarStatusGamepad(gp);
        } else if (!btnLb && gpLbAnterior && gpModoGiroBumper === "LB") {
          finalizarGiroBumper(-1, gp);
        }

        gpLbAnterior = btnLb;
        gpRbAnterior = btnRb;

        // D-Pad esquerda (14) e direita (15): giro relativo acumulado sobre o alvo absoluto atual.
        let dpadEsq = !!(gp.buttons[14] && gp.buttons[14].pressed);
        let dpadDir = !!(gp.buttons[15] && gp.buttons[15].pressed);

        // Fallback para POV hat (axes[9]) caso a chave traseira do Logitech F310 esteja em modo DirectInput (D).
        if (!dpadEsq && !dpadDir && gp.axes.length > 9) {
          const pov = gp.axes[9];
          if (pov >= 0.65 && pov <= 0.80) dpadEsq = true;
          if (pov >= -0.50 && pov <= -0.35) dpadDir = true;
        }

        if (dpadEsq && !gpDpadEsqAnterior) executarGiroSeta(-1);
        if (dpadDir && !gpDpadDirAnterior) executarGiroSeta(1);
        gpDpadEsqAnterior = dpadEsq;
        gpDpadDirAnterior = dpadDir;

        const agora = performance.now();
        const mudou = Math.abs(gpX - gpUltimoX) >= 0.03 ||
                      Math.abs(gpY - gpUltimoY) >= 0.03 ||
                      Math.abs(gpRot - gpUltimoRot) >= 0.03 ||
                      ((gpX === 0 && gpY === 0 && gpRot === 0) && (gpUltimoX !== 0 || gpUltimoY !== 0 || gpUltimoRot !== 0));

        // Limita envio contínuo a 40 Hz (25 ms) para não enfileirar pacotes na pilha WebSocket da ESP32.
        if (mudou && (agora - gpUltimoEnvioMs >= 25)) {
          gpUltimoX = gpX;
          gpUltimoY = gpY;
          gpUltimoRot = gpRot;
          gpUltimoEnvioMs = agora;
          sincronizarMovimentoManual();
        }
      }

      requestAnimationFrame(lerGamepadLoop);
    }
    requestAnimationFrame(lerGamepadLoop);

    function finalizarGiroBumper(direcao, gp) {
      const graus = parseFloat(gpBufferAngulo);
      gpModoGiroBumper = null;
      gpBufferAngulo = "";
      atualizarStatusGamepad(gp);

      if (!isNaN(graus) && graus > 0) {
        document.getElementById("anguloGiro").value = graus;
        executarGiroSeta(direcao);
      }
    }

    function fecharSegmentoBraco(tempo) {
      if (!bracoAtivo) return;
      const ticksFim = (bracoAtivo === "be") ? ticksOmbroEsq : ticksOmbroDir;
      // Ignora microvariações menores que 5 ticks (~0,2°) para evitar passos falsos por ruído mecânico.
      if (Math.abs(ticksFim - ticksBracoStart) > 5) {
        // Limita ao intervalo físico [0°, 90°] de config.h para evitar colisão estrutural na reprodução.
        const graus = Math.max(0.0, Math.min(90.0, ticksFim / kTicksPorGrauOmbro)).toFixed(1);
        const velBraco = parseInt(document.getElementById("forcaBraco").value) || 130;

        if (bracoAtivo === "be") {
          registrarPasso(tempo, `funcoes_.moverOmbroEsq(${graus}f, ${velBraco});`);
          eventosGravados.push({ tempo, tipo: "raw", cmd: `OMB_POS_E:${graus},${velBraco}`, parte: parteAtualId });
        } else {
          registrarPasso(tempo, `funcoes_.moverOmbroDir(${graus}f, ${velBraco});`);
          eventosGravados.push({ tempo, tipo: "raw", cmd: `OMB_POS_D:${graus},${velBraco}`, parte: parteAtualId });
        }
      }
      bracoAtivo = "";
    }

    function iniciarBraco(cmd) {
      pararBraco();
      comandoBracoAtivo = cmd;
      if (gravando) {
        bracoAtivo = cmd.startsWith("be_") ? "be" : "bd";
        ticksBracoStart = (bracoAtivo === "be") ? ticksOmbroEsq : ticksOmbroDir;
      }
      sendCmd(cmd);
      // Reenvia o comando a cada 150 ms porque o firmware dos braços possui watchdog que corta o motor se o sinal parar.
      intervaloBraco = setInterval(() => sendCmd(cmd), 150);
    }

    function pararBraco() {
      comandoBracoAtivo = "";
      if (intervaloBraco) {
        clearInterval(intervaloBraco);
        intervaloBraco = null;
      }
      if (gravando && bracoAtivo !== "") {
        fecharSegmentoBraco(Date.now() - inicioRec);
      }
      sendCmd("stop");
    }

    window.addEventListener("mouseup", () => {
      if (intervaloBraco) pararBraco();
      if (Object.keys(botoesAtivos).length > 0) {
        for (const k in botoesAtivos) delete botoesAtivos[k];
        sincronizarMovimentoManual();
      }
    });

    window.addEventListener("touchend", () => { if (intervaloBraco) pararBraco(); });

    // Zera teclas e botões presos caso a aba perca o foco para evitar que o robô continue andando sozinho.
    window.addEventListener("blur", () => {
      for (const k in teclasAtivas) delete teclasAtivas[k];
      for (const k in botoesAtivos) delete botoesAtivos[k];
      gpX = gpY = gpRot = 0;
      gpModoGiroBumper = null;
      gpBufferAngulo = "";
      atualizarStatusGamepad(null);
      sincronizarMovimentoManual();
    });

    function toggleFod() {
      fodAtivo = !fodAtivo;
      const btn = document.getElementById("btnFod");
      btn.innerText = fodAtivo ? "🧭 CAMPO: ON" : "🤖 ROBÔ: OFF";
      btn.style.background = fodAtivo ? "#1B5E20" : "";
      btn.style.borderColor = fodAtivo ? "#00E676" : "";
      btn.style.color = fodAtivo ? "#00E676" : "";
      sendCmd(`fod:${fodAtivo ? 1 : 0}`);
    }

    function calcularDistanciaMediaMm() {
      let somaTicks = 0;
      for (let i = 0; i < 4; i++) {
        somaTicks += Math.abs(ticks[i] - ticksInicioMedicao[i]);
      }
      return (somaTicks / 4.0) / kTicksPorMm;
    }

    function toggleMedirDistancia() {
      const btn = document.getElementById("btnMedir");
      if (!medindo) {
        medindo = true;
        ticksInicioMedicao = [...ticks];
        btn.innerText = "⏹ PARAR E SALVAR (M)";
        btn.style.background = "#1B5E20";
        btn.style.borderColor = "#00E676";
        document.getElementById("medidaAtualDisp").innerText = "0.0 mm";
      } else {
        medindo = false;
        const distFinal = calcularDistanciaMediaMm().toFixed(1);
        btn.innerText = "📏 MEDIR (M)";
        btn.style.background = "";
        btn.style.borderColor = "";
        adicionarMedidaNaBarra(distFinal);
      }
    }

    function adicionarMedidaNaBarra(valorMm) {
      const barra = document.getElementById("barraMedidas");
      const chip = document.createElement("span");
      chip.className = "chip-medida";
      chip.innerText = `${valorMm} mm`;
      chip.title = "Clique para copiar o valor";
      chip.onclick = () => navigator.clipboard?.writeText(valorMm);
      barra.appendChild(chip);
    }

    function limparMedidas() {
      document.getElementById("barraMedidas").innerHTML = "";
    }

    function aplicarForcaBase(valor) {
      const v = Math.max(0, Math.min(255, parseInt(valor) || 0));
      document.getElementById("forcaBase").value = v;
      if (ws.readyState === WebSocket.OPEN) ws.send(`vel:${v}`);
    }

    function aplicarRampa() {
      // Envia via oninput sem reescrever o campo para permitir apagar e digitar novos números sem travar o cursor.
      const acel = Math.max(1, Math.min(255, parseInt(document.getElementById("passoAcel").value) || 8));
      const freio = Math.max(1, Math.min(255, parseInt(document.getElementById("passoFreio").value) || 15));
      if (ws.readyState === WebSocket.OPEN) {
        ws.send(`acel:${acel}`);
        ws.send(`freio:${freio}`);
      }
    }

    function alterarForcaBase(delta) {
      const atual = parseInt(document.getElementById("forcaBase").value) || 0;
      aplicarForcaBase(atual + delta);
    }

    function aplicarForcaBraco(valor) {
      const v = Math.max(50, Math.min(255, parseInt(valor) || 130));
      document.getElementById("forcaBraco").value = v;
      if (ws.readyState === WebSocket.OPEN) ws.send(`vel_braco:${v}`);
    }

    function selecionarPreset(indice) {
      for (let i = 1; i <= 4; i++) {
        document.getElementById(`cardVel${i}`).classList.toggle("ativo", i === indice);
      }
      const val = document.getElementById(`preset${indice}`).value;
      aplicarForcaBase(val);
    }

    function atualizarPreset(indice) {
      if (document.getElementById(`cardVel${indice}`).classList.contains("ativo")) {
        selecionarPreset(indice);
      }
    }

    function executarGiroSeta(direcao) {
      const ang = Math.abs(parseFloat(document.getElementById("anguloGiro").value) || 0);
      if (ang <= 0) return;
      const angFinal = direcao * ang;
      const vel = parseInt(document.getElementById("forcaBase").value) || 120;
      sendCmd(`turn:${angFinal},${vel}`);
    }

    function executarGiroAbsoluto(anguloOpcional) {
      const alvo = (anguloOpcional !== undefined)
        ? anguloOpcional
        : (parseFloat(document.getElementById("anguloAbsoluto").value) || 0);
      document.getElementById("anguloAbsoluto").value = alvo;
      const vel = parseInt(document.getElementById("forcaBase").value) || 120;
      // Envia turn_to para orientar o chassi na direção fixa equivalente (módulo 360°) pelo menor arco.
      sendCmd(`turn_to:${alvo},${vel}`);
    }

    function cancelarReplay() {
      timersReplay.forEach((id) => clearTimeout(id));
      timersReplay = [];
    }

    function atualizarBotoesGravacao() {
      const btnRec = document.getElementById("btnRec");
      const btnContinuar = document.getElementById("btnContinuar");
      const btnOverdub = document.getElementById("btnOverdub");
      const btnDesfazer = document.getElementById("btnDesfazer");
      const temConteudo = passosGravados.length > 0 || duracaoTotalGravada > 0;

      if (gravando) {
        btnRec.innerText = "⏹ PARAR GRAVAÇÃO";
        btnRec.classList.add("ativo");
        btnContinuar.disabled = true;
        btnOverdub.disabled = true;
        btnDesfazer.disabled = true;
      } else {
        btnRec.innerText = "🔴 GRAVAR DO ZERO";
        btnRec.classList.remove("ativo");
        btnContinuar.disabled = !temConteudo;
        btnOverdub.disabled = !temConteudo;
        btnDesfazer.disabled = historicoPartes.length === 0;
      }

      const info = document.getElementById("recInfo");
      if (info) {
        info.innerText = `⏱ Duração gravada: ${(duracaoTotalGravada / 1000).toFixed(1)}s | Partes: ${historicoPartes.length} | Passos: ${passosGravados.length}`;
      }
    }

    function toggleRec(modo) {
      cancelarReplay();
      if (!gravando) {
        gravando = true;
        cmdAtivo = "";
        bracoAtivo = "";
        movX = movY = movRot = movPicoMag = 0;

        if (modo === "novo") {
          duracaoTotalGravada = 0;
          parteAtualId = 1;
          historicoPartes = [{ parteId: 1, duracaoAntes: 0 }];
          passosGravados = [];
          eventosGravados = [];
          inicioRec = Date.now();
          atualizarTextoRoteiro();
        } else if (modo === "continuar") {
          // Desloca inicioRec para trás pela duração já gravada para encadear a nova parte no fim da linha do tempo.
          parteAtualId++;
          historicoPartes.push({ parteId: parteAtualId, duracaoAntes: duracaoTotalGravada });
          inicioRec = Date.now() - duracaoTotalGravada;
        } else if (modo === "overdub") {
          parteAtualId++;
          historicoPartes.push({ parteId: parteAtualId, duracaoAntes: duracaoTotalGravada });
          inicioRec = Date.now();
          if (document.getElementById("chkReplayOverdub").checked) {
            testarRoteiroGravado([...eventosGravados]);
          }
        }
        atualizarBotoesGravacao();
      } else {
        // Fecha qualquer segmento pendente e reordena toda a linha do tempo antes de encerrar.
        const tempoFim = Date.now() - inicioRec;
        fecharSegmentoGravado(tempoFim);
        fecharSegmentoBraco(tempoFim);
        duracaoTotalGravada = Math.max(duracaoTotalGravada, tempoFim);
        eventosGravados.sort((a, b) => a.tempo - b.tempo);
        atualizarTextoRoteiro();
        gravando = false;
        atualizarBotoesGravacao();
      }
    }

    function desfazerUltimaParte() {
      if (gravando || historicoPartes.length === 0) return;
      cancelarReplay();
      const ultima = historicoPartes.pop();
      passosGravados = passosGravados.filter((p) => p.parte !== ultima.parteId);
      eventosGravados = eventosGravados.filter((e) => e.parte !== ultima.parteId);
      duracaoTotalGravada = ultima.duracaoAntes;
      atualizarTextoRoteiro();
      atualizarBotoesGravacao();
    }

    function limparRoteiro() {
      cancelarReplay();
      document.getElementById("codeOut").value = "";
      duracaoTotalGravada = 0;
      parteAtualId = 0;
      historicoPartes = [];
      passosGravados = [];
      eventosGravados = [];
      atualizarBotoesGravacao();
    }

    function testarRoteiroGravado(listaOpcional) {
      cancelarReplay();
      const lista = listaOpcional || [...eventosGravados].sort((a, b) => a.tempo - b.tempo);
      if (lista.length === 0) return;

      lista.forEach((ev) => {
        const id = setTimeout(() => {
          if (ev.tipo === "raw" && ws.readyState === WebSocket.OPEN) {
            ws.send(ev.cmd);
            // Evita que o comando "stop" da base interrompa o motor de um ombro segurado durante o overdub.
            if (ev.cmd === "stop" && comandoBracoAtivo !== "") {
              ws.send(comandoBracoAtivo);
            }
          }
        }, ev.tempo);
        timersReplay.push(id);
      });
    }

    function substituirGiroNoReplay(segId, tempoInicio, anguloAbsolutoFinal, vel) {
      // Remove os comandos brutos em malha aberta do giro manual e injeta turn_to: pela orientação absoluta do SPIKE.
      eventosGravados = eventosGravados.filter((e) => e.segId !== segId);
      eventosGravados.push({
        tempo: tempoInicio,
        tipo: "raw",
        cmd: `turn_to:${anguloAbsolutoFinal},${vel}`,
        parte: parteAtualId
      });
    }

    function fecharSegmentoGravado(tempo) {
      if (!cmdAtivo) return;
      const vel = parseInt(document.getElementById("forcaBase").value) || 120;
      // Usa o maior deslocamento entre M1 e M2 porque nas diagonais esquerdas M1 fica parado.
      const ticksDesloc = Math.max(Math.abs(ticks[0] - m1Start), Math.abs(ticks[1] - m2Start));
      const dist = (ticksDesloc / kTicksPorMm).toFixed(1);
      const ang = (gyroAtual - gyroStart).toFixed(1);
      const angAbsFinal = normalizarAngulo180(gyroAtual);
      let acao = "";

      if (cmdAtivo === "w") acao = `funcoes_.run(${dist}f, ${vel});`;
      else if (cmdAtivo === "s") acao = `funcoes_.run(-${dist}f, ${vel});`;
      else if (cmdAtivo === "d") acao = `funcoes_.strafe(${dist}f, ${vel});`;
      else if (cmdAtivo === "a") acao = `funcoes_.strafe(-${dist}f, ${vel});`;
      else if (cmdAtivo === "wd") acao = `funcoes_.diagonal(${dist}f, true, ${vel});`;
      else if (cmdAtivo === "wa") acao = `funcoes_.diagonal(${dist}f, false, ${vel});`;
      else if (cmdAtivo === "sd") acao = `funcoes_.diagonal(-${dist}f, false, ${vel});`;
      else if (cmdAtivo === "sa") acao = `funcoes_.diagonal(-${dist}f, true, ${vel});`;
      else if (cmdAtivo === "e" || cmdAtivo === "q") {
        // Grava a orientação absoluta final para evitar acúmulo de erro entre passos na rotina autônoma.
        acao = `funcoes_.turnTo(${angAbsFinal}f, ${vel});`;
        substituirGiroNoReplay(segAtualId, segInicioTempo, angAbsFinal, vel);
      } else if (cmdAtivo === "mov") {
        const temLinear = movPicoMag > 0.05;
        const temGiro = Math.abs(movRot) > 0.05;
        if (temLinear && temGiro && Math.abs(parseFloat(ang)) > 1.0) {
          acao = `funcoes_.moverGirando(${movX.toFixed(2)}f, ${movY.toFixed(2)}f, ${ang}f, ${vel});`;
        } else if (temGiro && !temLinear && Math.abs(parseFloat(ang)) > 1.0) {
          acao = `funcoes_.turnTo(${angAbsFinal}f, ${vel});`;
          substituirGiroNoReplay(segAtualId, segInicioTempo, angAbsFinal, vel);
        } else if (temLinear && !temGiro && parseFloat(dist) > 1.0) {
          const velProp = Math.max(25, Math.round(vel * Math.min(1.0, movPicoMag)));
          const angDir = Math.atan2(movY, movX) * (180.0 / Math.PI);
          if (angDir >= 67.5 && angDir <= 112.5) {
            acao = `funcoes_.run(${dist}f, ${velProp});`;
          } else if (angDir <= -67.5 && angDir >= -112.5) {
            acao = `funcoes_.run(-${dist}f, ${velProp});`;
          } else if (angDir > -22.5 && angDir < 22.5) {
            acao = `funcoes_.strafe(${dist}f, ${velProp});`;
          } else if (angDir > 157.5 || angDir < -157.5) {
            acao = `funcoes_.strafe(-${dist}f, ${velProp});`;
          } else if (angDir > 22.5 && angDir < 67.5) {
            acao = `funcoes_.diagonal(${dist}f, true, ${velProp});`;
          } else if (angDir > 112.5 && angDir < 157.5) {
            acao = `funcoes_.diagonal(${dist}f, false, ${velProp});`;
          } else if (angDir > -67.5 && angDir < -22.5) {
            acao = `funcoes_.diagonal(-${dist}f, false, ${velProp});`;
          } else {
            acao = `funcoes_.diagonal(-${dist}f, true, ${velProp});`;
          }
        }
      }

      if (acao && (parseFloat(dist) > 1.0 || cmdAtivo === "e" || cmdAtivo === "q" || cmdAtivo === "mov")) {
        registrarPasso(tempo, acao);
      }
      cmdAtivo = "";
    }

    function sendCmd(cmd) {
      if (ws.readyState === WebSocket.OPEN) ws.send(cmd);
      if (!gravando) return;

      const tempo = Date.now() - inicioRec;
      const velBase = parseInt(document.getElementById("forcaBase").value) || 120;

      if (cmd.startsWith("turn:")) {
        const partes = cmd.substring(5).split(",");
        const delta = parseFloat(partes[0]) || 0;
        const v = partes[1] || velBase;
        // Converte o giro relativo no alvo absoluto correspondente ao 0° fixo da arena.
        const angAbs = normalizarAngulo180(gyroAtual + delta);
        registrarPasso(tempo, `funcoes_.turnTo(${angAbs}f, ${v});`);
        eventosGravados.push({ tempo, tipo: "raw", cmd: `turn_to:${angAbs},${v}`, parte: parteAtualId });
      } else if (cmd.startsWith("turn_to:")) {
        const partes = cmd.substring(8).split(",");
        const ang = parseFloat(partes[0]).toFixed(1);
        const v = partes[1] || velBase;
        registrarPasso(tempo, `funcoes_.turnTo(${ang}f, ${v});`);
        eventosGravados.push({ tempo, tipo: "raw", cmd, parte: parteAtualId });
      } else if (cmd.startsWith("srv_pos_e:")) {
        registrarPasso(tempo, `funcoes_.moverCotoveloEsq(${cmd.split(":")[1]}.0f);`);
        eventosGravados.push({ tempo, tipo: "raw", cmd, parte: parteAtualId });
      } else if (cmd.startsWith("srv_pos_d:")) {
        registrarPasso(tempo, `funcoes_.moverCotoveloDir(${cmd.split(":")[1]}.0f);`);
        eventosGravados.push({ tempo, tipo: "raw", cmd, parte: parteAtualId });
      } else if (cmd.startsWith("vel:") || cmd.startsWith("fod:")) {
        eventosGravados.push({ tempo, tipo: "raw", cmd, parte: parteAtualId });
      } else if (cmd.startsWith("mov:")) {
        const partes = cmd.substring(4).split(",");
        const nx = parseFloat(partes[0]) || 0;
        const ny = parseFloat(partes[1]) || 0;
        const nrot = parseFloat(partes[2]) || 0;

        // Evita fragmentar a gravação quando o polegar acelera ou desacelera gradualmente no mesmo rumo.
        const tinhaLinear = movPicoMag > 0.05;
        const magNovo = Math.hypot(nx, ny);
        const temLinear = magNovo > 0.05;
        const tinhaGiro = Math.abs(movRot) > 0.05;
        const temGiro = Math.abs(nrot) > 0.05;

        let mudouDirecao = false;
        if (tinhaLinear && temLinear) {
          const angAnt = Math.atan2(movY, movX);
          const angNovo = Math.atan2(ny, nx);
          let diffAng = Math.abs(angNovo - angAnt) * (180.0 / Math.PI);
          if (diffAng > 180.0) diffAng = 360.0 - diffAng;
          mudouDirecao = diffAng > 35.0;
        }

        const mudouModo = (tinhaLinear !== temLinear) || (tinhaGiro !== temGiro) ||
                          (temGiro && Math.sign(nrot) !== Math.sign(movRot)) ||
                          mudouDirecao;

        if (cmdAtivo !== "" && (cmdAtivo !== "mov" || mudouModo)) {
          fecharSegmentoGravado(tempo);
        }
        if (cmdAtivo !== "mov") {
          m1Start = ticks[0];
          m2Start = ticks[1];
          gyroStart = gyroAtual;
          cmdAtivo = "mov";
          movPicoMag = 0;
          segAtualId = ++contadorSegmentos;
          segInicioTempo = tempo;
          eventosGravados.push({ tempo, tipo: "raw", cmd: `vel:${velBase}`, parte: parteAtualId, segId: segAtualId });
        }
        movX = nx;
        movY = ny;
        movRot = nrot;
        if (magNovo > movPicoMag) movPicoMag = magNovo;
        eventosGravados.push({ tempo, tipo: "raw", cmd, parte: parteAtualId, segId: segAtualId });
      } else if (cmd === "stop") {
        const tinhaSegmento = (cmdAtivo !== "");
        const segFechadoId = segAtualId;
        fecharSegmentoGravado(tempo);
        // Só registra "stop" bruto se não acabou de ser substituído por um turn_to: fechado pelo giroscópio.
        if (!tinhaSegmento || eventosGravados.some((e) => e.segId === segFechadoId)) {
          eventosGravados.push({ tempo, tipo: "raw", cmd: "stop", parte: parteAtualId });
        }
      } else if (["w","s","a","d","wd","wa","sd","sa","q","e"].includes(cmd)) {
        if (cmdAtivo !== "" && cmdAtivo !== cmd) {
          fecharSegmentoGravado(tempo);
        }
        if (cmdAtivo !== cmd) {
          m1Start = ticks[0];
          m2Start = ticks[1];
          gyroStart = gyroAtual;
          cmdAtivo = cmd;
          segAtualId = ++contadorSegmentos;
          segInicioTempo = tempo;
          eventosGravados.push({ tempo, tipo: "raw", cmd: `vel:${velBase}`, parte: parteAtualId, segId: segAtualId });
          eventosGravados.push({ tempo, tipo: "raw", cmd, parte: parteAtualId, segId: segAtualId });
        }
      }
    }

    function registrarPasso(tempo, acao) {
      passosGravados.push({ tempo, acao, parte: parteAtualId });
      // Mantém os comandos sempre em ordem crescente de tempo mesmo após gravar novas camadas por cima.
      passosGravados.sort((a, b) => a.tempo - b.tempo);
      atualizarTextoRoteiro();
      atualizarBotoesGravacao();
    }

    function atualizarTextoRoteiro() {
      const box = document.getElementById("codeOut");
      let texto = "// Cole este bloco dentro de BaseCenas::atualizarCenas() para reproduzir a rotina.\n";
      passosGravados.forEach((item, indice) => {
        texto += `if (passoAtual_ == ${indice} && decorridoMs >= ${item.tempo}) {\n    ${item.acao}\n    passoAtual_++;\n}\n`;
      });
      box.value = texto;
      box.scrollTop = box.scrollHeight;
    }

    function pressionarBotaoDir(id) {
      botoesAtivos[id] = true;
      sincronizarMovimentoManual();
    }

    function soltarBotaoDir(id) {
      delete botoesAtivos[id];
      sincronizarMovimentoManual();
    }

    function pararTudoManual() {
      cancelarReplay();
      for (const k in teclasAtivas) delete teclasAtivas[k];
      for (const k in botoesAtivos) delete botoesAtivos[k];
      gpX = gpY = gpRot = 0;
      cmdManualAtual = "stop";
      atualizarDestaqueBotoes(0, 0, 0);
      sendCmd("stop");
    }

    function calcularVetorManual() {
      // Prioriza os analógicos do controle Logitech quando movimentados; caso contrário usa teclado/touch.
      if (gpX !== 0 || gpY !== 0 || gpRot !== 0) {
        return { x: gpX, y: gpY, rot: gpRot, viaGamepad: true };
      }

      const ativo = (k) => !!teclasAtivas[k] || !!botoesAtivos[k];
      let x = 0, y = 0, rot = 0;

      if (ativo("w") || ativo("wa") || ativo("wd")) y += 1;
      if (ativo("s") || ativo("sa") || ativo("sd")) y -= 1;
      if (ativo("d") || ativo("wd") || ativo("sd")) x += 1;
      if (ativo("a") || ativo("wa") || ativo("sa")) x -= 1;
      if (ativo("q")) rot -= 1;
      if (ativo("e")) rot += 1;

      x = Math.max(-1, Math.min(1, x));
      y = Math.max(-1, Math.min(1, y));
      rot = Math.max(-1, Math.min(1, rot));

      // Normaliza apenas (x, y) nas diagonais para manter velocidade linear constante sem anular a rotação.
      const magXY = Math.hypot(x, y);
      if (magXY > 1) {
        x /= magXY;
        y /= magXY;
      }
      return { x, y, rot, viaGamepad: false };
    }

    function resolverComandoManual(x, y, rot, viaGamepad) {
      if (x === 0 && y === 0 && rot === 0) return "stop";

      // Envia vetor contínuo mov: para preservar o controle proporcional dos analógicos do gamepad.
      if (viaGamepad) {
        return `mov:${x.toFixed(2)},${y.toFixed(2)},${rot.toFixed(2)}`;
      }

      if (rot === 0) {
        if (y > 0 && x > 0) return "wd";
        if (y > 0 && x < 0) return "wa";
        if (y < 0 && x > 0) return "sd";
        if (y < 0 && x < 0) return "sa";
        if (y > 0) return "w";
        if (y < 0) return "s";
        if (x > 0) return "d";
        if (x < 0) return "a";
      }
      if (x === 0 && y === 0) {
        return rot < 0 ? "q" : "e";
      }

      return `mov:${x.toFixed(2)},${y.toFixed(2)},${rot.toFixed(2)}`;
    }

    function atualizarDestaqueBotoes(x, y, rot) {
      const ativos = new Set();
      if (y > 0.25 && x > 0.25) ativos.add("wd");
      else if (y > 0.25 && x < -0.25) ativos.add("wa");
      else if (y < -0.25 && x > 0.25) ativos.add("sd");
      else if (y < -0.25 && x < -0.25) ativos.add("sa");
      else if (y > 0.25) ativos.add("w");
      else if (y < -0.25) ativos.add("s");
      else if (x > 0.25) ativos.add("d");
      else if (x < -0.25) ativos.add("a");

      if (rot < -0.25) ativos.add("q");
      if (rot > 0.25) ativos.add("e");

      ["w","s","a","d","wd","wa","sd","sa","q","e"].forEach((id) => {
        const el = document.getElementById(`btn_${id}`);
        if (el) el.classList.toggle("tecla-ativa", ativos.has(id));
      });
    }

    function sincronizarMovimentoManual() {
      const { x, y, rot, viaGamepad } = calcularVetorManual();
      atualizarDestaqueBotoes(x, y, rot);
      const novoCmd = resolverComandoManual(x, y, rot, viaGamepad);
      if (novoCmd !== cmdManualAtual) {
        cmdManualAtual = novoCmd;
        sendCmd(novoCmd);
      }
    }

    document.addEventListener("keydown", (ev) => {
      // Intercepta dígitos do teclado enquanto RB ou LB estiverem pressionados no gamepad.
      if (gpModoGiroBumper) {
        if ((ev.key >= "0" && ev.key <= "9") || ((ev.key === "." || ev.key === ",") && !gpBufferAngulo.includes("."))) {
          ev.preventDefault();
          gpBufferAngulo += (ev.key === ",") ? "." : ev.key;
          atualizarStatusGamepad(null);
        } else if (ev.key === "Backspace") {
          ev.preventDefault();
          gpBufferAngulo = gpBufferAngulo.slice(0, -1);
          atualizarStatusGamepad(null);
        } else if (ev.key === "Escape") {
          ev.preventDefault();
          gpBufferAngulo = "";
          atualizarStatusGamepad(null);
        }
        return;
      }

      if (ev.repeat) return;

      if (ev.key === "ArrowLeft") {
        ev.preventDefault();
        executarGiroSeta(-1);
        return;
      }
      if (ev.key === "ArrowRight") {
        ev.preventDefault();
        executarGiroSeta(1);
        return;
      }

      const tag = document.activeElement?.tagName;
      if (tag === "INPUT" || tag === "TEXTAREA") return;

      const k = ev.key.toLowerCase();

      if (k === "1" || k === "2" || k === "3" || k === "4") {
        selecionarPreset(parseInt(k));
        return;
      }

      if (k === "m") {
        toggleMedirDistancia();
        return;
      }

      if (["w","a","s","d","q","e"].includes(k)) {
        teclasAtivas[k] = true;
        sincronizarMovimentoManual();
      }
    });

    document.addEventListener("keyup", (ev) => {
      const k = ev.key.toLowerCase();
      if (["w","a","s","d","q","e"].includes(k)) {
        delete teclasAtivas[k];
        sincronizarMovimentoManual();
      }
    });
  </script>
</body>
</html>
)rawliteral";