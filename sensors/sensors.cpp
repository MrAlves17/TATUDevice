#include "sensors.h"
#include <TATUDevice.h>

char *str_aux = "rapaz";

/*bool  luminosity_sensor(uint8_t PIN,int VAR,char *RESPONSE,uint8_t CODE){
  VAR = (analogRead(PIN) - 1023) * (-1); 
  switch(CODE){ 
    case TATU_CODE_INFO: 
      VAR = map (VAR,0,1023,0,100); 
      ITOS(VAR,RESPONSE); 
      aux = strlen((char*)RESPONSE); 
      ((char*)RESPONSE)[aux++] = '%'; 
      ((char*)RESPONSE)[aux] = 0; 
      break; 
    case TATU_CODE_VALUE: 
      ITOI(VAR,RESPONSE); 
      break; 
    default: 
      return false; 
  }
  return true;
}*/

bool aux_get(uint32_t hash,void* response,uint8_t code){
	/*#ifdef MQ2_SENSOR
	    Serial.println("GAS");
	#endif*/
	Serial.println("HASH");
	itoa(hash,str_aux,10);
	Serial.println(str_aux);
	switch(hash){
		/*#ifdef MQ2_SENSOR
	    case H_gas:
	      	//The gas_sensor supports INFO,VALUE and STATE requests.
	      	gas_sensor(MQ2_SENSOR,gas_amount,response,code);
	      	break;
	    #endif

	    #ifdef DHT_SENSOR
	    case H_temp:
          	//The dht_temperatures_sensor supports INFO and VALUE requests.
	        dht_temperature_sensor(dht,t,response,code);
	        break;
        case H_humid:
          	//The dht_humidity_sensor supports INFO and VALUE requests.
          	dht_humidity_sensor(dht,h,response,code);
          	break;
        #endif*/

		//#ifdef LDR_SENSOR_
       
        /*case H_luminosity:
        	Serial.println("luminosity");
          	//The lumisity_sensor supports INFO and VALUE,requests. 
          	return luminosity_sensor(LDR_SENSOR,luminosity,response,code);
          	break;*/
	    //#endif 

	    default:
	    	Serial.println("FALSE");
	     	return false;
	}
	return true; 
}