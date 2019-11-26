#include <ESP8266WiFi.h>
#include <coap_server.h>
#include <ESP8266mDNS.h>

const char *SSID = "TCC";          // SSID da sua rede Wi-Fi
const char *PASSWORD = "11041994"; // Senha da sua rede Wi-Fi

const char *idESP = "nodemcuserver";

// CoAP server endpoint url callback
void callback_light(coapPacket &packet, IPAddress ip, int port, int obs);

//Variáveis e objetos globais
WiFiClient espClient;
coapServer coap;

// LED STATE
bool LEDSTATE;

// CoAP server endpoint URL
void callback_light(coapPacket *packet, IPAddress ip, int port,int obs) {
  
  Serial.println("Hello Word!");

  // send response
  char p[packet->payloadlen + 1];
  memcpy(p, packet->payload, packet->payloadlen);
  p[packet->payloadlen] = NULL;
  Serial.println(p);

  String message(p);

  if (message.equals("0"))
  {
    digitalWrite(16,LOW);
    Serial.println("if loop");
  }
  else if (message.equals("1"))
  {
    digitalWrite(16,HIGH);
    Serial.println("else loop");
  } 
  char *light = "Constrained Application Protocol (CoAP) is a specialized Internet Application Protocol for constrained devices, as defined in RFC 7252. It enables those constrained devices called nodes to communicate with the wider Internet using similar protocols. CoAP is designed for use between devices on the same constrained network (e.g., low-power, lossy networks), between devices and general nodes on the Internet, and between devices on different constrained networks both joined by an internet. CoAP is also being used via other mechanisms, such as SMS on mobile communication networks.";
  
   //coap.sendResponse(packet, ip, port, light);
   if(obs==1)
    coap.sendResponse(light);
   else
    coap.sendResponse(ip,port,light);
 
}



void callback_lightled(coapPacket *packet, IPAddress ip, int port,int obs) {
  Serial.println("Lightled");

  // send response
  char p[packet->payloadlen + 1];
  memcpy(p, packet->payload, packet->payloadlen);
  p[packet->payloadlen] = NULL;

  String message(p);

  if (message.equals("0"))
    LEDSTATE = false;
  else if (message.equals("1"))
    LEDSTATE = true;

  if (LEDSTATE) {
    digitalWrite(5, HIGH) ;
    if(obs==1)
     coap.sendResponse("1");
     else
    coap.sendResponse(ip, port, "1");
    
    //coap.sendResponse("1");
  } else {
    digitalWrite(5, LOW) ;
    if (obs==1)
    coap.sendResponse("0");
    else
    coap.sendResponse(ip, port, "0");
    //coap.sendResponse("0");
  }
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

//mDNS/DNS-SD
// Inicia o mDNS (este dispositivo poderá ser acessado por "esp1.local")
void initmDNS()
{
  if (MDNS.begin(idESP))
  {
    Serial.println("mDNS iniciado");
  }

  // Adiciona o serviço CoAP para que seja descoberto pelo DNS-SD
  MDNS.addService("coap", "udp", 5683);
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
  initCOAP();

// LED State
  pinMode(16, OUTPUT);
  digitalWrite(16, HIGH);
  LEDSTATE = true;

  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  //LEDSTATE = true;


  // add server url endpoints.
  // can add multiple endpoint urls.
  coap.server(callback_light, "light");
  coap.server(callback_lightled, "lightled");
 
  // start coap server/client
  coap.start();
}

void loop() {

  coap.loop();
  delay(1000);
}
