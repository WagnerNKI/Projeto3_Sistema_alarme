#include <Keypad.h>

String senhaAtiva = "0";
String senha = "1234";
int buzz = A1;
int ledRed = A3;
int ledYellow = A4;
int ledGreen = A5;
int infrared = 8;
bool alarmeAtivado = false;
bool senhaCerta = false;
String senhaDigitada;
bool digitandoSenha = false;
long lastReconnectAttempt = 0;

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {5, 4, 3, A0}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {A2, 7, 6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


#include <SPI.h>
#include <UIPEthernet.h>
#include <utility/logging.h>
#include <PubSubClient.h>


void callback(char* topic, byte* payload, unsigned int length);

// Alterar o último valor para o id do seu kit
const byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xF1, 0x64 };
EthernetClient ethclient;


// Dados do MQTT Cloud
PubSubClient clientMqtt("m14.cloudmqtt.com", 12207, callback, ethclient);

void apagaAcendeLed(int ledAcesso, int ledApagado1, int ledApagado2, int buzzer) {
  digitalWrite (ledAcesso, HIGH);
  digitalWrite (ledApagado1, LOW);
  digitalWrite (ledApagado2, LOW);

  if (buzzer == 1) {
    tone(buzz, 1000);
    delay (1000);
    noTone(buzz);
  }
}


void guardaSenha() {

  char key = keypad.getKey();

  if (key) {
    if (key == '#') {

      digitandoSenha = false;

      if (senhaDigitada == senhaAtiva) {
        //        Serial.println ("Ativado");
        alarmeAtivado = true;
        apagaAcendeLed(ledYellow, ledGreen, ledRed, 0);
        delay(1000);
      }
      else if (senhaDigitada == senha && alarmeAtivado) {
        //        Serial.println ("Desativado");
        senhaCerta = true;
        apagaAcendeLed(ledGreen, ledRed, ledYellow, 0);
        alarmeAtivado = false;
      }
      else {
        //        Serial.println ("Incorreta");
        apagaAcendeLed(ledRed, ledGreen, ledYellow, 1);
      }
    }

    if (digitandoSenha) {
      senhaDigitada += key;
      Serial.println(senhaDigitada);
    }

    if (key == '*') {
      senhaDigitada = "";
      digitandoSenha = true;
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length)
{

}

boolean reconnect() {
  if (clientMqtt.connect("teclado", "teclado", "teclado")) {
    // Once connected, publish an announcement...
    clientMqtt.publish("teste", "hello world");
  }
  Serial.println("Conectado MQTT");
  return clientMqtt.connected();
}


void setup() {

  Serial.begin(9600);
  pinMode(A2, INPUT);
  pinMode(A0, INPUT);
  pinMode(A1, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(infrared, INPUT);

  Serial.println ("Conectando...");
  Ethernet.begin(mac);

  // Faz a conexão no cloud com nome do dispositivo, usuário e senha respectivamente
  if (client.connect("arduino", "arduino", "ard123"))
  {
    // Envia uma mensagem para o cloud no topic portao
    client.publish("teste", "v");

    // Conecta no topic para receber mensagens
    client.subscribe("recebido");
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

  guardaSenha();

  int doorOpenClosed = digitalRead(infrared);
  if (alarmeAtivado == true && doorOpenClosed == 1) {

    Serial.println("Digitar a senha");

    long timeDoorOpen = millis();
    senhaCerta = false;

    while (senhaCerta == false) {
      long now = millis();

      guardaSenha();

      if (now - timeDoorOpen > 10000) {
        timeDoorOpen = now;
        apagaAcendeLed(ledRed, ledGreen, ledYellow, 1);
      }

    }

  }
}



