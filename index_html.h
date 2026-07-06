#ifndef INDEX_HTML_H
#define INDEX_HTML_H

#include <Arduino.h>

// Página web do GIZMO (servida da flash via PROGMEM).
// Responsiva: 1 coluna no celular, dashboard de 2 colunas no desktop.
static const char PAGE_HTML[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>GIZMO</title>
<style>
  :root{
    --bg:#0b0b0e;--surface:#151519;--surface2:#1d1d22;--surface3:#27272e;
    --border:#2a2a31;--border2:#3a3a44;--text:#f2f2f5;--muted:#8a8a9c;
    --accent:#7c6cf0;--accent2:#5b4fd6;--accent-dim:#241f3d;--accent-light:#b3aafa;
    --green:#4ade80;--green-dim:#13321f;--red:#f87171;--red-dim:#341717;
    --amber:#fbbf24;--amber-dim:#2c2310;--radius:14px;
  }
  *{box-sizing:border-box;margin:0;padding:0;-webkit-tap-highlight-color:transparent;}
  body{font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;background:
       radial-gradient(1200px 600px at 50% -10%,#1a1730 0%,var(--bg) 55%);color:var(--text);
       min-height:100vh;padding-bottom:48px;font-size:15px;line-height:1.55;}

  .header{background:rgba(21,21,25,.85);backdrop-filter:blur(10px);border-bottom:1px solid var(--border);
          padding:14px 18px;display:flex;align-items:center;justify-content:space-between;position:sticky;top:0;z-index:20;}
  .logo{display:flex;align-items:center;gap:11px;}
  .logo-icon{width:38px;height:38px;border-radius:11px;background:linear-gradient(135deg,var(--accent),var(--accent2));
             display:flex;align-items:center;justify-content:center;font-size:19px;box-shadow:0 4px 14px rgba(124,108,240,.4);}
  .logo-text{font-size:17px;font-weight:800;letter-spacing:-.4px;line-height:1.1;}
  .logo-sub{font-size:11px;color:var(--muted);}
  .hdr-right{display:flex;align-items:center;gap:9px;}
  .wifi-dot{display:flex;align-items:center;gap:5px;font-size:10px;color:var(--muted);font-weight:700;}
  .wifi-dot i{width:8px;height:8px;border-radius:50%;background:var(--red);display:inline-block;}
  .wifi-dot.on i{background:var(--green);box-shadow:0 0 8px var(--green);}
  .time-badge{background:var(--surface2);border:1px solid var(--border);border-radius:9px;padding:6px 12px;
              font-size:14px;font-variant-numeric:tabular-nums;letter-spacing:1px;color:var(--accent-light);font-weight:800;}
  .icon-btn{width:38px;height:38px;border-radius:11px;background:var(--surface2);border:1px solid var(--border);
            display:flex;align-items:center;justify-content:center;font-size:17px;cursor:pointer;transition:.15s;}
  .icon-btn:hover{border-color:var(--accent);background:var(--surface3);}

  .container{max-width:600px;margin:0 auto;padding:18px 14px 0;}
  .grid{display:flex;flex-direction:column;gap:16px;}
  .section-title{font-size:10px;font-weight:800;text-transform:uppercase;letter-spacing:1.5px;color:var(--muted);margin-bottom:9px;padding-left:3px;}
  .stats-grid{display:grid;grid-template-columns:1fr 1fr;gap:10px;}
  .stat-card{background:var(--surface2);border:1px solid var(--border);border-radius:var(--radius);padding:14px 15px;}
  .stat-label{font-size:10px;color:var(--muted);font-weight:700;text-transform:uppercase;letter-spacing:.8px;margin-bottom:4px;}
  .stat-value{font-size:26px;font-weight:800;font-variant-numeric:tabular-nums;letter-spacing:-1px;}
  .stat-value.accent{color:var(--accent-light);}
  .stat-value.green{color:var(--green);}
  .pill{display:inline-flex;align-items:center;gap:5px;font-size:11px;font-weight:700;padding:3px 9px;border-radius:20px;margin-top:6px;}
  .pill.idle{background:var(--surface3);color:var(--muted);}
  .pill.active{background:var(--green-dim);color:var(--green);}
  .pill-dot{width:6px;height:6px;border-radius:50%;background:currentColor;}
  .pill.active .pill-dot{animation:pulse 1s infinite;}
  @keyframes pulse{0%,100%{opacity:1}50%{opacity:.25}}

  .card{background:var(--surface);border:1px solid var(--border);border-radius:var(--radius);padding:16px 18px;}
  .btn{display:flex;align-items:center;justify-content:center;gap:8px;padding:11px 16px;border-radius:10px;font-size:13px;
       font-weight:700;cursor:pointer;border:1px solid var(--border2);background:var(--surface2);color:var(--text);transition:all .15s;width:100%;font-family:inherit;}
  .btn:hover{background:var(--surface3);border-color:var(--accent);}
  .btn:active{transform:scale(.98);}
  .btn.primary{background:linear-gradient(135deg,var(--accent),var(--accent2));border-color:transparent;color:#fff;box-shadow:0 4px 16px rgba(124,108,240,.35);}
  .btn.big{padding:16px;font-size:16px;}
  .btn.danger{background:var(--red-dim);border-color:var(--red);color:var(--red);font-size:12px;}
  .btn.sm{padding:7px 13px;font-size:12px;width:auto;}
  .btn.warn{background:var(--amber-dim);border-color:var(--amber);color:var(--amber);}
  .btn.ghost{background:transparent;}
  .input{background:var(--surface2);border:1px solid var(--border2);border-radius:9px;color:var(--text);font-size:13px;
         padding:9px 12px;outline:none;transition:border .15s;font-family:inherit;}
  .input:focus{border-color:var(--accent);}
  .input::placeholder{color:var(--muted);}
  .input.small{width:62px;text-align:center;}
  .time-sep{color:var(--muted);font-size:18px;font-weight:300;}
  .row{display:flex;align-items:center;gap:9px;}
  .settings-label{font-size:11px;color:var(--muted);font-weight:600;display:block;margin-bottom:5px;}
  .divider{border:none;border-top:1px solid var(--border);margin:14px 0;}
  .note{font-size:11px;color:var(--muted);margin-top:8px;line-height:1.5;}

  .profiles{display:flex;gap:8px;flex-wrap:wrap;}
  .chip{padding:9px 15px;border-radius:22px;border:1px solid var(--border2);background:var(--surface2);font-weight:700;
        font-size:13px;cursor:pointer;display:flex;align-items:center;gap:7px;transition:.15s;}
  .chip:hover{border-color:var(--accent);}
  .chip.active{background:linear-gradient(135deg,var(--accent),var(--accent2));border-color:transparent;color:#fff;}
  .chip.add{border-style:dashed;color:var(--muted);}

  .schedule-item{display:flex;align-items:center;gap:9px;background:var(--surface2);border:1px solid var(--border);border-radius:10px;padding:10px 13px;margin-bottom:6px;}
  .schedule-item.disabled{opacity:.4;}
  .schedule-time{font-size:21px;font-weight:800;font-variant-numeric:tabular-nums;letter-spacing:-.5px;flex:1;}
  .toggle{width:38px;height:21px;border-radius:11px;border:none;cursor:pointer;transition:background .2s;flex-shrink:0;position:relative;}
  .toggle.on{background:var(--accent);}
  .toggle.off{background:var(--surface3);border:1px solid var(--border2);}
  .toggle::after{content:'';position:absolute;top:2px;width:17px;height:17px;border-radius:50%;background:#fff;transition:left .2s;}
  .toggle.on::after{left:19px;}.toggle.off::after{left:2px;}

  .info-row{display:flex;align-items:center;justify-content:space-between;padding:8px 0;border-bottom:1px solid var(--border);}
  .info-row:last-child{border-bottom:none;}
  .info-k{font-size:12px;color:var(--muted);font-weight:600;}
  .info-v{font-size:13px;font-weight:700;font-variant-numeric:tabular-nums;}
  .info-v.mono{letter-spacing:.3px;color:var(--accent-light);}
  .badge{display:inline-flex;align-items:center;gap:5px;font-size:11px;padding:3px 9px;border-radius:7px;font-weight:700;}
  .badge.ok{background:var(--green-dim);color:var(--green);}
  .badge.err{background:var(--red-dim);color:var(--red);}

  .hist-item{display:flex;align-items:center;justify-content:space-between;padding:9px 0;border-bottom:1px solid var(--border);}
  .hist-item:last-child{border-bottom:none;}
  .hist-time{font-size:15px;font-weight:800;font-variant-numeric:tabular-nums;}
  .hist-src{font-size:10px;font-weight:700;text-transform:uppercase;letter-spacing:.6px;padding:2px 8px;border-radius:6px;background:var(--surface3);color:var(--muted);}
  .hist-src.manual{color:var(--accent-light);background:var(--accent-dim);}
  .hist-src.agendado{color:var(--green);background:var(--green-dim);}
  .empty{text-align:center;padding:18px 0;color:var(--muted);font-size:13px;}
  .pet-manage{display:flex;align-items:center;justify-content:space-between;padding:8px 0;border-bottom:1px solid var(--border);}
  .pet-manage:last-child{border-bottom:none;}

  .toast{position:fixed;bottom:18px;left:50%;transform:translateX(-50%) translateY(10px);background:var(--surface3);
         border:1px solid var(--border2);border-radius:10px;padding:10px 20px;font-size:13px;font-weight:700;opacity:0;
         transition:all .25s;pointer-events:none;white-space:nowrap;z-index:200;box-shadow:0 8px 24px rgba(0,0,0,.4);}
  .toast.ok{border-color:var(--green);color:var(--green);}
  .toast.err{border-color:var(--red);color:var(--red);}
  .toast.visible{opacity:1;transform:translateX(-50%) translateY(0);}

  .overlay{position:fixed;inset:0;background:rgba(0,0,0,.6);backdrop-filter:blur(5px);display:none;
           align-items:flex-start;justify-content:center;z-index:60;padding:18px;overflow:auto;}
  .overlay.open{display:flex;}
  .modal{background:var(--surface);border:1px solid var(--border2);border-radius:18px;width:100%;max-width:540px;
         padding:20px;margin:18px 0;box-shadow:0 20px 60px rgba(0,0,0,.5);}
  .modal-head{display:flex;align-items:center;justify-content:space-between;margin-bottom:16px;}
  .modal-title{font-size:17px;font-weight:800;}
  .modal-card{background:var(--surface2);border:1px solid var(--border);border-radius:12px;padding:14px;margin-bottom:12px;}
  .modal-card .section-title{margin-bottom:10px;}

  /* ---------- Desktop: dashboard unificado ---------- */
  @media(min-width:860px){
    .container{max-width:1080px;padding:26px 24px 0;}
    .grid{display:grid;grid-template-columns:1fr 1fr;gap:18px;align-items:start;}
    .span2{grid-column:1 / -1;}
    .stat-value{font-size:30px;}
  }
</style>
</head>
<body>
<div class="header">
  <div class="logo">
    <div class="logo-icon">🐾</div>
    <div>
      <div class="logo-text" id="hdr-device">GIZMO</div>
      <div class="logo-sub" id="hdr-pet">Pet: —</div>
    </div>
  </div>
  <div class="hdr-right">
    <span class="wifi-dot" id="wifi-dot"><i></i>OFF</span>
    <span class="time-badge" id="hdr-clock">--:--</span>
    <button class="icon-btn" onclick="openSettings()" title="Configurações">⚙️</button>
  </div>
</div>

<div class="container">
  <div class="grid">

    <!-- Seletor de pets -->
    <div class="span2">
      <div class="section-title">Pets</div>
      <div class="profiles" id="profiles"></div>
    </div>

    <!-- Status -->
    <div class="span2">
      <div class="stats-grid">
        <div class="stat-card">
          <div class="stat-label">Próxima refeição</div>
          <div class="stat-value accent" id="st-next">--:--</div>
          <div class="pill idle" id="pill-next"><span class="pill-dot"></span><span id="st-countdown">—</span></div>
        </div>
        <div class="stat-card">
          <div class="stat-label">Última refeição</div>
          <div class="stat-value green" id="st-last">--:--</div>
          <div class="pill idle" id="pill-motor"><span class="pill-dot"></span><span id="st-motor">Parado</span></div>
        </div>
      </div>
    </div>

    <!-- Alimentar agora -->
    <div class="span2">
      <button class="btn primary big" onclick="feedNow()">🍽 Alimentar agora</button>
    </div>

    <!-- Horários -->
    <div>
      <div class="section-title">Horários de alimentação</div>
      <div class="card">
        <div class="row">
          <input class="input small" type="number" id="add-h" placeholder="HH" min="0" max="23">
          <span class="time-sep">:</span>
          <input class="input small" type="number" id="add-m" placeholder="MM" min="0" max="59">
          <button class="btn sm primary" onclick="addSchedule()" style="margin-left:4px">+ Adicionar</button>
        </div>
        <div id="schedule-list" style="margin-top:12px"></div>
      </div>
    </div>

    <!-- Perfil do pet (alimentação) -->
    <div>
      <div class="section-title">Perfil do pet</div>
      <div class="card">
        <label class="settings-label">Nome do pet</label>
        <input class="input" type="text" id="pet-name" maxlength="22" style="width:100%;margin-bottom:11px">
        <label class="settings-label">Porção (segundos de motor)</label>
        <input class="input small" type="number" id="pet-portion" step="0.5" min="0.5" max="30">
        <button class="btn" style="margin-top:13px" onclick="savePet()">Salvar perfil</button>
      </div>
    </div>

    <!-- Histórico -->
    <div>
      <div class="section-title">Histórico recente</div>
      <div class="card"><div id="history-list"></div></div>
    </div>

  </div>
</div>

<!-- ===================== MODAL DE CONFIGURAÇÕES ===================== -->
<div class="overlay" id="settings-overlay">
  <div class="modal">
    <div class="modal-head">
      <span class="modal-title">⚙️ Configurações</span>
      <button class="icon-btn" onclick="closeSettings()">✕</button>
    </div>

    <!-- Conexão -->
    <div class="modal-card">
      <div class="section-title">Conexão</div>
      <div class="info-row"><span class="info-k">Rede de casa</span><span class="badge err" id="net-sta">Desconectado</span></div>
      <div class="info-row"><span class="info-k">IP na sua rede</span><span class="info-v mono" id="net-staip">—</span></div>
      <div class="info-row"><span class="info-k">IP do ponto GIZMO</span><span class="info-v mono" id="net-apip">192.168.4.1</span></div>
      <hr class="divider">
      <div class="row" style="flex-wrap:wrap;gap:7px">
        <input class="input" type="text" id="wifi-ssid" placeholder="Rede (2.4GHz)" style="flex:1;min-width:150px">
        <input class="input" type="password" id="wifi-pass" placeholder="Senha" style="flex:1;min-width:120px">
      </div>
      <div class="row" style="margin-top:8px;gap:7px">
        <button class="btn primary" onclick="saveWifi()">Conectar</button>
        <button class="btn ghost sm" onclick="forgetWifi()">Esquecer</button>
      </div>
      <p class="note">Só redes 2.4 GHz. Ao religar, ele reconecta sozinho nesta rede e o NTP acerta a hora.</p>
    </div>

    <!-- Dispositivo -->
    <div class="modal-card">
      <div class="section-title">Dispositivo</div>
      <label class="settings-label">Nome do dispositivo</label>
      <input class="input" type="text" id="cfg-device" maxlength="32" style="width:100%;margin-bottom:11px">
      <label class="settings-label">Fuso horário (UTC) · Brasília = -3</label>
      <input class="input small" type="number" id="cfg-tz" min="-12" max="14">
      <button class="btn" style="margin-top:13px" onclick="saveDevice()">Salvar dispositivo</button>
    </div>

    <!-- Relógio manual -->
    <div class="modal-card">
      <div class="section-title">Relógio (sem internet)</div>
      <div class="info-row" style="border:none;padding-top:0">
        <span class="info-k">Sincronização NTP</span><span class="badge err" id="ntp-badge">Sem sinal</span>
      </div>
      <div class="row">
        <input class="input small" type="number" id="man-h" placeholder="HH" min="0" max="23">
        <span class="time-sep">:</span>
        <input class="input small" type="number" id="man-m" placeholder="MM" min="0" max="59">
        <button class="btn sm warn" onclick="setManualTime()" style="margin-left:4px">Definir hora</button>
      </div>
      <p class="note">Use só quando não houver internet. A hora é gravada e recuperada após queda de energia.</p>
    </div>

    <!-- Gerenciar pets -->
    <div class="modal-card">
      <div class="section-title">Gerenciar pets</div>
      <div id="pet-manage-list"></div>
    </div>

    <!-- Manutenção -->
    <div class="modal-card">
      <div class="section-title">Manutenção</div>
      <button class="btn warn" onclick="testRelay()">⚙ Testar relé (1s)</button>
      <p class="note">Aciona o relé por 1 s sem contar como refeição. Você deve ouvir o clique.</p>
    </div>

  </div>
</div>

<div class="toast" id="toast"></div>

<script>
const $ = id => document.getElementById(id);
let state = null;        // último /status recebido
let lastActive = -1;     // perfil ativo já refletido nos inputs

async function api(path){ try{ return await (await fetch(path)).json(); }catch(e){ toast('Erro de conexão','err'); return null; } }
function toast(msg,type='ok'){ const el=$('toast'); el.textContent=msg; el.className='toast '+type+' visible'; setTimeout(()=>el.className='toast '+type,2400); }
function settingsOpen(){ return $('settings-overlay').classList.contains('open'); }

function renderProfiles(d){
  const el=$('profiles');
  let html=d.profiles.map((name,i)=>`
    <div class="chip ${i===d.activeProfile?'active':''}" onclick="selectPet(${i})">🐾 ${name}</div>`).join('');
  if(d.profiles.length<4) html+=`<div class="chip add" onclick="addPet()">+ pet</div>`;
  el.innerHTML=html;
}

function renderSchedules(s){
  const el=$('schedule-list');
  if(!s.length){ el.innerHTML='<div class="empty">⏰ Nenhum horário cadastrado</div>'; return; }
  const sorted=s.map((x,i)=>({...x,idx:i})).sort((a,b)=>(a.h*60+a.m)-(b.h*60+b.m));
  el.innerHTML=sorted.map(x=>`
    <div class="schedule-item ${x.on?'':'disabled'}">
      <span class="schedule-time">${String(x.h).padStart(2,'0')}:${String(x.m).padStart(2,'0')}</span>
      <button class="toggle ${x.on?'on':'off'}" onclick="toggleSch(${x.idx})"></button>
      <button class="btn danger sm" onclick="removeSch(${x.idx})">✕</button>
    </div>`).join('');
}

function renderHistory(h){
  const el=$('history-list');
  if(!h||!h.length){ el.innerHTML='<div class="empty">Nenhuma refeição ainda</div>'; return; }
  el.innerHTML=h.map(e=>`<div class="hist-item"><span class="hist-time">${e.t}</span><span class="hist-src ${e.src}">${e.src}</span></div>`).join('');
}

function renderPetManage(d){
  const el=$('pet-manage-list');
  el.innerHTML=d.profiles.map((name,i)=>`
    <div class="pet-manage">
      <span style="font-weight:700">${i===d.activeProfile?'▶ ':''}${name}</span>
      ${d.profiles.length>1?`<button class="btn danger sm" onclick="deletePet(${i})">Excluir</button>`:'<span class="note" style="margin:0">único</span>'}
    </div>`).join('');
}

async function refresh(){
  const d=await api('/status'); if(!d) return; state=d;
  $('hdr-clock').textContent=d.time;
  $('hdr-device').textContent=d.device;
  $('hdr-pet').textContent='Pet: '+d.pet;
  $('st-next').textContent=d.nextTime;
  $('st-last').textContent=d.lastFeed;
  $('st-countdown').textContent=d.countdown;

  const mp=$('pill-motor'), mt=$('st-motor');
  if(d.motor){ mp.className='pill active'; mt.textContent='Alimentando…'; }
  else{ mp.className='pill idle'; mt.textContent='Parado'; }

  const wd=$('wifi-dot');
  wd.className='wifi-dot'+(d.wifi.sta?' on':'');
  wd.lastChild.textContent=d.wifi.sta?'ON':'AP';

  // Infos de conexão (read-only) — sempre atualizadas
  const ns=$('net-sta');
  ns.textContent=d.wifi.sta?('Conectado'+(d.wifi.ssid?' · '+d.wifi.ssid:'')):'Desconectado';
  ns.className='badge '+(d.wifi.sta?'ok':'err');
  $('net-staip').textContent=d.wifi.staIp||'—';
  $('net-apip').textContent=d.wifi.apIp;
  const nb=$('ntp-badge'); nb.textContent=d.ntp?'Sincronizado':'Sem sinal'; nb.className='badge '+(d.ntp?'ok':'err');

  renderProfiles(d);
  renderSchedules(d.schedules);
  renderHistory(d.history);
  renderPetManage(d);

  // Inputs do perfil só são repreenchidos quando o pet ativo muda
  if(d.activeProfile!==lastActive){
    $('pet-name').value=d.pet;
    $('pet-portion').value=d.portion;
    lastActive=d.activeProfile;
  }
}

// ----- Alimentação -----
async function feedNow(){ const d=await api('/feed'); if(d&&d.ok) toast('🍽 Ração liberada!'); setTimeout(refresh,600); }
async function testRelay(){ const d=await api('/test-relay'); if(d&&d.ok) toast('⚙ Relé acionado por 1s'); }

async function addSchedule(){
  const h=parseInt($('add-h').value), m=parseInt($('add-m').value);
  if(isNaN(h)||isNaN(m)||h<0||h>23||m<0||m>59){ toast('Horário inválido','err'); return; }
  const d=await api(`/add-schedule?h=${h}&m=${m}`);
  if(d&&d.ok){ $('add-h').value=''; $('add-m').value=''; toast('Horário adicionado'); refresh(); }
  else if(d&&d.error) toast(d.error,'err');
}
async function removeSch(i){ const d=await api(`/remove-schedule?i=${i}`); if(d&&d.ok){ toast('Horário removido'); refresh(); } }
async function toggleSch(i){ await api(`/toggle-schedule?i=${i}`); refresh(); }

// ----- Perfis de pets -----
async function selectPet(i){ if(state&&i===state.activeProfile) return; const d=await api(`/profile-select?i=${i}`); if(d&&d.ok){ lastActive=-1; refresh(); } }
async function addPet(){
  const name=prompt('Nome do novo pet:'); if(name===null) return;
  const d=await api(`/profile-add?name=${encodeURIComponent(name.trim()||'Pet')}`);
  if(d&&d.ok){ lastActive=-1; toast('Pet adicionado'); refresh(); }
  else if(d&&d.error) toast(d.error,'err');
}
async function deletePet(i){
  if(!confirm('Excluir este pet e seus horários?')) return;
  const d=await api(`/profile-delete?i=${i}`);
  if(d&&d.ok){ lastActive=-1; toast('Pet excluído'); refresh(); }
  else if(d&&d.error) toast(d.error,'err');
}
async function savePet(){
  const name=encodeURIComponent($('pet-name').value.trim());
  const portion=$('pet-portion').value;
  if(!name){ toast('Informe o nome do pet','err'); return; }
  const d=await api(`/save-config?pet=${name}&portion=${portion}`);
  if(d&&d.ok){ toast('Perfil salvo'); lastActive=-1; refresh(); }
}

// ----- Configurações (modal) -----
function openSettings(){
  if(state){
    $('cfg-device').value=state.device;
    $('cfg-tz').value=state.tz;
    if(!$('wifi-ssid').value) $('wifi-ssid').value=state.wifi.ssid||'';
  }
  $('settings-overlay').classList.add('open');
}
function closeSettings(){ $('settings-overlay').classList.remove('open'); }
$('settings-overlay').addEventListener('click',e=>{ if(e.target.id==='settings-overlay') closeSettings(); });

async function saveDevice(){
  const dev=encodeURIComponent($('cfg-device').value.trim());
  const tz=$('cfg-tz').value;
  if(!dev){ toast('Informe o nome','err'); return; }
  const d=await api(`/save-config?device=${dev}&tz=${tz}`);
  if(d&&d.ok){ toast('Dispositivo salvo'); refresh(); }
}
async function saveWifi(){
  const ssid=encodeURIComponent($('wifi-ssid').value.trim());
  const pass=encodeURIComponent($('wifi-pass').value);
  if(!ssid){ toast('Informe a rede','err'); return; }
  toast('Conectando…');
  const d=await api(`/wifi?ssid=${ssid}&pass=${pass}`);
  if(d&&d.ok){ toast('Tentando conectar…'); setTimeout(refresh,6000); }
}
async function forgetWifi(){ const d=await api('/wifi-forget'); if(d&&d.ok){ toast('Rede esquecida'); $('wifi-pass').value=''; refresh(); } }
async function setManualTime(){
  const h=parseInt($('man-h').value), m=parseInt($('man-m').value);
  if(isNaN(h)||isNaN(m)||h<0||h>23||m<0||m>59){ toast('Hora inválida','err'); return; }
  const d=await api(`/set-time?h=${h}&m=${m}`); if(d&&d.ok) toast('Hora definida e gravada');
}

refresh();
setInterval(refresh,5000);
</script>
</body>
</html>
)rawhtml";

#endif // INDEX_HTML_H
