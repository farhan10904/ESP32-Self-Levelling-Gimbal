#include <Wire.h>
#include <MPU6050.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>

// PIN CONFIGURATION
constexpr int SDA_PIN = 21;
constexpr int SCL_PIN = 22;

constexpr int ROLL_SERVO_PIN = 13;
constexpr int PITCH_SERVO_PIN = 19;

// WIFI CONFIGURATION
const char* ssid = "Wokwi-GUEST";
const char* password = "";

constexpr unsigned long WIFI_TIMEOUT_MS = 15000;

WebServer server(80);


// HARDWARE OBJECTS
MPU6050 mpu;

Servo rollServo;
Servo pitchServo;

// SERVO CONFIGURATION
constexpr int SERVO_MIN_PULSE_US = 500;
constexpr int SERVO_MAX_PULSE_US = 2500;

// Calibrated mechanical neutral positions
constexpr int ROLL_NEUTRAL = 99;
constexpr int PITCH_NEUTRAL = 85;

// Restricted prototype travel
constexpr int ROLL_MIN_POSITION = ROLL_NEUTRAL - 5;
constexpr int ROLL_MAX_POSITION = ROLL_NEUTRAL + 5;

constexpr int PITCH_MIN_POSITION = PITCH_NEUTRAL - 5;
constexpr int PITCH_MAX_POSITION = PITCH_NEUTRAL + 5;

constexpr float MAX_CONTROL_OUTPUT = 5.0f;


// MPU6050 / FILTER CONFIGURATION
constexpr float ALPHA = 0.96f;

float roll = 0.0f;
float pitch = 0.0f;

float rollOffset = 0.0f;
float pitchOffset = 0.0f;

unsigned long lastControlTimeMicros = 0;

constexpr float MIN_DT_SECONDS = 0.001f;
constexpr float MAX_DT_SECONDS = 0.100f;

// PID CONFIGURATION
constexpr float rollKp = 1.5f;
constexpr float rollKi = 0.0f;
constexpr float rollKd = 0.1f;

constexpr float pitchKp = 1.5f;
constexpr float pitchKi = 0.0f;
constexpr float pitchKd = 0.1f;

float rollError = 0.0f;
float lastRollError = 0.0f;
float rollIntegral = 0.0f;

float pitchError = 0.0f;
float lastPitchError = 0.0f;
float pitchIntegral = 0.0f;

constexpr float INTEGRAL_LIMIT = 10.0f;

bool firstControlCycle = true;

// CONTROL MODE

bool dashboardMode = false;

float targetRoll = 0.0f;
float targetPitch = 0.0f;

unsigned long lastHeartbeat = 0;

constexpr unsigned long HEARTBEAT_TIMEOUT_MS = 3000;

// SERIAL OUTPUT TIMING
unsigned long lastSerialUpdate = 0;

constexpr unsigned long SERIAL_UPDATE_INTERVAL_MS = 100;


