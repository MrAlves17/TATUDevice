#include <stdint.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <Ethernet.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <string.h>
#include <DHT.h>
#include <sensors.h>

//Digital pin for the dht sensor
#define DHTPIN 8

//Dht sensor version
#define DHTTYPE DHT11  

// Constants to connection with the broker
#define DEVICE_NAME "dht11"
#define MQTT_USER  "device"
#define MQTT_PASS  "boteco@wiser"
#define MQTTPORT 1883

//Hash's that represents the attributes "temp" and "ar"
#define H_temp 2090755995
#define H_humid 261814908

DHT dht(DHTPIN, DHTTYPE);

// Message for annoucement of connection
const char hello[] PROGMEM = DEVICE_NAME " has connected";

//variveis
int t,h,aux;
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xAC, 0xDC };
byte server[] = { 192, 168, 0, 101 };
byte ip[4]    = { 192, 168, 0, 127}; 

//int t,h,count1,count2;
unsigned long int time, lastConnect,prevTime,iTime;


bool get(uint32_t hash,void* response,uint8_t code){
  
  switch(hash){
      case H_temp:
        //The dht_temperatures_sensor supports INFO and VALUE requests.
        dht_temperature_sensor(dht,t,response,code);
        break;
      case H_humid:
        //The dht_humidity_sensor supports INFO and VALUE requests.
        dht_humidity_sensor(dht,h,response,code);
        break;
      default:
        return false;
  }

  
  return true;
  
}

// This is obrigatory, and defines this DEVICE
CREATE_DOD(DEVICE_NAME,
  ADD_SENSORS("temp", "dht11", "8")
  ADD_LAST_SENSOR("ar", "dht11", "8"),
  ADD_NONE()
);

// Objects to example that uses ethernet
EthernetClient EthClient;
TATUInterpreter interpreter;
TATUDevice device(DEVICE_NAME, ip, 121, 88, 0, server, MQTTPORT, 1, &interpreter, get);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , EthClient);
MQTT_PUBLISH(bridge, client);

void setup() {
  device.pub= &bridge;
  char aux[16];  
  Serial.begin(9600);
  Ethernet.begin(mac, ip);  
  pinMode(DHTPIN,INPUT);

    //Trying connect to the broker  
  while(!client.connect(device.name,MQTT_USER,MQTT_PASS));
  client.publish("dev/CONNECTIONS",hello);
  client.subscribe(device.aux_topic_name);
  client.subscribe("dev");
  
  Serial.println("Conected");

}
void loop() { client.loop(); 
  //Watchdog for connection with the broker
  if (!client.connected()) {
    reconnect();
  }
 
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
