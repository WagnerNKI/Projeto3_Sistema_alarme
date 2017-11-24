#include <Keypad.h>

bool digitandoSenha = false;
String senhaDigitada;
String senha = "1234";
long timeDoorOpen = 0;
int buzz = A1;
int ledRed = A3;
int ledYellow = A4;
int ledGreen = A5;
String senhaAtiva = "0";
int infrared = 8;

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
const char* sid = "AC5ff5d08021fda59e7088098d55945157";
const char* token = "a33eab98854ffcb2546bde7618d21da2";
const char* to = "5511963650954";
const char* from = "13217668522";

String response = "";


void apagaAcendeLed(int ledAcesso, int ledApagado1, int ledApagado2) {
  digitalWrite (ledAcesso, HIGH);
  digitalWrite (ledApagado1, LOW);
  digitalWrite (ledApagado2, LOW);
}


void enviaSms(){
  
  String parametros = "sid=";
  parametros.concat(sid);

  parametros.concat("&token=");
  parametros.concat(token);

  parametros.concat("&to=");
  parametros.concat(to);

  parametros.concat("&from=");
  parametros.concat(from);

//  switch(situacao){
//    case 1:{
//      Serial.println("Alarme desativado");
//      parametros.concat("&body=Alarme desativado");
//      break;
//    }
//    case 2:{
//      Serial.println("Sua casa esta sendo invadida");
      parametros.concat("&body=Sua casa esta sendo invadida");
//      break;
//    }
//    case 3:{
////      Serial.println("Presenca detectada");
//      parametros.concat("&body=Presenca detectada");
//        break;
//    }
//  }
  

  Serial.println(parametros);

  int statusCode = client.post("/sms", parametros.c_str(), &response);
  Serial.print("Status da resposta: ");
  Serial.println(statusCode);
  Serial.print("Resposta do servidor: ");
  Serial.println(response);
  delay(1000);
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

    // Connect via DHCP
      Serial.println ("Conectando...");
  if(Ethernet.begin(mac)) {
    Serial.println("Conectado via DHCP");
    delay(50);
    Serial.print("IP recebido:"); Serial.println(Ethernet.localIP());
    delay(50);
  }
}

void loop() {

  char key = keypad.getKey();
  bool alarmeAtivado = false;
  
  if (key) {
    if (key == '#') {
      Serial.println("Finalizou senha");
      digitandoSenha = false;

      if (senhaDigitada == senhaAtiva) {
        Serial.println ("Alarme ativado");
        alarmeAtivado = true;
        apagaAcendeLed(ledYellow, ledGreen, ledRed);
        delay(1000);
      }
      else {
        Serial.println ("Senha incorreta");
      }

    }

    if (digitandoSenha) {
      senhaDigitada += key;
      Serial.println (senhaDigitada);
    }

    if (key == '*') {
      senhaDigitada = "";
      Serial.println("Iniciando digitação da senha...");
      digitandoSenha = true;
    }
  }

    while (alarmeAtivado == true) {
    int doorOpenClosed = digitalRead(infrared);
    
    if (doorOpenClosed == 1) {
      Serial.println("Favor digitar a senha em ate 10 segundos");
//      enviaSms ();
      bool senhaCerta = false;
      timeDoorOpen = millis();

      while (senhaCerta == false) {
        long now = millis();
//        Serial.println(now - timeDoorOpen);
        
        char key = keypad.getKey();

        if (key) {
          if (key == '#') {
            Serial.println("Finalizou senha");
            digitandoSenha = false;
            if (senhaDigitada == senha) {
              Serial.println ("Alarme desativado");
              senhaCerta = true;
              timeDoorOpen = now;
//              enviaSms (1);
              apagaAcendeLed(ledGreen, ledRed, ledYellow);
              alarmeAtivado = false;
            }
            else {
              Serial.println ("Senha incorreta");
              apagaAcendeLed(ledRed, ledGreen, ledYellow);
              enviaSms ();
              tone(buzz, 500);
              delay (1000);
              noTone(buzz);
            }

          }

          if (digitandoSenha) {
            senhaDigitada += key;
            Serial.println (senhaDigitada);
          }

          if (key == '*') {
            senhaDigitada = "";
            Serial.println("Iniciando digitação da senha...");
            digitandoSenha = true;
          }
        }

        if (now - timeDoorOpen > 10000) {
          timeDoorOpen = now;
          apagaAcendeLed(ledRed, ledGreen, ledYellow);
          enviaSms ();
          tone(buzz, 1000);
          delay (1000);
          noTone(buzz);
        }

      }

    }
  }
}


