#include "TATUDevice.h"
#include <TATUInterpreter.h>
#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "Arduino.h"

/* DEBUG! */
#ifdef DEBUG
const char GENERATE_BODY[]          PROGMEM = "[DEBUG] Generating Body";
const char CALLBACK_INFO[]          PROGMEM = "[DEBUG] INFO function is being executed";
const char CALLBACK_VALUE[]         PROGMEM = "[DEBUG] VALUE function is being executed";
const char CALLBACK_STATE[]         PROGMEM = "[DEBUG] STATE function is being executed";
const char GET_PIN[]                PROGMEM = "[DEBUG] The value of the pin is: ";
const char SET_PIN[]                PROGMEM = "[DEBUG] The value of the pin has been set";
const char THE_INFO_RESPONSE[]      PROGMEM = "[DEBUG] The response for the GET INFO requisition is: ";
const char THE_VALUE_RESPONSE[]     PROGMEM = "[DEBUG] The response for the GET VALUE requisition is: ";
const char THE_STATE_RESPONSE[]     PROGMEM = "[DEBUG] The response for the GET STATE requisition is: ";
const char BODY_GENERATED[]         PROGMEM = "[DEBUG] The body generation is done";
const char IS_A_POST[]              PROGMEM = "[DEBUG] It's a post so it doesn't have to publish anything";
const char PUBLISHING[]             PROGMEM = "[DEBUG] Publishing...";
const char PUBLISHED[]              PROGMEM = "[DEBUG] The message has been published";
const char NOT_A_GET[]              PROGMEM = "[DEBUG] It isn't a GET requisition";
const char SYSTEM[]                 PROGMEM = "[DEBUG] The system function isn't working yet";
const char THE_RESPONSE[]           PROGMEM = "[DEBUG] The message is:";
const char INITIATING[] 		    PROGMEM = "[DEBUG] Initianting the class...";
const char FINISHED_INIT[] 		    PROGMEM = "[DEBUG] Finished init!";
const char STARTING_GENERATE[]	    PROGMEM = "[DEBUG] Starting generate HEADER...";
const char ENDING_GENERATE[] 	    PROGMEM = "[DEBUG] Finished generate HEADER!";
const char HEADER_STR[] 		    PROGMEM = "[DEBUG] HEADER Value : ";
const char CLASS_CONSTRUCTED[] 	    PROGMEM = "[DEBUG] Class constructed with success!";
const char EXEC_ERROR[]			    PROGMEM = "[DEBUG] Execution Error!";
const char EXEC_ERROR_TYPE_VAR[]	PROGMEM = "[DEBUG] Unknown variable type!";

#endif

// Constantes
const char start_post[] PROGMEM = "POST ";
const char null_body[]  PROGMEM = "\"BODY\":null}";
const char true_body[]  PROGMEM = "\"BODY\":true}";
const char false_body[] PROGMEM = "\"BODY\":false}";
const char header_str[] PROGMEM = "\"HEADER\":{";
const char name_str[]   PROGMEM = "\"NAME\":\"";
const char id_str[]     PROGMEM = "\"ID\":";
const char pan_str[]    PROGMEM = "\"PAN\":";
const char ip_str[]     PROGMEM = "\"IP\":\"";
const char body_str[]   PROGMEM = "\"BODY\":{";
const char true_str[]   PROGMEM = "true";
const char false_str[]  PROGMEM = "false";
const char pin_st[]     PROGMEM = "PIN";

/* Utilidades */

int freeRAM(){
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int)__brkval);
}

void ipToString(byte *ip, char *str){
    int i, j;
    for(i = 0, j = 0; i < 4; i++){ itoa(ip[i], &str[j], 10); j += strlen(&str[j]); str[j++]= '.'; }
    str[j-1] = 0;
}
//INFO
bool info_default(uint32_t, char*, char*, uint8_t){
    return false;
}
//VALUE
bool value_default(uint32_t, uint16_t*, uint16_t, uint8_t){
    return false;
}
//STATE
bool state_default(uint32_t, bool*, bool, uint8_t){
    return false;
} 

