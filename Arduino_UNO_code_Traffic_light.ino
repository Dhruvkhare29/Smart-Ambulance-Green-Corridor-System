// ----- SMART AMBULANCE TRAFFIC LIGHT CONTROLLER -----
// Receives road commands (A, B, C, D, NORMAL) from ESP32 via Serial
// and switches traffic lights accordingly.

// ----- LED Pin Assignments -----
#define A_RED    2
#define A_YELLOW 3
#define A_GREEN  4

#define B_RED    5
#define B_YELLOW 6
#define B_GREEN  7

#define C_RED    8
#define C_YELLOW 9
#define C_GREEN  10

#define D_RED    11
#define D_YELLOW 12
#define D_GREEN  13

String incomingData = "";
bool normalMode = true;

// ---------- SETUP ----------
void setup() {
  Serial.begin(115200);   // Must match ESP32 baud rate
  Serial.println("Arduino ready... waiting for ESP32 data");

  // Set all LED pins as OUTPUT
  int pins[] = {A_RED, A_YELLOW, A_GREEN,
                B_RED, B_YELLOW, B_GREEN,
                C_RED, C_YELLOW, C_GREEN,
                D_RED, D_YELLOW, D_GREEN};
  for (int i = 0; i < 12; i++) pinMode(pins[i], OUTPUT);

  // Start with all RED lights
  allRed();
  Serial.println("All lights RED (initial)");
}

// ---------- FUNCTIONS ----------

// Turn all lights RED
void allRed() {
  digitalWrite(A_RED, HIGH);
  digitalWrite(B_RED, HIGH);
  digitalWrite(C_RED, HIGH);
  digitalWrite(D_RED, HIGH);

  digitalWrite(A_GREEN, LOW);
  digitalWrite(B_GREEN, LOW);
  digitalWrite(C_GREEN, LOW);
  digitalWrite(D_GREEN, LOW);

  digitalWrite(A_YELLOW, LOW);
  digitalWrite(B_YELLOW, LOW);
  digitalWrite(C_YELLOW, LOW);
  digitalWrite(D_YELLOW, LOW);
}

// Make specific road GREEN (for emergency)
void makeGreen(char road) {
  allRed();
  switch (road) {
    case 'A': digitalWrite(A_GREEN, HIGH); break;
    case 'B': digitalWrite(B_GREEN, HIGH); break;
    case 'C': digitalWrite(C_GREEN, HIGH); break;
    case 'D': digitalWrite(D_GREEN, HIGH); break;
  }
  Serial.print(">>> Road ");
  Serial.print(road);
  Serial.println(" set to GREEN (Emergency Priority)");
}

// Normal automatic cycle for traffic lights
void normalTrafficCycle() {
  // A & C GREEN
  digitalWrite(A_GREEN, HIGH);
  digitalWrite(C_GREEN, HIGH);
  digitalWrite(A_RED, LOW);
  digitalWrite(C_RED, LOW);
  digitalWrite(B_RED, HIGH);
  digitalWrite(D_RED, HIGH);
  Serial.println("Normal Mode: A & C GREEN");
  delay(4000);

  // A & C YELLOW
  digitalWrite(A_GREEN, LOW);
  digitalWrite(C_GREEN, LOW);
  digitalWrite(A_YELLOW, HIGH);
  digitalWrite(C_YELLOW, HIGH);
  Serial.println("Normal Mode: A & C YELLOW");
  delay(1500);

  // B & D GREEN
  digitalWrite(A_YELLOW, LOW);
  digitalWrite(C_YELLOW, LOW);
  digitalWrite(A_RED, HIGH);
  digitalWrite(C_RED, HIGH);
  digitalWrite(B_RED, LOW);
  digitalWrite(D_RED, LOW);
  digitalWrite(B_GREEN, HIGH);
  digitalWrite(D_GREEN, HIGH);
  Serial.println("Normal Mode: B & D GREEN");
  delay(4000);

  // B & D YELLOW
  digitalWrite(B_GREEN, LOW);
  digitalWrite(D_GREEN, LOW);
  digitalWrite(B_YELLOW, HIGH);
  digitalWrite(D_YELLOW, HIGH);
  Serial.println("Normal Mode: B & D YELLOW");
  delay(1500);
}

// ---------- MAIN LOOP ----------
void loop() {
  // Check if ESP32 sent something
  if (Serial.available()) {
    incomingData = Serial.readStringUntil('\n');
    incomingData.trim();

    if (incomingData.length() > 0) {
      Serial.print("Received from ESP32 -> ");
      Serial.println(incomingData);

      if (incomingData == "NORMAL") {
        Serial.println("Switching to NORMAL traffic mode...");
        normalMode = true;
      } 
      else if (incomingData == "A" || incomingData == "B" ||
               incomingData == "C" || incomingData == "D") {
        Serial.println("Emergency mode detected!");
        makeGreen(incomingData.charAt(0));
        normalMode = false;
      }
    }
  }

  // Run normal cycle only if in NORMAL mode
  if (normalMode) {
    normalTrafficCycle();
  }
}