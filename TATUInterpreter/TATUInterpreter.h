#ifndef TATUInterpreter_h
#define TATUInterpreter_h

#ifdef DEBUG
#define DEBUG 1
#endif

#include <stdint.h>
#include <avr/pgmspace.h>

// System definitions
#define PROGMEM __ATTR_PROGMEM__
#define putstring(x) SerialPrint_P(PSTR(x))
#define PRINT_DEBUG(MSG) putstring(MSG)

// TATU Protocol available commands
#define TATU_POST   0
#define TATU_SET    1
#define TATU_GET    2
#define TATU_EDIT   3

// TATU Protocol available properties
#define TATU_CODE_ALL   0
#define TATU_CODE_INFO  1
#define TATU_CODE_VALUE 2
#define TATU_CODE_STATE 3

// TATU Protocol avaliable type variables
#define TATU_TYPE_ALIAS  0
#define TATU_TYPE_PIN    1
#define TATU_TYPE_SYSTEM 2
//#define TATU_TYPE_CRYPTO 3
#define TATU_TYPE_ANALOG 3

// Char that represents the TATU Protocol commands
#define CODE_ALL   'A'
#define CODE_INFO  'I'
#define CODE_VALUE 'V'
#define CODE_STATE 'S'

// Char that represents the TATU Protocol properties
#define COMMAND_POST 'P'
#define COMMAND_SET  'S'
#define COMMAND_GET  'G'
#define COMMAND_EDIT 'E'

// DJB Hash macro(deprecated)
//#define HASH_DJB(START, LEN, INPUT, OUTPUT) for(i = START; INPUT[i] != ' '; i++){ OUTPUT = ((OUTPUT << 5) + OUTPUT) + INPUT[i]; }
// Copy property to begin of the payload array
#define VAR_COPY(START, LEN, VECTOR) for(i = START; i < LEN; i++){ VECTOR[i-START] = VECTOR[i]; if(i+1 == LEN) VECTOR[i-START+1] = '\0'; else if(VECTOR[i] == ' ') VECTOR[i-START] = 0; }
// Verifica se uma variavel é um numero
#define IS_NUM(VAR) ((VAR < 58)&&(VAR > 47))||(VAR == '#')
#define IS_SYS(VAR) (VAR == '$')
// Deprecated!
//#define IS_CRYPTO(VAR) (VAR == '#')
#define IS_ANALOG(VAR) (VAR == '#')

/* Utilities */
uint32_t hash_djb(char *string);
uint8_t atoi_T(char *p);
void SerialPrint_PROGMEM(PGM_P str);

class TATUInterpreter{
private:
    typedef union {
        struct {
            uint8_t ERROR : 1;
            uint8_t STATE : 1;
            uint8_t CODE  : 2;
            uint8_t TYPE  : 2;
            uint8_t VAR   : 2;
            uint8_t PIN      ;
        } OBJ;
        uint16_t STRUCTURE;
    } Command;
public:
    Command cmd;
    uint32_t str_hash;
    TATUInterpreter(){ cmd.OBJ.ERROR = true; }
    bool parse(char *, unsigned int);
};

#endif
