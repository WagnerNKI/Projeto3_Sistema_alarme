#include <Wire.h>

//Configuração para envio de SMS
#include <UIPEthernet.h>
#include <RestClient.h>


// Alterar o último valor para o id do seu kit
const byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xF1, 0x64 };
EthernetClient ethclient;

RestClient client = RestClient("192.168.3.186", 3000, ethclient);

#define SMS_SID "AC5ff5d08021fda59e7088098d55945157"
#define SMS_TOKEN "a33eab98854ffcb2546bde7618d21da2"
#define SMS_TO "5511963650954"
#define SMS_FROM "13217668522"
#define PARAMETROS "sid=" SMS_SID "&token=" SMS_TOKEN "&to=" SMS_TO "&from=" SMS_FROM "&body=Sua casa esta sendo invadida"


void setup() {
  // put your setup code here, to run once:
  Wire.begin(8);
  Serial.begin(9600);

  // Connect via DHCP
  Serial.println (F("Conectando..."));
  if (Ethernet.begin(mac)) {
    Serial.println(F("Conectado via DHCP"));
    //    Serial.print(F("IP recebido:")); Serial.println(Ethernet.localIP());
    delay(50);
  }

  Wire.onReceive (comandoRecebido);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void comandoRecebido() {

  char comando = Wire.read();

  if (comando == 'h') {
    Serial.println(F("SMS enviado"));
    Serial.flush();


    char response[10];

    String parametros = F(PARAMETROS);
    int statusCode = client.post("/sms", parametros.c_str(), response);
    Serial.print(F("Status da resposta: "));
    Serial.println(statusCode);
    Serial.print(F("Resposta do servidor: "));
    Serial.println(response);

    //    Serial.println(PARAMETROS);


  }
}
