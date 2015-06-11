#include "TATUInterpreter.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "Arduino.h"
#include <avr/pgmspace.h>

#ifdef DEBUG
const char START_PARSE[]        PROGMEM = "[DEBUG] Starting Parse";
const char FOUND_GET[]          PROGMEM = "[DEBUG] Found GET";
const char FOUND_SET[]          PROGMEM = "[DEBUG] Found SET";
const char FOUND_NUM[]          PROGMEM = "[DEBUG] Is Pin";
const char FOUND_ALL[]          PROGMEM = "[DEBUG] Found ALL";
const char FOUND_EDIT[]         PROGMEM = "[DEBUG] Found EDIT";
const char FOUND_POST[]         PROGMEM = "[DEBUG] Found POST";
const char FOUND_INFO[]         PROGMEM = "[DEBUG] Found INFO";
const char FOUND_VALUE[]        PROGMEM = "[DEBUG] Found VALUE";
const char FOUND_STATE[]        PROGMEM = "[DEBUG] Found STATE";
const char FOUND_SYSTEM[]       PROGMEM = "[DEBUG] Is System";
const char FOUND_ANALOG[]       PROGMEM = "[DEBUG] And is analog";
const char PARAM_VAR[]          PROGMEM = "[DEBUG] Variable received: ";
const char CODE_ERROR[]         PROGMEM = "[DEBUG] Code ERROR";
const char PARAM_VALUE[]        PROGMEM = "[DEBUG] Value received: ";
const char START_CODING[]       PROGMEM = "[DEBUG] Starting coding";
const char HASH_GENERATED[]     PROGMEM = "[DEBUG] Hash generated: ";
const char COMMAND_RECEIVED[]   PROGMEM = "[DEBUG] Command received: ";
const char REQUISITION_ERROR[]  PROGMEM = "[DEBUG] Requisition ERROR";
#endif

/* Generate the djb hash */
uint32_t hash_djb(char *string){
    uint32_t hash = 5381;
    int i;
    for(i = 0; string[i] != 0; i++){ hash = ((hash << 5) + hash) + string[i]; }
    return hash;
}

/* Array to String **Optimized */
uint8_t atoi_T(char *p){
    int k = 0;
    while(*p){k = (k << 3) + (k << 1) + (*p) - '0'; p++; }
    return k;
}

/* Little utilitie to print progmem char */
void SerialPrint_PROGMEM(PGM_P str){
    for (uint8_t c; (c = pgm_read_byte(str)); str++) Serial.write(c);
}

