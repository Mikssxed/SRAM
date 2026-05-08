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
const int DEFAULT_THRESHOLD_CM = 115;

// ==========================================
// USTAWIENIA OSZCZĘDZANIA ENERGII:
// ==========================================
#define TIME_TO_SLEEP 5
#define NIGHT_SLEEP_MINUTES 30  // jak długo spać w nocy (minuty)
#define NIGHT_START_HOUR 18
#define NIGHT_END_HOUR 6
#define CONFIG_FETCH_INTERVAL 180  // fetch config every 3 minutes (seconds)
#define HEARTBEAT_INTERVAL 60     // send heartbeat every 1 minute

// NTP
const char* NTP_SERVER = "pool.ntp.org";
const char* TIMEZONE = "CET-1CEST,M3.5.0,M10.5.0/3"; // Poland auto DST

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
RTC_DATA_ATTR int sleepSeconds_cfg = TIME_TO_SLEEP;
RTC_DATA_ATTR int nightStartHour = NIGHT_START_HOUR;
RTC_DATA_ATTR int nightEndHour = NIGHT_END_HOUR;
RTC_DATA_ATTR int nightSleepMin = NIGHT_SLEEP_MINUTES;
RTC_DATA_ATTR int heartbeatInterval = HEARTBEAT_INTERVAL;
RTC_DATA_ATTR int numSamples = 5;
RTC_DATA_ATTR int freeConsecutive = 0;  // consecutive FREE cycles (hold timer)
RTC_DATA_ATTR int freeHoldCycles = 2;   // require N consecutive FREE readings to flip
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
    Serial.print("Config payload: "); Serial.println(payload);

    // Parse threshold_cm
    int idx = payload.indexOf("\"threshold_cm\":");
    if (idx >= 0) {
      int val = payload.substring(idx + 15).toInt();
      if (val > 0 && val < 400) { thresholdCm = val; }
    }

    // Parse sleep_seconds
    idx = payload.indexOf("\"sleep_seconds\":");
    if (idx >= 0) {
      int val = payload.substring(idx + 16).toInt();
      if (val >= 1 && val <= 300) { sleepSeconds_cfg = val; }
    }

    // Parse night_start_hour
    idx = payload.indexOf("\"night_start_hour\":");
    if (idx >= 0) {
      int val = payload.substring(idx + 19).toInt();
      if (val >= 0 && val <= 23) { nightStartHour = val; }
    }

    // Parse night_end_hour
    idx = payload.indexOf("\"night_end_hour\":");
    if (idx >= 0) {
      int val = payload.substring(idx + 17).toInt();
      if (val >= 0 && val <= 23) { nightEndHour = val; }
    }

    // Parse night_sleep_minutes
    idx = payload.indexOf("\"night_sleep_minutes\":");
    if (idx >= 0) {
      int val = payload.substring(idx + 22).toInt();
      if (val >= 1 && val <= 120) { nightSleepMin = val; }
    }

    // Parse heartbeat_interval
    idx = payload.indexOf("\"heartbeat_interval\":");
    if (idx >= 0) {
      int val = payload.substring(idx + 21).toInt();
      if (val >= 10 && val <= 3600) { heartbeatInterval = val; }
    }

    // Parse num_samples
    idx = payload.indexOf("\"num_samples\":");
    if (idx >= 0) {
      int val = payload.substring(idx + 14).toInt();
      if (val >= 3 && val <= 15) { numSamples = val; }
    }

    // Parse free_hold_cycles
    idx = payload.indexOf("\"free_hold_cycles\":");
    if (idx >= 0) {
      int val = payload.substring(idx + 19).toInt();
      if (val >= 1 && val <= 20) { freeHoldCycles = val; }
    }

    // Parse reboot flag
    idx = payload.indexOf("\"reboot\":true");
    if (idx >= 0) {
      Serial.println("Reboot requested via config!");
      // Clear the flag in Firebase
      HTTPClient http2;
      http2.begin(String(CONFIG_URL).substring(0, String(CONFIG_URL).length() - 5) + "/reboot.json");
      http2.PUT("false");
      http2.end();
      http.end();
      ESP.restart();
    }

    timeSinceConfig = 0;
    Serial.print("Config applied: threshold="); Serial.print(thresholdCm);
    Serial.print(" sleep="); Serial.print(sleepSeconds_cfg);
    Serial.print(" night="); Serial.print(nightStartHour);
    Serial.print("-"); Serial.print(nightEndHour);
    Serial.print(" hb="); Serial.print(heartbeatInterval);
    Serial.print(" samples="); Serial.print(numSamples);
    Serial.print(" freeHold="); Serial.println(freeHoldCycles);
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

