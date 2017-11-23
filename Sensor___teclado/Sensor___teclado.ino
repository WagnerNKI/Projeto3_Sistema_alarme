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

void apagaAcendeLed(int ledAcesso, int ledApagado1, int ledApagado2) {
  digitalWrite (ledAcesso, HIGH);
  digitalWrite (ledApagado1, LOW);
  digitalWrite (ledApagado2, LOW);
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
      bool senhaCerta = false;
      timeDoorOpen = millis();

      while (senhaCerta == false) {
        long now = millis();
        Serial.println(now - timeDoorOpen);
        
        char key = keypad.getKey();

        if (key) {
          if (key == '#') {
            Serial.println("Finalizou senha");
            digitandoSenha = false;
            if (senhaDigitada == senha) {
              Serial.println ("Alarme desativado");
              senhaCerta = true;
              timeDoorOpen = now;
              apagaAcendeLed(ledGreen, ledRed, ledYellow);
              alarmeAtivado = false;
            }
            else {
              Serial.println ("Senha incorreta");
              apagaAcendeLed(ledRed, ledGreen, ledYellow);
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

        if (now - timeDoorOpen > 20000) {
          timeDoorOpen = now;
          apagaAcendeLed(ledRed, ledGreen, ledYellow);
          tone(buzz, 1000);
          delay (1000);
          noTone(buzz);
        }

      }

    }
  }
}