// DASHBOARD
void handleRoot() {
  String html = R"rawhtml(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>Gimbal Control</title>

  <style>
    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
    }

    body {
      font-family: 'Courier New', monospace;
      background: #000;
      color: #00ff66;
      min-height: 100vh;
      display: flex;
      justify-content: center;
      align-items: center;
    }

    .dashboard {
      border: 1px solid #00ff66;
      padding: 24px;
      background: rgba(0, 0, 0, 0.85);
      width: fit-content;
    }

    .header {
      display: flex;
      justify-content: space-between;
      align-items: flex-start;
      border-bottom: 1px solid rgba(0, 255, 100, 0.3);
      padding-bottom: 14px;
      margin-bottom: 20px;
    }

    h1 {
      font-size: 18px;
      letter-spacing: 1px;
    }

    .subtitle {
      font-size: 11px;
      color: #77ff99;
      margin-top: 4px;
    }

    .pill {
      padding: 6px 14px;
      border: 1px solid #00ff66;
      border-radius: 999px;
      font-size: 11px;
      font-weight: bold;
      letter-spacing: 1px;
    }

    .pill.tilted {
      border-color: #ff3333;
      color: #ff3333;
    }

    .main {
      display: grid;
      grid-template-columns: 300px 650px;
      gap: 20px;
    }

    .gauges {
      display: flex;
      flex-direction: column;
      gap: 16px;
    }

    .gauge-box {
      border: 1px solid rgba(0, 255, 100, 0.4);
      padding: 16px;
      background: rgba(0, 30, 10, 0.4);
      display: flex;
      flex-direction: column;
      align-items: center;
    }

    .blabel {
      font-size: 10px;
      letter-spacing: 2px;
      color: #77ff99;
      margin-bottom: 12px;
      align-self: flex-start;
    }

    canvas {
      display: block;
    }

    .val-row {
      margin-top: 10px;
      text-align: center;
    }

    .big-val {
      font-size: 30px;
      font-weight: bold;
      color: #fff;
    }

    .unit {
      font-size: 11px;
      color: #77ff99;
      margin-top: 2px;
    }

    .controls {
      display: flex;
      flex-direction: column;
      gap: 16px;
    }

    .ctrl-box {
      border: 1px solid rgba(0, 255, 100, 0.4);
      padding: 18px;
      background: rgba(0, 30, 10, 0.4);
    }

    .toggle-row {
      display: flex;
      align-items: center;
      gap: 12px;
    }

    .switch {
      position: relative;
      width: 44px;
      height: 22px;
      flex-shrink: 0;
    }

    .switch input {
      opacity: 0;
      width: 0;
      height: 0;
    }

    .s-track {
      position: absolute;
      inset: 0;
      background: rgba(0, 255, 100, 0.1);
      border: 1px solid #00ff66;
      border-radius: 999px;
      cursor: pointer;
      transition: background 0.2s;
    }

    .s-track::before {
      content: '';
      position: absolute;
      width: 16px;
      height: 16px;
      left: 2px;
      top: 2px;
      background: #00ff66;
      border-radius: 50%;
      transition: transform 0.2s;
    }

    input:checked + .s-track {
      background: rgba(0, 255, 100, 0.25);
    }

    input:checked + .s-track::before {
      transform: translateX(22px);
    }

    .mode-text {
      font-size: 13px;
      color: #fff;
    }

    .manual {
      display: none;
      margin-top: 18px;
      border-top: 1px solid rgba(0, 255, 100, 0.2);
      padding-top: 18px;
      max-width: 550px;
    }

    .manual.active {
      display: block;
    }

    .ctrl-row {
      margin-bottom: 18px;
    }

    .ctrl-row label {
      font-size: 10px;
      letter-spacing: 1px;
      color: #77ff99;
      display: flex;
      justify-content: space-between;
      margin-bottom: 8px;
    }

    .ctrl-row label span {
      color: #fff;
      font-size: 12px;
    }

    input[type=range] {
      width: 100%;
      accent-color: #00ff66;
    }

    button {
      background: transparent;
      border: 1px solid #00ff66;
      color: #00ff66;
      padding: 8px 18px;
      font-family: 'Courier New', monospace;
      font-size: 11px;
      letter-spacing: 1px;
      cursor: pointer;
    }

    button:hover {
      background: rgba(0, 255, 100, 0.1);
    }

    .info-box {
      border: 1px solid rgba(0, 255, 100, 0.4);
      padding: 16px;
      background: rgba(0, 30, 10, 0.4);
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 14px;
    }

    .stat .slabel {
      font-size: 10px;
      letter-spacing: 1px;
      color: #77ff99;
    }

    .stat .sval {
      font-size: 14px;
      color: #fff;
      margin-top: 3px;
    }
  </style>
</head>

