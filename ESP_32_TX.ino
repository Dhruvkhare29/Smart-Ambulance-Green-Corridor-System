#include <WiFi.h>

#include <esp_now.h>



const char* receiverMacStr = "F0:24:F9:0D:E9:E8"; 

uint8_t receiverMac[6];



// SoftAP Credentials

const char* ssid = "AMBULANCE_CTRL";

const char* password = "12345678";



// Convert string MAC → bytes

void hexStrToUint8(const char* macStr, uint8_t *mac) {

  int values[6];

  sscanf(macStr, "%x:%x:%x:%x:%x:%x",

         &values[0], &values[1], &values[2],

         &values[3], &values[4], &values[5]);

  for (int i = 0; i < 6; ++i) mac[i] = (uint8_t) values[i];

}



// ✅ ESP-NOW Send Callback

void onDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {

  Serial.print("ESPNOW Status: ");

  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivered ✅" : "Failed ❌");

}



WiFiServer server(80);



void sendESPNOW(String msg) {

  esp_now_send(receiverMac, (uint8_t*)msg.c_str(), msg.length());

  Serial.print("Sent: "); Serial.println(msg);

}



String htmlPage = R"rawliteral(

<!DOCTYPE html>

<html>

<head><title>Ambulance Traffic Control</title></head>

<body style="text-align:center; font-family:Arial;">

<h2>Ambulance Traffic Priority</h2>

<h3>Select Road to Clear</h3>

<button onclick="send('A')" style="padding:15px;font-size:20px;">Road A</button><br><br>

<button onclick="send('B')" style="padding:15px;font-size:20px;">Road B</button><br><br>

<button onclick="send('C')" style="padding:15px;font-size:20px;">Road C</button><br><br>

<button onclick="send('D')" style="padding:15px;font-size:20px;">Road D</button>



<script>

function send(road){

  fetch("/road?select=" + road);

}

</script>

</body></html>

)rawliteral";



void setup() {

  Serial.begin(115200);

  delay(1000);



  Serial.println("--- ESP32 Transmitter with Web UI ---");



  // ✅ ESPNOW MODE

  WiFi.mode(WIFI_AP_STA);



  // ✅ Create Hotspot

  WiFi.softAP(ssid, password);

  Serial.print("Connect Phone to WiFi: ");

  Serial.println(ssid);

  Serial.print("IP Address: ");

  Serial.println(WiFi.softAPIP());



  // ✅ ESPNOW INIT

  if (esp_now_init() != ESP_OK) {

    Serial.println("ESPNOW Init Failed!");

    while(1);

  }



  esp_now_register_send_cb(onDataSent);

  

  hexStrToUint8(receiverMacStr, receiverMac);

  

  esp_now_peer_info_t peerInfo = {};

  memcpy(peerInfo.peer_addr, receiverMac, 6);

  peerInfo.channel = 0;

  peerInfo.encrypt = false;

  esp_now_add_peer(&peerInfo);



  server.begin();

  Serial.println("Web Server Started ✅");

}



void loop() {

  WiFiClient client = server.available();

  if (!client) return;



  // Wait for request

  String req = client.readStringUntil('\r');

  client.readStringUntil('\n');



  if (req.indexOf("GET /road?select=A") != -1) sendESPNOW("A");

  if (req.indexOf("GET /road?select=B") != -1) sendESPNOW("B");

  if (req.indexOf("GET /road?select=C") != -1) sendESPNOW("C");

  if (req.indexOf("GET /road?select=D") != -1) sendESPNOW("D");



  client.println("HTTP/1.1 200 OK");

  client.println("Content-Type: text/html");

  client.println();

  client.print(htmlPage);

}