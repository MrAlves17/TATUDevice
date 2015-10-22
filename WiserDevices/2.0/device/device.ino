#include <stdint.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <Ethernet.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <string.h>
#include <DHT.h>

// Digital pins used
#define LUMINOSITY A3
#define MOVE 3
#define DHTPIN 8
#define SOUND A1
#define GAS A0

// DHT TYPE
#define DHTTYPE 11

//Port to connect with the broker 
#define MQTTPORT 1883

//Hash that represents the attribute "lamp" 
#define H_sound 274653294
#define H_gas 193492480
#define H_temp 2090755995
#define H_ar 5863224
#define H_move 2090515612
#define H_luminosity 1516126306

#define ligar(PIN) digitalWrite(PIN,true)
#define desligar(PIN) digitalWrite(PIN,false)

#define MQTT_USER  "device"
#define MQTT_PASS  "boteco@wiser"

DHT dht(DHTPIN, DHTTYPE);

//variveis
volatile int soundReading,movement,gas_amount,t,h,luminosity;
bool lamp;
char str[20];1
int aux;
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xAC, 0xDC };
byte server[] = { 10, 41, 0, 93 };
byte ip[4]    = { 10, 41, 0, 97}; 

unsigned long int time, lastConnect,prevTime,iTime;

bool get(uint32_t hash,void* response,uint8_t code){
  switch(hash){
      case H_move:
        switch(code){   
          case TATU_CODE_INFO:
            ITOS(movement,response);
            break;
          case TATU_CODE_VALUE:
            ITOI(movement,response);
            break;
          default:
            return false;
        } 
        movement = 0;
        break;
      case H_sound:
        soundReading = analogRead(SOUND);
        switch(code){
          case TATU_CODE_INFO:
            ITOS(soundReading,response);
            break;
          case TATU_CODE_VALUE:
            ITOI(soundReading,response);
            break;
          default:
            return false;
        }
        break;
      case H_temp:
        t = (int)dht.readTemperature();
        switch(code){   
          case TATU_CODE_INFO:
            ITOS(t,response);
            break;
          case TATU_CODE_VALUE:
            ITOI(t,response);
            break;
          default:
            return false;
        } 
        break;
      case H_ar:
        h = (int)dht.readHumidity();
        switch(code){   
          case TATU_CODE_INFO:
            ITOS(h,response);
            break;
          case TATU_CODE_VALUE:
            ITOI(h,response);
            break;
          default:
            return false;
        } 
        break;
      case H_luminosity:
        luminosity = (analogRead(LUMINOSITY) - 1023) * (-1);
        
        switch(code){   
          case TATU_CODE_INFO:
            luminosity = map (luminosity,0,1023,0,100);
            ITOS(luminosity,response);
            aux = strlen((char*)response);
            ((char*)response)[aux++] = '%';
            ((char*)response)[aux] = 0;
            break;
          case TATU_CODE_VALUE:
            ITOI(luminosity,response);
            break;
          default:
            return false;
        } 
        break;
      case H_gas:
        gas_amount = analogRead(GAS);
        gas_amount = map (gas_amount,0,1023,0,100);
        switch(code){   
          case TATU_CODE_INFO:
            ITOS(gas_amount,response);
            aux = strlen((char*)response);
            ((char*)response)[aux++] = '%';
            ((char*)response)[aux] = 0;
            break;
          case TATU_CODE_VALUE:
            ITOI(gas_amount,response);
            break;
          case TATU_CODE_STATE:
            if (gas_amount > 55) BTOB(true,response);
            else BTOB(false,response);
            break;
        } 
        break;
      default:
        return false;
  }
  return true; 
}

// Objects to example that uses ethernet
EthernetClient EthClient;
TATUInterpreter interpreter;
TATUDevice device("device", ip, 121, 88, 0, server, MQTTPORT, 1, &interpreter, get);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , EthClient);
MQTT_PUBLISH(bridge, client);

// This is obrigatory, and defines this DEVICE
CREATE_DOD("device",
  ADD_SENSORS("luminosity", "ldr", "A3")
  ADD_SENSORS("move", "pir", "3")
  ADD_SENSORS("temp", "dht11", "8")
  ADD_SENSORS("gas", "mq2", "A0")
  ADD_SENSORS("sound", "mic", "A1")
  ADD_LAST_SENSOR("ar", "dht11", "3"),
  ADD_NONE()
);

void setup() {
  cli();

  device.publish_test = &bridge;
  char aux[16];  
  Serial.begin(9600);
  Ethernet.begin(mac, ip);  
  
  pinMode(DHTPIN,INPUT);
  pinMode(MOVE, INPUT);
  
  device.DOD = DOD;
  
  digitalWrite(MOVE, HIGH);
  attachInterrupt(1, mexeu, FALLING);

  //Trying connect to the broker  
  while(!client.connect(device.name,MQTT_PASS,MQTT_USER));
  client.subscribe(device.aux_topic_name);
  client.subscribe("dev");
  sei();
  Serial.println("Conected");
}
void loop() { 
  client.loop(); 
  //Watchdog for connection with the broker
  if (!client.connected()) {
    reconnect();
  }
  
  interruption_luminosity();
}
/*
void interruption_luminosidade(){
  luminosity = (analogRead(LUMINOSITY) - 1023) * (-1);
  luminosity = map (luminosity,0,1023,0,100);
  itoa(luminosity,str,10);
  device.interruption("luminosity",str,'=',"23%");
}
*/
void interruption_luminosity(){
  luminosity = (analogRead(LUMINOSITY) - 1023) * (-1);
  luminosity = map (luminosity,0,1023,0,100);
  device.interruption("luminosity",luminosity,'<',35);
}

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
    if (client.publish("dev",device.name)) {
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