<body>
<div class="dashboard">

  <div class="header">
    <div>
      <h1>ESP32 SELF-LEVELLING GIMBAL</h1>

      <div class="subtitle">
        MPU6050 IMU + MG996R servos + PID stabilisation + live WiFi dashboard
      </div>
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

            <input
              type="checkbox"
              id="modeToggle"
              onchange="toggleMode()"
            >

            <span class="s-track"></span>
          </label>

          <span class="mode-text" id="modeText">
            MPU STABILISE
          </span>
        </div>

        <div class="manual" id="manual">

          <div class="ctrl-row">
            <label>
              TARGET ROLL
              <span id="tRollOut">0&deg;</span>
            </label>

            <input
              type="range"
              min="-5"
              max="5"
              value="0"
              step="1"
              id="tRoll"
              oninput="updateSP()"
            >
          </div>

          <div class="ctrl-row">
            <label>
              TARGET PITCH
              <span id="tPitchOut">0&deg;</span>
            </label>

            <input
              type="range"
              min="-5"
              max="5"
              value="0"
              step="1"
              id="tPitch"
              oninput="updateSP()"
            >
          </div>

          <button onclick="resetFlat()">
            RESET TO FLAT
          </button>

        </div>
      </div>

      <div class="info-box">

        <div class="stat">
          <div class="slabel">UPDATE RATE</div>
          <div class="sval">10 Hz</div>
        </div>

        <div class="stat">
          <div class="slabel">FILTER</div>
          <div class="sval">Complementary</div>
        </div>

        <div class="stat">
          <div class="slabel">ALPHA</div>
          <div class="sval">0.96</div>
        </div>

        <div class="stat">
          <div class="slabel">SERVO RANGE</div>
          <div class="sval">&plusmn;5&deg;</div>
        </div>

      </div>
    </div>
  </div>
</div>

<script>
function drawGauge(canvas, angle) {
  const ctx = canvas.getContext('2d');

  const w = canvas.width;
  const h = canvas.height;

  ctx.clearRect(0, 0, w, h);

  const cx = w / 2;
  const cy = h - 8;
  const radius = h - 18;

  for (let degrees = 0; degrees <= 180; degrees += 30) {
    const radians = degrees * Math.PI / 180;

    ctx.beginPath();

    ctx.moveTo(
      cx + (radius - 12) * Math.cos(Math.PI - radians),
      cy - (radius - 12) * Math.sin(Math.PI - radians)
    );

    ctx.lineTo(
      cx + radius * Math.cos(Math.PI - radians),
      cy - radius * Math.sin(Math.PI - radians)
    );

    ctx.strokeStyle = 'rgba(0,255,100,0.2)';
    ctx.lineWidth = 1;
    ctx.stroke();
  }

  ctx.beginPath();
  ctx.arc(cx, cy, radius, Math.PI, 0);
  ctx.strokeStyle = 'rgba(0,255,100,0.25)';
  ctx.lineWidth = 1;
  ctx.stroke();

  ctx.beginPath();
  ctx.arc(cx, cy, radius - 12, Math.PI, 0);
  ctx.strokeStyle = 'rgba(0,255,100,0.08)';
  ctx.lineWidth = 9;
  ctx.stroke();

  const clippedAngle = Math.max(
    -90,
    Math.min(90, angle)
  );

  const needleAngle =
    Math.PI -
    ((clippedAngle + 90) / 180) *
    Math.PI;

  const colour =
    Math.abs(angle) < 5 ?
    '#00ff66' :
    '#ff3333';

  ctx.beginPath();
  ctx.moveTo(cx, cy);

  ctx.lineTo(
    cx + (radius - 14) * Math.cos(needleAngle),
    cy - (radius - 14) * Math.sin(needleAngle)
  );

  ctx.strokeStyle = colour;
  ctx.lineWidth = 2.5;
  ctx.stroke();

  ctx.beginPath();
  ctx.arc(cx, cy, 5, 0, Math.PI * 2);
  ctx.fillStyle = colour;
  ctx.fill();

  ctx.fillStyle = 'rgba(0,255,100,0.5)';
  ctx.font = '10px Courier New';
  ctx.textAlign = 'center';

  ctx.fillText('-90\u00b0', cx - radius + 8, cy);
  ctx.fillText('0\u00b0', cx, cy - radius + 13);
  ctx.fillText('+90\u00b0', cx + radius - 8, cy);
}

