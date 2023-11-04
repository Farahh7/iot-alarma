#include "arduino_secrets.h"

#include <ThingSpeak.h>

#include <WiFi.h>


int gas_snsr = 34;
int co2_snsr = 39;
int temp_snsr = 36;
int buzzer = 26;
int led = 2;
int gas_val = 0;
int co2_val = 0;
int temp_val = 0;
float gas_voltage = 0;
float co2_voltage = 0;

const int co2_lethal_threshold = 800;  
const int co_lethal_threshold = 100;    
const int temp_threshold = 55; 


const char* ssid = "WiFi_Universo_Net_pft_2.4GHz";     
const char* password = "hpjrea55jp";    

const char* thingSpeakServer = "api.thingspeak.com";
const char* WRITE_API_KEY = "8M17O81LYSSQEX1O"; 
const long CHANNEL_ID = 2319494;  
WiFiClient client;
void setup() {
  pinMode(gas_snsr, INPUT);
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conexión WiFi establecida!");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  ThingSpeak.begin(client);
}

void loop() {
  // Lectura de sensores
  gas_val = analogRead(gas_snsr);
  co2_val = analogRead(co2_snsr);
  temp_val = analogRead(temp_snsr);
  float millivolts = (temp_val / 4095.0) * 3300.0;
  float celsius = (millivolts / 10.0) - 30.0;
  float temperature = celsius;

  gas_voltage = gas_val * (3.3 / 4095.0);
  float gas_ppm = gas_voltage / 0.1;

  co2_voltage = co2_val / 1023.0;
  float co2_ppm = co2_voltage - 200;

  Serial.print("Temperatura: ");
  Serial.print(celsius);
  Serial.println(" °C");

  Serial.print("CO2: ");
  Serial.println(co2_val);

  Serial.print("Gas: ");
  Serial.println(gas_ppm);

  // Enviar datos a ThingSpeak
  
int field1 = static_cast<int>(temperature );  
int field2 = co2_val;
int field3 = static_cast<int>(gas_ppm); 

  ThingSpeak.writeField(CHANNEL_ID, 1, field1, WRITE_API_KEY);
  ThingSpeak.writeField(CHANNEL_ID, 2, field2, WRITE_API_KEY);
  ThingSpeak.writeField(CHANNEL_ID, 3, field3, WRITE_API_KEY);

  // Verificación de los valores de los sensores
  if (celsius > temp_threshold) {
    digitalWrite(led, HIGH);
    Serial.println("¡Alerta! Se ha detectado un incendio");
    tone(buzzer, 2000, 500);
    delay(1000);
  } else {
    digitalWrite(led, LOW);
    digitalWrite(buzzer, LOW);
  }

  if (co2_val > co2_lethal_threshold && gas_ppm <= co_lethal_threshold) {
    Serial.println("¡Alerta! Nivel de CO2 letal detectado");
    tone(buzzer, 250, 200);
  } else if (gas_ppm > co_lethal_threshold && co2_val <= co2_lethal_threshold) {
    Serial.println("¡Alerta! Nivel de CO letal detectado");
    tone(buzzer, 300, 200);
  } else if (co2_val > co2_lethal_threshold && gas_ppm > co_lethal_threshold) {
    Serial.println("¡Alerta! Niveles letales de CO y CO2 detectados");
    tone(buzzer, 100, 200);
  } else {
    Serial.println("Fuera de peligro");
  }

  delay(5000);
}