/* Construct the TATUDevice Class passing the object Callback */
TATUDevice::TATUDevice( const char *name_d, byte *ip_d, const int id_d,    const int pan_d,
                        const int sample_d, byte *ip_m, const int port_m, const int os_v,
                        TATUInterpreter *req,Callback callback_struct){
	TATUCallback = callback_struct;
    init(name_d,ip_d,id_d,pan_d,sample_d,ip_m,port_m,os_v,req);

    #ifdef DEBUG
    PRINT_DEBUG(CLASS_CONSTRUCTED);
    DEBUG_NL;
	#endif
}

/* Construct the TATUDevice Class with only a callback */
TATUDevice::TATUDevice( const char *name_d,   byte *ip_d, const int id_d,    const int pan_d,
                        const int sample_d,   byte *ip_m, const int port_m,  const int os_v,
                        TATUInterpreter *req, bool (*callback_con)(uint32_t, char*, char*, uint8_t)){
    TATUCallback.info = callback_con;
    TATUCallback.value = value_default;
    TATUCallback.state = state_default;

    init(name_d,ip_d,id_d,pan_d,sample_d,ip_m,port_m,os_v,req);

    #ifdef DEBUG
    PRINT_DEBUG(CLASS_CONSTRUCTED);
    DEBUG_NL;
	#endif
}

TATUDevice::TATUDevice( const char *name_d,   byte *ip_d, const int id_d,    const int pan_d,
                        const int sample_d,   byte *ip_m, const int port_m,  const int os_v,
                        TATUInterpreter *req, bool (*callback_con)(uint32_t, uint16_t*, uint16_t, uint8_t)){
    TATUCallback.info = info_default;
    TATUCallback.value = callback_con;
    TATUCallback.state = state_default;

    init(name_d,ip_d,id_d,pan_d,sample_d,ip_m,port_m,os_v,req);

    #ifdef DEBUG
    PRINT_DEBUG(CLASS_CONSTRUCTED);
    DEBUG_NL;
    #endif
}
TATUDevice::TATUDevice( const char *name_d,   byte *ip_d, const int id_d,    const int pan_d,
                        const int sample_d,   byte *ip_m, const int port_m,  const int os_v,
                        TATUInterpreter *req, bool (*callback_con)(uint32_t, bool*, bool, uint8_t)){
    TATUCallback.info = info_default;
    TATUCallback.value = value_default;
    TATUCallback.state = callback_con;

    init(name_d,ip_d,id_d,pan_d,sample_d,ip_m,port_m,os_v,req);

    #ifdef DEBUG
    PRINT_DEBUG(CLASS_CONSTRUCTED);
    DEBUG_NL;
    #endif
}

/* Initialize the class */
void TATUDevice::init(  const char *name_d, byte *ip_d, const int id_d,   const int pan_d,
                        const int sample_d, byte *ip_m, const int port_m, const int os_v,
                        TATUInterpreter *req){
    int i;
    char aux[20];

	#ifdef DEBUG
    PRINT_DEBUG(INITIATING);
    DEBUG_NL;
	#endif

    // Define os atributos básicos
    STRCPY(name_d, name);
    ipToString(ip_d, aux);
    STRCPY(aux, ip);
    id = (uint8_t)  id_d;
    pan = (uint8_t) pan_d;
    samples = (uint8_t) sample_d;
    ipToString(ip_m, aux);
    STRCPY(aux, mqtt_ip);
    mqtt_port = (uint16_t) port_m;
    os_version = (uint8_t) os_v;
    requisition = req;
    //callback = callback_con;
    // Gera o header padrão e coloca no output_message atualizando a posição final do header
    generateHeader();

	#ifdef DEBUG
    PRINT_DEBUG(FINISHED_INIT);
	DEBUG_NL;
    #endif
}

