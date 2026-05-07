#include <WiFi.h>
#include <HTTPClient.h>
#include <esp_wifi.h>
#include <time.h>

// ==========================================
// USTAWIENIA - ZMIEŃ NA SWOJE:
// ==========================================

const char* SSID = "Gamefound";
const char* PASSWORD = "gf11jeden";
const char* BACKEND_URL = "https://sram-gf-default-rtdb.europe-west1.firebasedatabase.app/restrooms/men1/history.json";
const char* CONFIG_URL = "https://sram-gf-default-rtdb.europe-west1.firebasedatabase.app/restrooms/men1/config.json";
const char* RESTROOM_ID = "men1";
const int DEFAULT_THRESHOLD_CM = 140;

// ==========================================
// USTAWIENIA OSZCZĘDZANIA ENERGII:
// ==========================================
#define TIME_TO_SLEEP 5
#define NIGHT_SLEEP_MINUTES 30  // jak długo spać w nocy (minuty)
#define NIGHT_START_HOUR 18
#define NIGHT_END_HOUR 6
#define CONFIG_FETCH_INTERVAL 180  // fetch config every 3 minutes (seconds)
#define HEARTBEAT_INTERVAL 180     // send heartbeat every 3 minutes

// NTP
const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = 3600;      // UTC+1 (Polska zima)
const int DAYLIGHT_OFFSET_SEC = 3600;  // +1h letni czas

// ==========================================
// ZMIENNE W PAMIĘCI RTC (przetrwają Deep Sleep)
// ==========================================
RTC_DATA_ATTR bool previousState = false;
RTC_DATA_ATTR bool firstBoot = true;
RTC_DATA_ATTR bool pendingRetry = false;
RTC_DATA_ATTR int retryCount = 0;
RTC_DATA_ATTR unsigned long rtcEpoch = 0;       // last synced epoch (seconds)
RTC_DATA_ATTR unsigned long rtcSleepTotal = 0;  // accumulated sleep since last sync (seconds)
RTC_DATA_ATTR unsigned long timeSinceConfig = 0; // seconds since last config fetch
RTC_DATA_ATTR int thresholdCm = DEFAULT_THRESHOLD_CM;
RTC_DATA_ATTR unsigned long timeSinceHeartbeat = 0; // seconds since last heartbeat
const int MAX_RETRIES = 3;

// ==========================================
// PONIŻEJ NIC NIE ZMIENIAJ
// ==========================================

const int TRIG_PIN = 4;
const int ECHO_PIN = 5;

void fetchConfig() {
  HTTPClient http;
  http.begin(CONFIG_URL);
  int httpCode = http.GET();

  if (httpCode == 200) {
    String payload = http.getString();
    // Expects JSON like: {"threshold_cm":80}
    // Simple parsing without ArduinoJson (lightweight)
    int idx = payload.indexOf("\"threshold_cm\":");
    if (idx >= 0) {
      int valStart = idx + 15; // length of "threshold_cm":
      int newThreshold = payload.substring(valStart).toInt();
      if (newThreshold > 0 && newThreshold < 400) {
        thresholdCm = newThreshold;
        Serial.print("Config: threshold_cm = "); Serial.println(thresholdCm);
      }
    }
    timeSinceConfig = 0;
  } else {
    Serial.print("Config fetch failed: "); Serial.println(httpCode);
  }

  http.end();
}

float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 999.0; 
  float dist = (duration * 0.034) / 2;
  if (dist < 2.0) return 999.0;  // HC-SR04 min range is ~2cm, below = noise
  return dist;
}

void sendHeartbeat() {
  HTTPClient http;
  http.begin("https://sram-gf-default-rtdb.europe-west1.firebasedatabase.app/restrooms/men1/heartbeat.json");
  http.addHeader("Content-Type", "application/json");
  int code = http.PUT("{\"last_seen\":{\".sv\":\"timestamp\"},\"threshold_cm\":" + String(thresholdCm) + "}");
  if (code > 0) {
    Serial.println("Heartbeat sent OK");
  } else {
    Serial.print("Heartbeat failed: "); Serial.println(code);
  }
  http.end();
  timeSinceHeartbeat = 0;
}

bool connectWiFiAndSend(bool occupied) {
  Serial.print("Lacze z WiFi: ");
  Serial.println(SSID);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  esp_wifi_set_max_tx_power(44); 

  int proba = 0;
  while (WiFi.status() != WL_CONNECTED && proba < 20) {
    delay(250);
    Serial.print(".");
    proba++;
  }

  bool success = false;

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nPolaczono! Wysylam dane do backendu...");

    // Sync time via NTP while WiFi is up
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 2000)) {
      time_t now;
      time(&now);
      rtcEpoch = (unsigned long)now;
      rtcSleepTotal = 0;
      Serial.print("Czas zsynchronizowany: ");
      Serial.println(&timeinfo, "%H:%M:%S");
    }

    // Fetch config if due
    if (timeSinceConfig >= CONFIG_FETCH_INTERVAL || firstBoot) {
      fetchConfig();
    }

    // Send heartbeat if due
    if (timeSinceHeartbeat >= HEARTBEAT_INTERVAL || firstBoot) {
      sendHeartbeat();
    }

    HTTPClient http;
    http.begin(BACKEND_URL);
    http.addHeader("Content-Type", "application/json");

    String body = "{\"occupied\":" + String(occupied ? "true" : "false") + 
                  ",\"timestamp\":{\".sv\":\"timestamp\"}}";

    int httpCode = http.POST(body);

    if (httpCode > 0) {
      Serial.print("Wyslano: "); Serial.print(body);
      Serial.print(" | Odpowiedz: "); Serial.println(httpCode);
      success = true;
    } else {
      Serial.println("Blad wysylania! Kod: " + String(httpCode));
    }
    http.end();

    // Aktualizujemy też stan aktualny (aby dashboard nie musiał przeszukiwać historii)
    http.begin("https://sram-gf-default-rtdb.europe-west1.firebasedatabase.app/restrooms/men1/current.json");
    http.addHeader("Content-Type", "application/json");
    int putCode = http.PUT("{\"occupied\":" + String(occupied ? "true" : "false") + "}");
    if (putCode <= 0) {
      Serial.println("Blad PUT current! Kod: " + String(putCode));
    }
    http.end();
  } else {
    Serial.println("\nNie udalo sie polaczyc z WiFi. Sprobujemy nastepnym razem.");
  }

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  return success;
}

