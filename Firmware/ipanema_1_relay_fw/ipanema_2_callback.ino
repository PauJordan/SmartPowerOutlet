#include <string> 

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  //Missatge MQTT ha arribat, compara topic.
  if(strcmp(topic, "timestamp") == 0){
    ipanema_timestamp(payload, length);
  }
  else if(strcmp(topic, "relay") == 0){
    ipanema_relay(payload, length);
  }
  else{
  
  }
    
  
 


}
/*
 * @brief parses incoming timestamp messages and updates variables.
 */
void ipanema_timestamp(byte* payload, unsigned int length){

  uint64_t val = 0;
  for (int i = 0; i < length; i++) {
    char ch = (char) *(payload+i);
    if (not isdigit(ch)) return;
    val = 10 * val + (ch - '0');
  }

  timestamp = val - 3600*1000*24;

  Serial.print("Sync message recieved, timestamp=");
  Serial.println(timestamp);  
}


/*
 * @brief parses incoming relay messages and updates variables.
 */
void ipanema_relay(byte* payload, unsigned int length){

  payload[length] = '\0';

  if(strcmp((char *) payload, "on") == 0){
    relay_switch(1);
    Serial.println("Relay ON");  
  }
  else if (strcmp((char *) payload, "off") == 0){
    relay_switch(0);
    Serial.println("Relay OFF");  
  }
  else{
    Serial.print("parse error: ");
    Serial.println((char*) payload);
  }

}



/*

void ipanema_timestamp(byte* payload, unsigned int length){

  String json_string = (char *) payload;

  int pos = json_string.indexOf("\"timestamp\"");
  pos = json_string.indexOf(":", pos);

  int timestamp = atoi((char *) &payload[pos]);

  pos = json_string.indexOf("\"value\"", pos);
  pos = json_string.indexOf(":", pos);

  int aenergy = atoi((char *) &payload[pos]);

  Serial.print("Sync message recieved, timestamp=");
  Serial.print(timestamp);
  Serial.print(" aenergy=");
  Serial.println(aenergy);
  
  
}

*/
