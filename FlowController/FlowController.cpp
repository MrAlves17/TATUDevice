#include "FlowController.h"

FlowController::FlowController(TATUDevice* aux_device, char* aux_response){
	device = aux_device;
	vector_response = aux_response;
	flow_buffer.end = flow_buffer.vector;
}

//find the next string in a array composed by strings
int FlowController::nextStr(char* str){
	int i = 0;
	while(str[i++])
	return i;
}

//increment a iterator and return the previous
void* FlowController::vector_iterator(FlowList unit) {
	if (unit->type == STR_T){
		//Jumps to the next iterator according to the size of the next string
		unit->iterator += sizeof(char)*nextStr((char*)unit->iterator);
	}
	//Jumps the iterator according to the type size
	else unit->iterator += unit->t_size;
	return unit->iterator - unit->t_size;
}

void* FlowController::iterator_reset(FlowList unit){
	unit->iterator = unit->vector;
}

void FlowController::loop() {
	FlowList unit = activity;
	while (unit) {
		if (!unit->used) break;
		//"Collect" timeout
		if ((millis() / unit->collect_freq) >= unit->lastTimeCollect) {
		  unit->lastTimeCollect++;
		  requisition(vector_iterator(unit), unit->att);
		}
		//"Shipping" timeout
		if ((millis() / unit->publish_freq) >= unit->lastTimePub) {
		  unit->lastTimePub++;
		  flow_publish(unit);
		  unit->iterator = 0;
		}
	unit = unit->next;
	}

}
void FlowController::flow_pub(FlowList unit){
	buildResponse((int*)unit->vector,unit->size);
	pubResponse(unit);
}

//Who collects the samples(void*)
void FlowController::requisition(void* response, uint32_t hash) {
	DEBUG_PORT.begin(115200);
	uint8_t code;
	code = INT_T;
	device->get_function(hash, response, code);
	PRINTLN(*(int*)response);
}
//#define nextStr(STR,COUNT) while(STR[COUNT]++)
void* FlowController::vector_acess(FlowList unit, int i) {

	if (unit->type == STR_T){
		int j,k;
		char* str = (char*)unit->vector;
		for (j = 0; j < i; ++j){
			//nextStr(str,k);
		}
	}

	return ((unit->vector) + (unit->t_size * i));
	
}

//responseBuilder
void FlowController::buildResponse(int* arr,int length) {
	
	uint8_t aux;//char response Iterator
	uint8_t i;//int* arr Iterator

	// Writes the response on the flowController buffer 
	char* response = vector_response;
	response[0] = '\0';

	//<dataManipulation>
	// Array values construction
	for (i = 0; i < length; i++){
		//<block> insert comma
		aux = strlen(response);
		response[aux] = ',';
		response[++aux] = '\0';
		//</block>

		// Insert integer value
		strcpy(&response[aux],response);		
	}

	//<closeData> Encapsula a resposta nos colchetes
	response[0] = '[';
	aux = strlen(response);
	response[aux] = ']';
	response[++aux] = '\0';
	//</closeData>
	//</dataManipulation>
}
// response "Shipping"
void FlowController::pubResponse(FlowList unit){
	/*
		req = "SET INFO flow"
		the objective is to call the device->callback function internally
	*/
	byte req[20];
	strcpy_P((char*)req, (const char*)unit->message);
	device->mqtt_callback("", req, strlen((char*)req) );	

}

void FlowController::push_value(char* response, uint8_t type, void* iterator) {
	switch (type) {
		case STR_T:
			//strcpy((*(int*)vector_acess(unit, i)));
		  	strcpy((char*)iterator,response);
		  	// do something
		  break;
		case INT_T:
		  	itoa(*(int*)(iterator), response, 10);
		  	// do something
		  	break;
		case BOOL_T:
		  	// do something
		  	break;
		default:
		  	itoa(*(int*)(iterator), response, 10);
		  return;
		  // do something
	}
}

