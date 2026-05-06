#include <WiFi.h>
#include <HTTPClient.h>

// ==========================================
// USTAWIENIA - ZMIEŃ NA SWOJE:
// ==========================================

// Wpisz nazwę WiFi w biurze:
const char* SSID = "Gamefound";

// Wpisz hasło do WiFi:
const char* PASSWORD = "gf11jeden";

// Wpisz adres IP komputera z backendem (dowiesz się w Dniu 2):
const char* BACKEND_URL = "http://192.168.1.XXX:3000/api/status";

// Nazwa toalety:
const char* RESTROOM_ID = "men1";

// Próg odległości (z kalibracji, Krok 66):
const int THRESHOLD_CM = 80;

// ==========================================
// PONIŻEJ NIC NIE ZMIENIAJ
// ==========================================

const int TRIG_PIN = 4;
const int ECHO_PIN = 5;
bool previousState = false;
unsigned long lastSendTime = 0;

float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  return (duration * 0.034) / 2;
}

void sendStatus(bool occupied) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi rozlaczone! Probuje ponownie...");
    WiFi.begin(SSID, PASSWORD);
    delay(3000);
    return;
  }

  HTTPClient http;
  http.begin(BACKEND_URL);
  http.addHeader("Content-Type", "application/json");

  String body = "{\"id\":\"" + String(RESTROOM_ID) +
                "\",\"occupied\":" + (occupied ? "true" : "false") + "}";

  int httpCode = http.POST(body);

  if (httpCode > 0) {
    Serial.println("Wyslano: " + body + " | Odpowiedz: " + String(httpCode));
  } else {
    Serial.println("Blad wysylania! Kod: " + String(httpCode));
    Serial.println("Sprawdz czy backend dziala i czy adres IP jest poprawny.");
  }

  http.end();
}

void setup() {
  Serial.begin(115200);
  Serial.println("=== SRAM - Smart Restroom Availability Monitor ===");
  Serial.println("Uruchamiam...");

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Łączenie z WiFi
  Serial.print("Lacze z WiFi: ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASSWORD);

  int proba = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    proba++;
    if (proba > 30) {
      Serial.println("\nBLAD: Nie moge polaczyc z WiFi!");
      Serial.println("Sprawdz nazwe sieci i haslo w kodzie.");
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nPolaczono z WiFi!");
    Serial.print("Adres IP ESP32: ");
    Serial.println(WiFi.localIP());
  }

  Serial.println("Czujnik gotowy. Prog odleglosci: " + String(THRESHOLD_CM) + " cm");
  Serial.println("==========================================");
}

void loop() {
  // Pomiar odległości
  float distance = measureDistance();

  // Czy toaleta jest zajęta?
  bool occupied = (distance > 0 && distance < THRESHOLD_CM);

  // Wypisz na Serial Monitor co się dzieje
  Serial.print("Odleglosc: ");
  Serial.print(distance);
  Serial.print(" cm | Status: ");
  Serial.println(occupied ? "ZAJETE" : "WOLNE");

  // Wyślij do backendu TYLKO gdy stan się zmienił
  if (occupied != previousState) {
    Serial.println(">>> ZMIANA STANU! Wysylam do backendu...");
    sendStatus(occupied);
    previousState = occupied;
  }

  // Czekaj 1 sekundę przed następnym pomiarem
  delay(1000);
}