function toggleMode() {
  const enabled =
    document.getElementById('modeToggle').checked;

  document.getElementById('modeText').textContent =
    enabled ?
    'ATTITUDE SETPOINT CONTROL' :
    'MPU STABILISE';

  document.getElementById('manual').className =
    'manual' +
    (enabled ? ' active' : '');

  fetch('/mode?dashboard=' + (enabled ? '1' : '0'));

  if (!enabled) {
    fetch('/setpoint?roll=0&pitch=0');
  }
}

function updateSP() {
  const rollTarget =
    document.getElementById('tRoll').value;

  const pitchTarget =
    document.getElementById('tPitch').value;

  document.getElementById('tRollOut').textContent =
    rollTarget + '\u00b0';

  document.getElementById('tPitchOut').textContent =
    pitchTarget + '\u00b0';

  fetch(
    '/setpoint?roll=' +
    rollTarget +
    '&pitch=' +
    pitchTarget
  );
}

function resetFlat() {
  document.getElementById('tRoll').value = 0;
  document.getElementById('tPitch').value = 0;

  updateSP();
}

setInterval(() => {
  fetch('/heartbeat').catch(() => {});
}, 1000);

setInterval(() => {
  fetch('/data')
    .then(response => response.json())
    .then(data => {
      document.getElementById('rollVal').textContent =
        data.roll.toFixed(2);

      document.getElementById('pitchVal').textContent =
        data.pitch.toFixed(2);

      drawGauge(
        document.getElementById('rollC'),
        data.roll
      );

      drawGauge(
        document.getElementById('pitchC'),
        data.pitch
      );

      const tilted =
        Math.abs(data.roll) > 5 ||
        Math.abs(data.pitch) > 5;

      const pill =
        document.getElementById('pill');

      pill.textContent =
        tilted ?
        'TILTED' :
        'LEVEL';

      pill.className =
        'pill' +
        (tilted ? ' tilted' : '');
    })
    .catch(() => {});
}, 100);

drawGauge(
  document.getElementById('rollC'),
  0
);

drawGauge(
  document.getElementById('pitchC'),
  0
);
</script>

</body>
</html>
)rawhtml";

  server.send(
    200,
    "text/html",
    html
  );
}


// WEB SERVER HANDLERS
void handleData() {
  String json =
    "{\"roll\":" +
    String(roll, 2) +
    ",\"pitch\":" +
    String(pitch, 2) +
    "}";

  server.send(
    200,
    "application/json",
    json
  );
}

void handleHeartbeat() {
  lastHeartbeat = millis();

  server.send(
    200,
    "text/plain",
    "ok"
  );
}

void resetPID() {
  rollIntegral = 0.0f;
  pitchIntegral = 0.0f;

  lastRollError = 0.0f;
  lastPitchError = 0.0f;

  firstControlCycle = true;
}

void handleMode() {
  if (server.hasArg("dashboard")) {
    dashboardMode =
      server.arg("dashboard") == "1";

    targetRoll = 0.0f;
    targetPitch = 0.0f;

    lastHeartbeat = millis();

    resetPID();
  }

  server.send(
    200,
    "text/plain",
    "ok"
  );
}

void handleSetpoint() {
  if (server.hasArg("roll")) {
    targetRoll = constrain(
      server.arg("roll").toFloat(),
      -5.0f,
      5.0f
    );
  }

  if (server.hasArg("pitch")) {
    targetPitch = constrain(
      server.arg("pitch").toFloat(),
      -5.0f,
      5.0f
    );
  }

  server.send(
    200,
    "text/plain",
    "ok"
  );
}

// MPU6050 FUNCTIONS
void readRawMotion(
  int16_t& ax,
  int16_t& ay,
  int16_t& az,
  int16_t& gx,
  int16_t& gy,
  int16_t& gz
) {
  mpu.getMotion6(
    &ax,
    &ay,
    &az,
    &gx,
    &gy,
    &gz
  );
}

void calculateAccelerometerAngles(
  int16_t ax,
  int16_t ay,
  int16_t az,
  float& accelerometerRoll,
  float& accelerometerPitch
) {
  const float axFloat =
    static_cast<float>(ax);

  const float ayFloat =
    static_cast<float>(ay);

  const float azFloat =
    static_cast<float>(az);

  accelerometerRoll =
    atan2f(
      ayFloat,
      azFloat
    ) *
    180.0f /
    PI;

  accelerometerPitch =
    atan2f(
      -axFloat,
      sqrtf(
        ayFloat * ayFloat +
        azFloat * azFloat
      )
    ) *
    180.0f /
    PI;
}

