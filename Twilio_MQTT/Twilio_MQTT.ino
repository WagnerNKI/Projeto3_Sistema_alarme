
//Conexão MQTT
#include <SPI.h>
#include <UIPEthernet.h>
#include <utility/logging.h>
#include <PubSubClient.h>

//Conexão Twilio
#include <RestClient.h>


// Update these with values suitable for your network.
byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xF1, 0x64};

// Callback function header
void callback(char* topic, byte* payload, unsigned int length);



EthernetClient ethClient;
EthernetClient ethclient;

RestClient client = RestClient("192.168.3.186", 3000, ethclient);

// Dados do MQTT Cloud
PubSubClient clientMqtt("192.168.3.186", 1883, callback, ethClient);

long lastReconnectAttempt = 0;

void enviaSms() {

  const char* sid = "AC5ff5d08021fda59e7088098d55945157";
  const char* token = "a33eab98854ffcb2546bde7618d21da2";
  const char* to = "5511963650954";
  const char* from = "13217668522";

  String response = "";

  String parametros = "sid=";
  parametros.concat(sid);

  parametros.concat("&token=");
  parametros.concat(token);

  parametros.concat("&to=");
  parametros.concat(to);

  parametros.concat("&from=");
  parametros.concat(from);

  parametros.concat("&body=Sua casa esta sendo invadida");

//  Serial.println(parametros);

  int statusCode = client.post("/sms", parametros.c_str(), &response);
//  Serial.print("Status da resposta: ");
//  Serial.println(statusCode);
//  Serial.print("Resposta do servidor: ");
//  Serial.println(response);
  delay(1000);
}


void callback(char* topic, byte* payload, unsigned int length)
{

  enviaSms();
}

boolean reconnect() {
  if (clientMqtt.connect("", "", "", "vagas/25", 0, true, "")) {
    // Once connected, publish an announcement...
    clientMqtt.publish("teste", "hello world");
  }
  Serial.println("Conectado MQTT");
  return clientMqtt.connected();
}

void setup() {
  // Inicializa a porta Serial
  Serial.begin(9600);
  Serial.println("Conectando...");
  delay(50);
  Ethernet.begin(mac);

  // Faz a conexão no cloud com nome do dispositivo, usuário e senha respectivamente
  if (clientMqtt.connect("", "", "", "vagas/25", 0, true, ""))
  {
    // Envia uma mensagem para o cloud no topic
    clientMqtt.publish("vagas/25", "teste");
    clientMqtt.publish("vagas/25" , "1", true);
    //    estadoAtual = 2;

    Serial.println("Conectado MQTT");
    delay(50);
  }
}
void loop() {


  if (!clientMqtt.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      Serial.println("Reconectando...");
      if (reconnect()) {
        lastReconnectAttempt = 0;

      }
    }
  } else {
    // Client connected
    clientMqtt.loop();
  }
}

