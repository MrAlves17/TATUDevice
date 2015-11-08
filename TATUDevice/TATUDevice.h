#ifndef TATUDevice_h
#define TATUDevice_h

#include <avr/wdt.h>
#include <stdint.h>
#include <TATUInterpreter.h>

typedef uint8_t byte;

#ifndef MAX_SIZE_RESPONSE
#define MAX_SIZE_RESPONSE   20
#endif

#ifndef MAX_SIZE_OUTPUT
#define MAX_SIZE_OUTPUT     200
#endif

// System definitions
#define PROGMEM             __ATTR_PROGMEM__
#define putstring(x)        SerialPrint_PROGMEM(x)
#define PRINT_DEBUG(MSG)    SerialPrint_PROGMEM(MSG)
#define DEBUG_NL            Serial.write('\n')
#define OUT_STR             &output_message[aux]
#define MAX_SIZE_IP         16
#define MAX_SIZE_NAME       20
#define OS_VERSION          2
#define MQTTPORT_STANDARD   1883

// Constantes da mensagem
#define COMMA       output_message[aux++]=','
#define COLON       output_message[aux++]=':'
#define QUOTE       output_message[aux++]='\"'
#define BRACE_LEFT  output_message[aux++]='{'
#define BRACE_RIGHT output_message[aux++]='}'
#define CLOSE_MSG   output_message[aux]=0

// DOD - Device Object Description 
#define CREATE_DOD(NAME, SENSORS, ACTUATORS)   const char DOD[] PROGMEM = "POST " NAME ":{\"name\":\"" NAME "\""\
                                                ",\"mqtt_address\":\"dev/" NAME "\""\
                                                ",\"sensors\":[" SENSORS "]," \
                                                "\"actuators\":[" ACTUATORS "]}"

#define ADD_SINGLE_SENSOR(NAME, TYPE, PIN)      "{\"name\":\"" NAME "\",\"type\":\"" TYPE "\",\"pin\":" PIN "}"
#define ADD_SINGLE_ACTUATOR(NAME, TYPE, PIN)    ADD_SINGLE_SENSOR(NAME,TYPE,PIN)
#define ADD_LAST_SENSOR(NAME, TYPE, PIN)        ADD_SINGLE_SENSOR(NAME,TYPE,PIN)
#define ADD_LAST_ACTUATOR(NAME, TYPE, PIN)      ADD_SINGLE_SENSOR(NAME,TYPE,PIN)
#define ADD_SENSORS(NAME, TYPE, PIN)            ADD_SINGLE_SENSOR(NAME,TYPE,PIN) ","
#define ADD_ACTUATORS(NAME, TYPE, PIN)          ADD_SINGLE_SENSOR(NAME,TYPE,PIN) ","
#define ADD_NONE()                              ""

// Constantes do Sistema
#define SAMPLE_NUMBER 0

// Calcula o HASH DJB (deprecated)
//#define HASH_DJB(START, LEN, INPUT, OUTPUT) for(i = START; i < LEN; i++){ OUTPUT = ((OUTPUT << 5) + OUTPUT) + INPUT[i]; }
// Copiar uma string para a outra
#define STRCPY(INPUT, OUTPUT) do{ for(i = 0;INPUT[i] != 0; ++i) OUTPUT[i] = INPUT[i]; OUTPUT[i] = 0; }while(0)
#define STRCPY_I(INPUT, OUTPUT, INDEX) do{ for(i = INDEX;INPUT[i - INDEX] != 0; ++i) OUTPUT[i] = INPUT[i - INDEX]; OUTPUT[i] = 0; }while(0)

// Gera o body tendo o OBJETO dispositivo
#define generatePost(DEVICE) do{ DEVICE.generateHeader(); DEVICE.generateBody(); }while(0)
// Verifica se uma STRING está vazia
#define ISEMPTY(VAR) (VAR[0] == 0)
// Switch topic to the following topic
// REQ
//#define SWITCH_REQ_TOPIC(TOPIC, )

// Cria wrapper para a função de callback da classe
#define MQTT_CALLBACK(BRIDGE,OBJ, NAME) void BRIDGE(char *, char *);\
                                        void NAME(char *topic, byte *payload, unsigned int length) \
                                        {OBJ.mqtt_callback(topic, payload, length, BRIDGE);}
#define MQTT_PUBLISH(BRIDGE, OBJ) void BRIDGE(char *topic, char *out)\
                                  { OBJ.publish(topic,out); }

#define MQTT_PUBLISH2(OBJ, BRIDGE) OBJ.publish_test = &BRIDGE;

// Macro para interrupções
#define INTERRUPTION_VALUE(DEVICE,VAR_NAME,VAR,OPER,TRIGGER)switch (OPER){\
        case '=':\
            if(VAR == TRIGGER)\
                break;\
            return;\
        case '>':\
            if(VAR > TRIGGER)\
                break;\
            return;\
        case '<':\
            if(VAR < TRIGGER)\
                break;\
            return;\
        case '!':\
            if(VAR != TRIGGER)\
                break;\
            return;\
        default:\
            return;\
    }\
    DEVICE.interruption(VAR_NAME,VAR);

