#include <PubSubClient.h>
#include <ESP8266WiFi.h>

const char *SSID = "TCC";          // SSID da sua rede Wi-Fi
const char *PASSWORD = "11041994"; // Senha da sua rede Wi-Fi

const char *ID_MQTT = "nodemcuServer"; // Identificação para o dispositivo

//Broker MQTT
const char *BROKER_MQTT = "raspberrypi.local"; // Endereço do servidor MQTT
////const IPAddress serverIPAddress (192, 168, 4, 1);

//const char *TOPIC_SUBSCRIBE = "inTopic"; // Endereço do servidor MQTT
//const char *TOPIC_PUBLISH = "outTopic";  // Endereço do servidor MQTT

const char *topic = "Mensagem";

//Variáveis e objetos globais
WiFiClient espClient;
PubSubClient client(espClient);

// MQTT response callback
void callback_mqtt(char* msgtopic, byte* payload, unsigned int length) {
  byte* p = (byte*)malloc(length);
  memcpy(p,payload,length);
  client.publish(topic, p, length);
  
  free(p);
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
  WiFi.mode(WIFI_STA);
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

//Inicializa parâmetros de conexão MQTT
void initMQTT()
{
  Serial.println("------Conexao Broker------");
  Serial.print("Conectando-se no Broker: ");
  Serial.println("Aguarde");
  client.setServer(BROKER_MQTT, 1883);
  Serial.println("Conectado com sucesso ao broker MQTT!");
  //client.publish(topic, "hello from ESP8266");
    
}

//Reconecta-se ao broker MQTT
void reconnectMQTT()
{
  while (!client.connected())
  {
    Serial.print("* Tentando se conectar ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    if (client.connect(ID_MQTT))
    {
      Serial.println("Conectado com sucesso ao broker MQTT!");
      //client.publish(topic, "hello from ESP8266");
    }
    else
    {
      Serial.println("Falha ao reconectar no broker.");
      Serial.println("Havera nova tentatica de conexao em 2s");
      delay(2000);
    }
  }
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

//Função: verifica o estado das conexões WiFI e ao broker MQTT.
void VerificaConexoesWiFIEMQTT()
{
  if (!client.connected())
    reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita

    reconectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

void setup()
{
  initSerial();
  
  initWiFi();
 
  initMQTT();

  client.setCallback(callback_mqtt);
  
}


//programa principal
void loop()
{
  //garante funcionamento das conexões WiFi e ao broker MQTT
  VerificaConexoesWiFIEMQTT();

  client.publish(topic, "Hello from ESP8266. Hello from ESP8266. Hello from ESP8266. Hello from ESP8266. Hello from ESP8266.");
  
  client.loop(); //loop MQTT

  delay(1000);
}
