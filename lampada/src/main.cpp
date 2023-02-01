#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include <ArduinoOTA.h>
#include <WebServer.h>

#include "FS.h"

// Informações de conexão WiFi
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Informações do servidor MQTT
const char* mqtt_server = "your_MQTT_SERVER";
const int mqtt_port = 1883;
const char* mqtt_user = "your_MQTT_USER";
const char* mqtt_password = "your_MQTT_PASSWORD";

WiFiClient espClient;
PubSubClient client(espClient);

// Variáveis para armazenar o estado da lâmpada
bool lampada_status = false;

void callback(char* topic, byte* payload, unsigned int length) {
    // Converte o payload para string
    String payloadStr = "";
    for (int i = 0; i < length; i++) {
        payloadStr += (char)payload[i];
    }
    // Verifica se o tópico é "lampada"
    if (strcmp(topic, "lampada") == 0) {
        // Verifica se o comando é "ligar" ou "desligar"
        if (payloadStr == "ligar") {
            // aciona o relé
            lampada_status = true;
        } 
        else if (payloadStr == "desligar") {
            // desaciona o relé
            lampada_status = false;
        }
    }
}

void setup() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando ao WiFi...");
    }
    Serial.println("Conectado ao WiFi");

    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);

    // Publica o endereço IP da lâmpada
    client.publish("lampada/ip", WiFi.localIP().toString().c_str(), true);
    // Inscreve-se no tópico "lampada" para receber comandos
    client.subscribe("lampada");
}

void loop() {
    // Verifica se está conectado ao servidor MQTT
    if (!client.connected()) {
        client.connect("lampada", mqtt_user, mqtt_password);
    }
    client.loop();

    // Publica o status da lâmpada a cada segundo
    if (millis() % 1000 == 0) {
        client.publish("lampada/status", lampada_status ? "ligada" : "desligada");
    }
}



