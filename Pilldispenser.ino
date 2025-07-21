#define BLYNK_TEMPLATE_ID "TMPL36gcehqNv"
#define BLYNK_TEMPLATE_NAME "Pill Dispenser"

#include <AccelStepper.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <BlynkSimpleEsp8266.h>

// Wi-Fi Credentials
char auth[] = "9e1U02YmFwk6rKPMRvf9JJapLQPVEXso";
char ssid[] = "ameya";
char pass[] = "ameyaaaa";

// Pin Definitions
#define stepPin D2
#define dirPin D1
#define ledPin D7
#define buzzerPin D8
#define IRSensorPin D6
#define buttonPin D3

const float stepsPerRevolution = 400.0;
const int compartments = 4;

AccelStepper stepper(AccelStepper::DRIVER, stepPin, dirPin);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800); // GMT+5:30

BlynkTimer timer;

int currentCompartment = 0;
bool manualDispensed[compartments] = {false};
bool alreadyChecked[compartments] = {false};
bool notificationSent = false;

unsigned long rotationTimes[compartments] = {
  1155, 1156, 1157, 1158
};

void connectToWiFiAndBlynk() {
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, pass);
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("✅ Wi-Fi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("❌ Failed to connect to Wi-Fi");
    return;
  }

  Serial.println("Connecting to Blynk...");
  Blynk.config(auth);
  if (Blynk.connect(5000)) {
    Serial.println("✅ Connected to Blynk!");
    Blynk.logEvent("pill_dispensed", "Device powered on.");
  } else {
    Serial.println("❌ Blynk connection failed");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(IRSensorPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  connectToWiFiAndBlynk();

  Serial.println("Starting NTP time client...");
  timeClient.begin();
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  Serial.println("✅ Time synced: " + String(timeClient.getFormattedTime()));

  stepper.setMaxSpeed(500);
  stepper.setAcceleration(500);

  timer.setInterval(10000L, checkDispensing); // Check every 10 seconds
}

void loop() {
  Blynk.run();
  timer.run();
  timeClient.update();

  if (!timeClient.update()) {
    timeClient.forceUpdate();
  }

  // Manual button pressed
  if (digitalRead(buttonPin) == LOW) {
    Serial.println("Manual button pressed");
    dispensePill(true);
    delay(1000); // Debounce
  }
}

void checkDispensing() {
  if (currentCompartment >= compartments) return;

  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  unsigned long currentTime = currentHour * 100 + currentMinute;

  Serial.println("Current Time: " + String(currentHour) + ":" + String(currentMinute));
  Serial.println("Current Compartment: " + String(currentCompartment));

  if (currentTime >= rotationTimes[currentCompartment] &&
      currentTime < rotationTimes[currentCompartment] + 5 &&
      !alreadyChecked[currentCompartment]) {

    if (!manualDispensed[currentCompartment]) {
      Serial.println("Scheduled time match. Auto dispense.");
      dispensePill(false);
    } else {
      Serial.println("Scheduled time skipped. Already manually dispensed.");
      manualDispensed[currentCompartment] = false;
      currentCompartment++;
    }

    if (currentCompartment <= compartments) {
      alreadyChecked[currentCompartment - 1] = true; // Mark checked
    }
  }
}

void dispensePill(bool isManual) {
  if (currentCompartment >= compartments) {
    Serial.println("All pills dispensed.");
    return;
  }

  long targetPosition = stepper.currentPosition() + stepsPerRevolution; // ✅ Always move forward
  Serial.println("Moving to position: " + String(targetPosition));
  stepper.runToNewPosition(stepper.currentPosition() + stepsPerRevolution);


  Serial.print("Pill ");
  Serial.print(currentCompartment + 1);
  if (isManual) {
    Serial.println(" dispensed manually.");
  } else {
    Serial.println(" dispensed automatically.");
    String msg = "Pill " + String(currentCompartment + 1) + " dispensed automatically.";
    Blynk.logEvent("pill_dispensed", msg);
  }

  digitalWrite(ledPin, HIGH);
  digitalWrite(buzzerPin, HIGH);

  unsigned long startTime = millis();
  bool handDetected = false;
  notificationSent = false;

  while (millis() - startTime < 10000) {
    if (digitalRead(IRSensorPin) == 0 && !notificationSent) {
      String msg = "Pill " + String(currentCompartment + 1) + " picked up (" + (isManual ? "manual" : "auto") + ")";
      Serial.println(msg);
      Blynk.logEvent("pill_picked_up", msg);
      notificationSent = true;
      handDetected = true;
      break;
    }
    delay(100);
  }

  if (!handDetected) {
    String msg = "Pill " + String(currentCompartment + 1) + " NOT picked up (" + (isManual ? "manual" : "auto") + ")";
    Serial.println(msg);
    Blynk.logEvent("pill_not_picked", msg);
  }

  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);

  if (isManual) {
    manualDispensed[currentCompartment] = true;
    String msg = "Pill " + String(currentCompartment + 1) + " dispensed manually.";
    Blynk.logEvent("pill_dispensed", msg);
    currentCompartment++; // Skip the scheduled one
  } else {
    currentCompartment++;
  }
}