void setup() {
  setCpuFrequencyMhz(80);

  Serial.begin(115200);
  delay(10);

  if (firstBoot) {
    Serial.println("\n=== SRAM - Smart Restroom (LOW POWER MODE) ===");
  }

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // 1. Wykonaj pomiar — dwa odczyty z krótką przerwą
  float distance1 = measureDistance();
  delay(50);
  float distance2 = measureDistance();

  bool isOcc1 = (distance1 > 0 && distance1 < thresholdCm);
  bool isOcc2 = (distance2 > 0 && distance2 < thresholdCm);
  
  bool occupied = previousState; // Zaczynamy od założenia, że stan się nie zmienił

  // LOGIKA SMART DEBOUNCE (Histereza)
  if (!previousState && isOcc1 && isOcc2) {
    // Było wolne, ale OBA pomiary mówią, że zajęte -> Zmiana na ZAJĘTE
    occupied = true;
  } else if (previousState && !isOcc1 && !isOcc2) {
    // Było zajęte, ale OBA pomiary mówią, że wolne -> Zmiana na WOLNE
    occupied = false;
  }
  // W każdym innym przypadku (np. jeden pomiar to glitch), ignorujemy i zostawiamy 'previousState'

  Serial.print("Odczyty: ");
  Serial.print(distance1);
  Serial.print("cm, ");
  Serial.print(distance2);
  Serial.print("cm | Status: ");
  Serial.println(occupied ? "ZAJETE" : "WOLNE");

  // 2. Jeśli status się zmienił, to pierwszy boot, heartbeat, lub ostatnie wysyłanie nie powiodło się — wyślij
  // Skip heartbeat at night to save power
  bool isNight = false;
  if (rtcEpoch > 0) {
    unsigned long estimatedNow = rtcEpoch + rtcSleepTotal;
    struct tm* tmInfo = localtime((time_t*)&estimatedNow);
    if (tmInfo) {
      int h = tmInfo->tm_hour;
      isNight = (h >= NIGHT_START_HOUR || h < NIGHT_END_HOUR);
    }
  }
  bool heartbeatDue = !isNight && (timeSinceHeartbeat >= HEARTBEAT_INTERVAL);
  bool shouldSend = firstBoot || (occupied != previousState) ||
                    (pendingRetry && retryCount < MAX_RETRIES) || heartbeatDue;

  if (shouldSend) {
    Serial.println(">>> Uruchamiam radio WiFi...");
    bool success = connectWiFiAndSend(occupied);
    
    if (success) {
      previousState = occupied;
      firstBoot = false;
      pendingRetry = false;
      retryCount = 0;
    } else {
      pendingRetry = true;
      retryCount++;
      previousState = occupied;
      firstBoot = false;
      if (retryCount >= MAX_RETRIES) {
        Serial.println("Max prob osiagniete. Rezygnuje do nastepnej zmiany stanu.");
      }
    }
  } else {
    Serial.println("Brak zmiany stanu. Radio WiFi pozostaje wylaczone.");
    if (!pendingRetry) retryCount = 0;
  }

  // 3. Oblicz czas snu — dłuższy w nocy
  unsigned long sleepSeconds = TIME_TO_SLEEP;

  if (rtcEpoch > 0) {
    // Szacuj aktualną godzinę na podstawie ostatniego synca + skumulowanego snu
    unsigned long estimatedNow = rtcEpoch + rtcSleepTotal;
    int currentHour = (int)((estimatedNow % 86400UL) / 3600UL);
    // Adjust for timezone (already applied in NTP sync, so currentHour is local)
    // Recalculate from struct if available, otherwise use epoch math
    struct tm* tmInfo = localtime((time_t*)&estimatedNow);
    if (tmInfo) currentHour = tmInfo->tm_hour;

    Serial.print("Szacowana godzina: "); Serial.println(currentHour);

    if (currentHour >= NIGHT_START_HOUR || currentHour < NIGHT_END_HOUR) {
      sleepSeconds = NIGHT_SLEEP_MINUTES * 60UL;
      Serial.println("Tryb nocny — dlugi sen.");
      // Upewnij się, że toaleta jest raportowana jako wolna na noc
      if (previousState) {
        previousState = false;
        // Nie wysyłamy — WiFi jest już wyłączone, wyślemy rano
      }
    }
  }

  rtcSleepTotal += sleepSeconds;
  timeSinceConfig += sleepSeconds;
  timeSinceHeartbeat += sleepSeconds;

  Serial.print("Ide spac na "); Serial.print(sleepSeconds); Serial.println(" sekund. Dobranoc!\n");
  Serial.flush(); 
  
  esp_sleep_enable_timer_wakeup(sleepSeconds * 1000000ULL); 
  esp_deep_sleep_start();
}

void loop() {
}