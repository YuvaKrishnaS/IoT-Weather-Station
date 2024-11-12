#define BLYNK_TEMPLATE_ID "TMPL3yctEEQQA"
#define BLYNK_TEMPLATE_NAME "Mini Weather Station"
#define BLYNK_AUTH_TOKEN "37Q1zPWRo0VdlBq1p7Zz_wyXIP0Eu_U1"
#define BLYNK_PRINT Serial

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Blynk authentication token
char auth[] = BLYNK_AUTH_TOKEN;

// WiFi credentials
char ssid[] = "Excitel_Naveenkunj";  // Your WiFi SSID
char pass[] = "Naveen@47";           // Your WiFi Password

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

void displayMessage(String message, int textSize = 1) {
    display.clearDisplay();
    display.setTextSize(textSize);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor((SCREEN_WIDTH - (message.length() * 6 * textSize)) / 2, SCREEN_HEIGHT / 2 - (8 * textSize));
    display.println(message);
    display.display();
}

void sendSensorData() {
    // Read temperature and humidity from DHT sensor
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature(); // Read in Celsius

    // Check if readings are valid
    if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Failed to read from DHT sensor!");
        humidity = NAN;
        temperature = NAN;
    }

    // Read gas sensor value
    int gasValue = analogRead(GAS_PIN);

    // Send data to Blynk
    Blynk.virtualWrite(V0, temperature);
    Blynk.virtualWrite(V1, humidity);
    Blynk.virtualWrite(V2, gasValue);

    // Print data to Serial Monitor
    if (isnan(temperature)) {
        Serial.println("Temperature: NA");
    } else {
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println(" °C");
    }

    if (isnan(humidity)) {
        Serial.println("Humidity: NA");
    } else {
        Serial.print("Humidity: ");
        Serial.print(humidity);
        Serial.println(" %");
    }

    Serial.print("Gas Value: ");
    Serial.println(gasValue);

    // Display data on OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Weather Station");

    // Display Temperature
    display.setCursor(0, 16);
    if (isnan(temperature)) {
        display.print("Temp: NA");
    } else {
        display.print("Temp: ");
        display.print(temperature);
        display.println(" C");
    }

    // Display Humidity
    display.setCursor(0, 32);
    if (isnan(humidity)) {
        display.print("Humidity: NA");
    } else {
        display.print("Humidity: ");
        display.print(humidity);
        display.println(" %");
    }

    // Display Gas value
    display.setCursor(0, 48);
    display.print("Gas: ");
    display.println(gasValue);

    display.display();

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

    // Initialize display
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        while (true); // Stay here if initialization fails
    }

    // Display welcome message
    displayMessage("Welcome", 2);
    delay(3000);

    // Clear screen and show WiFi connecting message
    displayMessage("Connecting to WiFi", 1);
    Serial.println("Connecting to WiFi...");

    // Initialize Blynk
    Blynk.begin(auth, ssid, pass);

    // Wait for WiFi connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected");

    // Display IP address
    displayMessage("Connected", 2);
    delay(1000);
    String ipAddress = WiFi.localIP().toString();
    displayMessage("IP: " + ipAddress, 1);
    delay(2000);

    // Initialize DHT sensor
    dht.begin();

    // Set a timer to call sendSensorData every 30 seconds
    timer.setInterval(30000L, sendSensorData);

    // Clear the display after WiFi connection
    display.clearDisplay();
    display.display();
}

void loop() {
    // Run Blynk and timer
    Blynk.run();
    timer.run();
}
