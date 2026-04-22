#include <WiFi.h>

const char* ssid = "shree";
const char* password = "11111111";

WiFiServer server(80);

int ldrPin = 34;
int led1 = 2;
int led2 = 4;
int led3 = 5;

int ldrValue = 0;
bool autoMode = true; // Start in AUTO mode

void setup() {
  Serial.begin(115200);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);

  // Connect WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi Connected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  ldrValue = analogRead(ldrPin);

  // Auto mode LED control
  if (autoMode) {
    if (ldrValue <= 1000) {
      digitalWrite(led1, HIGH); digitalWrite(led2, LOW); digitalWrite(led3, LOW);
    } else if (ldrValue <= 2000) {
      digitalWrite(led1, LOW); digitalWrite(led2, HIGH); digitalWrite(led3, LOW);
    } else {
      digitalWrite(led1, LOW); digitalWrite(led2, LOW); digitalWrite(led3, HIGH);
    }
  }

  String request = client.readStringUntil('\r');
  client.flush();

  // Mode Toggle
  if (request.indexOf("/mode") != -1) autoMode = !autoMode;

  // Manual controls
  if (!autoMode) {
    if (request.indexOf("/led1") != -1) digitalWrite(led1, !digitalRead(led1));
    if (request.indexOf("/led2") != -1) digitalWrite(led2, !digitalRead(led2));
    if (request.indexOf("/led3") != -1) digitalWrite(led3, !digitalRead(led3));
  }

  // Serve LDR value for AJAX
  if (request.indexOf("/ldr") != -1) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/plain");
    client.println("Connection: close");
    client.println();
    client.println(String(ldrValue));
    client.stop();
    return;
  }

  // Web page HTML
  String html = "<!DOCTYPE html><html><head><title>ESP32 Control</title>";
  html += "<style>";
  html += "body { font-family: Arial; text-align:center; background:white; color:black; }";
  html += "h1 { color:#FF6A00; margin-bottom:20px; }";

  // Big square blocks
  html += ".box { border-radius:20px; display:block; padding:30px; margin:20px auto; background:black; width:90%; max-width:700px; color:white; min-height:220px; }";
  html += ".btn { display:inline-block; padding:20px; margin:15px; font-size:18px; border:none; border-radius:50%; cursor:pointer; width:90px; height:90px; }";
  html += ".on { background:#4CAF50; color:white; }";
  html += ".off { background:#f44336; color:white; }";
  html += ".mode { background:#2196F3; color:white; border-radius:12px; width:auto; padding:15px 30px; }";
  html += "canvas { border-radius:20px; background:#111; margin-top:20px; width:100%; height:200px; }";
  html += "</style>";

  // Water wave + LED status JS
  html += "<script>";
  html += "window.onload = function() {";
  html += "let ldrValue = " + String(ldrValue) + ";";

  // Update LDR
  html += "function updateLDR() {";
  html += " var xhttp = new XMLHttpRequest();";
  html += " xhttp.onreadystatechange = function() {";
  html += "  if(this.readyState==4 && this.status==200) {";
  html += "   ldrValue = parseInt(this.responseText);";
  html += "   document.getElementById('ldrVal').innerHTML = ldrValue;";
  html += "   updateLEDStatus();";
  html += "  }";
  html += " };";
  html += " xhttp.open('GET','/ldr',true);";
  html += " xhttp.send();";
  html += "}";
  html += "setInterval(updateLDR, 1000);";

  // LED status update
  html += "function updateLEDStatus() {";
  html += "let led1 = document.getElementById('led1');";
  html += "let led2 = document.getElementById('led2');";
  html += "let led3 = document.getElementById('led3');";
  html += "if(ldrValue<=1000){ led1.className='btn on'; led2.className='btn off'; led3.className='btn off'; }";
  html += "else if(ldrValue<=2000){ led1.className='btn off'; led2.className='btn on'; led3.className='btn off'; }";
  html += "else{ led1.className='btn off'; led2.className='btn off'; led3.className='btn on'; }";
  html += "}";

  // Water wave with 4 overlapping transparent orange waves
  html += "const canvas = document.getElementById('ldrChart');";
  html += "const ctx = canvas.getContext('2d');";
  html += "let phase = 0;";
  html += "function drawWave() {";
  html += " const width = canvas.width;";
  html += " const height = canvas.height;";
  html += " ctx.clearRect(0,0,width,height);";
  html += " for(let i=0;i<4;i++){";
  html += "  const amplitude = 10 + i*5;";
  html += "  const frequency = 0.02 + i*0.01;";
  html += "  const offset = i*20;";
  html += "  ctx.fillStyle = 'rgba(255,165,0,0.2)';"; // transparent orange
  html += "  ctx.beginPath();";
  html += "  ctx.moveTo(0,height);";
  html += "  for(let x=0;x<=width;x++){";
  html += "    let y = height - (ldrValue/4095)*(height/2) + Math.sin(x*frequency + phase + offset)*amplitude;";
  html += "    ctx.lineTo(x,y);";
  html += "  }";
  html += "  ctx.lineTo(width,height); ctx.closePath(); ctx.fill();";
  html += " }";
  html += " phase += 0.1;";
  html += " requestAnimationFrame(drawWave);";
  html += "}";
  html += "drawWave();";

  html += "};";
  html += "</script></head><body>";

  html += "<h1>ESP32 LED + LDR Control</h1>";

  // Mode Block
  html += "<div class='box'><h2>Mode: " + String(autoMode ? "AUTO" : "MANUAL") + "</h2>";
  html += "<a href='/mode'><button class='mode'>Switch Mode</button></a></div>";

  // LED Status Block
  html += "<div class='box'><h2>LED Status</h2>";
  html += "<a href='/led1'><button id='led1' class='btn " + String(digitalRead(led1)? "on":"off") + "'>LED1</button></a>";
  html += "<a href='/led2'><button id='led2' class='btn " + String(digitalRead(led2)? "on":"off") + "'>LED2</button></a>";
  html += "<a href='/led3'><button id='led3' class='btn " + String(digitalRead(led3)? "on":"off") + "'>LED3</button></a>";
  html += "</div>";

  // LDR Wave Block
  html += "<div class='box'><h2>LDR Sensor</h2>";
  html += "<p><b id='ldrVal'>" + String(ldrValue) + "</b></p>";
  html += "<canvas id='ldrChart'></canvas></div>";

  html += "</body></html>";

  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();
  client.println(html);
  client.stop();
}