/* Generate the header post */
void TATUDevice::generateHeader(){
    /* Auxiliary variable */
    int aux;
    char aux_str[10];

	#ifdef DEBUG
	PRINT_DEBUG(STARTING_GENERATE);
	DEBUG_NL;
	#endif

    // Primeiro se coloca a seguinte string padrão no vetor
    strcpy_P(output_message, start_post);
    strcpy(&output_message[5], name);
    
    // Inicia o JSON
    aux = strlen(name) + 5;
    COLON; BRACE_LEFT;
    
    // As próximas linhas produzem o HEADER
    strcpy_P(OUT_STR, header_str); /* Copia o HEADER */
    aux += 10;
    
    /* Coloca o NAME */
    strcpy_P(OUT_STR, name_str);
    aux += 8;
    strcpy(OUT_STR, name);
    aux += strlen(name);
    QUOTE; COMMA;

    /* Coloca o ID */
    strcpy_P(OUT_STR, id_str);
    aux += 5;
    itoa(id, aux_str, 10);
    strcpy(OUT_STR, aux_str);
    aux += strlen(aux_str);
    COMMA;
    
    /* Coloca o PAN */
    strcpy_P(OUT_STR, pan_str);
    aux += 6;
    itoa(pan, aux_str, 10);
    strcpy(OUT_STR, aux_str);
    aux += strlen(aux_str);
    COMMA;
    
    /* Coloca o IP */
    strcpy_P(OUT_STR, ip_str);
    aux += 6;
    strcpy(OUT_STR, ip);
    aux += strlen(ip);
    QUOTE;

    /* Fecha a mensagem de saída */
    BRACE_RIGHT; COMMA;
    CLOSE_MSG;

    last_char = aux;

	#ifdef DEBUG
    PRINT_DEBUG(HEADER_STR);
    Serial.println(output_message);
    PRINT_DEBUG(ENDING_GENERATE);
    DEBUG_NL;
	#endif
}

