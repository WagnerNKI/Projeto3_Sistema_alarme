#include <Ultrasonic.h>
#include <Keypad.h>


Ultrasonic ultrasonic(9, 8);
bool digitandoSenha = false;
String senhaDigitada;
String senha = "1234";
long timeDoorOpen;
int buzz = A1;

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {5, 4, 3, A0}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {A2, 7, 6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup(){
  Serial.begin(9600);
  pinMode(A2, INPUT);
  pinMode(A0, INPUT);
  pinMode(A1, OUTPUT);
}
  
void loop(){
  int distancia = ultrasonic.distanceRead();

  if (distancia <= 5){
    Serial.println("Favor digitar a senha em ate 10 segundos");
    long now = millis();
    bool senhaCerta = false;

    while(senhaCerta == false){
    if (now - timeDoorOpen > 10000){
      timeDoorOpen = now;
      tone(buzz, 500);
      delay (1000);
      noTone(buzz);
    }
    char key = keypad.getKey();
  
    if (key){
      if (key == '#'){
        Serial.println("Finalizou senha");
        digitandoSenha = false;
        if (senhaDigitada == senha){
          Serial.println ("Alarme desativado");
          senhaCerta = true;
          break;
        }
        else{
          Serial.println ("Senha incorreta");
        }
        
    }
    
      if (digitandoSenha){
        senhaDigitada += key;
        Serial.println (senhaDigitada);
    }
    
      if(key == '*'){
        senhaDigitada = "";
        Serial.println("Iniciando digitação da senha...");
        digitandoSenha = true;
            }
    }
  }
}
}
