#include <UIPEthernet.h>
#include <RestClient.h>

// Alterar o último valor para o id do seu kit
const byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xF1, 0x64 };
EthernetClient ethclient;

RestClient client = RestClient("192.168.3.186", 3000, ethclient);


String response = "";

#define SMS_SID "AC5ff5d08021fda59e7088098d55945157"
#define SMS_TOKEN "a33eab98854ffcb2546bde7618d21da2"
#define SMS_TO "5511963650954"
#define SMS_FROM "13217668522"

const char* parametros = "sid=SMS_SID&token=SMS_TOKEN&to=SMS_TO&from=SMS_FROM&body=Sua casa esta sendo invadida";


void enviaSms(){
  String parametros = "sid=";
  parametros.concat(SMS_SID);

  parametros.concat("&token=");
  parametros.concat(SMS_TOKEN);

  parametros.concat("&to=");
  parametros.concat(SMS_TO);

  parametros.concat("&from=");
  parametros.concat(SMS_FROM);

  parametros.concat("&body=Sua casa esta sendo invadida");

  Serial.println(parametros);

  int statusCode = client.post("/sms", parametros.c_str(), &response);
  Serial.print("Status da resposta: ");
  Serial.println(statusCode);
  Serial.print("Resposta do servidor: ");
  Serial.println(response);
  delay(1000);
}

//Setup
void setup() {
  Serial.begin(9600);
  // Connect via DHCP
  if(Ethernet.begin(mac)) {
    Serial.println("Conectado via DHCP");
    Serial.print("IP recebido:"); Serial.println(Ethernet.localIP());

//  Serial.begin(9600);
//  // Connect via DHCP
//  if(Ethernet.begin(mac)) {
//    Serial.println("Conectado via DHCP");
//    Serial.print("IP recebido:"); Serial.println(Ethernet.localIP());

  }

enviaSms();
}

void loop(){

}
