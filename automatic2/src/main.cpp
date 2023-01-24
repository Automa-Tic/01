#include <WiFi.h>
#include <PubSubClient.h>

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

// Armazena os endereços IP dos outros ESP32s
String lampada_ip;
String cortina_ip;

// Armazena o status dos outros ESP32s
bool lampada_status;
bool cortina_status;

void callback(char* topic, byte* payload, unsigned int length) {
  // Converte o payload para string
  String payloadStr = "";
  for (int i = 0; i < length; i++) {
    payloadStr += (char)payload[i];
  }
  // Verifica se o tópico é "lampada/status"
  if (strcmp(topic, "lampada/status") == 0) {
    lampada_status = (payloadStr == "ligada");
  }
  // Verifica se o tópico é "cortina/status"
  if (strcmp(topic, "cortina/status") == 0) {
    cortina_status = (payloadStr == "aberta");
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
  
  // Publica um comando para a lâmpada se conectar e enviar seu endereço IP
  client.publish("lampada/connect", "");
  // Publica um comando para a cortina se conectar e enviar seu endereço IP
  client.publish("cortina/connect", "");
}
void loop() {
    // Verifica se está conectado ao servidor MQTT
    if (!client.connected()) {
        client.connect("central", mqtt_user, mqtt_password);
    }
    client.loop();

    // Lê o comando do usuário
    if (Serial.available()) {
        String command = Serial.readString();
        // Verifica se o comando é "ligar lampada"
        if (command == "ligar lampada") {
            // Envia o comando para a lâmpada
            client.publish("lampada", "ligar");
        }
        // Verifica se o comando é "desligar lampada"
        if (command == "desligar lampada") {
            // Envia o comando para a lâmpada
            client.publish("lampada", "desligar");
        }
        // Verifica se o comando é "abrir cortina"
        if (command == "abrir cortina") {
            // Envia o comando para a cortina
            client.publish("cortina", "abrir");
        }
        // Verifica se o comando é "fechar cortina"
        if (command == "fechar cortina") {
            // Envia o comando para a cortina
            client.publish("cortina", "fechar");
        }
    }
    // Imprime o status da lâmpada e da cortina
    Serial.print("Lâmpada: ");
    Serial.println(lampada_status ? "ligada" : "desligada");
    Serial.print("Cortina: ");
    Serial.println(cortina_status ? "aberta" : "fechada");
    // Imprime os endereços IP da lâmpada e da cortina
    Serial.print("IP da lâmpada: ");
    Serial.println(lampada_ip);
    Serial.print("IP da cortina: ");
    Serial.println(cortina_ip);
}
