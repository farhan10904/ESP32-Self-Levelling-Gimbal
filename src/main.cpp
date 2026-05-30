#include <Wire.h>
#include <MPU6050.h>
#include <ESP32Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebServer.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* ssid     = "Wokwi-GUEST";
const char* password = "";
WebServer server(80);

MPU6050 mpu;
Servo rollServo;
Servo pitchServo;

float roll = 0, pitch = 0;
unsigned long lastTime = 0;
const float ALPHA = 0.96;

// PID
const float rollKp = 2.5, rollKi = 0.0, rollKd = 0.4;
const float pitchKp = 2.5, pitchKi = 0.0, pitchKd = 0.4;
float rollError = 0, lastRollError = 0, rollIntegral = 0;
float pitchError = 0, lastPitchError = 0, pitchIntegral = 0;

// Control mode
bool dashboardMode = false;
float targetRoll  = 0.0;
float targetPitch = 0.0;
unsigned long lastHeartbeat = 0;
const unsigned long HEARTBEAT_TIMEOUT = 3000;

void handleRoot() {
  String html = R"rawhtml(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>Gimbal Control</title>
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body { font-family: 'Courier New', monospace;  background: #000; color: #00ff66; margin: 0;min-height: 100vh;display: flex;justify-content: center;align-items: center; }
    .dashboard { border: 1px solid #00ff66; padding: 24px; background: rgba(0,0,0,0.85); width: fit-content; margin: 0; }
    .header { display: flex; justify-content: space-between; align-items: flex-start; border-bottom: 1px solid rgba(0,255,100,0.3); padding-bottom: 14px; margin-bottom: 20px; }
    h1 { font-size: 18px; letter-spacing: 1px; }
    .subtitle { font-size: 11px; color: #77ff99; margin-top: 4px; }
    .pill { padding: 6px 14px; border: 1px solid #00ff66; border-radius: 999px; font-size: 11px; font-weight: bold; letter-spacing: 1px; }
    .pill.tilted { border-color: #ff3333; color: #ff3333; }
    .main { display: grid; grid-template-columns: 300px 650px; gap: 20px; }
    .gauges { display: flex; flex-direction: column; gap: 16px; }
    .gauge-box { border: 1px solid rgba(0,255,100,0.4); padding: 16px; background: rgba(0,30,10,0.4); display: flex; flex-direction: column; align-items: center; }
    .blabel { font-size: 10px; letter-spacing: 2px; color: #77ff99; margin-bottom: 12px; align-self: flex-start; }
    canvas { display: block; }
    .val-row { margin-top: 10px; text-align: center; }
    .big-val { font-size: 30px; font-weight: bold; color: #fff; }
    .unit { font-size: 11px; color: #77ff99; margin-top: 2px; }
    .controls { display: flex; flex-direction: column; gap: 16px; }
    .ctrl-box { border: 1px solid rgba(0,255,100,0.4); padding: 18px; background: rgba(0,30,10,0.4); }
    .toggle-row { display: flex; align-items: center; gap: 12px; }
    .switch { position: relative; width: 44px; height: 22px; flex-shrink: 0; }
    .switch input { opacity: 0; width: 0; height: 0; }
    .s-track { position: absolute; inset: 0; background: rgba(0,255,100,0.1); border: 1px solid #00ff66; border-radius: 999px; cursor: pointer; transition: background 0.2s; }
    .s-track:before { content: ''; position: absolute; width: 16px; height: 16px; left: 2px; top: 2px; background: #00ff66; border-radius: 50%; transition: transform 0.2s; }
    input:checked + .s-track { background: rgba(0,255,100,0.25); }
    input:checked + .s-track:before { transform: translateX(22px); }
    .mode-text { font-size: 13px; color: #fff; }
    .manual { display: none; margin-top: 18px; border-top: 1px solid rgba(0,255,100,0.2); padding-top: 18px; max-width: 550px; }
    .manual.active { display: block; }
    .ctrl-row { margin-bottom: 18px; }
    .ctrl-row label { font-size: 10px; letter-spacing: 1px; color: #77ff99; display: flex; justify-content: space-between; margin-bottom: 8px; }
    .ctrl-row label span { color: #fff; font-size: 12px; }
    input[type=range] { width: 100%; accent-color: #00ff66; }
    button { background: transparent; border: 1px solid #00ff66; color: #00ff66; padding: 8px 18px; font-family: 'Courier New', monospace; font-size: 11px; letter-spacing: 1px; cursor: pointer; }
    button:hover { background: rgba(0,255,100,0.1); }
    .info-box { border: 1px solid rgba(0,255,100,0.4); padding: 16px; background: rgba(0,30,10,0.4); display: grid; grid-template-columns: 1fr 1fr; gap: 14px; }
    .stat .slabel { font-size: 10px; letter-spacing: 1px; color: #77ff99; }
    .stat .sval { font-size: 14px; color: #fff; margin-top: 3px; }
  </style>
</head>
<body>
<div class="dashboard">
  <div class="header">
    <div>
      <h1>ESP32 SELF-LEVELLING GIMBAL</h1>
      <div class="subtitle">MPU6050 IMU + MG996R servos + PID stabilisation + live WiFi dashboard</div>
    </div>
    <div class="pill" id="pill">LEVEL</div>
  </div>

  <div class="main">
    <div class="gauges">
      <div class="gauge-box">
        <div class="blabel">ROLL AXIS</div>
        <canvas id="rollC" width="220" height="130"></canvas>
        <div class="val-row">
          <div class="big-val" id="rollVal">0.00</div>
          <div class="unit">degrees</div>
        </div>
      </div>
      <div class="gauge-box">
        <div class="blabel">PITCH AXIS</div>
        <canvas id="pitchC" width="220" height="130"></canvas>
        <div class="val-row">
          <div class="big-val" id="pitchVal">0.00</div>
          <div class="unit">degrees</div>
        </div>
      </div>
    </div>

    <div class="controls">
      <div class="ctrl-box">
        <div class="blabel">CONTROL MODE</div>
        <div class="toggle-row">
          <label class="switch">
            <input type="checkbox" id="modeToggle" onchange="toggleMode()">
            <span class="s-track"></span>
          </label>
          <span class="mode-text" id="modeText">MPU STABILISE</span>
        </div>
        <div class="manual" id="manual">
          <div class="ctrl-row">
            <label>TARGET ROLL <span id="tRollOut">0&deg;</span></label>
            <input type="range" min="-90" max="90" value="0" id="tRoll" oninput="updateSP()">
          </div>
          <div class="ctrl-row">
            <label>TARGET PITCH <span id="tPitchOut">0&deg;</span></label>
            <input type="range" min="-90" max="90" value="0" id="tPitch" oninput="updateSP()">
          </div>
          <button onclick="resetFlat()">RESET TO FLAT</button>
        </div>
      </div>

      <div class="info-box">
        <div class="stat"><div class="slabel">UPDATE RATE</div><div class="sval">10 Hz</div></div>
        <div class="stat"><div class="slabel">FILTER</div><div class="sval">Complementary</div></div>
        <div class="stat"><div class="slabel">ALPHA</div><div class="sval">0.96</div></div>
        <div class="stat"><div class="slabel">DATA SOURCE</div><div class="sval">ESP32 /data</div></div>
      </div>
    </div>
  </div>
</div>

<script>
function drawGauge(canvas, angle) {
  const ctx = canvas.getContext('2d');
  const w = canvas.width, h = canvas.height;
  ctx.clearRect(0, 0, w, h);
  const cx = w/2, cy = h-8, r = h-18;
  for (let a = 0; a <= 180; a += 30) {
    const rad = (a*Math.PI)/180;
    ctx.beginPath();
    ctx.moveTo(cx+(r-12)*Math.cos(Math.PI-rad), cy-(r-12)*Math.sin(Math.PI-rad));
    ctx.lineTo(cx+r*Math.cos(Math.PI-rad), cy-r*Math.sin(Math.PI-rad));
    ctx.strokeStyle='rgba(0,255,100,0.2)'; ctx.lineWidth=1; ctx.stroke();
  }
  ctx.beginPath(); ctx.arc(cx,cy,r,Math.PI,0);
  ctx.strokeStyle='rgba(0,255,100,0.25)'; ctx.lineWidth=1; ctx.stroke();
  ctx.beginPath(); ctx.arc(cx,cy,r-12,Math.PI,0);
  ctx.strokeStyle='rgba(0,255,100,0.08)'; ctx.lineWidth=9; ctx.stroke();
  const clipped=Math.max(-90,Math.min(90,angle));
  const na=Math.PI-((clipped+90)/180)*Math.PI;
  const col=Math.abs(angle)<5?'#00ff66':'#ff3333';
  ctx.beginPath(); ctx.moveTo(cx,cy);
  ctx.lineTo(cx+(r-14)*Math.cos(na),cy-(r-14)*Math.sin(na));
  ctx.strokeStyle=col; ctx.lineWidth=2.5; ctx.stroke();
  ctx.beginPath(); ctx.arc(cx,cy,5,0,Math.PI*2);
  ctx.fillStyle=col; ctx.fill();
  ctx.fillStyle='rgba(0,255,100,0.5)'; ctx.font='10px Courier New'; ctx.textAlign='center';
  ctx.fillText('-90\u00b0',cx-r+8,cy);
  ctx.fillText('0\u00b0',cx,cy-r+13);
  ctx.fillText('+90\u00b0',cx+r-8,cy);
}

function toggleMode() {
  const on = document.getElementById('modeToggle').checked;
  document.getElementById('modeText').textContent = on ? 'DASHBOARD CONTROL' : 'MPU STABILISE';
  document.getElementById('manual').className = 'manual'+(on?' active':'');
  fetch('/mode?dashboard='+(on?'1':'0'));
  if (!on) fetch('/setpoint?roll=0&pitch=0');
}

function updateSP() {
  const r = document.getElementById('tRoll').value;
  const p = document.getElementById('tPitch').value;
  document.getElementById('tRollOut').textContent  = r+'\u00b0';
  document.getElementById('tPitchOut').textContent = p+'\u00b0';
  fetch('/setpoint?roll='+r+'&pitch='+p);
}

function resetFlat() {
  document.getElementById('tRoll').value=0;
  document.getElementById('tPitch').value=0;
  updateSP();
}

setInterval(()=>fetch('/heartbeat'),1000);

setInterval(()=>{
  fetch('/data').then(r=>r.json()).then(d=>{
    document.getElementById('rollVal').textContent  = d.roll.toFixed(2);
    document.getElementById('pitchVal').textContent = d.pitch.toFixed(2);
    drawGauge(document.getElementById('rollC'),  d.roll);
    drawGauge(document.getElementById('pitchC'), d.pitch);
    const tilted=Math.abs(d.roll)>5||Math.abs(d.pitch)>5;
    const pill=document.getElementById('pill');
    pill.textContent=tilted?'TILTED':'LEVEL';
    pill.className='pill'+(tilted?' tilted':'');
  });
},100);

drawGauge(document.getElementById('rollC'),0);
drawGauge(document.getElementById('pitchC'),0);
</script>
</body></html>
)rawhtml";
  server.send(200, "text/html", html);
}

void handleData() {
  String json = "{\"roll\":" + String(roll,2) + ",\"pitch\":" + String(pitch,2) + "}";
  server.send(200, "application/json", json);
}

void handleHeartbeat() {
  lastHeartbeat = millis();
  server.send(200, "text/plain", "ok");
}

void handleMode() {
  if (server.hasArg("dashboard")) {
    dashboardMode = server.arg("dashboard") == "1";
    if (!dashboardMode) { targetRoll = 0; targetPitch = 0; }
  }
  server.send(200, "text/plain", "ok");
}

void handleSetpoint() {
  if (server.hasArg("roll"))  targetRoll  = server.arg("roll").toFloat();
  if (server.hasArg("pitch")) targetPitch = server.arg("pitch").toFloat();
  server.send(200, "text/plain", "ok");
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  mpu.initialize();
  rollServo.attach(13);
  pitchServo.attach(12);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println(WiFi.localIP());

  server.on("/",          handleRoot);
  server.on("/data",      handleData);
  server.on("/heartbeat", handleHeartbeat);
  server.on("/mode",      handleMode);
  server.on("/setpoint",  handleSetpoint);
  server.begin();

  lastHeartbeat = millis();
  lastTime = millis();
}

void loop() {
  server.handleClient();

  // Heartbeat timeout — snap back to MPU mode
  if (dashboardMode && (millis() - lastHeartbeat > HEARTBEAT_TIMEOUT)) {
    dashboardMode = false;
    targetRoll = 0;
    targetPitch = 0;
  }

  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0;
  lastTime = now;

  float accelRoll  = atan2(ay, az) * 180.0 / PI;
  float accelPitch = atan2(-ax, sqrt(ay*ay + az*az)) * 180.0 / PI;
  float gyroRollRate  = gx / 131.0;
  float gyroPitchRate = gy / 131.0;

  roll  = ALPHA * (roll  + gyroRollRate  * dt) + (1 - ALPHA) * accelRoll;
  pitch = ALPHA * (pitch + gyroPitchRate * dt) + (1 - ALPHA) * accelPitch;

  float setRoll  = dashboardMode ? targetRoll  : 0.0;
  float setPitch = dashboardMode ? targetPitch : 0.0;

  rollError  = setRoll  - roll;
  pitchError = setPitch - pitch;

  rollIntegral  += rollError  * dt;
  pitchIntegral += pitchError * dt;

  float rollDerivative  = (rollError  - lastRollError)  / dt;
  float pitchDerivative = (pitchError - lastPitchError) / dt;

  float rollOutput  = rollKp  * rollError  + rollKi  * rollIntegral  + rollKd  * rollDerivative;
  float pitchOutput = pitchKp * pitchError + pitchKi * pitchIntegral + pitchKd * pitchDerivative;

  lastRollError  = rollError;
  lastPitchError = pitchError;

  int rollPos  = constrain(90 + (int)rollOutput,  0, 180);
  int pitchPos = constrain(90 + (int)pitchOutput, 0, 180);

  rollServo.write(rollPos);
  pitchServo.write(pitchPos);

  // OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(dashboardMode ? "MODE: DASHBOARD" : "MODE: STABILISE");
  display.setTextSize(2);
  display.setCursor(0, 16);
  display.print("R:"); display.println(roll, 1);
  display.print("P:"); display.println(pitch, 1);
  display.display();

  Serial.print("Roll: "); Serial.print(roll);
  Serial.print("  Pitch: "); Serial.println(pitch);

  delay(10);
}