void initialiseAttitude() {
  constexpr int CALIBRATION_SAMPLES = 100;

  float rollTotal = 0.0f;
  float pitchTotal = 0.0f;

  Serial.println(
    "Calibrating MPU6050. Keep platform level and still."
  );

  for (
    int sample = 0;
    sample < CALIBRATION_SAMPLES;
    sample++
  ) {
    int16_t ax;
    int16_t ay;
    int16_t az;
    int16_t gx;
    int16_t gy;
    int16_t gz;

    readRawMotion(
      ax,
      ay,
      az,
      gx,
      gy,
      gz
    );

    float accelerometerRoll;
    float accelerometerPitch;

    calculateAccelerometerAngles(
      ax,
      ay,
      az,
      accelerometerRoll,
      accelerometerPitch
    );

    rollTotal += accelerometerRoll;
    pitchTotal += accelerometerPitch;

    delay(10);
  }

  rollOffset =
    rollTotal /
    CALIBRATION_SAMPLES;

  pitchOffset =
    pitchTotal /
    CALIBRATION_SAMPLES;

  roll = 0.0f;
  pitch = 0.0f;

  Serial.print("Roll offset: ");
  Serial.println(rollOffset, 2);

  Serial.print("Pitch offset: ");
  Serial.println(pitchOffset, 2);
}

// WIFI
void initialiseWiFi() {
  WiFi.mode(WIFI_STA);

  WiFi.begin(
    ssid,
    password
  );

  Serial.print("Connecting to WiFi");

  const unsigned long connectionStart =
    millis();

  while (
    WiFi.status() != WL_CONNECTED &&
    millis() - connectionStart <
    WIFI_TIMEOUT_MS
  ) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi connected. IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println(
      "WiFi unavailable. Stabilisation will continue without dashboard."
    );
  }

  server.on(
    "/",
    handleRoot
  );

  server.on(
    "/data",
    handleData
  );

  server.on(
    "/heartbeat",
    handleHeartbeat
  );

  server.on(
    "/mode",
    handleMode
  );

  server.on(
    "/setpoint",
    handleSetpoint
  );

  server.begin();
}

// SETUP
void setup() {
  Serial.begin(115200);

  delay(500);

  Serial.println();
  Serial.println("ESP32 gimbal starting");

  Wire.begin(
    SDA_PIN,
    SCL_PIN
  );

  Wire.setClock(400000);

  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println(
      "FATAL: MPU6050 connection failed"
    );

    while (true) {
      delay(1000);
    }
  }

  Serial.println(
    "MPU6050 connected"
  );

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);

  rollServo.setPeriodHertz(50);
  pitchServo.setPeriodHertz(50);

  rollServo.attach(
    ROLL_SERVO_PIN,
    SERVO_MIN_PULSE_US,
    SERVO_MAX_PULSE_US
  );

  pitchServo.attach(
    PITCH_SERVO_PIN,
    SERVO_MIN_PULSE_US,
    SERVO_MAX_PULSE_US
  );

  rollServo.write(
    ROLL_NEUTRAL
  );

  pitchServo.write(
    PITCH_NEUTRAL
  );

  delay(1500);

  initialiseAttitude();

  resetPID();

  initialiseWiFi();

  lastHeartbeat = millis();
  lastControlTimeMicros = micros();

  Serial.println(
    "System ready"
  );
}