/* Parse TATU and return if this fail or not */
bool TATUInterpreter::parse(char *string, unsigned int length){
    #ifdef DEBUG
    PRINT_DEBUG(START_PARSE);
    #endif
    /* Default Initialization */
    unsigned int i, j = 0;
    cmd.STRUCTURE = 0;
    cmd.OBJ.VAR = TATU_TYPE_ALIAS;
    cmd.OBJ.TYPE = 1;
    
    #ifdef DEBUG
    // Print the received command
    Serial.print(COMMAND_RECEIVED);
    for(i = 0; string[i] != 0 && string[i] != ' '; i++)
        Serial.print(string[i]);
    Serial.println(".");
    #endif
    
    switch(string[0]){
        case COMMAND_GET:
            #ifdef DEBUG
            PRINT_DEBUG(FOUND_GET);
            #endif
            
            cmd.OBJ.TYPE = TATU_GET;
            j = 9;
            
            switch(string[4]){
                case CODE_ALL:
                    #ifdef DEBUG
                    PRINT_DEBUG(FOUND_ALL);
                    #endif
                    cmd.OBJ.CODE = TATU_CODE_ALL;
                    return true;
                case CODE_INFO:
                    #ifdef DEBUG
                    PRINT_DEBUG(FOUND_INFO);
                    #endif
                    cmd.OBJ.CODE = TATU_CODE_INFO;
                    break; 
                case CODE_STATE:
                    #ifdef DEBUG
                    PRINT_DEBUG(FOUND_STATE);
                    #endif
                    cmd.OBJ.CODE = TATU_CODE_STATE;
                    break; 
                case CODE_VALUE:
                    #ifdef DEBUG
                    PRINT_DEBUG(FOUND_VALUE);
                    #endif
                    cmd.OBJ.CODE = TATU_CODE_VALUE;
                    j++;
                   break;
                default:
                    #ifdef DEBUG
                    PRINT_DEBUG(CODE_ERROR);
                    #endif
                    /* If the desired command is not found return error */
                    return false;
            }
            break;
        case COMMAND_SET: 
            #ifdef DEBUG
            PRINT_DEBUG(FOUND_SET);
            #endif
            
            cmd.OBJ.TYPE = TATU_SET;
            j = 9; // Default String position
            switch(string[4]){
                case CODE_INFO:
                    #ifdef DEBUG
                    PRINT_DEBUG(FOUND_INFO);
                    #endif
                    cmd.OBJ.CODE = TATU_CODE_INFO;
                    break;
                case CODE_STATE:
                    #ifdef DEBUG
                    PRINT_DEBUG(FOUND_STATE);
                    #endif
                    cmd.OBJ.CODE = TATU_CODE_STATE;
                    j++;
                    break;
                case CODE_VALUE:
                    #ifdef DEBUG
                    PRINT_DEBUG(FOUND_VALUE);
                    #endif
                    cmd.OBJ.CODE = TATU_CODE_VALUE;
                    j++;
                    break;
                default:
                    #ifdef DEBUG
                    PRINT_DEBUG(CODE_ERROR);
                    #endif
                    /* If the desired command is not found return error */
                    return false;
            }
            break;
        case COMMAND_EDIT:
            #ifdef DEBUG
            PRINT_DEBUG(FOUND_EDIT);
            #endif
            
            cmd.OBJ.TYPE = TATU_EDIT;
            j = 10;
            switch(string[5]){
                case CODE_INFO:
                    #ifdef DEBUG
                    PRINT_DEBUG(FOUND_INFO);
                    #endif
                    cmd.OBJ.CODE = TATU_CODE_INFO;
                    break;
                case CODE_STATE:
                    #ifdef DEBUG
                    PRINT_DEBUG(FOUND_STATE);
                    #endif
                    cmd.OBJ.CODE = TATU_CODE_STATE;
                    j++; 
                    break;
                case CODE_VALUE:
                    #ifdef DEBUG
                    PRINT_DEBUG(FOUND_VALUE);
                    #endif
                    cmd.OBJ.CODE = TATU_CODE_VALUE;
                    j++; 
                    break;
                default:
                    #ifdef DEBUG
                    PRINT_DEBUG(CODE_ERROR);
                    #endif
                    /* If the desired command is not found return error */
                    return false;
            }
            break;
        case COMMAND_POST:
            #ifdef DEBUG
            PRINT_DEBUG(FOUND_POST);
            #endif
        
            cmd.OBJ.TYPE = TATU_POST;
            // Ignore the name of device
            for(j = 6; string[j] != ':'; j++);
            // Copy the entire JSON to the String variable
            strcpy(string, &string[++j]);
            return true;
        default:
            #ifdef DEBUG
            PRINT_DEBUG(REQUISITION_ERROR);
            #endif
            /* If the desired command is not found return error */
            return false;
    }

    if(IS_NUM(string[j])){
        // If is Analog
        #ifdef DEBUG
        PRINT_DEBUG(FOUND_NUM);
        #endif
        if(IS_ANALOG(string[j])){
            #ifdef DEBUG
            PRINT_DEBUG(FOUND_ANALOG);
            #endif
            j++;
            cmd.OBJ.VAR = TATU_TYPE_ANALOG;
        }
        else cmd.OBJ.VAR = TATU_TYPE_PIN;
        /* Divide os parametros caso haja mais que um */
        for(i = j; string[i] != ' ' && string[i] != 0; i++);
        string[i] = 0;
        cmd.OBJ.PIN = atoi_T(&string[j]);
        /* If it is a set */
        
    } 
    else if(IS_SYS(string[j])){ 
        #ifdef DEBUG
        PRINT_DEBUG(FOUND_SYSTEM);
        #endif
        j++; 
        cmd.OBJ.VAR = TATU_TYPE_SYSTEM;
    }

    VAR_COPY(j, length, string);
    str_hash = hash_djb(string);
    if(cmd.OBJ.TYPE != TATU_GET && cmd.OBJ.CODE == TATU_CODE_STATE)
        cmd.OBJ.STATE = string[strlen(string+1)] == 'T' ? 1:0;
        
    #ifdef DEBUG
    // Print the received command
    Serial.print(PARAM_VAR);
    Serial.println(string);
    if(cmd.OBJ.TYPE != TATU_GET){
        Serial.print(PARAM_VALUE);
        Serial.println(&string[strlen(string)+1]);
    }
    Serial.print(HASH_GENERATED);
    Serial.println(str_hash);
    #endif
    
    return true;
}
