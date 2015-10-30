#include <stdint.h>
#include "Arduino.h"

#define gas_sensor(PIN,VAR,RESPONSE,CODE) \
		VAR = analogRead(PIN); \
        VAR = map (VAR,0,1023,0,100); \
        switch(CODE){ \
          case TATU_CODE_INFO: \
            ITOS(VAR,RESPONSE); \
            aux = strlen((char*)RESPONSE); \
            ((char*)RESPONSE)[aux++] = '%'; \
            ((char*)RESPONSE)[aux] = 0; \
            break; \
          case TATU_CODE_VALUE: \
            ITOI(VAR,RESPONSE); \
            break; \
          case TATU_CODE_STATE: \
            if (VAR > 55) BTOB(true,RESPONSE); \
            else BTOB(false,RESPONSE); \
            break; \
        }

#define luminosity_sensor(PIN,VAR,RESPONSE,CODE)  \
        VAR = (analogRead(PIN) - 1023) * (-1); \
        switch(CODE){ \
          case TATU_CODE_INFO: \
            VAR = map (VAR,0,1023,0,100); \
            ITOS(VAR,RESPONSE); \
            aux = strlen((char*)RESPONSE); \
            ((char*)RESPONSE)[aux++] = '%'; \
            ((char*)RESPONSE)[aux] = 0; \
            break; \
          case TATU_CODE_VALUE: \
            ITOI(VAR,RESPONSE); \
            break; \
          default: \
            return false; \
        } 

#define dht_temperature_sensor(DHT,VAR,RESPONSE,CODE) \
        VAR = (int)DHT.readTemperature(); \
        switch(CODE){ \ 
          case TATU_CODE_INFO: \
            ITOS(VAR,RESPONSE); \
            break; \
          case TATU_CODE_VALUE: \
            ITOI(VAR,RESPONSE); \
            break; \
          default: \
            return false; \
        } 
#define dht_humidity_sensor(DHT,VAR,RESPONSE,CODE) \
        VAR = (int)DHT.readHumidity(); \
        switch(CODE){ \ 
          case TATU_CODE_INFO: \
            ITOS(VAR,RESPONSE); \
            break; \
          case TATU_CODE_VALUE: \
            ITOI(VAR,RESPONSE); \
            break; \
          default: \
            return false; \
        } 
#define IV_analog_sensor(PIN,VAR,RESPONSE,CODE) \
        VAR = analogRead(PIN); \
        switch(CODE){ \ 
          case TATU_CODE_INFO: \
            ITOS(VAR,RESPONSE); \
            break; \
          case TATU_CODE_VALUE: \
            ITOI(VAR,RESPONSE); \
            break; \
          default: \
            return false; \
        }
#define IV_sensor(VAR,RESPONSE,CODE) \
        switch(CODE){ \ 
          case TATU_CODE_INFO: \
            ITOS(VAR,RESPONSE); \
            break; \
          case TATU_CODE_VALUE: \
            ITOI(VAR,RESPONSE); \
            break; \
          default: \
            return false; \
        } 
