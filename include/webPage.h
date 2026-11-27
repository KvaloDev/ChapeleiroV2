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
    .box { background: var(--card); border: 2px solid var(--yellow); border-radius: 8px; padding: 10px; margin: 8px auto; max-width: 740px; }
    .box-base { border-color: #42A5F5; }
    .box-bracos { border-color: #CE93D8; }
    .grid-2 { display: grid; grid-template-columns: 1fr 1fr; gap: 8px; max-width: 740px; margin: 8px auto; }
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
    .rec-box { width: 100%; height: 145px; background: #02050A; color: #00FF66; font-family: monospace; font-size: 12px; border: 1px solid var(--btn); padding: 8px; box-sizing: border-box; user-select: text; }
    .rec-box.bracos { color: #E1BEE7; border-color: #6A1B9A; }
    .btn-rec { background: var(--red); border-color: var(--red); }
    .btn-rec.ativo { background: #555; border-color: #555; }
    .btn-rec-bracos { background: #6A1B9A; border-color: #CE93D8; color: #FFF; }
    .btn-rec-bracos.ativo { background: #555; border-color: #555; }
    .btn-cont { background: #0277BD; border-color: #29B6F6; color: #FFF; }
    .btn-cont-bracos { background: #4A148C; border-color: #BA68C8; color: #FFF; }
    .btn-play { background: #1B5E20; border-color: var(--green); color: var(--green); }
    .btn-add { background: #283593; border-color: #7986CB; color: #FFF; }
    .btn-modo-base { background: #0D47A1; border-color: #42A5F5; color: #FFF; }
    .btn-modo-bracos { background: #6A1B9A; border-color: #CE93D8; color: #FFF; }
    .btn-cha { background: #00695C; border-color: #4DB6AC; color: #FFF; }
    .btn-sm { padding: 5px 9px; font-size: 11px; }
    .medida-bar { display: flex; flex-wrap: wrap; gap: 6px; justify-content: center; margin-top: 8px; min-height: 28px; }
    .chip-medida { background: var(--bg); border: 1px solid var(--green); color: var(--green); padding: 4px 10px; border-radius: 14px; font-family: monospace; font-size: 13px; cursor: pointer; }
    .rampa-row { display: flex; gap: 8px; justify-content: center; align-items: center; font-size: 12px; color: #CCC; }
    .rampa-row input { width: 48px; padding: 4px; font-size: 12px; }
    .gamepad-status { font-size: 12px; margin-top: 6px; padding: 4px 8px; border-radius: 4px; background: var(--bg); color: #AAA; border: 1px solid var(--btn); }
    .gamepad-status.conectado { color: var(--green); border-color: var(--green); }
    .gamepad-status.digitando { color: var(--bg); background: var(--yellow); border-color: var(--yellow); font-weight: bold; }
    .gamepad-status.erro { color: #FF8A80; border-color: var(--red); cursor: pointer; }
    .gamepad-legenda { font-size: 10px; color: #90CAF9; margin-top: 4px; line-height: 1.35; }
    .visor-digitado { display: none; margin-top: 6px; padding: 5px 8px; border-radius: 4px; background: var(--yellow); color: var(--bg); font-family: monospace; font-weight: bold; font-size: 13px; }
    .rec-info { font-size: 12px; color: #CCC; margin-top: 6px; font-family: monospace; }
    .area-topo { display: flex; justify-content: space-between; align-items: center; gap: 8px; flex-wrap: wrap; margin-bottom: 6px; }
    .area-titulo-base { color: #90CAF9; font-weight: 900; font-size: 14px; text-transform: uppercase; letter-spacing: 0.5px; }
    .area-titulo-bracos { color: #E1BEE7; font-weight: 900; font-size: 14px; text-transform: uppercase; letter-spacing: 0.5px; }
    .tabela-partes { width: 100%; border-collapse: collapse; margin-top: 8px; background: var(--bg); border: 1px solid var(--btn); border-radius: 6px; overflow: hidden; }
    .tabela-partes th { background: #101E4A; color: var(--yellow); font-size: 12px; text-transform: uppercase; padding: 7px 10px; text-align: left; border-bottom: 1px solid var(--btn); }
    .tabela-partes.bracos th { background: #2A0E4A; color: #E1BEE7; border-bottom-color: #4A148C; }
    .tabela-partes td { padding: 8px 10px; border-bottom: 1px solid #13224F; vertical-align: top; }
    .tabela-partes.bracos td { border-bottom-color: #2A1247; }
    .parte-topo { display: flex; justify-content: space-between; align-items: center; gap: 8px; margin-bottom: 6px; flex-wrap: wrap; }
    .parte-titulo { font-weight: bold; color: var(--yellow); font-size: 13px; }
    .parte-badge { font-family: monospace; font-size: 10px; padding: 2px 6px; border-radius: 4px; margin-left: 6px; font-weight: bold; }
    .badge-base { background: #0D47A1; color: #90CAF9; border: 1px solid #42A5F5; }
    .badge-bracos { background: #4A148C; color: #E1BEE7; border: 1px solid #CE93D8; }
    .parte-meta { font-family: monospace; font-size: 11px; color: #90CAF9; margin-left: 6px; }
    .parte-acoes { display: flex; gap: 6px; }
    .parte-code { width: 100%; min-height: 82px; background: #02050A; color: #00FF66; font-family: monospace; font-size: 12px; border: 1px solid #1E3270; border-radius: 4px; padding: 6px; box-sizing: border-box; resize: vertical; user-select: text; }
    .parte-code.bracos { color: #E1BEE7; border-color: #4A148C; }
    .parte-code:focus { outline: none; border-color: var(--yellow); }
    .vazio-partes { text-align: center; color: #888; font-size: 12px; padding: 14px; }
    .saida-header { font-size: 11px; color: #AAA; display: flex; justify-content: space-between; align-items: center; gap: 6px; flex-wrap: wrap; margin: 10px 0 4px; }
    .atalhos-pills { display: flex; flex-wrap: wrap; gap: 5px; justify-content: center; margin-bottom: 8px; }
    .atalho-pill { padding: 6px 10px; font-size: 12px; font-family: monospace; background: var(--bg); border: 1px solid var(--btn); color: #E2E8F0; }
    .atalho-pill.ativo { background: var(--yellow); color: var(--bg); border-color: var(--yellow); }
    .atalho-info { background: #02050A; border: 1px solid var(--btn); border-left: 3px solid var(--yellow); border-radius: 5px; padding: 8px 10px; text-align: left; margin-bottom: 8px; }
    .atalho-assinatura { font-family: monospace; color: var(--green); font-size: 13px; font-weight: bold; display: block; margin-bottom: 4px; }
    .atalho-desc { font-size: 12px; color: #DDD; line-height: 1.4; }
    .atalho-desc b { color: var(--yellow); }
    .atalho-campos { display: flex; flex-wrap: wrap; gap: 10px; justify-content: center; align-items: flex-end; margin-bottom: 8px; }
    .atalho-campo { display: flex; flex-direction: column; align-items: center; gap: 3px; }
    .atalho-campo label { font-size: 11px; color: #90CAF9; font-family: monospace; }
    .atalho-campo input { width: 92px; }
    .atalho-acoes { display: flex; gap: 6px; justify-content: center; flex-wrap: wrap; }
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
      <div style="margin-top:6px;">
        <button id="btnModoControle" class="btn-modo-base" style="width:100%;" onclick="alternarModoControleGamepad()">🎮 CONTROLE: BASE (Tab p/ Braços)</button>
      </div>
      <div id="wsStatus" class="gamepad-status erro" onclick="conectarWebSocket()">🔴 Wi-Fi / WS: Conectando...</div>
      <div id="gamepadStatus" class="gamepad-status">🎮 Controle: Desconectado (aperte um botão)</div>
      <div id="gamepadLegenda" class="gamepad-legenda">BASE | Y:0° B:+90° A:180° X:-90° | D-Pad ◄►: Giro | Segure LB/RB + Teclado: ±X°</div>
    </div>

    <div class="box" style="margin:0;">
      <h3>Força da Base e Hora do Chá (BLE)</h3>
      <button onclick="alterarForcaBase(-10)">-10</button>
      <input type="number" id="forcaBase" value="120" min="0" max="255" onchange="aplicarForcaBase(this.value)">
      <button onclick="alterarForcaBase(10)">+10</button>
      <div class="rampa-row" style="margin-top:8px;">
        <span>Acel: <input type="number" id="passoAcel" value="8" min="1" max="255" oninput="aplicarRampa()" onkeydown="if(event.key==='Enter')this.blur()"></span>
        <span>Freio: <input type="number" id="passoFreio" value="15" min="1" max="255" oninput="aplicarRampa()" onkeydown="if(event.key==='Enter')this.blur()"></span>
      </div>
      <div style="margin-top:8px; padding-top:8px; border-top:1px solid var(--btn); display:flex; gap:6px; justify-content:center; align-items:center; flex-wrap:wrap;">
        <span style="font-size:12px;">Canal BLE:</span>
        <input type="number" id="canalTea" value="120" min="0" max="255" style="width:52px; padding:4px;">
        <button class="btn-cha btn-sm" onclick="enviarCodigoCha()">☕ ENVIAR 2565 (CHÁ)</button>
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
      <h3>Giro Preciso (D-Pad ◄► ou Setas ←→)</h3>
      <input type="number" id="anguloGiro" value="90" min="1" max="360" step="1"> °
      <button class="turn" onclick="executarGiroSeta(-1)">↶ ESQ (◄)</button>
      <button class="turn" onclick="executarGiroSeta(1)">DIR (►) ↷</button>
      <div id="visorGiroDigitado" class="visor-digitado"></div>
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

  <div class="box">
    <h3>Atalhos de Funções (Execução Rápida &amp; C++)</h3>
    <div class="atalhos-pills" id="atalhosPills"></div>
    <div class="atalho-info">
      <code class="atalho-assinatura" id="atalhoAssinatura"></code>
      <div class="atalho-desc" id="atalhoDesc"></div>
    </div>
    <div class="atalho-campos" id="atalhoCampos"></div>
    <div class="atalho-acoes">
      <button class="btn-play" onclick="executarAtalhoSelecionado()">▶ EXECUTAR NO ROBÔ</button>
      <button id="btnCopiarCppAtalho" onclick="copiarCppAtalho()" style="border-color:var(--yellow); color:var(--yellow);">📋 COPIAR C++</button>
      <button class="btn-sm" onclick="sendCmd('zero_gyro')" style="border-color:var(--green); color:var(--green);">zerarGyro()</button>
      <button class="btn-sm" onclick="sendCmd('stop_base')" style="border-color:var(--red); color:#FF8A80;">pararBase()</button>
    </div>
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
    <h3>Força dos Braços (Ombros PWM 20–255 | D-Pad ▲▼ ±5)</h3>
    <button onclick="alterarForcaBraco(-5)">-5</button>
    <input type="number" id="forcaBraco" value="130" min="20" max="255" onchange="aplicarForcaBraco(this.value)">
    <button onclick="alterarForcaBraco(5)">+5</button>
    <button onclick="aplicarForcaBraco(document.getElementById('forcaBraco').value)">APLICAR</button>
  </div>

  <div class="grid-2">
    <div class="box" style="margin:0;">
      <div>OMBRO ESQ (Y ▲ / B ▼ | Analóg. Dir ↕): <span class="telem" id="encBE">0</span></div><br>
      <button onmousedown="iniciarOmbro('e', 'up')" onmouseup="pararOmbro('e')" onmouseleave="pararOmbro('e')" ontouchstart="event.preventDefault();iniciarOmbro('e', 'up')" ontouchend="pararOmbro('e')" ontouchcancel="pararOmbro('e')">▲ SOBE</button>
      <button onmousedown="iniciarOmbro('e', 'dwn')" onmouseup="pararOmbro('e')" onmouseleave="pararOmbro('e')" ontouchstart="event.preventDefault();iniciarOmbro('e', 'dwn')" ontouchend="pararOmbro('e')" ontouchcancel="pararOmbro('e')">▼ DESCE</button>
    </div>
    <div class="box" style="margin:0;">
      <div>OMBRO DIR (X ▲ / A ▼): <span class="telem" id="encBD">0</span></div><br>
      <button onmousedown="iniciarOmbro('d', 'up')" onmouseup="pararOmbro('d')" onmouseleave="pararOmbro('d')" ontouchstart="event.preventDefault();iniciarOmbro('d', 'up')" ontouchend="pararOmbro('d')" ontouchcancel="pararOmbro('d')">▲ SOBE</button>
      <button onmousedown="iniciarOmbro('d', 'dwn')" onmouseup="pararOmbro('d')" onmouseleave="pararOmbro('d')" ontouchstart="event.preventDefault();iniciarOmbro('d', 'dwn')" ontouchend="pararOmbro('d')" ontouchcancel="pararOmbro('d')">▼ DESCE</button>
    </div>
    <div class="box" style="margin:0;">
      <div>COTOVELO ESQ (RB ▼ / RT ▲ | D-Pad Ambos)</div><br>
      <input type="number" id="srvE" value="0" min="0" max="120">
      <button onclick="definirCotovelo('e', document.getElementById('srvE').value)">SET</button>
      <div style="display:grid; grid-template-columns:repeat(3,1fr); gap:4px; margin-top:6px;">
        <button onclick="definirCotovelo('e', 0)">0°</button>
        <button onclick="definirCotovelo('e', 60)">60°</button>
        <button onclick="definirCotovelo('e', 120)">120°</button>
      </div>
    </div>
    <div class="box" style="margin:0;">
      <div>COTOVELO DIR (LB ▼ / LT ▲ | D-Pad Ambos)</div><br>
      <input type="number" id="srvD" value="0" min="0" max="120">
      <button onclick="definirCotovelo('d', document.getElementById('srvD').value)">SET</button>
      <div style="display:grid; grid-template-columns:repeat(3,1fr); gap:4px; margin-top:6px;">
        <button onclick="definirCotovelo('d', 0)">0°</button>
        <button onclick="definirCotovelo('d', 60)">60°</button>
        <button onclick="definirCotovelo('d', 120)">120°</button>
      </div>
    </div>
  </div>

  <div class="box">
    ENCODERS: M1:<span class="telem" id="enc1">0</span> | M2:<span class="telem" id="enc2">0</span> | M3:<span class="telem" id="enc3">0</span> | M4:<span class="telem" id="enc4">0</span>
  </div>

  <!-- BARRA GERAL DE GRAVAÇÃO E TESTE SINCRONIZADO -->
  <div class="box" style="text-align:left;">
    <div style="display:flex; gap:6px; flex-wrap:wrap; align-items:center;">
      <button id="btnRecBaseNovo" class="btn-rec" onclick="toggleRec('base_novo')">🔴 GRAVAR BASE (0s)</button>
      <button id="btnRecBaseCont" class="btn-cont" onclick="toggleRec('base_continuar')" disabled>⏭ CONTINUAR BASE</button>
      <button id="btnRecBracosNovo" class="btn-rec-bracos" onclick="toggleRec('bracos_novo')">🦾 GRAVAR BRAÇOS (0s)</button>
      <button id="btnRecBracosCont" class="btn-cont-bracos" onclick="toggleRec('bracos_continuar')" disabled>⏭ CONTINUAR BRAÇOS</button>
      <button class="btn-play" onclick="testarTudoSincronizado()">▶ TESTAR TUDO (BASE + BRAÇOS)</button>
      <button onclick="cancelarReplay()">⏹ PARAR TESTE</button>
    </div>
    <div style="margin-top:6px; display:flex; gap:12px; flex-wrap:wrap; align-items:center;">
      <label style="font-size:12px; color:#CCC; cursor:pointer;">
        <input type="checkbox" id="chkReplayBaseAoGravarBracos"> Reproduzir Base ao gravar Braços
      </label>
      <span id="recStatusGeral" style="font-size:12px; font-family:monospace; color:#90CAF9;">💾 Auto-save ativo (Base e Braços separados)</span>
    </div>
  </div>

  <!-- ÁREA 1: PARTES DA BASE -->
  <div class="box box-base" style="text-align:left;">
    <div class="area-topo">
      <span class="area-titulo-base">🔵 ÁREA DA BASE (BaseCenas)</span>
      <div style="display:flex; gap:6px; flex-wrap:wrap;">
        <button class="btn-add btn-sm" onclick="adicionarParteManual('base')">➕ NOVA PARTE BASE</button>
        <button class="btn-play btn-sm" onclick="testarAPartirDaParte('base', 0)">▶ TESTAR BASE</button>
        <button class="btn-sm" onclick="limparCategoria('base')">🗑 LIMPAR BASE</button>
      </div>
    </div>
    <div id="recInfoBase" class="rec-info">⏱ Duração Base: 0.0s | Partes: 0 | Passos: 0</div>

    <table class="tabela-partes">
      <thead>
        <tr>
          <th>PARTES DA BASE (EDITE O CÓDIGO OU COLE COMANDOS DA BASE)</th>
        </tr>
      </thead>
      <tbody id="corpoTabelaPartesBase">
        <tr><td class="vazio-partes">Nenhuma parte da Base gravada. Clique em "GRAVAR BASE (0s)" ou "NOVA PARTE BASE".</td></tr>
      </tbody>
    </table>

    <div class="saida-header">
      <span style="color:#90CAF9; font-weight:bold;">CÓDIGO COMPLETO DA BASE (BaseCenas::atualizarCenas):</span>
      <button id="btnCopiarBase" class="btn-sm" onclick="copiarCodigoCaixa('codeOutBase', 'btnCopiarBase')">📋 COPIAR BASE</button>
    </div>
    <textarea id="codeOutBase" class="rec-box" readonly></textarea>
  </div>

  <!-- ÁREA 2: PARTES DOS BRAÇOS -->
  <div class="box box-bracos" style="text-align:left;">
    <div class="area-topo">
      <span class="area-titulo-bracos">🟣 ÁREA DOS BRAÇOS (BracosCenas)</span>
      <div style="display:flex; gap:6px; flex-wrap:wrap;">
        <button class="btn-add btn-sm" onclick="adicionarParteManual('bracos')">➕ NOVA PARTE BRAÇOS</button>
        <button class="btn-play btn-sm" onclick="testarAPartirDaParte('bracos', 0)">▶ TESTAR BRAÇOS</button>
        <button class="btn-sm" onclick="limparCategoria('bracos')">🗑 LIMPAR BRAÇOS</button>
      </div>
    </div>
    <div id="recInfoBracos" class="rec-info">⏱ Duração Braços: 0.0s | Partes: 0 | Passos: 0</div>

    <table class="tabela-partes bracos">
      <thead>
        <tr>
          <th>PARTES DOS BRAÇOS (EDITE O CÓDIGO OU COLE COMANDOS DOS BRAÇOS)</th>
        </tr>
      </thead>
      <tbody id="corpoTabelaPartesBracos">
        <tr><td class="vazio-partes">Nenhuma parte dos Braços gravada. Clique em "GRAVAR BRAÇOS (0s)" ou "NOVA PARTE BRAÇOS".</td></tr>
      </tbody>
    </table>

    <div class="saida-header">
      <span style="color:#E1BEE7; font-weight:bold;">CÓDIGO COMPLETO DOS BRAÇOS (BracosCenas::atualizarCenas):</span>
      <button id="btnCopiarBracos" class="btn-sm" onclick="copiarCodigoCaixa('codeOutBracos', 'btnCopiarBracos')">📋 COPIAR BRAÇOS</button>
    </div>
    <textarea id="codeOutBracos" class="rec-box bracos" readonly></textarea>
  </div>

  <script>
    let ws = null;
    let timerReconexaoWs = null;
    const kStorageV1Key = "chapeleiro_v2_partes_v1";
    const kStorageV2Key = "chapeleiro_v2_partes_v2";
    const kTicksPorMm = 1.65;
    const kTicksPorGrauOmbro = 3.78;
    const kPassoCotoveloGraus = 30;
    const kPwmMinOmbroUi = 20;
    const kVelMaxCotoveloGrausSeg = 110.0;

    let ticks = [0, 0, 0, 0];
    let ticksOmbroEsq = 0;
    let ticksOmbroDir = 0;
    let gyroAtual = 0;
    let fodAtivo = true;

    // Estado da gravação independente por área ("base" ou "bracos").
    let gravando = false;
    let modoGravacaoAtual = "";
    let inicioRec = 0, m1Start = 0, m2Start = 0, m3Start = 0, m4Start = 0, gyroStart = 0, cmdAtivo = "";
    let movX = 0, movY = 0, movRot = 0, movPicoMag = 0;
    const estadoGravOmbro = {
      e: { ativo: false, ticksStart: 0, inicioTempo: 0 },
      d: { ativo: false, ticksStart: 0, inicioTempo: 0 }
    };
    const estadoGravCotoveloAnalog = {
      ativo: false,
      inicioTempo: 0,
      esqInicial: 0,
      dirInicial: 0
    };

    let contadorSegmentos = 0;
    let segAtualId = 0;
    let segInicioTempo = 0;

    let partesBase = [];
    let partesBracos = [];
    let indiceParteBaseAtiva = -1;
    let indiceParteBracosAtiva = -1;
    let duracaoBaseMs = 0;
    let duracaoBracosMs = 0;
    let timersReplay = [];

    let medindo = false;
    let ticksInicioMedicao = [0, 0, 0, 0];

    const teclasAtivas = {};
    const botoesAtivos = {};
    let cmdManualAtual = "stop";
    const intervaloOmbro = { e: null, d: null };
    const comandoOmbroAtivo = { e: "", d: "" };

    // Modo do controle Logitech FTC: "base" ou "bracos".
    let modoControleGamepad = "base";
    let gpNomeAtivo = "";
    let gpUltimoBotaoInfo = "";
    let gpX = 0, gpY = 0, gpRot = 0;
    let gpBtnAnterior = {};
    let gpUltimoEnvioMs = 0;
    let gpUltimoTickMs = performance.now();
    let gpUltimoX = 0, gpUltimoY = 0, gpUltimoRot = 0;
    let gpOmbroEsqDir = "";
    let gpOmbroDirDir = "";

    // Estado contínuo dos cotovelos para controle proporcional no analógico direito.
    let cotoveloEsqPos = 0;
    let cotoveloDirPos = 0;
    let ultimoCotoveloEsqEnviado = -1;
    let ultimoCotoveloDirEnviado = -1;
    let ultimoEnvioCotoveloAnalogMs = 0;
    let gpCotoveloAnalogAtivo = false;

    // Estado para RB / LB segurado + digitação dos graus no teclado do notebook (no Modo Base).
    let modoGiroDigitado = null;
    let bufferGrausDigitados = "";
    let giroDigitadoCancelado = false;

    function isAcaoBraco(acao) {
      return /\b(moverOmbroEsq|moverOmbroDir|moverCotoveloEsq|moverCotoveloDir)\b/.test(acao || "");
    }

    function obterListaPartes(categoria) {
      return categoria === "bracos" ? partesBracos : partesBase;
    }

    // Definição dos atalhos de funções com descrição dos parâmetros e geração de C++.
    const FUNCOES_ATALHO = {
      run: {
        nome: "run",
        assinatura: "funcoes_.run(distanciaMm, velocidade)",
        descricao: "Anda em linha reta mantendo o ângulo atual travado pelo giroscópio.<br>• <b>distanciaMm:</b> distância em mm (positivo = frente, negativo = ré)<br>• <b>velocidade:</b> força PWM (28 a 255)",
        campos: [
          { id: "p1", label: "distanciaMm", valor: 500, step: "10" },
          { id: "p2", label: "velocidade (28-255)", valor: 90, step: "5" }
        ],
        cmd: (v) => `run:${v[0]},${Math.round(v[1])}`,
        cpp: (v) => `funcoes_.run(${Number(v[0]).toFixed(1)}f, ${Math.round(v[1])});`
      },
      turn: {
        nome: "turn",
        assinatura: "funcoes_.turn(graus, velocidade)",
        descricao: "Gira no próprio eixo a partir da orientação onde o robô está.<br>• <b>graus:</b> ângulo relativo (+ direita/horário, - esquerda/anti-horário)<br>• <b>velocidade:</b> força PWM (28 a 255)",
        campos: [
          { id: "p1", label: "graus (+dir / -esq)", valor: 90, step: "5" },
          { id: "p2", label: "velocidade (28-255)", valor: 100, step: "5" }
        ],
        cmd: (v) => `turn:${v[0]},${Math.round(v[1])}`,
        cpp: (v) => `funcoes_.turn(${Number(v[0]).toFixed(1)}f, ${Math.round(v[1])});`
      },
      turnTo: {
        nome: "turnTo",
        assinatura: "funcoes_.turnTo(anguloAlvo, velocidade)",
        descricao: "Aponta a frente do robô para um ângulo fixo da arena (relativo ao 0° inicial) pelo menor caminho.<br>• <b>anguloAlvo:</b> 0° frente, 90° direita, -90° esquerda, 180° costas<br>• <b>velocidade:</b> força PWM (28 a 255)",
        campos: [
          { id: "p1", label: "anguloAlvo (graus)", valor: 0, step: "5" },
          { id: "p2", label: "velocidade (28-255)", valor: 100, step: "5" }
        ],
        cmd: (v) => `turn_to:${v[0]},${Math.round(v[1])}`,
        cpp: (v) => `funcoes_.turnTo(${Number(v[0]).toFixed(1)}f, ${Math.round(v[1])});`
      },
      strafe: {
        nome: "strafe",
        assinatura: "funcoes_.strafe(distanciaMm, velocidade)",
        descricao: "Desloca lateralmente (caranguejo) mantendo a frente alinhada no giroscópio.<br>• <b>distanciaMm:</b> distância em mm (+ direita, - esquerda)<br>• <b>velocidade:</b> força PWM (28 a 255)",
        campos: [
          { id: "p1", label: "distanciaMm (+dir/-esq)", valor: 300, step: "10" },
          { id: "p2", label: "velocidade (28-255)", valor: 100, step: "5" }
        ],
        cmd: (v) => `strafe:${v[0]},${Math.round(v[1])}`,
        cpp: (v) => `funcoes_.strafe(${Number(v[0]).toFixed(1)}f, ${Math.round(v[1])});`
      },
      diagonal: {
        nome: "diagonal",
        assinatura: "funcoes_.diagonal(distanciaMm, isDireita, velocidade)",
        descricao: "Move em diagonal a 45°.<br>• <b>distanciaMm:</b> distância em mm (+ frente, - ré)<br>• <b>isDireita:</b> 1 = direita (true), 0 = esquerda (false)<br>• <b>velocidade:</b> força PWM (28 a 255)",
        campos: [
          { id: "p1", label: "distanciaMm", valor: 400, step: "10" },
          { id: "p2", label: "isDireita (1 ou 0)", valor: 1, step: "1" },
          { id: "p3", label: "velocidade (28-255)", valor: 95, step: "5" }
        ],
        cmd: (v) => `diag:${v[0]},${Math.round(v[1]) ? 1 : 0},${Math.round(v[2])}`,
        cpp: (v) => `funcoes_.diagonal(${Number(v[0]).toFixed(1)}f, ${Math.round(v[1]) ? "true" : "false"}, ${Math.round(v[2])});`
      },
      ombroEsq: {
        nome: "moverOmbroEsq",
        assinatura: "funcoes_.moverOmbroEsq(graus, velocidade)",
        descricao: "Leva o motor DC do ombro esquerdo até o ângulo alvo (BracosCenas).<br>• <b>graus:</b> 0° (baixado) a 90° (levantado)<br>• <b>velocidade:</b> força PWM (20 a 255)",
        campos: [
          { id: "p1", label: "graus (0 a 90)", valor: 45, step: "5" },
          { id: "p2", label: "velocidade (20-255)", valor: 130, step: "5" }
        ],
        cmd: (v) => `OMB_POS_E:${Number(v[0]).toFixed(1)},${Math.round(v[1])}`,
        cpp: (v) => `funcoes_.moverOmbroEsq(${Number(v[0]).toFixed(1)}f, ${Math.round(v[1])});`
      },
      ombroDir: {
        nome: "moverOmbroDir",
        assinatura: "funcoes_.moverOmbroDir(graus, velocidade)",
        descricao: "Leva o motor DC do ombro direito até o ângulo alvo (BracosCenas).<br>• <b>graus:</b> 0° (baixado) a 90° (levantado)<br>• <b>velocidade:</b> força PWM (20 a 255)",
        campos: [
          { id: "p1", label: "graus (0 a 90)", valor: 45, step: "5" },
          { id: "p2", label: "velocidade (20-255)", valor: 130, step: "5" }
        ],
        cmd: (v) => `OMB_POS_D:${Number(v[0]).toFixed(1)},${Math.round(v[1])}`,
        cpp: (v) => `funcoes_.moverOmbroDir(${Number(v[0]).toFixed(1)}f, ${Math.round(v[1])});`
      },
      cotoveloEsq: {
        nome: "moverCotoveloEsq",
        assinatura: "funcoes_.moverCotoveloEsq(graus)",
        descricao: "Posiciona o servo do cotovelo esquerdo (BracosCenas).<br>• <b>graus:</b> 0° (fechado/recolhido) a 120° (aberto/estendido)",
        campos: [
          { id: "p1", label: "graus (0 a 120)", valor: 60, step: "5" }
        ],
        cmd: (v) => `srv_pos_e:${Math.round(v[0])}`,
        cpp: (v) => `funcoes_.moverCotoveloEsq(${Number(v[0]).toFixed(1)}f);`
      },
      cotoveloDir: {
        nome: "moverCotoveloDir",
        assinatura: "funcoes_.moverCotoveloDir(graus)",
        descricao: "Posiciona o servo do cotovelo direito (BracosCenas).<br>• <b>graus:</b> 0° (fechado/recolhido) a 120° (aberto/estendido)",
        campos: [
          { id: "p1", label: "graus (0 a 120)", valor: 60, step: "5" }
        ],
        cmd: (v) => `srv_pos_d:${Math.round(v[0])}`,
        cpp: (v) => `funcoes_.moverCotoveloDir(${Number(v[0]).toFixed(1)}f);`
      },
      cenario: {
        nome: "enviarCodigoCenario",
        assinatura: "funcoes_.enviarCodigoCenario(codigo, canal)",
        descricao: "Dispara pacote BLE pelo rádio dos braços para o SPIKE externo.<br>• <b>codigo:</b> 2565 (Hora do Chá)<br>• <b>canal:</b> 0 a 255 (padrão 120)",
        campos: [
          { id: "p1", label: "codigo", valor: 2565, step: "1" },
          { id: "p2", label: "canal (0-255)", valor: 120, step: "1" }
        ],
        cmd: (v) => `ble_tx:${Math.round(v[0])},${Math.round(v[1])}`,
        cpp: (v) => `funcoes_.enviarCodigoCenario(${Math.round(v[0])}, ${Math.round(v[1])});`
      }
    };

    let atalhoSelecionadoKey = "run";

    function selecionarAtalhoFuncao(key) {
      atalhoSelecionadoKey = key;
      const f = FUNCOES_ATALHO[key];
      if (!f) return;

      document.getElementById("atalhoAssinatura").innerText = f.assinatura;
      document.getElementById("atalhoDesc").innerHTML = f.descricao;

      document.querySelectorAll("#atalhosPills button").forEach((btn) => {
        btn.classList.toggle("ativo", btn.dataset.key === key);
      });

      const cont = document.getElementById("atalhoCampos");
      cont.innerHTML = "";
      f.campos.forEach((c) => {
        const div = document.createElement("div");
        div.className = "atalho-campo";
        div.innerHTML = `<label for="atalhoIn_${c.id}">${c.label}</label>
          <input type="number" id="atalhoIn_${c.id}" value="${c.valor}" step="${c.step}"
            onkeydown="if(event.key==='Enter'){event.preventDefault();executarAtalhoSelecionado();}">`;
        cont.appendChild(div);
      });
    }

    function lerValoresAtalho() {
      const f = FUNCOES_ATALHO[atalhoSelecionadoKey];
      return f.campos.map((c) => {
        const el = document.getElementById(`atalhoIn_${c.id}`);
        return el ? (parseFloat(el.value) || 0) : 0;
      });
    }

    function executarAtalhoSelecionado() {
      const f = FUNCOES_ATALHO[atalhoSelecionadoKey];
      if (!f) return;
      const vals = lerValoresAtalho();
      sendCmd(f.cmd(vals));
    }

    function copiarCppAtalho() {
      const f = FUNCOES_ATALHO[atalhoSelecionadoKey];
      if (!f) return;
      const linha = f.cpp(lerValoresAtalho());
      navigator.clipboard?.writeText(linha);
      const btn = document.getElementById("btnCopiarCppAtalho");
      if (btn) {
        btn.innerText = "✓ COPIADO!";
        setTimeout(() => { btn.innerText = "📋 COPIAR C++"; }, 1200);
      }
    }

    function inicializarAtalhosFuncoes() {
      const pills = document.getElementById("atalhosPills");
      if (!pills) return;
      pills.innerHTML = "";
      Object.keys(FUNCOES_ATALHO).forEach((key) => {
        const b = document.createElement("button");
        b.type = "button";
        b.className = "atalho-pill";
        b.dataset.key = key;
        b.innerText = FUNCOES_ATALHO[key].nome;
        b.onclick = () => selecionarAtalhoFuncao(key);
        pills.appendChild(b);
      });
      selecionarAtalhoFuncao("run");
    }

    function atualizarStatusWs(conectado) {
      const el = document.getElementById("wsStatus");
      if (!el) return;
      if (conectado) {
        el.innerText = "🟢 Wi-Fi / WS: Conectado ao Robô";
        el.classList.remove("erro");
        el.classList.add("conectado");
      } else {
        el.innerText = "🔴 Wi-Fi / WS: Desconectado (clique p/ reconectar)";
        el.classList.remove("conectado");
        el.classList.add("erro");
      }
    }

    function conectarWebSocket() {
      if (ws && (ws.readyState === WebSocket.OPEN || ws.readyState === WebSocket.CONNECTING)) return;
      if (timerReconexaoWs) {
        clearTimeout(timerReconexaoWs);
        timerReconexaoWs = null;
      }

      ws = new WebSocket(`ws://${window.location.hostname}/ws`);

      ws.onopen = () => {
        atualizarStatusWs(true);
        aplicarForcaBase(document.getElementById("forcaBase").value);
        aplicarRampa();
        aplicarForcaBraco(document.getElementById("forcaBraco").value);
        wsSend(`fod:${fodAtivo ? 1 : 0}`);
      };

      ws.onclose = () => {
        atualizarStatusWs(false);
        timerReconexaoWs = setTimeout(conectarWebSocket, 2000);
      };

      ws.onerror = () => {
        atualizarStatusWs(false);
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
    }

    function wsSend(msg) {
      if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send(msg);
        return true;
      }
      return false;
    }

    conectarWebSocket();
    inicializarAtalhosFuncoes();

    function enviarCodigoCha() {
      const canal = Math.max(0, Math.min(255, parseInt(document.getElementById("canalTea").value) || 120));
      document.getElementById("canalTea").value = canal;
      sendCmd(`ble_tx:2565,${canal}`);
    }

    function alternarModoControleGamepad() {
      if (modoGiroDigitado !== null) {
        modoGiroDigitado = null;
        bufferGrausDigitados = "";
        giroDigitadoCancelado = false;
        atualizarVisorGiroDigitado();
      }
      gpX = gpY = gpRot = 0;
      sincronizarMovimentoManual();
      if (gpOmbroEsqDir) { pararOmbro("e"); gpOmbroEsqDir = ""; }
      if (gpOmbroDirDir) { pararOmbro("d"); gpOmbroDirDir = ""; }
      if (gpCotoveloAnalogAtivo) {
        gpCotoveloAnalogAtivo = false;
        fecharSegmentoCotoveloAnalog(Date.now() - inicioRec);
      }

      modoControleGamepad = (modoControleGamepad === "base") ? "bracos" : "base";
      const btn = document.getElementById("btnModoControle");
      const leg = document.getElementById("gamepadLegenda");

      if (modoControleGamepad === "base") {
        btn.innerText = "🎮 CONTROLE: BASE (Tab p/ Braços)";
        btn.className = "btn-modo-base";
        leg.innerText = "BASE | Y:0° B:+90° A:180° X:-90° | D-Pad ◄►: Giro | Segure LB/RB + Teclado: ±X°";
      } else {
        btn.innerText = "🦾 CONTROLE: BRAÇOS (Tab p/ Base)";
        btn.className = "btn-modo-bracos";
        leg.innerText = "BRAÇOS | Analóg. Dir ↕: Braço Esq | Y/B: Esq ▲▼ | X/A: Dir ▲▼ | RB/RT: Cotov. Esq ±30° | LB/LT: Cotov. Dir ±30° | D-Pad: Ambos Cotov. 0°/30°/60°/120°";
      }
      gpUltimoBotaoInfo = `Modo ${modoControleGamepad.toUpperCase()}`;
    }

    function aplicarZonaMorta(valor, zonaMorta = 0.12) {
      if (Math.abs(valor) < zonaMorta) return 0;
      const sinal = Math.sign(valor);
      return sinal * ((Math.abs(valor) - zonaMorta) / (1.0 - zonaMorta));
    }

    function isBtnPressed(gp, idx) {
      const b = gp.buttons && gp.buttons[idx];
      return !!(b && (b.pressed || b.value > 0.35));
    }

    function definirCotovelo(lado, graus) {
      const g = Math.max(0, Math.min(120, Math.round(parseFloat(graus) || 0)));
      const inputId = (lado === "e") ? "srvE" : "srvD";
      document.getElementById(inputId).value = g;
      if (lado === "e") {
        cotoveloEsqPos = g;
        ultimoCotoveloEsqEnviado = g;
      } else {
        cotoveloDirPos = g;
        ultimoCotoveloDirEnviado = g;
      }
      sendCmd(`srv_pos_${lado}:${g}`);
    }

    function fecharSegmentoCotoveloAnalog(tempo) {
      if (!estadoGravCotoveloAnalog.ativo || !gravando) {
        estadoGravCotoveloAnalog.ativo = false;
        return;
      }
      const parteBr = obterParteBracosParaGravacao();
      const esqFinal = Math.round(cotoveloEsqPos);
      const dirFinal = Math.round(cotoveloDirPos);

      if (esqFinal !== estadoGravCotoveloAnalog.esqInicial) {
        registrarPassoNaParte("bracos", parteBr, estadoGravCotoveloAnalog.inicioTempo, `funcoes_.moverCotoveloEsq(${esqFinal.toFixed(1)}f);`, tempo);
      }
      if (dirFinal !== estadoGravCotoveloAnalog.dirInicial) {
        registrarPassoNaParte("bracos", parteBr, estadoGravCotoveloAnalog.inicioTempo, `funcoes_.moverCotoveloDir(${dirFinal.toFixed(1)}f);`, tempo);
      }
      estadoGravCotoveloAnalog.ativo = false;
    }

    function atualizarCotovelosProporcional(ryCotovelo, dt, agora) {
      if (ryCotovelo === 0) {
        if (gpCotoveloAnalogAtivo) {
          gpCotoveloAnalogAtivo = false;
          fecharSegmentoCotoveloAnalog(Date.now() - inicioRec);
        }
        return false;
      }

      if (!gpCotoveloAnalogAtivo) {
        gpCotoveloAnalogAtivo = true;
        cotoveloEsqPos = Math.max(0, Math.min(120, parseFloat(document.getElementById("srvE").value) || 0));
        cotoveloDirPos = Math.max(0, Math.min(120, parseFloat(document.getElementById("srvD").value) || 0));
        if (gravando) {
          estadoGravCotoveloAnalog.ativo = true;
          estadoGravCotoveloAnalog.inicioTempo = Date.now() - inicioRec;
          estadoGravCotoveloAnalog.esqInicial = Math.round(cotoveloEsqPos);
          estadoGravCotoveloAnalog.dirInicial = Math.round(cotoveloDirPos);
        }
      }

      const delta = ryCotovelo * kVelMaxCotoveloGrausSeg * dt;
      cotoveloEsqPos = Math.max(0, Math.min(120, cotoveloEsqPos + delta));
      cotoveloDirPos = Math.max(0, Math.min(120, cotoveloDirPos + delta));

      const intE = Math.round(cotoveloEsqPos);
      const intD = Math.round(cotoveloDirPos);
      document.getElementById("srvE").value = intE;
      document.getElementById("srvD").value = intD;

      if (agora - ultimoEnvioCotoveloAnalogMs >= 45) {
        const tempoGrav = Date.now() - inicioRec;
        const parteBr = gravando ? obterParteBracosParaGravacao() : null;

        if (intE !== ultimoCotoveloEsqEnviado) {
          ultimoCotoveloEsqEnviado = intE;
          const cmdE = `srv_pos_e:${intE}`;
          wsSend(cmdE);
          if (parteBr) parteBr.eventos.push({ tempo: tempoGrav, tipo: "raw", cmd: cmdE });
        }
        if (intD !== ultimoCotoveloDirEnviado) {
          ultimoCotoveloDirEnviado = intD;
          const cmdD = `srv_pos_d:${intD}`;
          wsSend(cmdD);
          if (parteBr) parteBr.eventos.push({ tempo: tempoGrav, tipo: "raw", cmd: cmdD });
        }
        ultimoEnvioCotoveloAnalogMs = agora;
      }
      return true;
    }

    function moverDegrauCotovelo(lado, deltaPasso) {
      const inputId = (lado === "e") ? "srvE" : "srvD";
      const atual = parseInt(document.getElementById(inputId).value) || 0;
      const novoGrau = Math.max(0, Math.min(120, atual + (deltaPasso * kPassoCotoveloGraus)));
      definirCotovelo(lado, novoGrau);
    }

    function atualizarVisorGiroDigitado() {
      const visor = document.getElementById("visorGiroDigitado");
      const statusEl = document.getElementById("gamepadStatus");
      if (!modoGiroDigitado) {
        if (visor) visor.style.display = "none";
        if (statusEl) statusEl.classList.remove("digitando");
        return;
      }

      const sinal = (modoGiroDigitado === "rb") ? "+" : "-";
      const btnNome = (modoGiroDigitado === "rb") ? "RB (Horário)" : "LB (Anti-horário)";
      const textoVal = giroDigitadoCancelado
        ? "CANCELADO (solte o botão)"
        : (bufferGrausDigitados ? `${sinal}${bufferGrausDigitados}° (solte p/ girar)` : "digite os graus no teclado...");

      if (visor) {
        visor.style.display = "block";
        visor.innerText = `⌨️ ${btnNome}: ${textoVal}`;
      }
      if (statusEl) {
        statusEl.classList.add("digitando");
        statusEl.innerText = `⌨️ ${btnNome}: ${textoVal}`;
      }
    }

    function finalizarGiroDigitado() {
      const modo = modoGiroDigitado;
      const raw = bufferGrausDigitados;
      const cancelado = giroDigitadoCancelado;

      modoGiroDigitado = null;
      bufferGrausDigitados = "";
      giroDigitadoCancelado = false;
      atualizarVisorGiroDigitado();

      if (!modo || cancelado || !raw) {
        gpUltimoBotaoInfo = `${modo === "rb" ? "RB" : "LB"}: Cancelado`;
        return;
      }

      const graus = parseFloat(raw);
      if (isNaN(graus) || graus <= 0) {
        gpUltimoBotaoInfo = `${modo === "rb" ? "RB" : "LB"}: Valor inválido`;
        return;
      }

      const grausFinal = Math.min(360, graus);
      const direcao = (modo === "rb") ? 1 : -1;
      document.getElementById("anguloGiro").value = grausFinal;
      executarGiroSeta(direcao);
      gpUltimoBotaoInfo = `${modo === "rb" ? "RB" : "LB"}: ${direcao > 0 ? "+" : "-"}${grausFinal}°`;
    }

    function atualizarStatusGamepad(gp, infoExtra = "") {
      const el = document.getElementById("gamepadStatus");
      if (!el) return;
      if (modoGiroDigitado) {
        atualizarVisorGiroDigitado();
        return;
      }
      if (gp) {
        gpNomeAtivo = gp.id.substring(0, 22);
        if (infoExtra) gpUltimoBotaoInfo = infoExtra;
        el.innerText = `🎮 [${modoControleGamepad.toUpperCase()}] ${gpNomeAtivo}${gpUltimoBotaoInfo ? " | " + gpUltimoBotaoInfo : ""}`;
        el.classList.add("conectado");
      } else {
        gpNomeAtivo = "";
        gpUltimoBotaoInfo = "";
        el.innerText = "🎮 Controle: Desconectado (aperte um botão)";
        el.classList.remove("conectado");
      }
    }

    window.addEventListener("gamepadconnected", (e) => atualizarStatusGamepad(e.gamepad));
    window.addEventListener("gamepaddisconnected", () => {
      modoGiroDigitado = null;
      bufferGrausDigitados = "";
      giroDigitadoCancelado = false;
      atualizarVisorGiroDigitado();
      atualizarStatusGamepad(null);
      gpX = gpY = gpRot = 0;
      if (gpOmbroEsqDir) { pararOmbro("e"); gpOmbroEsqDir = ""; }
      if (gpOmbroDirDir) { pararOmbro("d"); gpOmbroDirDir = ""; }
      if (gpCotoveloAnalogAtivo) {
        gpCotoveloAnalogAtivo = false;
        fecharSegmentoCotoveloAnalog(Date.now() - inicioRec);
      }
      gpBtnAnterior = {};
      sincronizarMovimentoManual();
    });

    function lerGamepadLoop() {
      const agoraLoop = performance.now();
      const dtSeg = Math.min(0.1, Math.max(0.001, (agoraLoop - gpUltimoTickMs) / 1000.0));
      gpUltimoTickMs = agoraLoop;

      const lista = navigator.getGamepads ? navigator.getGamepads() : [];
      let gp = null;
      for (let i = 0; i < lista.length; i++) {
        if (lista[i] && lista[i].connected) { gp = lista[i]; break; }
      }

      if (gp) {
        if (!gpNomeAtivo) atualizarStatusGamepad(gp);

        const btnA = isBtnPressed(gp, 0);
        const btnB = isBtnPressed(gp, 1);
        const btnX = isBtnPressed(gp, 2);
        const btnY = isBtnPressed(gp, 3);
        const btnLB = isBtnPressed(gp, 4);
        const btnRB = isBtnPressed(gp, 5);
        const btnLT = isBtnPressed(gp, 6);
        const btnRT = isBtnPressed(gp, 7);

        let dpadCima = isBtnPressed(gp, 12);
        let dpadBaixo = isBtnPressed(gp, 13);
        let dpadEsq = isBtnPressed(gp, 14);
        let dpadDir = isBtnPressed(gp, 15);
        if (!dpadCima && !dpadBaixo && !dpadEsq && !dpadDir && gp.axes.length > 9) {
          const pov = gp.axes[9];
          if (pov <= -0.85) dpadCima = true;
          if (pov >= 0.05 && pov <= 0.25) dpadBaixo = true;
          if (pov >= 0.65 && pov <= 0.80) dpadEsq = true;
          if (pov >= -0.50 && pov <= -0.35) dpadDir = true;
        }

        if (modoControleGamepad === "base") {
          let lx = aplicarZonaMorta(gp.axes[0] || 0);
          let ly = aplicarZonaMorta(-(gp.axes[1] || 0));
          const magL = Math.hypot(lx, ly);
          if (magL > 1.0) { lx /= magL; ly /= magL; }
          const rx = aplicarZonaMorta(gp.axes[2] || 0);
          gpX = lx;
          gpY = ly;
          gpRot = rx;

          if (btnRB && !gpBtnAnterior.rb && modoGiroDigitado === null) {
            modoGiroDigitado = "rb";
            bufferGrausDigitados = "";
            giroDigitadoCancelado = false;
            atualizarVisorGiroDigitado();
          } else if (!btnRB && gpBtnAnterior.rb && modoGiroDigitado === "rb") {
            finalizarGiroDigitado();
            atualizarStatusGamepad(gp);
          }

          if (btnLB && !gpBtnAnterior.lb && modoGiroDigitado === null) {
            modoGiroDigitado = "lb";
            bufferGrausDigitados = "";
            giroDigitadoCancelado = false;
            atualizarVisorGiroDigitado();
          } else if (!btnLB && gpBtnAnterior.lb && modoGiroDigitado === "lb") {
            finalizarGiroDigitado();
            atualizarStatusGamepad(gp);
          }

          if (btnY && !gpBtnAnterior.y) { executarGiroAbsoluto(0); atualizarStatusGamepad(gp, "Y: 0°"); }
          if (btnB && !gpBtnAnterior.b) { executarGiroAbsoluto(90); atualizarStatusGamepad(gp, "B: +90°"); }
          if (btnA && !gpBtnAnterior.a) { executarGiroAbsoluto(180); atualizarStatusGamepad(gp, "A: 180°"); }
          if (btnX && !gpBtnAnterior.x) { executarGiroAbsoluto(-90); atualizarStatusGamepad(gp, "X: -90°"); }

          if (dpadEsq && !gpBtnAnterior.dEsq) { executarGiroSeta(-1); atualizarStatusGamepad(gp, "D-Pad ◄"); }
          if (dpadDir && !gpBtnAnterior.dDir) { executarGiroSeta(1); atualizarStatusGamepad(gp, "D-Pad ►"); }

          const mudou = Math.abs(gpX - gpUltimoX) >= 0.03 ||
                        Math.abs(gpY - gpUltimoY) >= 0.03 ||
                        Math.abs(gpRot - gpUltimoRot) >= 0.03 ||
                        ((gpX === 0 && gpY === 0 && gpRot === 0) && (gpUltimoX !== 0 || gpUltimoY !== 0 || gpUltimoRot !== 0));

          if (mudou && (agoraLoop - gpUltimoEnvioMs >= 25)) {
            gpUltimoX = gpX;
            gpUltimoY = gpY;
            gpUltimoRot = gpRot;
            gpUltimoEnvioMs = agoraLoop;
            sincronizarMovimentoManual();
          }
        } else {
          // 1. Analógico direito (booleano): sobe/desce o braço (ombro) esquerdo.
          const ryBracoEsq = aplicarZonaMorta(-(gp.axes[3] || 0), 0.25);
          const dirAnalogEsq = (ryBracoEsq > 0) ? "up" : (ryBracoEsq < 0 ? "dwn" : "");

          // 2. Botões individuais dos ombros: Y sobe / B desce Esq; X sobe / A desce Dir.
          const dirBtnEsq = btnY ? "up" : (btnB ? "dwn" : "");
          const dirBtnDir = btnX ? "up" : (btnA ? "dwn" : "");

          const novaDirE = dirAnalogEsq || dirBtnEsq;
          const novaDirD = dirBtnDir;

          if (novaDirE !== gpOmbroEsqDir) {
            gpOmbroEsqDir = novaDirE;
            if (novaDirE) {
              iniciarOmbro("e", novaDirE);
              if (dirAnalogEsq) {
                atualizarStatusGamepad(gp, `Analóg. Dir: Braço Esq ${novaDirE === "up" ? "▲" : "▼"}`);
              } else {
                atualizarStatusGamepad(gp, `${novaDirE === "up" ? "Y: Ombro Esq ▲" : "B: Ombro Esq ▼"}`);
              }
            } else {
              pararOmbro("e");
            }
          }

          if (novaDirD !== gpOmbroDirDir) {
            gpOmbroDirDir = novaDirD;
            if (novaDirD) {
              iniciarOmbro("d", novaDirD);
              atualizarStatusGamepad(gp, `${novaDirD === "up" ? "X: Ombro Dir ▲" : "A: Ombro Dir ▼"}`);
            } else {
              pararOmbro("d");
            }
          }

          // 3. Gatilhos e Bumpers invertidos (±30°): R (RB/RT) -> Cotovelo Esquerdo; L (LB/LT) -> Cotovelo Direito.
          if (btnRB && !gpBtnAnterior.rb) {
            moverDegrauCotovelo("e", -1);
            atualizarStatusGamepad(gp, `RB: Cotov. Esq ${document.getElementById("srvE").value}°`);
          }
          if (btnRT && !gpBtnAnterior.rt) {
            moverDegrauCotovelo("e", 1);
            atualizarStatusGamepad(gp, `RT: Cotov. Esq ${document.getElementById("srvE").value}°`);
          }

          if (btnLB && !gpBtnAnterior.lb) {
            moverDegrauCotovelo("d", -1);
            atualizarStatusGamepad(gp, `LB: Cotov. Dir ${document.getElementById("srvD").value}°`);
          }
          if (btnLT && !gpBtnAnterior.lt) {
            moverDegrauCotovelo("d", 1);
            atualizarStatusGamepad(gp, `LT: Cotov. Dir ${document.getElementById("srvD").value}°`);
          }

          // 4. D-Pad: posições fixas dos dois cotovelos ao mesmo tempo (Up: 120°, Left: 30°, Right: 60°, Down: 0°).
          if (dpadCima && !gpBtnAnterior.dCima) {
            definirCotovelo("e", 120);
            definirCotovelo("d", 120);
            atualizarStatusGamepad(gp, "D-Pad ▲: Ambos Cotov. 120°");
          }
          if (dpadEsq && !gpBtnAnterior.dEsq) {
            definirCotovelo("e", 30);
            definirCotovelo("d", 30);
            atualizarStatusGamepad(gp, "D-Pad ◄: Ambos Cotov. 30°");
          }
          if (dpadDir && !gpBtnAnterior.dDir) {
            definirCotovelo("e", 60);
            definirCotovelo("d", 60);
            atualizarStatusGamepad(gp, "D-Pad ►: Ambos Cotov. 60°");
          }
          if (dpadBaixo && !gpBtnAnterior.dBaixo) {
            definirCotovelo("e", 0);
            definirCotovelo("d", 0);
            atualizarStatusGamepad(gp, "D-Pad ▼: Ambos Cotov. 0°");
          }
        }

        gpBtnAnterior = {
          a: btnA, b: btnB, x: btnX, y: btnY,
          lb: btnLB, rb: btnRB, lt: btnLT, rt: btnRT,
          dCima: dpadCima, dBaixo: dpadBaixo,
          dEsq: dpadEsq, dDir: dpadDir
        };
      }

      requestAnimationFrame(lerGamepadLoop);
    }
    requestAnimationFrame(lerGamepadLoop);

    function obterParteBracosParaGravacao() {
      if (indiceParteBracosAtiva >= 0 && partesBracos[indiceParteBracosAtiva]) {
        return partesBracos[indiceParteBracosAtiva];
      }
      partesBracos.push({
        inicioMs: 0,
        fimMs: 0,
        passos: [],
        eventos: [],
        editadoManualmente: false
      });
      indiceParteBracosAtiva = partesBracos.length - 1;
      return partesBracos[indiceParteBracosAtiva];
    }

    function obterParteBaseParaGravacao() {
      if (indiceParteBaseAtiva >= 0 && partesBase[indiceParteBaseAtiva]) {
        return partesBase[indiceParteBaseAtiva];
      }
      return null;
    }

    function fecharSegmentoOmbro(lado, tempo) {
      const est = estadoGravOmbro[lado];
      if (!est || !est.ativo || !gravando) return;

      const ticksFim = (lado === "e") ? ticksOmbroEsq : ticksOmbroDir;
      const graus = Math.max(0.0, Math.min(90.0, ticksFim / kTicksPorGrauOmbro)).toFixed(1);
      const velBraco = parseInt(document.getElementById("forcaBraco").value) || 130;
      const parte = obterParteBracosParaGravacao();

      if (lado === "e") {
        registrarPassoNaParte("bracos", parte, est.inicioTempo, `funcoes_.moverOmbroEsq(${graus}f, ${velBraco});`, tempo);
        parte.eventos.push({ tempo: est.inicioTempo, tipo: "raw", cmd: `OMB_POS_E:${graus},${velBraco}` });
      } else {
        registrarPassoNaParte("bracos", parte, est.inicioTempo, `funcoes_.moverOmbroDir(${graus}f, ${velBraco});`, tempo);
        parte.eventos.push({ tempo: est.inicioTempo, tipo: "raw", cmd: `OMB_POS_D:${graus},${velBraco}` });
      }
      est.ativo = false;
    }

    function iniciarOmbro(lado, dir) {
      pararOmbro(lado, false);
      const cmd = `b${lado}_${dir}`;
      comandoOmbroAtivo[lado] = cmd;

      if (gravando) {
        const est = estadoGravOmbro[lado];
        est.ativo = true;
        est.ticksStart = (lado === "e") ? ticksOmbroEsq : ticksOmbroDir;
        est.inicioTempo = Date.now() - inicioRec;
      }
      sendCmd(cmd);
      intervaloOmbro[lado] = setInterval(() => sendCmd(cmd), 150);
    }

    function pararOmbro(lado, enviarParada = true) {
      comandoOmbroAtivo[lado] = "";
      if (intervaloOmbro[lado]) {
        clearInterval(intervaloOmbro[lado]);
        intervaloOmbro[lado] = null;
      }
      if (gravando && estadoGravOmbro[lado].ativo) {
        fecharSegmentoOmbro(lado, Date.now() - inicioRec);
      }
      if (enviarParada) {
        sendCmd(lado === "e" ? "BE_STP" : "BD_STP");
      }
    }

    window.addEventListener("mouseup", () => {
      if (intervaloOmbro.e) pararOmbro("e");
      if (intervaloOmbro.d) pararOmbro("d");
      if (Object.keys(botoesAtivos).length > 0) {
        for (const k in botoesAtivos) delete botoesAtivos[k];
        sincronizarMovimentoManual();
      }
    });

    window.addEventListener("touchend", () => {
      if (intervaloOmbro.e) pararOmbro("e");
      if (intervaloOmbro.d) pararOmbro("d");
    });

    window.addEventListener("blur", () => {
      for (const k in teclasAtivas) delete teclasAtivas[k];
      for (const k in botoesAtivos) delete botoesAtivos[k];
      if (intervaloOmbro.e) pararOmbro("e");
      if (intervaloOmbro.d) pararOmbro("d");
      gpX = gpY = gpRot = 0;
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
      wsSend(`vel:${v}`);
    }

    function aplicarRampa() {
      const acel = Math.max(1, Math.min(255, parseInt(document.getElementById("passoAcel").value) || 8));
      const freio = Math.max(1, Math.min(255, parseInt(document.getElementById("passoFreio").value) || 15));
      wsSend(`acel:${acel}`);
      wsSend(`freio:${freio}`);
    }

    function alterarForcaBase(delta) {
      const atual = parseInt(document.getElementById("forcaBase").value) || 0;
      aplicarForcaBase(atual + delta);
    }

    function aplicarForcaBraco(valor) {
      const parsed = parseInt(valor);
      const base = isNaN(parsed) ? 130 : parsed;
      const v = Math.max(kPwmMinOmbroUi, Math.min(255, base));
      document.getElementById("forcaBraco").value = v;
      wsSend(`vel_braco:${v}`);
    }

    function alterarForcaBraco(delta) {
      const atual = parseInt(document.getElementById("forcaBraco").value);
      const base = isNaN(atual) ? 130 : atual;
      aplicarForcaBraco(base + delta);
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
      sendCmd(`turn_to:${alvo},${vel}`);
    }

    function cancelarReplay() {
      timersReplay.forEach((id) => clearTimeout(id));
      timersReplay = [];
    }

    function salvarPartesNoNavegador() {
      try {
        localStorage.setItem(kStorageV2Key, JSON.stringify({
          base: partesBase,
          bracos: partesBracos
        }));
      } catch (e) {}
    }

    function carregarPartesDoNavegador() {
      try {
        const brutoV2 = localStorage.getItem(kStorageV2Key);
        if (brutoV2) {
          const dados = JSON.parse(brutoV2);
          partesBase = Array.isArray(dados.base) ? dados.base : [];
          partesBracos = Array.isArray(dados.bracos) ? dados.bracos : [];
        } else {
          const brutoV1 = localStorage.getItem(kStorageV1Key);
          if (brutoV1) {
            const antigas = JSON.parse(brutoV1);
            if (Array.isArray(antigas)) {
              antigas.forEach((p) => {
                const passos = p.passos || [];
                const pBase = passos.filter((x) => !isAcaoBraco(x.acao));
                const pBracos = passos.filter((x) => isAcaoBraco(x.acao));
                if (pBase.length > 0 || pBracos.length === 0) {
                  partesBase.push({
                    inicioMs: pBase.length ? pBase[0].tempo : (p.inicioMs || 0),
                    fimMs: pBase.length ? pBase[pBase.length - 1].tempo : (p.fimMs || 0),
                    passos: pBase,
                    eventos: (p.eventos || []).filter((e) => !/^OMB_POS_|^srv_pos_/.test(e.cmd || "")),
                    editadoManualmente: !!p.editadoManualmente
                  });
                }
                if (pBracos.length > 0) {
                  partesBracos.push({
                    inicioMs: pBracos[0].tempo,
                    fimMs: pBracos[pBracos.length - 1].tempo,
                    passos: pBracos,
                    eventos: (p.eventos || []).filter((e) => /^OMB_POS_|^srv_pos_/.test(e.cmd || "")),
                    editadoManualmente: !!p.editadoManualmente
                  });
                }
              });
              salvarPartesNoNavegador();
            }
          }
        }
        recalcularLinhasDoTempo();
        renderizarTabelaCategoria("base");
        renderizarTabelaCategoria("bracos");
        atualizarTextosRoteirosCompletos();
        atualizarBotoesGravacao();
      } catch (e) {}
    }

    function formatarCodigoDaParte(passos, offsetIndice = 0) {
      if (!passos || passos.length === 0) return "";
      return passos.map((p, idx) =>
        `if (passoAtual_ == ${offsetIndice + idx} && decorridoMs >= ${p.tempo}) {\n    ${p.acao}\n    passoAtual_++;\n}`
      ).join("\n");
    }

    function normalizarChamadaCpp(linha) {
      let l = linha.replace(/\/\/.*$/, "").trim();
      if (!l || l.startsWith("passoAtual")) return "";
      if (l.startsWith("funcoes_.")) {
        return l.endsWith(";") ? l : `${l};`;
      }
      if (/^(run|turn|turnTo|strafe|diagonal|moverGirando|moverOmbroEsq|moverOmbroDir|moverCotoveloEsq|moverCotoveloDir|enviarCodigoCenario)\s*\(/.test(l)) {
        const comPrefixo = `funcoes_.${l}`;
        return comPrefixo.endsWith(";") ? comPrefixo : `${comPrefixo};`;
      }
      return "";
    }

    function parsearCodigoCpp(texto, tempoMinimoSugerido = 0) {
      const regexBloco = /if\s*\([^)]*decorridoMs\s*>=\s*(\d+)[^)]*\)\s*\{([\s\S]*?)\}/g;
      const encontrados = [];
      let match;

      while ((match = regexBloco.exec(texto)) !== null) {
        const tempo = parseInt(match[1], 10) || 0;
        const corpo = match[2];
        const linhas = corpo.split("\n");
        for (const raw of linhas) {
          const acao = normalizarChamadaCpp(raw);
          if (acao) encontrados.push({ tempo, acao });
        }
      }

      if (encontrados.length === 0) {
        const linhasSoltas = texto.split("\n");
        for (const raw of linhasSoltas) {
          const acao = normalizarChamadaCpp(raw);
          if (acao) {
            encontrados.push({ tempo: tempoMinimoSugerido, acao });
          }
        }
      }

      encontrados.sort((a, b) => a.tempo - b.tempo);
      return encontrados;
    }

    function calcularDuracaoLista(lista) {
      let maxTempo = 0;
      lista.forEach((p) => {
        if (p.passos && p.passos.length > 0) {
          const ultimo = p.passos[p.passos.length - 1].tempo;
          p.inicioMs = p.passos[0].tempo;
          p.fimMs = Math.max(p.fimMs || 0, ultimo);
        } else {
          p.inicioMs = p.inicioMs || 0;
          p.fimMs = p.fimMs || 0;
        }
        if (p.fimMs > maxTempo) maxTempo = p.fimMs;
      });
      return maxTempo;
    }

    function recalcularLinhasDoTempo() {
      duracaoBaseMs = calcularDuracaoLista(partesBase);
      duracaoBracosMs = calcularDuracaoLista(partesBracos);
    }

    function atualizarBotoesGravacao() {
      const btnBaseNovo = document.getElementById("btnRecBaseNovo");
      const btnBaseCont = document.getElementById("btnRecBaseCont");
      const btnBracosNovo = document.getElementById("btnRecBracosNovo");
      const btnBracosCont = document.getElementById("btnRecBracosCont");

      const passosBaseCount = partesBase.reduce((acc, p) => acc + (p.passos ? p.passos.length : 0), 0);
      const passosBracosCount = partesBracos.reduce((acc, p) => acc + (p.passos ? p.passos.length : 0), 0);
      const temBase = partesBase.length > 0 && (passosBaseCount > 0 || duracaoBaseMs > 0);
      const temBracos = partesBracos.length > 0 && (passosBracosCount > 0 || duracaoBracosMs > 0);

      btnBaseNovo.innerText = "🔴 GRAVAR BASE (0s)";
      btnBaseCont.innerText = "⏭ CONTINUAR BASE";
      btnBracosNovo.innerText = "🦾 GRAVAR BRAÇOS (0s)";
      btnBracosCont.innerText = "⏭ CONTINUAR BRAÇOS";
      [btnBaseNovo, btnBaseCont, btnBracosNovo, btnBracosCont].forEach((b) => b.classList.remove("ativo"));

      if (gravando) {
        btnBaseNovo.disabled = modoGravacaoAtual !== "base_novo";
        btnBaseCont.disabled = modoGravacaoAtual !== "base_continuar";
        btnBracosNovo.disabled = modoGravacaoAtual !== "bracos_novo";
        btnBracosCont.disabled = modoGravacaoAtual !== "bracos_continuar";

        const mapaAtivo = {
          base_novo: btnBaseNovo,
          base_continuar: btnBaseCont,
          bracos_novo: btnBracosNovo,
          bracos_continuar: btnBracosCont
        };
        const ativoBtn = mapaAtivo[modoGravacaoAtual];
        if (ativoBtn) {
          ativoBtn.innerText = "⏹ PARAR GRAVAÇÃO";
          ativoBtn.classList.add("ativo");
        }
      } else {
        btnBaseNovo.disabled = false;
        btnBaseCont.disabled = !temBase;
        btnBracosNovo.disabled = false;
        btnBracosCont.disabled = !temBracos;
      }

      const infoBase = document.getElementById("recInfoBase");
      if (infoBase) {
        infoBase.innerText = `⏱ Duração Base: ${(duracaoBaseMs / 1000).toFixed(1)}s | Partes: ${partesBase.length} | Passos: ${passosBaseCount}`;
      }
      const infoBracos = document.getElementById("recInfoBracos");
      if (infoBracos) {
        infoBracos.innerText = `⏱ Duração Braços: ${(duracaoBracosMs / 1000).toFixed(1)}s | Partes: ${partesBracos.length} | Passos: ${passosBracosCount}`;
      }
    }

    function renderizarTabelaCategoria(categoria) {
      const ehBracos = categoria === "bracos";
      const tbody = document.getElementById(ehBracos ? "corpoTabelaPartesBracos" : "corpoTabelaPartesBase");
      const lista = obterListaPartes(categoria);
      if (!tbody) return;

      if (lista.length === 0) {
        const msgVazia = ehBracos
          ? `Nenhuma parte dos Braços gravada. Clique em "GRAVAR BRAÇOS (0s)" ou "NOVA PARTE BRAÇOS".`
          : `Nenhuma parte da Base gravada. Clique em "GRAVAR BASE (0s)" ou "NOVA PARTE BASE".`;
        tbody.innerHTML = `<tr><td class="vazio-partes">${msgVazia}</td></tr>`;
        return;
      }

      const badgeHtml = ehBracos
        ? `<span class="parte-badge badge-bracos">BRAÇOS</span>`
        : `<span class="parte-badge badge-base">BASE</span>`;
      const classeCode = ehBracos ? "parte-code bracos" : "parte-code";
      const placeholder = ehBracos
        ? "Cole ou edite comandos dos Braços (moverOmbroEsq, moverCotoveloDir...) aqui..."
        : "Cole ou edite comandos da Base (run, turn, turnTo...) aqui...";

      let offsetPasso = 0;
      tbody.innerHTML = lista.map((parte, idx) => {
        const qtdPassos = parte.passos ? parte.passos.length : 0;
        const iniSeg = ((parte.inicioMs || 0) / 1000).toFixed(1);
        const fimSeg = ((parte.fimMs || 0) / 1000).toFixed(1);
        const textoExibir = (parte.textoManual !== undefined && parte.textoManual !== null)
          ? parte.textoManual
          : formatarCodigoDaParte(parte.passos, offsetPasso);
        offsetPasso += qtdPassos;

        return `
          <tr>
            <td>
              <div class="parte-topo">
                <div>
                  <span class="parte-titulo">Parte ${idx + 1}</span>
                  ${badgeHtml}
                  <span class="parte-meta">${iniSeg}s → ${fimSeg}s | ${qtdPassos} passo(s)</span>
                </div>
                <div class="parte-acoes">
                  <button class="btn-play btn-sm" onclick="testarAPartirDaParte('${categoria}', ${idx})">▶ PLAY DAQUI</button>
                  <button class="btn-rec btn-sm" onclick="excluirParte('${categoria}', ${idx})">🗑 EXCLUIR</button>
                </div>
              </div>
              <textarea class="${classeCode}" placeholder="${placeholder}" oninput="aoEditarCodigoParte('${categoria}', ${idx}, this.value)">${textoExibir}</textarea>
            </td>
          </tr>
        `;
      }).join("");
    }

    function aoEditarCodigoParte(categoria, idx, novoTexto) {
      const lista = obterListaPartes(categoria);
      const parte = lista[idx];
      if (!parte) return;

      parte.textoManual = novoTexto;
      parte.editadoManualmente = true;
      const tempoAnterior = idx > 0 ? (lista[idx - 1].fimMs || 0) : 0;
      const passosParseados = parsearCodigoCpp(novoTexto, tempoAnterior);
      parte.passos = passosParseados;
      parte.eventos = [];

      if (passosParseados.length > 0) {
        parte.inicioMs = passosParseados[0].tempo;
        parte.fimMs = passosParseados[passosParseados.length - 1].tempo + 600;
      } else {
        parte.inicioMs = tempoAnterior;
        parte.fimMs = tempoAnterior;
      }

      recalcularLinhasDoTempo();
      atualizarTextosRoteirosCompletos();
      atualizarBotoesGravacao();
      salvarPartesNoNavegador();

      const tbodyId = categoria === "bracos" ? "corpoTabelaPartesBracos" : "corpoTabelaPartesBase";
      const metas = document.querySelectorAll(`#${tbodyId} .parte-meta`);
      if (metas[idx]) {
        const iniSeg = ((parte.inicioMs || 0) / 1000).toFixed(1);
        const fimSeg = ((parte.fimMs || 0) / 1000).toFixed(1);
        metas[idx].innerText = `${iniSeg}s → ${fimSeg}s | ${passosParseados.length} passo(s)`;
      }
    }

    function adicionarParteManual(categoria) {
      if (gravando) return;
      const lista = obterListaPartes(categoria);
      const inicioBase = categoria === "bracos" ? duracaoBracosMs : duracaoBaseMs;
      lista.push({
        inicioMs: inicioBase,
        fimMs: inicioBase,
        passos: [],
        eventos: [],
        editadoManualmente: true,
        textoManual: ""
      });
      renderizarTabelaCategoria(categoria);
      atualizarBotoesGravacao();
      salvarPartesNoNavegador();

      const tbodyId = categoria === "bracos" ? "corpoTabelaPartesBracos" : "corpoTabelaPartesBase";
      const caixas = document.querySelectorAll(`#${tbodyId} .parte-code`);
      if (caixas.length > 0) caixas[caixas.length - 1].focus();
    }

    function excluirParte(categoria, idx) {
      const lista = obterListaPartes(categoria);
      if (gravando || idx < 0 || idx >= lista.length) return;
      cancelarReplay();
      lista.splice(idx, 1);
      recalcularLinhasDoTempo();
      renderizarTabelaCategoria(categoria);
      atualizarTextosRoteirosCompletos();
      atualizarBotoesGravacao();
      salvarPartesNoNavegador();
    }

    function limparCategoria(categoria) {
      if (gravando) return;
      cancelarReplay();
      if (categoria === "bracos") {
        partesBracos = [];
        indiceParteBracosAtiva = -1;
        duracaoBracosMs = 0;
        document.getElementById("codeOutBracos").value = "";
      } else {
        partesBase = [];
        indiceParteBaseAtiva = -1;
        duracaoBaseMs = 0;
        document.getElementById("codeOutBase").value = "";
      }
      renderizarTabelaCategoria(categoria);
      atualizarBotoesGravacao();
      salvarPartesNoNavegador();
    }

    function toggleRec(modo) {
      cancelarReplay();
      if (!gravando) {
        if ((modo === "bracos_novo" || modo === "bracos_continuar") && modoControleGamepad !== "bracos") {
          alternarModoControleGamepad();
        } else if ((modo === "base_novo" || modo === "base_continuar") && modoControleGamepad !== "base") {
          alternarModoControleGamepad();
        }

        gravando = true;
        modoGravacaoAtual = modo;
        cmdAtivo = "";
        estadoGravOmbro.e.ativo = false;
        estadoGravOmbro.d.ativo = false;
        estadoGravCotoveloAnalog.ativo = false;
        movX = movY = movRot = movPicoMag = 0;
        indiceParteBaseAtiva = -1;
        indiceParteBracosAtiva = -1;

        if (modo === "base_novo") {
          duracaoBaseMs = 0;
          partesBase = [{
            inicioMs: 0,
            fimMs: 0,
            passos: [],
            eventos: [],
            editadoManualmente: false
          }];
          indiceParteBaseAtiva = 0;
          inicioRec = Date.now();
          renderizarTabelaCategoria("base");
          atualizarTextosRoteirosCompletos();
        } else if (modo === "base_continuar") {
          const inicioParte = duracaoBaseMs;
          partesBase.push({
            inicioMs: inicioParte,
            fimMs: inicioParte,
            passos: [],
            eventos: [],
            editadoManualmente: false
          });
          indiceParteBaseAtiva = partesBase.length - 1;
          inicioRec = Date.now() - inicioParte;
          renderizarTabelaCategoria("base");
        } else if (modo === "bracos_novo") {
          partesBracos.push({
            inicioMs: 0,
            fimMs: 0,
            passos: [],
            eventos: [],
            editadoManualmente: false
          });
          indiceParteBracosAtiva = partesBracos.length - 1;
          inicioRec = Date.now();
          renderizarTabelaCategoria("bracos");
          if (document.getElementById("chkReplayBaseAoGravarBracos").checked && partesBase.length > 0) {
            agendarExecucaoLista(partesBase, 0);
          }
        } else if (modo === "bracos_continuar") {
          const inicioParte = duracaoBracosMs;
          partesBracos.push({
            inicioMs: inicioParte,
            fimMs: inicioParte,
            passos: [],
            eventos: [],
            editadoManualmente: false
          });
          indiceParteBracosAtiva = partesBracos.length - 1;
          inicioRec = Date.now() - inicioParte;
          renderizarTabelaCategoria("bracos");
        }
        atualizarBotoesGravacao();
      } else {
        const tempoFim = Date.now() - inicioRec;
        fecharSegmentoGravado(tempoFim);
        fecharSegmentoOmbro("e", tempoFim);
        fecharSegmentoOmbro("d", tempoFim);
        gpCotoveloAnalogAtivo = false;
        fecharSegmentoCotoveloAnalog(tempoFim);

        if (indiceParteBaseAtiva >= 0 && partesBase[indiceParteBaseAtiva]) {
          const p = partesBase[indiceParteBaseAtiva];
          p.fimMs = Math.max(p.fimMs || 0, tempoFim);
          p.eventos.sort((a, b) => a.tempo - b.tempo);
          p.passos.sort((a, b) => a.tempo - b.tempo);
        }
        if (indiceParteBracosAtiva >= 0 && partesBracos[indiceParteBracosAtiva]) {
          const p = partesBracos[indiceParteBracosAtiva];
          p.fimMs = Math.max(p.fimMs || 0, tempoFim);
          p.eventos.sort((a, b) => a.tempo - b.tempo);
          p.passos.sort((a, b) => a.tempo - b.tempo);
        }

        recalcularLinhasDoTempo();
        renderizarTabelaCategoria("base");
        renderizarTabelaCategoria("bracos");
        atualizarTextosRoteirosCompletos();
        gravando = false;
        modoGravacaoAtual = "";
        indiceParteBaseAtiva = -1;
        indiceParteBracosAtiva = -1;
        atualizarBotoesGravacao();
        salvarPartesNoNavegador();
      }
    }

    function executarAcaoCppNoRobo(acao) {
      const m = acao.match(/funcoes_\.(\w+)\(([^)]*)\)/);
      if (!m) return;

      const metodo = m[1];
      const args = m[2].split(",").map((s) => s.replace(/f$/i, "").trim());

      if (metodo === "turn") {
        wsSend(`turn:${parseFloat(args[0]) || 0},${parseInt(args[1]) || 120}`);
      } else if (metodo === "turnTo") {
        wsSend(`turn_to:${parseFloat(args[0]) || 0},${parseInt(args[1]) || 120}`);
      } else if (metodo === "moverOmbroEsq") {
        wsSend(`OMB_POS_E:${parseFloat(args[0]) || 0},${parseInt(args[1]) || 130}`);
      } else if (metodo === "moverOmbroDir") {
        wsSend(`OMB_POS_D:${parseFloat(args[0]) || 0},${parseInt(args[1]) || 130}`);
      } else if (metodo === "moverCotoveloEsq") {
        wsSend(`srv_pos_e:${parseFloat(args[0]) || 0}`);
      } else if (metodo === "moverCotoveloDir") {
        wsSend(`srv_pos_d:${parseFloat(args[0]) || 0}`);
      } else if (metodo === "enviarCodigoCenario") {
        const cod = parseInt(args[0]) || 2565;
        const ch = (args[1] !== undefined && args[1] !== "") ? parseInt(args[1]) : 10;
        wsSend(`ble_tx:${cod},${ch}`);
      } else if (metodo === "run") {
        wsSend(`run:${parseFloat(args[0]) || 0},${parseInt(args[1]) || 120}`);
      } else if (metodo === "strafe") {
        wsSend(`strafe:${parseFloat(args[0]) || 0},${parseInt(args[1]) || 120}`);
      } else if (metodo === "diagonal") {
        const dist = parseFloat(args[0]) || 0;
        const isDir = (args[1] === "true") ? 1 : 0;
        const vel = parseInt(args[2]) || 120;
        wsSend(`diag:${dist},${isDir},${vel}`);
      }
    }

    function agendarExecucaoLista(listaPartes, indiceInicial) {
      if (!listaPartes || indiceInicial < 0 || indiceInicial >= listaPartes.length) return;

      const fatia = listaPartes.slice(indiceInicial);
      let tempoBase = fatia[0].inicioMs || 0;
      if (fatia[0].passos && fatia[0].passos.length > 0 && fatia[0].passos[0].tempo < tempoBase) {
        tempoBase = fatia[0].passos[0].tempo;
      }

      fatia.forEach((parte) => {
        if (!parte.editadoManualmente && parte.eventos && parte.eventos.length > 0) {
          parte.eventos.forEach((ev) => {
            const atraso = Math.max(0, ev.tempo - tempoBase);
            const id = setTimeout(() => {
              if (ev.tipo === "raw") {
                wsSend(ev.cmd === "stop" ? "stop_base" : ev.cmd);
              }
            }, atraso);
            timersReplay.push(id);
          });
        } else if (parte.passos && parte.passos.length > 0) {
          parte.passos.forEach((passo) => {
            const atraso = Math.max(0, passo.tempo - tempoBase);
            const id = setTimeout(() => executarAcaoCppNoRobo(passo.acao), atraso);
            timersReplay.push(id);
          });
        }
      });
    }

    function testarAPartirDaParte(categoria, indiceParte) {
      if (!ws || ws.readyState !== WebSocket.OPEN) {
        conectarWebSocket();
        alert("WebSocket desconectado do robô! Verifique o Wi-Fi do Chapeleiro e tente clicar em PLAY novamente.");
        return;
      }
      cancelarReplay();
      agendarExecucaoLista(obterListaPartes(categoria), indiceParte);
    }

    function testarTudoSincronizado() {
      if (!ws || ws.readyState !== WebSocket.OPEN) {
        conectarWebSocket();
        alert("WebSocket desconectado do robô! Verifique o Wi-Fi do Chapeleiro e tente clicar em TESTAR TUDO novamente.");
        return;
      }
      cancelarReplay();
      if (partesBase.length > 0) agendarExecucaoLista(partesBase, 0);
      if (partesBracos.length > 0) agendarExecucaoLista(partesBracos, 0);
    }

    function substituirGiroNoReplay(parte, segId, tempoInicio, anguloFinal, vel) {
      parte.eventos = parte.eventos.filter((e) => e.segId !== segId);
      parte.eventos.push({
        tempo: tempoInicio,
        tipo: "raw",
        cmd: `turn:${anguloFinal},${vel}`
      });
    }

    function fecharSegmentoGravado(tempo) {
      const parte = obterParteBaseParaGravacao();
      if (!cmdAtivo || !parte) return;
      const vel = parseInt(document.getElementById("forcaBase").value) || 120;
      const ticksDesloc = Math.max(
        Math.abs(ticks[0] - m1Start),
        Math.abs(ticks[1] - m2Start),
        Math.abs(ticks[2] - m3Start),
        Math.abs(ticks[3] - m4Start)
      );
      const dist = (ticksDesloc / kTicksPorMm).toFixed(1);
      const ang = (gyroAtual - gyroStart).toFixed(1);
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
        acao = `funcoes_.turn(${ang}f, ${vel});`;
        substituirGiroNoReplay(parte, segAtualId, segInicioTempo, ang, vel);
      } else if (cmdAtivo === "mov") {
        const temLinear = movPicoMag > 0.05;
        const temGiro = Math.abs(movRot) > 0.05;
        if (temLinear && temGiro && Math.abs(parseFloat(ang)) > 1.0) {
          acao = `funcoes_.moverGirando(${movX.toFixed(2)}f, ${movY.toFixed(2)}f, ${ang}f, ${vel});`;
        } else if (temGiro && !temLinear && Math.abs(parseFloat(ang)) > 1.0) {
          acao = `funcoes_.turn(${ang}f, ${vel});`;
          substituirGiroNoReplay(parte, segAtualId, segInicioTempo, ang, vel);
        } else if (temLinear && !temGiro && parseFloat(dist) > 1.0) {
          const velProp = Math.max(25, Math.round(vel * Math.min(1.0, movPicoMag)));
          const angDir = Math.atan2(movY, movX) * (180.0 / Math.PI);
          if (angDir >= 67.5 && angDir <= 112.5) acao = `funcoes_.run(${dist}f, ${velProp});`;
          else if (angDir <= -67.5 && angDir >= -112.5) acao = `funcoes_.run(-${dist}f, ${velProp});`;
          else if (angDir > -22.5 && angDir < 22.5) acao = `funcoes_.strafe(${dist}f, ${velProp});`;
          else if (angDir > 157.5 || angDir < -157.5) acao = `funcoes_.strafe(-${dist}f, ${velProp});`;
          else if (angDir > 22.5 && angDir < 67.5) acao = `funcoes_.diagonal(${dist}f, true, ${velProp});`;
          else if (angDir > 112.5 && angDir < 157.5) acao = `funcoes_.diagonal(${dist}f, false, ${velProp});`;
          else if (angDir > -67.5 && angDir < -22.5) acao = `funcoes_.diagonal(-${dist}f, false, ${velProp});`;
          else acao = `funcoes_.diagonal(-${dist}f, true, ${velProp});`;
        }
      }

      if (acao && (parseFloat(dist) > 1.0 || cmdAtivo === "e" || cmdAtivo === "q" || cmdAtivo === "mov")) {
        registrarPassoNaParte("base", parte, segInicioTempo, acao, tempo);
      }
      cmdAtivo = "";
    }

    function sendCmd(cmd) {
      wsSend(cmd);
      if (!gravando) return;

      const tempo = Date.now() - inicioRec;
      const velBase = parseInt(document.getElementById("forcaBase").value) || 120;

      if (cmd.startsWith("OMB_POS_E:") || cmd.startsWith("OMB_POS_D:") || cmd.startsWith("srv_pos_e:") || cmd.startsWith("srv_pos_d:")) {
        const parteBr = obterParteBracosParaGravacao();
        if (cmd.startsWith("OMB_POS_E:")) {
          const partes = cmd.substring(10).split(",");
          const graus = parseFloat(partes[0]).toFixed(1);
          const v = partes[1] || (parseInt(document.getElementById("forcaBraco").value) || 130);
          registrarPassoNaParte("bracos", parteBr, tempo, `funcoes_.moverOmbroEsq(${graus}f, ${v});`, tempo);
          parteBr.eventos.push({ tempo, tipo: "raw", cmd });
        } else if (cmd.startsWith("OMB_POS_D:")) {
          const partes = cmd.substring(10).split(",");
          const graus = parseFloat(partes[0]).toFixed(1);
          const v = partes[1] || (parseInt(document.getElementById("forcaBraco").value) || 130);
          registrarPassoNaParte("bracos", parteBr, tempo, `funcoes_.moverOmbroDir(${graus}f, ${v});`, tempo);
          parteBr.eventos.push({ tempo, tipo: "raw", cmd });
        } else if (cmd.startsWith("srv_pos_e:")) {
          registrarPassoNaParte("bracos", parteBr, tempo, `funcoes_.moverCotoveloEsq(${parseFloat(cmd.split(":")[1]).toFixed(1)}f);`, tempo);
          parteBr.eventos.push({ tempo, tipo: "raw", cmd });
        } else if (cmd.startsWith("srv_pos_d:")) {
          registrarPassoNaParte("bracos", parteBr, tempo, `funcoes_.moverCotoveloDir(${parseFloat(cmd.split(":")[1]).toFixed(1)}f);`, tempo);
          parteBr.eventos.push({ tempo, tipo: "raw", cmd });
        }
        return;
      }

      const parte = obterParteBaseParaGravacao();
      if (!parte) return;

      if (cmd.startsWith("ble_tx:")) {
        const partes = cmd.substring(7).split(",");
        const cod = parseInt(partes[0]) || 2565;
        const ch = (partes[1] !== undefined) ? parseInt(partes[1]) : 10;
        registrarPassoNaParte("base", parte, tempo, `funcoes_.enviarCodigoCenario(${cod}, ${ch});`, tempo);
        parte.eventos.push({ tempo, tipo: "raw", cmd });
      } else if (cmd.startsWith("run:")) {
        const partes = cmd.substring(4).split(",");
        const dist = parseFloat(partes[0]).toFixed(1);
        const v = partes[1] || velBase;
        registrarPassoNaParte("base", parte, tempo, `funcoes_.run(${dist}f, ${v});`, tempo);
        parte.eventos.push({ tempo, tipo: "raw", cmd });
      } else if (cmd.startsWith("strafe:")) {
        const partes = cmd.substring(7).split(",");
        const dist = parseFloat(partes[0]).toFixed(1);
        const v = partes[1] || velBase;
        registrarPassoNaParte("base", parte, tempo, `funcoes_.strafe(${dist}f, ${v});`, tempo);
        parte.eventos.push({ tempo, tipo: "raw", cmd });
      } else if (cmd.startsWith("diag:")) {
        const partes = cmd.substring(5).split(",");
        const dist = parseFloat(partes[0]).toFixed(1);
        const isDir = (parseInt(partes[1]) === 1) ? "true" : "false";
        const v = partes[2] || velBase;
        registrarPassoNaParte("base", parte, tempo, `funcoes_.diagonal(${dist}f, ${isDir}, ${v});`, tempo);
        parte.eventos.push({ tempo, tipo: "raw", cmd });
      } else if (cmd.startsWith("turn:")) {
        const partes = cmd.substring(5).split(",");
        const ang = parseFloat(partes[0]).toFixed(1);
        const v = partes[1] || velBase;
        registrarPassoNaParte("base", parte, tempo, `funcoes_.turn(${ang}f, ${v});`, tempo);
        parte.eventos.push({ tempo, tipo: "raw", cmd });
      } else if (cmd.startsWith("turn_to:")) {
        const partes = cmd.substring(8).split(",");
        const ang = parseFloat(partes[0]).toFixed(1);
        const v = partes[1] || velBase;
        registrarPassoNaParte("base", parte, tempo, `funcoes_.turnTo(${ang}f, ${v});`, tempo);
        parte.eventos.push({ tempo, tipo: "raw", cmd });
      } else if (cmd.startsWith("vel:") || cmd.startsWith("fod:")) {
        parte.eventos.push({ tempo, tipo: "raw", cmd });
      } else if (cmd.startsWith("mov:")) {
        const partes = cmd.substring(4).split(",");
        const nx = parseFloat(partes[0]) || 0;
        const ny = parseFloat(partes[1]) || 0;
        const nrot = parseFloat(partes[2]) || 0;

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
          m1Start = ticks[0]; m2Start = ticks[1]; m3Start = ticks[2]; m4Start = ticks[3];
          gyroStart = gyroAtual;
          cmdAtivo = "mov";
          movPicoMag = 0;
          segAtualId = ++contadorSegmentos;
          segInicioTempo = tempo;
          parte.eventos.push({ tempo, tipo: "raw", cmd: `vel:${velBase}`, segId: segAtualId });
        }
        movX = nx;
        movY = ny;
        movRot = nrot;
        if (magNovo > movPicoMag) movPicoMag = magNovo;
        parte.eventos.push({ tempo, tipo: "raw", cmd, segId: segAtualId });
      } else if (cmd === "stop_base" || cmd === "stop") {
        const tinhaSegmento = (cmdAtivo !== "");
        const segFechadoId = segAtualId;
        fecharSegmentoGravado(tempo);
        if (!tinhaSegmento || parte.eventos.some((e) => e.segId === segFechadoId)) {
          parte.eventos.push({ tempo, tipo: "raw", cmd: "stop_base" });
        }
      } else if (["w","s","a","d","wd","wa","sd","sa","q","e"].includes(cmd)) {
        if (cmdAtivo !== "" && cmdAtivo !== cmd) {
          fecharSegmentoGravado(tempo);
        }
        if (cmdAtivo !== cmd) {
          m1Start = ticks[0]; m2Start = ticks[1]; m3Start = ticks[2]; m4Start = ticks[3];
          gyroStart = gyroAtual;
          cmdAtivo = cmd;
          segAtualId = ++contadorSegmentos;
          segInicioTempo = tempo;
          parte.eventos.push({ tempo, tipo: "raw", cmd: `vel:${velBase}`, segId: segAtualId });
          parte.eventos.push({ tempo, tipo: "raw", cmd, segId: segAtualId });
        }
      }
    }

    function registrarPassoNaParte(categoria, parte, tempoInicio, acao, tempoFimOpcional = tempoInicio) {
      parte.passos.push({ tempo: tempoInicio, acao });
      parte.passos.sort((a, b) => a.tempo - b.tempo);
      parte.fimMs = Math.max(parte.fimMs || 0, tempoFimOpcional);
      recalcularLinhasDoTempo();
      renderizarTabelaCategoria(categoria);
      atualizarTextosRoteirosCompletos();
      atualizarBotoesGravacao();
      salvarPartesNoNavegador();
    }

    function compilarBlocosCpp(listaPartes, cabecalhoComentario) {
      const itens = [];
      listaPartes.forEach((parte, idxParte) => {
        if (!parte.passos) return;
        parte.passos.forEach((p) => itens.push({ tempo: p.tempo, acao: p.acao, parteNum: idxParte + 1 }));
      });
      itens.sort((a, b) => a.tempo - b.tempo);

      let texto = cabecalhoComentario + "\n";

      // Agrupa comandos no mesmo instante (ms) em um único bloco if para execução simultânea.
      const grupos = [];
      itens.forEach((item) => {
        const ultimo = grupos[grupos.length - 1];
        if (ultimo && ultimo.tempo === item.tempo) {
          ultimo.acoes.push(item.acao);
          if (!ultimo.partes.includes(item.parteNum)) ultimo.partes.push(item.parteNum);
        } else {
          grupos.push({ tempo: item.tempo, acoes: [item.acao], partes: [item.parteNum] });
        }
      });

      grupos.forEach((g, idx) => {
        const tagPartes = g.partes.map((n) => `Parte ${n}`).join(" + ");
        const corpo = g.acoes.map((a) => `    ${a}`).join("\n");
        texto += `// [${tagPartes}]\nif (passoAtual_ == ${idx} && decorridoMs >= ${g.tempo}) {\n${corpo}\n    passoAtual_++;\n}\n`;
      });
      return texto;
    }

    function atualizarTextosRoteirosCompletos() {
      const boxBase = document.getElementById("codeOutBase");
      const boxBracos = document.getElementById("codeOutBracos");

      if (boxBase) {
        boxBase.value = compilarBlocosCpp(
          partesBase,
          "// Cole dentro de BaseCenas::atualizarCenas() (rodas e BLE)."
        );
      }
      if (boxBracos) {
        boxBracos.value = compilarBlocosCpp(
          partesBracos,
          "// Cole dentro de BracosCenas::atualizarCenas() (ombros e cotovelos)."
        );
      }
    }

    function copiarCodigoCaixa(boxId, btnId) {
      const box = document.getElementById(boxId);
      if (!box) return;
      box.select();
      navigator.clipboard?.writeText(box.value);
      const btn = document.getElementById(btnId);
      if (btn) {
        const original = btn.innerText;
        btn.innerText = "✓ COPIADO!";
        setTimeout(() => { btn.innerText = original; }, 1200);
      }
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
      if (intervaloOmbro.e) pararOmbro("e", false);
      if (intervaloOmbro.d) pararOmbro("d", false);
      for (const k in teclasAtivas) delete teclasAtivas[k];
      for (const k in botoesAtivos) delete botoesAtivos[k];
      gpX = gpY = gpRot = 0;
      cmdManualAtual = "stop";
      atualizarDestaqueBotoes(0, 0, 0);
      sendCmd("stop");
    }

    function calcularVetorManual() {
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

      const magXY = Math.hypot(x, y);
      if (magXY > 1) {
        x /= magXY;
        y /= magXY;
      }
      return { x, y, rot, viaGamepad: false };
    }

    function resolverComandoManual(x, y, rot, viaGamepad) {
      if (x === 0 && y === 0 && rot === 0) return "stop_base";
      if (viaGamepad) return `mov:${x.toFixed(2)},${y.toFixed(2)},${rot.toFixed(2)}`;

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
      if (x === 0 && y === 0) return rot < 0 ? "q" : "e";
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
      if (modoGiroDigitado !== null) {
        ev.preventDefault();
        if (ev.repeat) return;

        if (ev.key >= "0" && ev.key <= "9") {
          if (bufferGrausDigitados.length < 5) {
            bufferGrausDigitados += ev.key;
            giroDigitadoCancelado = false;
          }
        } else if ((ev.key === "." || ev.key === ",") && !bufferGrausDigitados.includes(".")) {
          bufferGrausDigitados = (bufferGrausDigitados || "0") + ".";
          giroDigitadoCancelado = false;
        } else if (ev.key === "Backspace") {
          bufferGrausDigitados = bufferGrausDigitados.slice(0, -1);
        } else if (ev.key === "Escape") {
          giroDigitadoCancelado = true;
          bufferGrausDigitados = "";
        }
        atualizarVisorGiroDigitado();
        return;
      }

      if (ev.repeat) return;

      const tag = document.activeElement?.tagName;
      if (tag === "INPUT" || tag === "TEXTAREA") return;

      if (ev.key === "Tab") {
        ev.preventDefault();
        alternarModoControleGamepad();
        return;
      }

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

    carregarPartesDoNavegador();
  </script>
</body>
</html>
)rawliteral";