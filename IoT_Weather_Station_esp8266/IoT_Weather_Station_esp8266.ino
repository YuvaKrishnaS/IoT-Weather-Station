#define BLYNK_TEMPLATE_ID "TMPL3yctEEQQA"
#define BLYNK_TEMPLATE_NAME "Mini Weather Station"
#define BLYNK_AUTH_TOKEN "37Q1zPWRo0VdlBq1p7Zz_wyXIP0Eu_U1"
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

// Blynk authentication token
char auth[] = BLYNK_AUTH_TOKEN;

// WiFi credentials
char ssid[] = "Excitel_Naveenkunj";  // Your WiFi SSID
char pass[] = "Naveen@47";            // Your WiFi Password

// Timer for Blynk
BlynkTimer timer;

// Pin definitions
#define DHTPIN 2      // DHT sensor connected to D4 on NodeMCU
#define DHTTYPE DHT11 // DHT 11 sensor
#define GAS_PIN A0    // Gas sensor connected to analog pin A0

// Sensor initialization
DHT dht(DHTPIN, DHTTYPE);

// Threshold for gas sensor
int sensorThreshold = 500;

void sendSensorData() {
    // Read temperature and humidity from DHT sensor
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature(); // Read in Celsius

    // Check if readings are valid
    if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    // Read gas sensor value
    int gasValue = analogRead(GAS_PIN);

    // Send data to Blynk
    Blynk.virtualWrite(V0, temperature);
    Blynk.virtualWrite(V1, humidity);
    Blynk.virtualWrite(V2, gasValue);

    // Print data to Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C    Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    Serial.print("Gas Value: ");
    Serial.println(gasValue);

    // Check for gas alert condition
    if (gasValue > sensorThreshold) {
        Serial.println("Bad Air Detected!");
        Blynk.logEvent("pollution_alert", "Bad Air Detected!");
    } else {
        Serial.println("Air Quality is Good");
    }
}

void setup() {
    // Start serial communication
    Serial.begin(115200);

    // Initialize Blynk
    Blynk.begin(auth, ssid, pass);

    // Initialize DHT sensor
    dht.begin();

    // Set a timer to call sendSensorData every 30 seconds
    timer.setInterval(30000L, sendSensorData);
}

void loop() {
    // Run Blynk and timer
    Blynk.run();
    timer.run();
}
