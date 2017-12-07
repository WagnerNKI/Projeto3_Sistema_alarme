#include <Keypad.h>
#include <Wire.h>

#define SENHA_CERTA "1234"
#define SENHA_ATIVA "0"
String senhaDigitada;
bool alarmeAtivado = false;
bool senhaCerta = false;
bool digitandoSenha = false;
int senhaIncorreta;
bool smsEnviado;
int ledRed = A3;
int ledYellow = 10;
int ledGreen = 9;
int buzz = A1;
int infra = 8;

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
//#include <UIPEthernet.h>
//#include <RestClient.h>


// Alterar o último valor para o id do seu kit
//const byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xF1, 0x64 };
//EthernetClient ethclient;

//RestClient client = RestClient("192.168.3.186", 3000, ethclient);



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

//#define SMS_SID "AC5ff5d08021fda59e7088098d55945157"
//#define SMS_TOKEN "a33eab98854ffcb2546bde7618d21da2"
//#define SMS_TO "5511963650954"
//#define SMS_FROM "13217668522"
//const char* parametros = "sid=" SMS_SID "&token=" SMS_TOKEN "&to=" SMS_TO "&from=" SMS_FROM "&body=Sua casa esta sendo invadida";
//
//
bool eviandoSMS;
//void enviaSms() {
//
//  eviandoSMS = true;
//  char response[30] = {};
//
//  //  Serial.println(parametros);
//
//  int statusCode = client.post("/sms", parametros, response);
//  Serial.print(F("Status da resposta: "));
//  Serial.println(statusCode);
//  Serial.print(F("Resposta do servidor: "));
//  Serial.println(response);
//  eviandoSMS = false;
//}

void enviaSms(){
  Wire.beginTransmission(8);
  Wire.write('h');
  Wire.endTransmission(8);
}

void guardaSenha() {


  char key = keypad.getKey();

  if (key) {
    if (key == '#') {

      digitandoSenha = false;

      if (!alarmeAtivado && senhaDigitada == SENHA_ATIVA) {
        Serial.println (F("Ativado"));
        alarmeAtivado = true;
        apagaAcendeLed(ledYellow, ledGreen, ledRed, 0);
        smsEnviado = false;
        delay(1000);
      }
      else if (alarmeAtivado && senhaDigitada == SENHA_CERTA) {
        Serial.println (F("Desativado"));
        senhaCerta = true;
        apagaAcendeLed(ledGreen, ledRed, ledYellow, 0);
        alarmeAtivado = false;
        senhaIncorreta = 0;
      }
      else {
        senhaIncorreta += 1;
        Serial.println (F("Incorreta"));
        if (senhaIncorreta >= 3) {
          apagaAcendeLed(ledRed, ledGreen, ledYellow, 1);
          if (!smsEnviado) {
            enviaSms ();
            smsEnviado = true;
          }
        }
      }
    }

    if (digitandoSenha) {
      senhaDigitada += key;
      Serial.println(senhaDigitada);
      delay(50);
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
  pinMode(buzz, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(infra , INPUT);
  Wire.begin();
  // Connect via DHCP
  //  Serial.println ("Conectando...");
//  if (Ethernet.begin(mac)) {
//    Serial.println(F("Conectado via DHCP"));
    //    Serial.print("IP recebido:"); Serial.println(Ethernet.localIP());
    //    delay(50);
//  }
}

void loop() {

//função para pegar a senha digitada e verificar ativação do alarme
  guardaSenha();

//leitura do infravermelho
  int doorOpenClosed = digitalRead(infra);

//  caso o alarme tenha sido ativado e o infravermelho seja obstruído
  if (alarmeAtivado && doorOpenClosed == 1) {

    Serial.println(F("Digitar a senha"));

    long timeDoorOpen = millis();
    senhaCerta = false;

    while (senhaCerta == false) {
      long now = millis();

      guardaSenha();

//timer de 10s para digitar a senha certa, caso contrário, um SMS é enviado
      if (!eviandoSMS && now - timeDoorOpen > 10000) {
        timeDoorOpen = now;
        apagaAcendeLed(ledRed, ledGreen, ledYellow, 1);
        if (!smsEnviado) {
          enviaSms ();
          smsEnviado = true;
        }
      }
    }
  }
}



