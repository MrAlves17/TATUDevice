#include <stdint.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <Ethernet.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <sensors.h>
#include <string.h>
#include <DHT.h>

// Pins used
#define GAS 1
#define DOOR 6
#define DHTPIN 8
#define LAMP 9
#define MOVE 20

// DHT TYPE
#define DHTTYPE DHT11  

// Constants to connection with the broker
#define DEVICE_NAME "temp-lamp"
#define MQTT_USER  "device"
#define MQTT_PASS  "boteco@wiser"
#define MQTTPORT 1883

//Hash that represents the attributes
#define H_gas 193492480
#define H_lamp 2090464143
#define H_temp 2090755995
#define H_humid 261814908
#define H_ar 5863224
#define H_move 2090515612
#define H_door 2090191961

// Message for annoucement of connection
const char hello[] PROGMEM = DEVICE_NAME " has connected";

#define ON(PIN) digitalWrite(PIN,true)
#define OFF(PIN) digitalWrite(PIN,false)

DHT dht(DHTPIN, DHTTYPE);

//variveis
bool lamp = 0,aux;
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte server[] = { 192, 168, 0, 101 };
byte ip[4]    = { 192, 168, 0, 68 };

//int t,h,count;
volatile int soundReading,movement,gas_amount,t,h;

  bool get(uint32_t hash,void* response,uint8_t code){
    switch(hash){
        case H_move:
          //The I_V_sensor supports INFO and VALUE requests for any integer variable.
          I_V_sensor(movement,response,code);
          movement = 0;
          break;
        case H_temp:
          //The dht_temperatures_sensor supports INFO and VALUE requests.
          dht_temperature_sensor(dht,t,response,code);
          break;
        case H_humid:
          //The dht_humidity_sensor supports INFO and VALUE requests.
          dht_humidity_sensor(dht,h,response,code);
          break;
        case H_gas:
          //The gas_sensor supports INFO,VALUE and STATE requests.
          gas_sensor(GAS,gas_amount,response,code);
          break;
        default:
          return false;
    }
    return true; 
  }
bool set(uint32_t hash,uint8_t code,void* response){
  switch(hash){
    case H_lamp:
      if (*(bool*)response){ON(LAMP);Serial.println("ON"); lamp = true;}
      else {OFF(LAMP);Serial.println("OFF"); lamp = false;}
      break;
    default:
      return false;
    }
    return true; 
}
// Funçao do usuario para variaveis do TATU
bool info(uint32_t hash,char* response,char* valor,uint8_t type) {
  
  Serial.print("Hash :");
  Serial.println(hash);
  
  switch(type){  
    case TATU_GET:
      switch(hash){
        case H_lamp:
          if(lamp) strcpy(response,"ON");
          else strcpy(response,"OFF");
          break;
        case H_temp:
          t = (int)dht.readTemperature();
          itoa(t,response,10);
          break;  
        case H_humid:
          h = (int)dht.readHumidity();
          itoa(h,response,10);
          break;
        case H_gas:
          gas_amount = analogRead(GAS);
          gas_amount = map (gas_amount,0,1023,0,100);
          itoa(gas_amount,response,10);
          aux = strlen(response);
          response[aux++] = '%';
          response[aux] = 0;
          break;
        case H_door:
          if (digitalRead(DOOR) == true)
            strcpy(response,"Aberta");
          else
            strcpy(response,"Fechada");
          break;
        default:
          return false;
      }
      break;
    case TATU_SET:
      switch(hash){
        case H_lamp:
          if (valor[0] == '1'){ON(LAMP);Serial.println("ON"); lamp = true;}
          else if (valor[0] == '0'){OFF(LAMP);Serial.println("OFF"); lamp = false;}
          break;
        default:
          return false;
      } 
      //Serial.println(valor);
      break;
  } 
  return true;

}

//STATE
bool state(uint32_t hash,bool* response,bool valor,uint8_t type){
  
  Serial.print("Hash :");
  Serial.println(hash);
  
  switch(type){  
    case TATU_GET:
      Serial.println("akiGET");
      switch(hash){
          Serial.println("akiHASH");
          case H_lamp:
            Serial.println("akiLAMP");
            *response = lamp;
            break;
          case H_gas:
            gas_amount = analogRead(GAS);
            gas_amount = map (gas_amount,0,1023,0,100);
            Serial.print("Gas amount: ");
            Serial.println(gas_amount);
            if (gas_amount > 55) *response = true;
            else *response = false;
            break;
          case H_door:
            *response = digitalRead(DOOR);
            break;
          default:
            return false;
      }
      break;
    case TATU_SET:
      switch(hash){
          case H_lamp:
            if (valor){ON(LAMP);Serial.println("ON"); lamp = true;}
            else {OFF(LAMP);Serial.println("OFF"); lamp = false;}
            break;
          default:
            return false;
      } 
      break;  
  }
  return true;
}


// Objetos para exemplo usando interface internet
EthernetClient EthClient;
TATUInterpreter interpreter;
TATUDevice device(DEVICE_NAME, ip, 121, 88, 0, server, MQTTPORT, 1, &interpreter, get,set);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , EthClient);
MQTT_PUBLISH(bridge, client);

// This is obrigatory, and defines this DEVICE
CREATE_DOD(DEVICE_NAME,
  ADD_SENSORS("luminosity", "ldr", "A3")
  ADD_SENSORS("move", "pir", "3")
  ADD_SENSORS("temp", "dht11", "8")
  ADD_SENSORS("gas", "mq2", "A0")
  ADD_SENSORS("sound", "mic", "A1")
  ADD_LAST_SENSOR("ar", "dht11", "8"),
  ADD_NONE()
);


//SETUP
void setup() {
  //In order to avoid problems caused by external interruptions,
  //is recomended disable the interruption when using the attachInterrupt function;
  Serial.println("Trying connect to the broker");  
  cli();//disable interruptions

  device.publish_test = &bridge;

  char aux[16];  
  Serial.begin(9600);
  dht.begin();
  Ethernet.begin(mac, ip);
  pinMode(DHTPIN,INPUT);
  pinMode(LAMP,OUTPUT);
  pinMode(MOVE, INPUT);
  pinMode(DOOR,INPUT);
  
  digitalWrite(MOVE, HIGH);
  attachInterrupt(3, mexeu, FALLING);
   
  //Trying connect to the broker
  //Serial.println("Trying connect to the broker");  
  while(!client.connect(device.name,MQTT_USER,MQTT_PASS));
  client.publish("dev/CONNECTIONS",hello);
  client.subscribe(device.aux_topic_name);
  client.subscribe("dev");
  sei();//unable interruptions
  Serial.println("Conected!!");
}

//LOOP
void loop() { 
  client.loop(); 
  //Watchdog for connection with the broker
  if (!client.connected()) {
    reconnect();
  }
  
}


//PERSONAL FUNCTIONS
void mexeu(){
  device.interrupt("move","mexeu");
  movement++;
  Serial.println("mexeu");
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connect(device.name, MQTT_USER, MQTT_PASS)) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.publish("dev/CONNECTIONS",hello)) {
      Serial.println("connected");
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