/* Parse and generate the body for the POST message */
void TATUDevice::generateBody(char *payload, uint8_t length){
    #ifdef DEBUG
    PRINT_DEBUG(GENERATE_BODY);
    DEBUG_NL;
    #endif
    
    int aux = last_char;
    bool isString;
    char response[MAX_SIZE_RESPONSE] = {0};
    uint16_t response_int = 0,
    		 value_int = 0;
    bool response_bool = false,
    	 value_bool = false;

    // Se encontrados erros no PARSE retorne "BODY":null
    if(!requisition->parse(payload, length)){ strcpy_P(OUT_STR, null_body); return; }
    // DEPRECATED //
    /* if(requisition->cmd.OBJ.TYPE == TATU_POST) return; */

    /*  */
    switch(requisition->cmd.OBJ.VAR){
        // Função do usuario
        case TATU_TYPE_ALIAS:
            //Baseado no código da resposta, decide qual função do usuário deve ser usada
            switch(requisition->cmd.OBJ.CODE){
                case TATU_CODE_INFO:
                    #ifdef DEBUG
                    PRINT_DEBUG(CALLBACK_INFO);
                    DEBUG_NL;
                    #endif
                    requisition->cmd.OBJ.ERROR = TATUCallback.info(requisition->str_hash,response,
                                                                    &payload[strlen(payload)+1],
                                                                    requisition->cmd.OBJ.TYPE);
                    break;
                case TATU_CODE_VALUE:
                    #ifdef DEBUG
                    PRINT_DEBUG(CALLBACK_VALUE);
                    DEBUG_NL;
                    #endif
                    value_int = atoi(&payload[strlen(payload)+1]);
                    requisition->cmd.OBJ.ERROR = TATUCallback.value(requisition->str_hash,&response_int,
                                                                    value_int,requisition->cmd.OBJ.TYPE);
                    break;
                case TATU_CODE_STATE:
                    #ifdef DEBUG
                    PRINT_DEBUG(CALLBACK_STATE);
                    DEBUG_NL;
                    #endif
                    requisition->cmd.OBJ.ERROR = TATUCallback.state(requisition->str_hash,&response_bool,
                                                                    requisition->cmd.OBJ.STATE,requisition->cmd.OBJ.TYPE);
                    break;
            }
            break;
        // Funções do sistema
        case TATU_TYPE_PIN:
            switch(requisition->cmd.OBJ.TYPE){
                case TATU_GET:
                    itoa(digitalRead(requisition->cmd.OBJ.PIN), response, 10);
                    #ifdef DEBUG
                    PRINT_DEBUG(GET_PIN);
                    Serial.println(response);
                    #endif
                    break;
                case TATU_SET:
                    digitalWrite(requisition->cmd.OBJ.PIN,requisition->cmd.OBJ.STATE);
                    requisition->cmd.OBJ.ERROR = false;
                    #ifdef DEBUG
                    PRINT_DEBUG(SET_PIN);
                    DEBUG_NL;
                    #endif
                    break;
            }
            break;
        case TATU_TYPE_SYSTEM:
            #ifdef DEBUG
            PRINT_DEBUG(SYSTEM);
            DEBUG_NL;
            #endif
            /** MODIFICA PROPRIEDADE **/
            // ISTO AINDA NÃO FOI IMPLEMENTADO
            strcpy_P(OUT_STR, false_body);
            break;
        default:
			#ifdef DEBUG
			PRINT_DEBUG(EXEC_ERROR);
			DEBUG_NL;
			PRINT_DEBUG(EXEC_ERROR_TYPE_VAR);
			DEBUG_NL;
			#endif
        	strcpy_P(OUT_STR, null_body);
        	return;
    }

    // Se não temos um GET verificamos se o SET ou EDIT não apresentaram erro
    if(requisition->cmd.OBJ.TYPE != TATU_GET){
        #ifdef DEBUG
        PRINT_DEBUG(NOT_A_GET);
        DEBUG_NL;
        #endif
        if(requisition->cmd.OBJ.ERROR) strcpy_P(OUT_STR, false_body);
        else strcpy_P(OUT_STR, true_body);
        return;
    }
    /* Coloca o BODY na resposta */
    strcpy_P(OUT_STR, body_str);
    aux += 8;

    // !IMPORTANT! Suporte para apenas uma variavel ''
    /* Copia a variavel vinda do payload */
    QUOTE; strcpy(OUT_STR, payload); aux += strlen(payload); QUOTE; COLON;

    /* Verifica o tipo de resposta esperada, e responde adequadamente*/
    switch(requisition->cmd.OBJ.CODE){
        case TATU_CODE_INFO:
            QUOTE; strcpy(OUT_STR, response); aux+=strlen(response); QUOTE;
            #ifdef DEBUG
            PRINT_DEBUG(THE_RESPONSE);
            Serial.println(response);
            #endif
            break;
        case TATU_CODE_VALUE:
            itoa(response_int,response,10);
            strcpy(OUT_STR, response);
            aux+=strlen(response);
            #ifdef DEBUG
            PRINT_DEBUG(THE_RESPONSE);
            Serial.println(response);
            #endif
            break;
        case TATU_CODE_STATE:
            if (response_bool)  response[0] = 'T';
            else response[0] = 'F';
            strcpy(OUT_STR, response);
            aux++;
            #ifdef DEBUG
            PRINT_DEBUG(THE_RESPONSE);
            Serial.println(response);
            #endif
            break;
    }
    // Fecha o JSON e a STRING
    BRACE_RIGHT; BRACE_RIGHT;
    CLOSE_MSG;
    #ifdef DEBUG
    PRINT_DEBUG(BODY_GENERATED);
    DEBUG_NL;
    #endif
}

void TATUDevice::mqtt_callback(char *topic, byte *payload, unsigned int length, void (*publish)(char *, char *)){
    /* Gera o body e publica o mesmo */
    generateBody((char *) payload, (uint8_t) length);

    if(requisition->cmd.OBJ.TYPE == TATU_POST){
        #ifdef DEBUG
        PRINT_DEBUG(IS_A_POST);
        DEBUG_NL;
        #endif
        return;
    }
    #ifdef DEBUG
    PRINT_DEBUG(PUBLISHING);
    Serial.println(output_message);
    #endif
    
    //publica a mensagem
    publish(name, output_message);
    
    #ifdef DEBUG
    PRINT_DEBUG(PUBLISHED);
    DEBUG_NL;
    #endif

}
