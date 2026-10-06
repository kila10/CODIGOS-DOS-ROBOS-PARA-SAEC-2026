#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

#define ENA D1  // GPIO5 
#define IN1 D2  // GPIO4 
#define IN2 D5  // GPIO14
#define ENB D6  // GPIO12
#define IN3 D7  // GPIO13
#define IN4 D0  // GPIO16

int velBase = 650; // PWM entre 0 e 1023

void setMotor(int pinPWM, int in1, int in2, int speed) {
  String nomeMotor = (pinPWM == ENA) ? "Motor A (ENA)" : "Motor B (ENB)";
  int absSpeed = abs(speed);
  float porcentagem = (absSpeed / 1023.0) * 100.0;

  if (speed > 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    analogWrite(pinPWM, absSpeed);
    
    Serial.printf("  [%s] -> Sentido: FRENTE  | IN1/IN3: HIGH | IN2/IN4: LOW  | PWM: %d / 1023 (%.1f%%)\n", 
                  nomeMotor.c_str(), absSpeed, porcentagem);

  } else if (speed < 0) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    analogWrite(pinPWM, absSpeed);

    Serial.printf("  [%s] -> Sentido: RE      | IN1/IN3: LOW  | IN2/IN4: HIGH | PWM: %d / 1023 (%.1f%%)\n", 
                  nomeMotor.c_str(), absSpeed, porcentagem);

  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(pinPWM, 0);

    Serial.printf("  [%s] -> Sentido: PARADO  | IN1/IN3: LOW  | IN2/IN4: LOW  | PWM: 0 / 1023 (0.0%%)\n", 
                  nomeMotor.c_str());
  }
}

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
  <title>Quantum Car</title>
  <style>
    body { font-family: sans-serif; text-align: center; background: #111; color: white; margin: 0; padding-top: 20px; touch-action: manipulation; }
    .btn { width: 85px; height: 85px; font-size: 24px; margin: 8px; border: none; border-radius: 12px; background: #0088cc; color: white; -webkit-user-select: none; user-select: none; }
    .btn:active { background: #005588; }
    .stop { background: #cc2200; width: 180px; }
  </style>
</head>
<body>
  <h2>Quantum Team UFSC</h2>
  <div>
    <button class="btn" onpointerdown="cmd('/F', event)" onpointerup="cmd('/S', event)">▲</button>
  </div>
  <div>
    <button class="btn" onpointerdown="cmd('/L', event)" onpointerup="cmd('/S', event)">◄</button>
    <button class="btn" onpointerdown="cmd('/R', event)" onpointerup="cmd('/S', event)">►</button>
  </div>
  <div>
    <button class="btn" onpointerdown="cmd('/B', event)" onpointerup="cmd('/S', event)">▼</button>
  </div>
  <div style="margin-top:15px;">
    <button class="btn stop" onclick="cmd('/S', event)">PARAR</button>
  </div>

<script>
let lastRoute = '';

function cmd(route, e) {
  if (e) {
    e.preventDefault();
    e.stopPropagation();
  }
  if (route === lastRoute && route !== '/S') return;
  lastRoute = route;

  fetch(route, { cache: 'no-store' }).catch(err => console.log(err));
}
</script>
</body>
</html>
)rawliteral";

void handleRoot() { 
  Serial.println("[HTTP] Cliente conectado na interface web (/)");
  server.send(200, "text/html", INDEX_HTML); 
}

void handleFrente() { 
  Serial.println("\n--- [REQUISIÇÃO: /F] MOVIMENTO FRENTE ---");
  setMotor(ENA, IN1, IN2, velBase); 
  setMotor(ENB, IN3, IN4, velBase); 
  server.send(200, "text/plain", "OK"); 
}

void handleRe() { 
  Serial.println("\n--- [REQUISIÇÃO: /B] MOVIMENTO RÉ ---");
  setMotor(ENA, IN1, IN2, -velBase); 
  setMotor(ENB, IN3, IN4, -velBase); 
  server.send(200, "text/plain", "OK"); 
}

void handleEsq() { 
  Serial.println("\n--- [REQUISIÇÃO: /L] GIRO ESQUERDA ---");
  setMotor(ENA, IN1, IN2, -velBase);   
  setMotor(ENB, IN3, IN4, velBase);  
  server.send(200, "text/plain", "OK"); 
}

void handleDir() { 
  Serial.println("\n--- [REQUISIÇÃO: /R] GIRO DIREITA ---");
  setMotor(ENA, IN1, IN2, velBase);  
  setMotor(ENB, IN3, IN4, -velBase);   
  server.send(200, "text/plain", "OK"); 
}

void handleParar() { 
  Serial.println("\n--- [REQUISIÇÃO: /S] PARAR MOTORES ---");
  setMotor(ENA, IN1, IN2, 0); 
  setMotor(ENB, IN3, IN4, 0); 
  server.send(200, "text/plain", "OK"); 
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=========================================");
  Serial.println("     SISTEMA QUANTUM CAR INICIADO        ");
  Serial.println("=========================================");

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);

  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);   analogWrite(ENB, 0);

  analogWriteRange(1023); // Escala PWM do ESP8266

  WiFi.softAP("Quantum_Robo_WiFi", "12345678");
  Serial.print("[WiFi] Ponto de Acesso Ativo: Quantum_Robo_WiFi | IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/F", handleFrente);
  server.on("/B", handleRe);
  server.on("/L", handleEsq);
  server.on("/R", handleDir);
  server.on("/S", handleParar);
  server.begin();

  Serial.println("[HTTP] Servidor aguardando comandos...");
  Serial.println("=========================================\n");
}

void loop() {
  server.handleClient();
}