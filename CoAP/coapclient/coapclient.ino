#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "coap_client.h"

const char *SSID = "TCC";          // SSID da sua rede Wi-Fi
const char *PASSWORD = "11041994"; // Senha da sua rede Wi-Fi

const char *idESP = "nodemcuclient";                 // Identificação para o dispositivo

// coap client response callback
void callback_response(coapPacket &packet, IPAddress ip, int port);

//Variáveis e objetos globais
WiFiClient espClient;
coapClient coap;

//IP e porta padrão do servidor que receberá a requisição (utilizado apenas se nenhum serviço for encontrado)
IPAddress ip(192,168,10,5);
int port = 5683;

// coap client response callback
void callback_response(coapPacket &packet, IPAddress ip, int port) {
    char p[packet.payloadlen + 1];
    memcpy(p, packet.payload, packet.payloadlen);
    p[packet.payloadlen] = NULL;
    String message(p);

    //response from coap server
    if(packet.type==3 && packet.code==0){
      Serial.println("ping ok");
    }

    Serial.println(p);
}

// Inicia a comunicação serial
void initSerial()
{
  Serial.begin(115200);
}

// Conecta ao WiFi
void initWiFi()
{
  Serial.println("------Conexao WI-FI------");
  Serial.print("Conectando-se na rede: ");
  WiFi.begin(SSID, PASSWORD);
  Serial.println("Aguarde");
  //Aguarda até que a conexão seja estabelecida
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

  
   
 void initmDNS(){
    if (MDNS.begin(idESP))
    {
      Serial.println("mDNS iniciado");
    }
  
    //Descobre qual ip e porta do servidor coap (esp1)
    int n = MDNS.queryService("coap","udp");
    if (n == 0) {
      Serial.println("serviço não encontrado");
    }
    else {
      Serial.print(n);
      Serial.println(" serviço(s) encrontrado(s)");
      for (int i = 0; i < n; ++i) {
        // Mostra na serial o(s) serviço(s) encontrado(s)
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(MDNS.hostname(i));
        Serial.print(" (");
        Serial.print(MDNS.IP(i));
        Serial.print(":");
        Serial.print(MDNS.port(i));
        Serial.println(")");
      }
      // Utiliza apenas o endereço do primeiro serviço encontrado para realizar a comunicação.
      ip = MDNS.IP(0);
      port = MDNS.port(0);
    }
  }


// Inicia o coap server
void initCOAP()
{
  coap.start();
}

//Função: reconecta-se ao WiFi
void reconectWiFi()
{
  //se já está conectado a rede WI-FI, nada é feito.
  //Caso contrário, são efetuadas tentativas de conexão
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado com sucesso na rede ");
  Serial.print(SSID);
  Serial.println("IP obtido: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  initSerial();
  initWiFi();
  initmDNS();
  
  // client response callback.
  // this endpoint is single callback.
  Serial.println("Setup Response Callback");
  coap.response(callback_response);
  
  initCOAP();
  
  int msgid = coap.get(ip,port,"light");

}

//programa principal
void loop()
{
  coap.loop();
  coap.get(ip,port,"light");
  delay(5000);
}
