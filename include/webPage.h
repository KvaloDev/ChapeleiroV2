#pragma once
#include <Arduino.h>

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
    .turn { border-color: var(--yellow); color: var(--yellow); }
    .telem { font-family: monospace; color: var(--yellow); font-size: 16px; font-weight: bold; }
    .gyro-big { font-size: 24px; color: var(--green); }
    input[type=number] { width: 68px; padding: 7px; background: var(--bg); color: var(--yellow); border: 1px solid var(--yellow); border-radius: 4px; text-align: center; font-weight: bold; font-size: 14px; }
    .preset-card { background: var(--bg); border: 1px solid var(--btn); border-radius: 6px; padding: 6px; }
    .preset-card.ativo { border: 2px solid var(--yellow); }
    .preset-card input { width: 54px; padding: 4px; margin-top: 4px; }
    .rec-box { width: 100%; height: 155px; background: #02050A; color: #00FF66; font-family: monospace; font-size: 12px; border: 1px solid var(--btn); padding: 8px; box-sizing: border-box; }
    .btn-rec { background: var(--red); border-color: var(--red); }
    .btn-rec.ativo { background: #555; border-color: #555; }
    .btn-play { background: #1B5E20; border-color: var(--green); color: var(--green); }
    .medida-bar { display: flex; flex-wrap: wrap; gap: 6px; justify-content: center; margin-top: 8px; min-height: 28px; }
    .chip-medida { background: var(--bg); border: 1px solid var(--green); color: var(--green); padding: 4px 10px; border-radius: 14px; font-family: monospace; font-size: 13px; cursor: pointer; }
    .badge-rampa { font-size: 11px; padding: 2px 6px; border-radius: 4px; background: #16265C; color: #AAA; }
    .badge-rampa.on { background: #E65100; color: #FFF; font-weight: bold; }
  </style>
</head>
<body>
  <h1>CHAPELEIRO V2 | TUCUNA</h1>

  <!-- TELEMETRIA GYRO E CONTROLE DE FORCA -->
  <div class="grid-2">
    <div class="box" style="margin:0;">
      <h3>Giroscopio SPIKE (BLE)</h3>
      <span class="telem gyro-big" id="gyroDisp">0.0</span><span class="telem gyro-big">°</span>
      <br><br>
      <button onclick="sendCmd('zero_gyro')" style="border-color:var(--green); color:var(--green);">ZERAR (0°)</button>
      <button onclick="sendCmd('cena_1')" style="border-color:var(--yellow);">CENA 1 (C++)</button>
    </div>

    <div class="box" style="margin:0;">
      <h3>Forca da Base (PWM)</h3>
      <button onclick="alterarForcaBase(-10)">-10</button>
      <input type="number" id="forcaBase" value="120" min="0" max="255" onchange="aplicarForcaBase(this.value)">
      <button onclick="alterarForcaBase(10)">+10</button>
      <br><br>
      <span id="badgeRampa" class="badge-rampa">RAMPA ATIVA (>60)</span>
    </div>
  </div>

  <!-- 4 VELOCIDADES RAPIDAS (TECLAS 1, 2, 3, 4) -->
  <div class="box">
    <h3>Velocidades Rapidas (Segure ou aperte 1, 2, 3 ou 4)</h3>
    <div class="grid-4">
      <div class="preset-card" id="cardVel1" onclick="selecionarPreset(1)">
        <div><b>[1]</b> Lenta</div>
        <input type="number" id="preset1" value="55" min="0" max="255" onchange="atualizarPreset(1)">
      </div>
      <div class="preset-card ativo" id="cardVel2" onclick="selecionarPreset(2)">
        <div><b>[2]</b> Media</div>
        <input type="number" id="preset2" value="95" min="0" max="255" onchange="atualizarPreset(2)">
      </div>
      <div class="preset-card" id="cardVel3" onclick="selecionarPreset(3)">
        <div><b>[3]</b> Rapida</div>
        <input type="number" id="preset3" value="145" min="0" max="255" onchange="atualizarPreset(3)">
      </div>
      <div class="preset-card" id="cardVel4" onclick="selecionarPreset(4)">
        <div><b>[4]</b> Turbo</div>
        <input type="number" id="preset4" value="200" min="0" max="255" onchange="atualizarPreset(4)">
      </div>
    </div>
  </div>

  <!-- GIRO PRECISO (SETAS ESQUERDA/DIREITA + ANGULO) E MEDIDOR DE DISTANCIA (M) -->
  <div class="grid-2">
    <div class="box" style="margin:0;">
      <h3>Giro Preciso (Setas ← / →)</h3>
      <input type="number" id="anguloGiro" value="90" min="1" max="360" step="1"> °
      <br><br>
      <button class="turn" onclick="executarGiroSeta(-1)">↶ ESQ (←)</button>
      <button class="turn" onclick="executarGiroSeta(1)">DIR (→) ↷</button>
    </div>

    <div class="box" style="margin:0;">
      <h3>Regua Digital (Tecla M)</h3>
      <span class="telem" id="medidaAtualDisp">0.0 mm</span>
      <br><br>
      <button id="btnMedir" onclick="toggleMedirDistancia()">📏 MEDIR (M)</button>
      <button onclick="limparMedidas()">LIMPAR BARRA</button>
    </div>
  </div>

  <!-- BARRA DE MEDIDAS SALVAS -->
  <div class="box" style="padding:6px 10px;">
    <div style="font-size:11px; color:#AAA;">MEDIDAS SALVAS (clique para copiar):</div>
    <div class="medida-bar" id="barraMedidas"></div>
  </div>

  <!-- D-PAD DIRECIONAL + DIAGONAIS -->
  <div class="grid-3">
    <button id="btn_wa" onmousedown="sendCmd('wa')" onmouseup="sendCmd('stop')" ontouchstart="event.preventDefault();sendCmd('wa')" ontouchend="event.preventDefault();sendCmd('stop')">↖ W+A</button>
    <button id="btn_w" onmousedown="sendCmd('w')" onmouseup="sendCmd('stop')" ontouchstart="event.preventDefault();sendCmd('w')" ontouchend="event.preventDefault();sendCmd('stop')">▲ W</button>
    <button id="btn_wd" onmousedown="sendCmd('wd')" onmouseup="sendCmd('stop')" ontouchstart="event.preventDefault();sendCmd('wd')" ontouchend="event.preventDefault();sendCmd('stop')">↗ W+D</button>

    <button id="btn_a" onmousedown="sendCmd('a')" onmouseup="sendCmd('stop')" ontouchstart="event.preventDefault();sendCmd('a')" ontouchend="event.preventDefault();sendCmd('stop')">◄ A</button>
    <button onmousedown="sendCmd('stop')" style="background:#333;">■ STOP</button>
    <button id="btn_d" onmousedown="sendCmd('d')" onmouseup="sendCmd('stop')" ontouchstart="event.preventDefault();sendCmd('d')" ontouchend="event.preventDefault();sendCmd('stop')">► D</button>

    <button id="btn_sa" onmousedown="sendCmd('sa')" onmouseup="sendCmd('stop')" ontouchstart="event.preventDefault();sendCmd('sa')" ontouchend="event.preventDefault();sendCmd('stop')">↙ S+A</button>
    <button id="btn_s" onmousedown="sendCmd('s')" onmouseup="sendCmd('stop')" ontouchstart="event.preventDefault();sendCmd('s')" ontouchend="event.preventDefault();sendCmd('stop')">▼ S</button>
    <button id="btn_sd" onmousedown="sendCmd('sd')" onmouseup="sendCmd('stop')" ontouchstart="event.preventDefault();sendCmd('sd')" ontouchend="event.preventDefault();sendCmd('stop')">↘ S+D</button>
  </div>
  <div style="max-width:340px; margin:0 auto 10px; display:grid; grid-template-columns:1fr 1fr; gap:6px;">
    <button id="btn_q" class="turn" onmousedown="sendCmd('q')" onmouseup="sendCmd('stop')" ontouchstart="event.preventDefault();sendCmd('q')" ontouchend="event.preventDefault();sendCmd('stop')">↶ GIRAR Q</button>
    <button id="btn_e" class="turn" onmousedown="sendCmd('e')" onmouseup="sendCmd('stop')" ontouchstart="event.preventDefault();sendCmd('e')" ontouchend="event.preventDefault();sendCmd('stop')">↷ GIRAR E</button>
  </div>

  <!-- CONTROLE DOS BRACOS + FORCA DO BRACO -->
  <div class="box">
    <h3>Forca dos Bracos (Ombros PWM)</h3>
    <input type="number" id="forcaBraco" value="130" min="50" max="255" onchange="aplicarForcaBraco(this.value)">
    <button onclick="aplicarForcaBraco(document.getElementById('forcaBraco').value)">APLICAR</button>
  </div>

  <div class="grid-2">
    <div class="box" style="margin:0;">
      <div>OMBRO ESQ: <span class="telem" id="encBE">0</span></div><br>
      <button onmousedown="sendCmd('be_up')" onmouseup="sendCmd('stop')" ontouchstart="event.preventDefault();sendCmd('be_up')" ontouchend="event.preventDefault();sendCmd('stop')">▲ SOBE</button>
      <button onmousedown="sendCmd('be_dwn')" onmouseup="sendCmd('stop')" ontouchstart="event.preventDefault();sendCmd('be_dwn')" ontouchend="event.preventDefault();sendCmd('stop')">▼ DESCE</button>
    </div>
    <div class="box" style="margin:0;">
      <div>OMBRO DIR: <span class="telem" id="encBD">0</span></div><br>
      <button onmousedown="sendCmd('bd_up')" onmouseup="sendCmd('stop')" ontouchstart="event.preventDefault();sendCmd('bd_up')" ontouchend="event.preventDefault();sendCmd('stop')">▲ SOBE</button>
      <button onmousedown="sendCmd('bd_dwn')" onmouseup="sendCmd('stop')" ontouchstart="event.preventDefault();sendCmd('bd_dwn')" ontouchend="event.preventDefault();sendCmd('stop')">▼ DESCE</button>
    </div>
    <div class="box" style="margin:0;">
      <div>COTOVELO ESQ (75-120°)</div><br>
      <input type="number" id="srvE" value="90" min="75" max="120">
      <button onclick="sendCmd('srv_pos_e:' + document.getElementById('srvE').value)">SET</button>
    </div>
    <div class="box" style="margin:0;">
      <div>COTOVELO DIR (0-50°)</div><br>
      <input type="number" id="srvD" value="25" min="0" max="50">
      <button onclick="sendCmd('srv_pos_d:' + document.getElementById('srvD').value)">SET</button>
    </div>
  </div>

  <!-- ENCODERS DA BASE -->
  <div class="box">
    ENCODERS: M1:<span class="telem" id="enc1">0</span> | M2:<span class="telem" id="enc2">0</span> | M3:<span class="telem" id="enc3">0</span> | M4:<span class="telem" id="enc4">0</span>
  </div>

  <!-- ROTEIRIZADOR DE CENAS -->
  <div class="box" style="text-align:left;">
    <div style="display:flex; gap:6px; flex-wrap:wrap;">
      <button id="btnRec" class="btn-rec" onclick="toggleRec()">🔴 GRAVAR ROTEIRO</button>
      <button class="btn-play" onclick="testarRoteiroGravado()">▶ TESTAR GRAVACAO</button>
      <button onclick="limparRoteiro()">LIMPAR</button>
    </div>
    <br>
    <textarea id="codeOut" class="rec-box" readonly></textarea>
  </div>

  <script>
    const ws = new WebSocket(`ws://${window.location.hostname}/ws`);
    const kTicksPorMm = 1.65;

    let ticks = [0, 0, 0, 0];
    let gyroAtual = 0;

    // Estado de gravacao
    let gravando = false, inicioRec = 0, passo = 0, m1Start = 0, gyroStart = 0, cmdAtivo = "";
    let eventosGravados = [];

    // Estado de medicao (tecla M)
    let medindo = false;
    let ticksInicioMedicao = [0, 0, 0, 0];

    // Estado do teclado
    const teclasAtivas = {};
    let cmdTecladoAtual = "stop";

    ws.onopen = () => {
      aplicarForcaBase(document.getElementById("forcaBase").value);
      aplicarForcaBraco(document.getElementById("forcaBraco").value);
    };

    ws.onmessage = (e) => {
      if (!e.data.startsWith("DAT:")) return;
      const d = e.data.substring(4).split(",");
      ticks = [parseInt(d[0]) || 0, parseInt(d[1]) || 0, parseInt(d[2]) || 0, parseInt(d[3]) || 0];
      gyroAtual = parseFloat(d[7]) || 0;

      document.getElementById("enc1").innerText = ticks[0];
      document.getElementById("enc2").innerText = ticks[1];
      document.getElementById("enc3").innerText = ticks[2];
      document.getElementById("enc4").innerText = ticks[3];
      document.getElementById("encBE").innerText = d[5];
      document.getElementById("encBD").innerText = d[6];
      document.getElementById("gyroDisp").innerText = gyroAtual.toFixed(1);

      if (medindo) {
        const distMm = calcularDistanciaMediaMm();
        document.getElementById("medidaAtualDisp").innerText = distMm.toFixed(1) + " mm";
      }
    };

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
      const badge = document.getElementById("badgeRampa");
      if (v > 60) {
        badge.innerText = "RAMPA ATIVA (>60)";
        badge.classList.add("on");
      } else {
        badge.innerText = "DIRETO (<=60)";
        badge.classList.remove("on");
      }
      if (ws.readyState === WebSocket.OPEN) ws.send(`vel:${v}`);
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

    function toggleRec() {
      gravando = !gravando;
      const btn = document.getElementById("btnRec");
      if (gravando) {
        btn.innerText = "⏹ PARAR GRAVACAO"; btn.classList.add("ativo");
        inicioRec = Date.now(); passo = 0; eventosGravados = []; cmdAtivo = "";
        document.getElementById("codeOut").value = "// Cole dentro de BaseCenas::atualizarCenas()\n";
      } else {
        btn.innerText = "🔴 GRAVAR ROTEIRO"; btn.classList.remove("ativo");
      }
    }

    function limparRoteiro() {
      document.getElementById("codeOut").value = "";
      eventosGravados = [];
      passo = 0;
    }

    function testarRoteiroGravado() {
      if (eventosGravados.length === 0) return;
      eventosGravados.forEach((ev) => {
        setTimeout(() => {
          if (ev.tipo === "raw") ws.send(ev.cmd);
        }, ev.tempo);
      });
    }

    function fecharSegmentoGravado(tempo) {
      if (!cmdAtivo) return;
      const vel = parseInt(document.getElementById("forcaBase").value) || 120;
      const dist = (Math.abs(ticks[0] - m1Start) / kTicksPorMm).toFixed(1);
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
      else if (cmdAtivo === "e" || cmdAtivo === "q") acao = `funcoes_.turn(${ang}f, ${vel});`;

      if (acao && parseFloat(dist) > 1.0 || (cmdAtivo === "e" || cmdAtivo === "q")) {
        registrarPasso(tempo, acao);
      }
      cmdAtivo = "";
    }

    function sendCmd(cmd) {
      if (ws.readyState === WebSocket.OPEN) ws.send(cmd);
      if (!gravando) return;

      const tempo = Date.now() - inicioRec;
      const velBase = parseInt(document.getElementById("forcaBase").value) || 120;
      const velBraco = parseInt(document.getElementById("forcaBraco").value) || 130;

      if (cmd.startsWith("turn:")) {
        const partes = cmd.substring(5).split(",");
        const ang = parseFloat(partes[0]).toFixed(1);
        const v = partes[1] || velBase;
        registrarPasso(tempo, `funcoes_.turn(${ang}f, ${v});`);
        eventosGravados.push({ tempo, tipo: "raw", cmd });
      } else if (cmd.startsWith("srv_pos_e:")) {
        registrarPasso(tempo, `funcoes_.moverCotoveloEsq(${cmd.split(":")[1]}.0f);`);
        eventosGravados.push({ tempo, tipo: "raw", cmd });
      } else if (cmd.startsWith("srv_pos_d:")) {
        registrarPasso(tempo, `funcoes_.moverCotoveloDir(${cmd.split(":")[1]}.0f);`);
        eventosGravados.push({ tempo, tipo: "raw", cmd });
      } else if (cmd.startsWith("vel:")) {
        eventosGravados.push({ tempo, tipo: "raw", cmd });
      } else if (cmd === "stop") {
        fecharSegmentoGravado(tempo);
        eventosGravados.push({ tempo, tipo: "raw", cmd: "stop" });
      } else if (["w","s","a","d","wd","wa","sd","sa","q","e"].includes(cmd)) {
        if (cmdAtivo !== "" && cmdAtivo !== cmd) {
          fecharSegmentoGravado(tempo);
        }
        if (cmdAtivo !== cmd) {
          m1Start = ticks[0];
          gyroStart = gyroAtual;
          cmdAtivo = cmd;
          eventosGravados.push({ tempo, tipo: "raw", cmd: `vel:${velBase}` });
          eventosGravados.push({ tempo, tipo: "raw", cmd });
        }
      }
    }

    function registrarPasso(tempo, acao) {
      const box = document.getElementById("codeOut");
      box.value += `if (passoAtual_ == ${passo} && decorridoMs >= ${tempo}) {\n    ${acao}\n    passoAtual_++;\n}\n`;
      box.scrollTop = box.scrollHeight;
      passo++;
    }

    function resolverComandoTeclado() {
      const w = !!teclasAtivas["w"];
      const s = !!teclasAtivas["s"];
      const a = !!teclasAtivas["a"];
      const d = !!teclasAtivas["d"];
      const q = !!teclasAtivas["q"];
      const e = !!teclasAtivas["e"];

      if (w && d) return "wd";
      if (w && a) return "wa";
      if (s && d) return "sd";
      if (s && a) return "sa";
      if (w) return "w";
      if (s) return "s";
      if (a) return "a";
      if (d) return "d";
      if (q) return "q";
      if (e) return "e";
      return "stop";
    }

    function atualizarDestaqueBotoes(cmd) {
      ["w","s","a","d","wd","wa","sd","sa","q","e"].forEach((id) => {
        const el = document.getElementById(`btn_${id}`);
        if (el) el.classList.toggle("tecla-ativa", id === cmd);
      });
    }

    document.addEventListener("keydown", (ev) => {
      if (ev.repeat) return;

      // Setas esquerda/direita executam o giro preciso mesmo se estiver digitando no campo de angulo
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

      // Ignora atalhos se estiver digitando dentro de uma caixa de numero
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
        const novoCmd = resolverComandoTeclado();
        if (novoCmd !== cmdTecladoAtual) {
          cmdTecladoAtual = novoCmd;
          atualizarDestaqueBotoes(novoCmd);
          sendCmd(novoCmd);
        }
      }
    });

    document.addEventListener("keyup", (ev) => {
      const k = ev.key.toLowerCase();
      if (["w","a","s","d","q","e"].includes(k)) {
        delete teclasAtivas[k];
        const novoCmd = resolverComandoTeclado();
        if (novoCmd !== cmdTecladoAtual) {
          cmdTecladoAtual = novoCmd;
          atualizarDestaqueBotoes(novoCmd);
          sendCmd(novoCmd);
        }
      }
    });
  </script>
</body>
</html>
)rawliteral";