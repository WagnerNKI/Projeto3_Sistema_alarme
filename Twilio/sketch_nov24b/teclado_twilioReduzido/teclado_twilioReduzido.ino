#include <Keypad.h>


#define SENHA_CERTA "1234 "
#define SENHA_ATIVA "0"

bool alarmeAtivado = false;
bool senhaCerta = false;

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


//Configuração para envio de SMS
#include <UIPEthernet.h>
#include <RestClient.h>

// Alterar o último valor para o id do seu kit
const byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xF1, 0x64 };
EthernetClient ethclient;

RestClient client = RestClient("192.168.3.186", 3000, ethclient);



void apagaAcendeLed(int ledAcesso, int ledApagado1, int ledApagado2, int buzzer) {
  digitalWrite (ledAcesso, HIGH);
  digitalWrite (ledApagado1, LOW);
  digitalWrite (ledApagado2, LOW);

  if (buzzer == 1) {
    tone(A1, 1000);
    delay (1000);
    noTone(A1);
  }
}

#define SMS_SID AC5ff5d08021fda59e7088098d55945157
#define SMS_TOKEN a33eab98854ffcb2546bde7618d21da2
#define SMS_TO 5511963650954
#define SMS_FROM 13217668522
const char* parametros = "sid=SMS_SID&token=SMS_TOKEN&to=SMS_TO&from=SMS_FROM&body=Sua casa esta sendo invadida";

void enviaSms() {

  String response = "";

  //  Serial.println(parametros);

  int statusCode = client.post("/sms", parametros, &response);
  //  Serial.print("Status da resposta: ");
  //  Serial.println(statusCode);
  //  Serial.print("Resposta do servidor: ");
  //  Serial.println(response);
  delay(1000);
}

void guardaSenha() {
  
  String senhaDigitada;
  bool digitandoSenha = false;

  char key = keypad.getKey();

  if (key) {
    if (key == '#') {

      digitandoSenha = false;

      if (senhaDigitada == SENHA_ATIVA) {
//        Serial.println ("Ativado");
        alarmeAtivado = true;
        apagaAcendeLed(A4, A5, A3, 0);
        delay(1000);
      }
      else if (alarmeAtivado && senhaDigitada == SENHA_CERTA) {
//        Serial.println ("Desativado");
        senhaCerta = true;
        apagaAcendeLed(A5, A3, A4, 0);
        alarmeAtivado = false;
      }
      else {
//        Serial.println ("Incorreta");
        apagaAcendeLed(A3, A5, A4, 1);
        enviaSms ();
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


void setup() {


  Serial.begin(9600);
  pinMode(A2, INPUT);
  pinMode(A0, INPUT);
  pinMode(A1, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(8, INPUT);

  // Connect via DHCP
  //  Serial.println ("Conectando...");
  if (Ethernet.begin(mac)) {
    Serial.println("Conectado via DHCP");
    //    Serial.print("IP recebido:"); Serial.println(Ethernet.localIP());
    //    delay(50);
  }
}

void loop() {

  guardaSenha();

  int doorOpenClosed = digitalRead(8);
  if (alarmeAtivado && doorOpenClosed == 1) {

    Serial.println("Digitar a senha");

    long timeDoorOpen = millis();
    senhaCerta = false;

    while (senhaCerta == false) {
      long now = millis();

      guardaSenha();

      if (now - timeDoorOpen > 10000) {
        timeDoorOpen = now;
        apagaAcendeLed(A3, A5, A4, 1);
        enviaSms ();

      }

    }

  }
}