#define INTERRUPTION_STATE(DEVICE,VAR_NAME,VAR,OPER,TRIGGER)switch (OPER){\
        case '=':\
            if(VAR == TRIGGER)\
                break;\
            return;\
        case '!':\
            if(VAR != TRIGGER)\
                break;\
            return;\
        default:\
            return;\
    }\
    DEVICE.interruption(VAR_NAME,VAR);

#define INTERRUPTION_INFO(DEVICE,VAR_NAME,VAR,OPER,TRIGGER)switch (OPER){\
        case '=':\
            if(!strcmp(VAR,TRIGGER))\
                break;\
            return;\
        case '!':\
            if(strcmp(VAR,TRIGGER))\
                break;\
            return;\
        default:\
            return;\
    }\
    DEVICE.interruption(VAR_NAME,VAR);

// Constrói o dispositivo e o cliente 
#define SETUP(NAME, IP, ID, PAN, IP_SERVER, MQTTPORT, CALLBACK, CLIENT)\
            TATUInterpreter interpreter;\
            TATUDevice device(NAME, IP, ID, PAN, SAMPLE, IP_SERVER, MQTTPORT, OS_VERSION, &interpreter, CALLBACK);\
            MQTT_CALLBACK( bridge, device, mqtt_callback);\
            PubSubClient client(IP_SERVER, MQTTPORT, mqtt_callback, CLIENT);\
            MQTT_PUBLISH(bridge, client)

#define SETUP(NAME, IP, ID, PAN, IP_SERVER, MQTTPORT, CALLBACK_GET, CALLBACK_SET, CLIENT)\
            TATUInterpreter interpreter;\
            TATUDevice device(NAME, IP, ID, PAN, SAMPLE, IP_SERVER, MQTTPORT, OS_VERSION, &interpreter, CALLBACK_GET, CALLBACK_SET);\
            MQTT_CALLBACK( bridge, device, mqtt_callback);\
            PubSubClient client(IP_SERVER, MQTTPORT, mqtt_callback, CLIENT);\
            MQTT_PUBLISH(bridge, client)

// -----------
// Omite a porta padrão do MQTT
#define SETUP(NAME, IP, ID, PAN, IP_SERVER, CALLBACK, CLIENT)\
            TATUInterpreter interpreter;\
            TATUDevice device(NAME, IP, ID, PAN, SAMPLE, IP_SERVER, MQTTPORT_STANDARD, OS_VERSION, &interpreter, CALLBACK);\
            MQTT_CALLBACK( bridge, device, mqtt_callback);\
            PubSubClient client(IP_SERVER, MQTTPORT_STANDARD, mqtt_callback, CLIENT);\
            MQTT_PUBLISH(bridge, client)

#define SETUP(NAME, IP, ID, PAN, IP_SERVER, CALLBACK_GET, CALLBACK_SET, CLIENT)\
            TATUInterpreter interpreter;\
            TATUDevice device(NAME, IP, ID, PAN, SAMPLE, IP_SERVER, MQTTPORT_STANDARD, OS_VERSION, &interpreter, CALLBACK_GET, CALLBACK_SET);\
            MQTT_CALLBACK( bridge, device, mqtt_callback);\
            PubSubClient client(IP_SERVER, MQTTPORT_STANDARD, mqtt_callback, CLIENT);\
            MQTT_PUBLISH(bridge, client)

#define SETUP(NAME, IP, ID, PAN, IP_SERVER, CLIENT)\
            TATUInterpreter interpreter;\
            TATUDevice device(NAME, IP, ID, PAN, SAMPLE, IP_SERVER, MQTTPORT_STANDARD, OS_VERSION, &interpreter);\
            MQTT_CALLBACK( bridge, device, mqtt_callback);\
            PubSubClient client(IP_SERVER, MQTTPORT_STANDARD, mqtt_callback, CLIENT);\
            MQTT_PUBLISH(bridge, client)

#define FLUXO(DEVICE, HASH, ATT1, ATT2, ATT3, ACTIVE)\
        switch(HASH){\
            case ATT1:\
              if (ACTIVE) DEVICE.bits.ATT.FLOW1 = true;\
              else DEVICE.bits.ATT.FLOW1 = false;\
              break;\
            default:\
              return false;\
            case ATT2:\
              if (ACTIVE) DEVICE.bits.ATT.FLOW2 = true;\
              else DEVICE.bits.ATT.FLOW2 = false;\
              break;\
            default:\
              return false;\
            case ATT3:\
              if (ACTIVE) DEVICE.bits.ATT.FLOW3 = true;\
              else DEVICE.bits.ATT.FLOW3 = false;\
              break;\
            default:\
              return false;\
        }\
        return true

// Conecta o cliente mqtt
#define DEVICECONNECT() Serial.println("Trying to connect to the broker");\
                        if(client.connect(device.name)){\
                            Serial.println("The connection has suceeded");\
                            client.subscribe(device.subscribe_topic);}\
                        else Serial.println("The connection has failed")

