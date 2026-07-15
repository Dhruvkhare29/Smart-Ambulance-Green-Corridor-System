/* ESP32 RECEIVER → Sends road command to Arduino via Serial */
#include <WiFi.h>
#include <esp_now.h>

// Using main UART (TX0, RX0)
#define ARDUINO Serial  

// ESP-NOW receive callback
void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  String msg;
  for (int i = 0; i < len; i++) msg += (char)incomingData[i];

  Serial.print("Received ESP-NOW: ");
  Serial.println(msg);

  // ✅ Forward to Arduino
  ARDUINO.println(msg);
  Serial.print("Forwarded to Arduino: ");
  Serial.println(msg);
}

void setup() {
  Serial.begin(115200);   // Debug + Arduino Link
  delay(200);

  Serial.println("\n--- ESP32 RECEIVER START ---");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.print("Receiver MAC: ");
  Serial.println(WiFi.macAddress());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    while (true);
  }

  // Callback
  esp_now_register_recv_cb(onDataRecv);
  Serial.println("ESP-NOW Ready & UART link to Arduino active");
}

void loop() {
  delay(200);
}