void sendHeartbeat(float median, float low, float high, int votes, int totalSamples, int freeStreak) {
  HTTPClient http;
  http.begin("https://sram-gf-default-rtdb.europe-west1.firebasedatabase.app/restrooms/men1/heartbeat.json");
  http.addHeader("Content-Type", "application/json");
  String body = "{\"last_seen\":{\".sv\":\"timestamp\"},\"threshold_cm\":" + String(thresholdCm) +
                ",\"median_cm\":" + String(median, 1) +
                ",\"low_cm\":" + String(low, 1) +
                ",\"high_cm\":" + String(high, 1) +
                ",\"votes\":" + String(votes) +
                ",\"samples\":" + String(totalSamples) +
                ",\"free_streak\":" + String(freeStreak) + "}";
  int code = http.PUT(body);
  if (code > 0) {
    Serial.println("Heartbeat sent OK");
  } else {
    Serial.print("Heartbeat failed: "); Serial.println(code);
  }
  http.end();
  timeSinceHeartbeat = 0;
}

bool connectWiFiAndSend(bool occupied, bool stateChanged, float median, float low, float high, int votes, int totalSamples, int freeStreak) {
  Serial.print("Lacze z WiFi: ");
  Serial.println(SSID);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  esp_wifi_set_max_tx_power(44); 

  int proba = 0;
  while (WiFi.status() != WL_CONNECTED && proba < 40) {
    delay(250);
    Serial.print(".");
    proba++;
  }

  bool success = false;

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nPolaczono! Wysylam dane do backendu...");

    // Sync time via NTP while WiFi is up
    configTzTime(TIMEZONE, NTP_SERVER);
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
    if (timeSinceHeartbeat >= (unsigned long)heartbeatInterval || firstBoot) {
      sendHeartbeat(median, low, high, votes, totalSamples, freeStreak);
    }

    // Only POST to history when state actually changed
    if (stateChanged) {
      HTTPClient http;
      http.begin(BACKEND_URL);
      http.addHeader("Content-Type", "application/json");

      String body = "{\"occupied\":" + String(occupied ? "true" : "false") +
                    ",\"median_cm\":" + String(median, 1) +
                    ",\"low_cm\":" + String(low, 1) +
                    ",\"high_cm\":" + String(high, 1) +                    ",\"votes\":" + String(votes) +
                    ",\"samples\":" + String(totalSamples) +
                    ",\"free_streak\":" + String(freeStreak) +                    ",\"timestamp\":{\".sv\":\"timestamp\"}}";

      int httpCode = http.POST(body);

      if (httpCode > 0) {
        Serial.print("Wyslano: "); Serial.print(body);
        Serial.print(" | Odpowiedz: "); Serial.println(httpCode);
      } else {
        Serial.println("Blad wysylania! Kod: " + String(httpCode));
      }
      http.end();

      // Aktualizujemy też stan aktualny
      http.begin("https://sram-gf-default-rtdb.europe-west1.firebasedatabase.app/restrooms/men1/current.json");
      http.addHeader("Content-Type", "application/json");
      int putCode = http.PUT("{\"occupied\":" + String(occupied ? "true" : "false") +
                             ",\"median_cm\":" + String(median, 1) +
                             ",\"low_cm\":" + String(low, 1) +
                             ",\"high_cm\":" + String(high, 1) +                             ",\"votes\":" + String(votes) +
                             ",\"samples\":" + String(totalSamples) +
                             ",\"free_streak\":" + String(freeStreak) +                             ",\"timestamp\":{\".sv\":\"timestamp\"}}");
      if (putCode <= 0) {
        Serial.println("Blad PUT current! Kod: " + String(putCode));
      }
      http.end();
    }

    success = true;
  } else {
    Serial.println("\nNie udalo sie polaczyc z WiFi. Sprobujemy nastepnym razem.");
  }

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  return success;
}