//May be build in future
//void FlowController::push_response(char* response,){}
//Who sends
void FlowController::flow_publish(FlowList unit) {
	uint8_t i, aux;
	char* response = vector_response;
	void* iterator;

	response[0] = '\0';

	//Describes the flow unit
	#ifdef FLOW_DEBUG
		PRINT("Unit: ");
		PRINT("collect :"); PRINTLN(unit->collect_freq);
		PRINT("publish :"); PRINTLN(unit->publish_freq);
		PRINT("Size : ");
		PRINTLN(unit->size);
	#endif

	for (unit->iterator = unit->vector; unit->iterator != unit->vector_end; vector_iterator(unit)) {
		//adiciona uma virgula a response
		aux = strlen(response);
		response[aux] = ',';
		response[++aux] = '\0';
		//PRINT("Pushing : ");
		//PRINTLN((*(int*)vector_acess(unit, unit->iterator)));

		//Dinamic array builder!
		push_value(&response[aux], unit->type, unit->iterator);
	}

	//encapsula a resposta nos colchetes
	response[0] = '[';
	aux = strlen(response);
	response[aux] = ']';
	response[++aux] = '\0';

	byte req[20];
	strcpy_P((char*)req, (const char*)unit->message);

	device->mqtt_callback("", req, strlen((char*)req) );
	PRINTLN(response);
}

void FlowController::flow_construct(uint32_t hash, int collect_freq, void* message, int publish_freq, uint8_t code, uint8_t type, void* vector, uint32_t flow, FlowList unit) {
	//This function constructs the flow unit
	DEBUG_PORT.begin(115200);
	#ifdef FLOW_DEBUG
		PRINTLN("Entrou!");
	#endif
	unit->iterator = 0;
	unit->collect_freq = collect_freq;
	unit->publish_freq = publish_freq;
	unit->lastTimeCollect = millis() / collect_freq;
	unit->lastTimePub = millis() / publish_freq;
	unit->att = hash;
	unit->vector = vector;
	unit->t_size = type;
	unit->flow = flow;
	unit->message = message;
	unit->used = true;
	unit->t_size = sizeof(int);

	uint8_t size = unit->publish_freq / unit->collect_freq;
	unit->size = size;
	//unit->used = true;
	#ifdef FLOW_DEBUG
		PRINTLN("Construiu!");
	#endif
}
void FlowController::buffer_alloc(FlowList unit) {
	//Allocate space on buffer according to the number of samples
	unit->vector = flow_buffer.end;
	flow_buffer.end = ((unit->vector) + (unit->t_size * unit->size - 1));

	#ifdef FLOW_DEBUG
		PRINTLN("Alocou!");
	#endif

}

void FlowController::flowbuilder(char* json, uint32_t hash, uint8_t code) {

	DEBUG_PORT.begin(115200);

	const int BUFFER_SIZE = JSON_OBJECT_SIZE(3);//needed to determine jsonbuffer size(abstarct it)
	StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(json);

	#ifdef FLOW_DEBUG
		PRINT("JSON: ");
		PRINTLN(json);
		PRINT("collect :"); PRINTLN((int)root["collect"]);
		PRINT("publish :"); PRINTLN((int)root["publish"]);
		PRINT("turn :"); PRINTLN((int)root["turn"]);
	#endif
	void* vector;
	FlowList unit = activity;

	//if turn is '1' stop the attribute flow
	if (root["turn"] == 1) {
		while (unit->att != hash) {
		  	unit = unit->next;
		  	// statement
		}
		unit->used = false;
		return;
	}

	PRINTLN("Checando");
	//Seek for unused flow unit
	while (unit->used) {
		unit = unit->next;
	}

	//set the type
	uint8_t type = code;

	//construct the flow unit
	PRINTLN("Construindo!");
	vector = flow_buffer.end;
	flow_construct(hash, root["collect"], (void*)get_flow,
	             root["publish"], TATU_GET, type, vector, H_flow, unit);


	//Allocate space on buffer according to the number of samples
	//int *p = new (buffer_alloc) int[unit->size];
	//unit->vector = p;
	buffer_alloc(unit);
}
/*void FlowController::push_method_char(char* vector,char* x){
	char pos;
	pos = vector[0];
	strcpy(&vector[pos],x);
	vector[0] += strlen(x)+1;
}
char* FlowController::acess_method_char(char* vector,int length){
	int i = 0;
	while(vector[i++])
	return &vector[i];
}*/