// MAIN LOOP
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    server.handleClient();
  }

  if (
    dashboardMode &&
    millis() - lastHeartbeat >
    HEARTBEAT_TIMEOUT_MS
  ) {
    dashboardMode = false;

    targetRoll = 0.0f;
    targetPitch = 0.0f;

    resetPID();

    Serial.println(
      "Dashboard heartbeat lost. Returning to stabilisation mode."
    );
  }

  const unsigned long currentTimeMicros =
    micros();

  const float dt =
    static_cast<float>(
      currentTimeMicros -
      lastControlTimeMicros
    ) /
    1000000.0f;

  lastControlTimeMicros =
    currentTimeMicros;

  if (
    dt < MIN_DT_SECONDS ||
    dt > MAX_DT_SECONDS
  ) {
    delay(1);
    return;
  }

  int16_t ax;
  int16_t ay;
  int16_t az;
  int16_t gx;
  int16_t gy;
  int16_t gz;

  readRawMotion(
    ax,
    ay,
    az,
    gx,
    gy,
    gz
  );

  float accelerometerRoll;
  float accelerometerPitch;

  calculateAccelerometerAngles(
    ax,
    ay,
    az,
    accelerometerRoll,
    accelerometerPitch
  );

  accelerometerRoll -= rollOffset;
  accelerometerPitch -= pitchOffset;

  const float gyroRollRate =
    static_cast<float>(gx) /
    131.0f;

  const float gyroPitchRate =
    static_cast<float>(gy) /
    131.0f;

  roll =
    ALPHA *
    (
      roll +
      gyroRollRate *
      dt
    ) +
    (
      1.0f -
      ALPHA
    ) *
    accelerometerRoll;

  pitch =
    ALPHA *
    (
      pitch +
      gyroPitchRate *
      dt
    ) +
    (
      1.0f -
      ALPHA
    ) *
    accelerometerPitch;

  const float setRoll =
    dashboardMode ?
    targetRoll :
    0.0f;

  const float setPitch =
    dashboardMode ?
    targetPitch :
    0.0f;

  rollError =
    setRoll -
    roll;

  pitchError =
    setPitch -
    pitch;

  if (firstControlCycle) {
    lastRollError =
      rollError;

    lastPitchError =
      pitchError;

    firstControlCycle =
      false;
  }

  rollIntegral +=
    rollError *
    dt;

  pitchIntegral +=
    pitchError *
    dt;

  rollIntegral = constrain(
    rollIntegral,
    -INTEGRAL_LIMIT,
    INTEGRAL_LIMIT
  );

  pitchIntegral = constrain(
    pitchIntegral,
    -INTEGRAL_LIMIT,
    INTEGRAL_LIMIT
  );

  const float rollDerivative =
    (
      rollError -
      lastRollError
    ) /
    dt;

  const float pitchDerivative =
    (
      pitchError -
      lastPitchError
    ) /
    dt;

  float rollOutput =
    rollKp *
    rollError +
    rollKi *
    rollIntegral +
    rollKd *
    rollDerivative;

  float pitchOutput =
    pitchKp *
    pitchError +
    pitchKi *
    pitchIntegral +
    pitchKd *
    pitchDerivative;

  rollOutput = constrain(
    rollOutput,
    -MAX_CONTROL_OUTPUT,
    MAX_CONTROL_OUTPUT
  );

  pitchOutput = constrain(
    pitchOutput,
    -MAX_CONTROL_OUTPUT,
    MAX_CONTROL_OUTPUT
  );

  lastRollError =
    rollError;

  lastPitchError =
    pitchError;

  const int rollServoPosition = constrain(
    ROLL_NEUTRAL +
    static_cast<int>(rollOutput),
    ROLL_MIN_POSITION,
    ROLL_MAX_POSITION
  );

  const int pitchServoPosition = constrain(
    PITCH_NEUTRAL +
    static_cast<int>(pitchOutput),
    PITCH_MIN_POSITION,
    PITCH_MAX_POSITION
  );

  rollServo.write(
    rollServoPosition
  );

  pitchServo.write(
    pitchServoPosition
  );

  if (
    millis() - lastSerialUpdate >=
    SERIAL_UPDATE_INTERVAL_MS
  ) {
    lastSerialUpdate = millis();

    Serial.print("Roll: ");
    Serial.print(roll, 2);

    Serial.print("  Pitch: ");
    Serial.print(pitch, 2);

    Serial.print("  Roll servo: ");
    Serial.print(rollServoPosition);

    Serial.print("  Pitch servo: ");
    Serial.println(pitchServoPosition);
  }

  delay(2);
}