void setup() {
  setCpuFrequencyMhz(80);

  // Restore timezone so localtime() returns correct local hours after deep sleep
  setenv("TZ", TIMEZONE, 1);
  tzset();

  Serial.begin(115200);
  delay(10);

  if (firstBoot) {
    Serial.println("\n=== SRAM - Smart Restroom (LOW POWER MODE) ===");
  }

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // 1. Wykonaj pomiar — N odczytów, użyj mediany
  float samples[15]; // max possible samples
  int n = constrain(numSamples, 3, 15);
  for (int i = 0; i < n; i++) {
    samples[i] = measureDistance();
    if (i < n - 1) delay(60);
  }

  // Sort for median (simple insertion sort)
  for (int i = 1; i < n; i++) {
    float key = samples[i];
    int j = i - 1;
    while (j >= 0 && samples[j] > key) {
      samples[j + 1] = samples[j];
      j--;
    }
    samples[j + 1] = key;
  }

  float medianDist = samples[n / 2]; // median
  float distLow = samples[0];         // min
  float distHigh = samples[n - 1];    // max

  // Count how many samples are below threshold (vote)
  int occupiedVotes = 0;
  for (int i = 0; i < n; i++) {
    if (samples[i] > 0 && samples[i] < thresholdCm) occupiedVotes++;
  }

  bool occupied = previousState;

  // Vote-based detection: require ~30% of samples to agree (scales with num_samples)
  int minVotes = max(2, n * 3 / 10);
  bool voteSaysOccupied = (occupiedVotes >= minVotes);

  if (!previousState && voteSaysOccupied) {
    // Was free, votes say occupied -> flip to OCCUPIED
    occupied = true;
    freeConsecutive = 0;
  } else if (previousState && !voteSaysOccupied) {
    // Was occupied, votes say free -> increment hold counter
    freeConsecutive++;
    if (freeConsecutive >= freeHoldCycles) {
      occupied = false; // Confirmed free after N consecutive cycles
      freeConsecutive = 0;
    }
    // else: stay occupied (debounce)
  } else {
    // State matches votes — reset counter
    if (previousState) freeConsecutive = 0;
  }

  Serial.print("Votes: "); Serial.print(occupiedVotes);
  Serial.print("/"); Serial.print(n);
  Serial.print(" | freeStreak: "); Serial.println(freeConsecutive);

  // Evaluate night mode BEFORE deciding whether to send
  bool isNight = false;
  unsigned long sleepSeconds = sleepSeconds_cfg;

  if (rtcEpoch > 0) {
    time_t estimatedNow = (time_t)(rtcEpoch + rtcSleepTotal);
    struct tm* tmInfo = localtime(&estimatedNow);
    if (tmInfo) {
      int currentHour = tmInfo->tm_hour;
      Serial.print("Szacowana godzina: "); Serial.println(currentHour);
      isNight = (currentHour >= nightStartHour || currentHour < nightEndHour);

      if (isNight) {
        occupied = false; // Force FREE at night
        sleepSeconds = nightSleepMin * 60UL;
        Serial.println("Tryb nocny — wymuszam WOLNE, dlugi sen.");
      }
    }
  }

  Serial.print("Odczyty: median=");
  Serial.print(medianDist);
  Serial.print("cm, low=");
  Serial.print(distLow);
  Serial.print("cm, high=");
  Serial.print(distHigh);
  Serial.print("cm | Status: ");
  Serial.println(occupied ? "ZAJETE" : "WOLNE");

  // 2. Jeśli status się zmienił, to pierwszy boot, heartbeat, lub ostatnie wysyłanie nie powiodło się — wyślij
  bool heartbeatDue = !isNight && (timeSinceHeartbeat >= (unsigned long)heartbeatInterval);
  bool shouldSend = firstBoot || (occupied != previousState) ||
                    (pendingRetry && retryCount < MAX_RETRIES) || heartbeatDue;

  if (shouldSend) {
    Serial.println(">>> Uruchamiam radio WiFi...");
    bool stateChanged = (occupied != previousState) || firstBoot;
    bool success = connectWiFiAndSend(occupied, stateChanged, medianDist, distLow, distHigh, occupiedVotes, n, freeConsecutive);
    
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

  // 3. Re-evaluate sleep duration after potential NTP sync
  if (rtcEpoch > 0) {
    time_t estimatedNow = (time_t)(rtcEpoch + rtcSleepTotal);
    struct tm* tmCheck = localtime(&estimatedNow);
    if (tmCheck) {
      int h = tmCheck->tm_hour;
      bool stillNight = (h >= nightStartHour || h < nightEndHour);
      sleepSeconds = stillNight ? (nightSleepMin * 60UL) : (unsigned long)sleepSeconds_cfg;
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