// Conecta o cliente mqtt usando usuário e senha
#define SECURE_DEVICECONNECT(USER,PASS) Serial.println("Trying to connect to the broker");\
                                        if(client.connect(device.name,USER,PASS)){\
                                            Serial.println("The connection has suceed");\
                                            client.subscribe(device.name);}\
                                        else Serial.println("The connection has failed")

#define STOS(STRING1,STRING2) strcpy((char*)STRING2,STRING1)
#define ITOS(INTEGER,STRING) (itoa(INTEGER,(char*)STRING,10))
#define ITOI(INTEGER1,INTEGER2) *(int*)INTEGER2 = INTEGER1
#define BTOS(BOOL1,STRING) if (BOOL1) strcpy((char*)STRING,"T"); else strcpy((char*)STRING,"F");
#define BTOB(BOOL1,BOOL2) *(bool*)BOOL2 = BOOL1

//#define RESPONSE_CONSTRUCT(NAME_VAR)/* Coloca o BODY na resposta */ \
                                    //strcpy_P(OUT_STR, body_str); \
                                    //aux += 8; \
                                    //QUOTE; strcpy(OUT_STR, NAME_VAR); aux += strlen(NAME_VAR); QUOTE; COLON;  /* Copia a variavel vinda do payload */ \
                                    //BRACE_RIGHT; BRACE_RIGHT;  /* Fecha o JSON e a STRING */ \
                                    //CLOSE_MSG; \
                                    //publish(name, output_message) //publish the message 

/* Utilidades */
int freeRAM();
void ipToString(byte *ip, char *str);

// Extern Variables
extern const char DOD[] PROGMEM;

    typedef union {
        struct {
            uint8_t FLOW1 : 2;
            uint8_t FLOW2 : 2;
            uint8_t FLOW3 : 2;
            uint8_t FLOW4 : 2;
            uint8_t FLOW5 : 2;
            uint8_t FLOW6 : 2;
            uint8_t FLOW7 : 2;
            uint8_t FLOW8 : 2;
        } ATT;
        uint16_t STRUCTURE;
    } Bitflow;

class TATUDevice{
public:
    // Atributos públicos
    // Atributos do sistema


    Bitflow bits;
    char        name[MAX_SIZE_NAME];
    int         len_name;
    char        aux_topic_name[MAX_SIZE_NAME + 15];
    char        ip[MAX_SIZE_IP];
    uint8_t     id;
    uint8_t     pan;
    uint8_t     samples;
    char        mqtt_ip[MAX_SIZE_IP];
    uint16_t    mqtt_port;
    uint8_t     os_version;
    bool        dod_used;
    bool (*get_function)(uint32_t hash, void* response, uint8_t code);
    bool (*set_function)(uint32_t hash, uint8_t type, void* request);
    void (*publish_test)(char *, char *);
    // Atributos variaveis
    TATUInterpreter *requisition;

    /* TEORICO */
    /* uint8_t reset_counter;
    uint8_t start_counter; */

    // Mensagem de saida
    char output_message[MAX_SIZE_OUTPUT];
    int last_char;

    /* Callback's do Sistema */
    // Callback MQTT
    void mqtt_callback(char *, byte *, unsigned int, void (*publish)(char *, char *));
    
    // NOVA ARQUITETURA DE CALLBACK
    // ONLY GET AND SET ARE NEEDED
    // > ONLY GET
    TATUDevice( const char *name_d, byte *ip_d, const int id_d,   const int pan_d,
                const int sample_d, byte *ip_m, const int port_m, const int os_v,
                TATUInterpreter *req, bool (*GET_FUNCTION)(uint32_t hash, void* response, uint8_t type));
    // > ONLY SET
    TATUDevice( const char *name_d, byte *ip_d, const int id_d,   const int pan_d,
                const int sample_d, byte *ip_m, const int port_m, const int os_v,
                TATUInterpreter *req, bool (*SET_FUNCTION)(uint32_t hash, uint8_t type, void* request));
    // > BOTH
    TATUDevice( const char *name_d, byte *ip_d, const int id_d,   const int pan_d,
                const int sample_d, byte *ip_m, const int port_m, const int os_v,
                TATUInterpreter *req, bool (*GET_FUNCTION)(uint32_t hash, void* response, uint8_t type), 
                bool (*SET_FUNCTION)(uint32_t hash, uint8_t type, void* request));
    // > NONE
    TATUDevice( const char *name_d, byte *ip_d, const int id_d,   const int pan_d,
                const int sample_d, byte *ip_m, const int port_m, const int os_v,
                TATUInterpreter *req);
    
    void init( const char *name_d, byte *ip_d, const int id_d,   const int pan_d,
            const int sample_d, byte *ip_m, const int port_m, const int os_v,
            TATUInterpreter *req); 
    
    void interruption(const char *name, int var);
    void interruption(const char *name, char *var);
    void interruption(const char *name, bool var);
    
    void interrupt(const char *var_name, char *var);

    void generateHeader();
    void generateBody(char *payload, uint8_t length);
    void loop();
};

class TATUWatchDog{
public:
    long int time;
    long int lastConnect;
    long int reset_time;
    //PubSubClient client;
    char name[MAX_SIZE_NAME];

    void watchdogSetup();
    void loop(); 
};

#endif
