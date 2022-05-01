int buf1[buf1_size];
int buf1_i = 0;
int current_sensor_midpoint = 512;
long int current_rms_mA = 0;


void setup_sensor(){
  unsigned long now = millis();
  unsigned long setup_start_ticks = now;
  unsigned long last_conversion_ticks;
  Serial.println("Callibrating sensor...");
  
  while( now - setup_start_ticks < 1000){
    now = millis();
    if(now != last_conversion_ticks){ //Each ms do a reading.
     last_conversion_ticks = now;
      buf1[(buf1_i++)%buf1_size] = analogRead(CURRENT_SENSOR_PIN);
    } 
  }
  
  current_sensor_midpoint = mean(buf1, buf1_size);
  Serial.print("End. Signal midpoint = ");
  Serial.println(current_sensor_midpoint);
    
}

void sensor_loop(){
  static unsigned long last_conversion_ticks = 0;
  unsigned long now = millis();
  
  if(now - last_conversion_ticks > Ts){ //Each ms do a reading.
   last_conversion_ticks = now;
   buf1[buf1_i] = (analogRead(CURRENT_SENSOR_PIN) - current_sensor_midpoint)*mA_per_count;

    if(++buf1_i >= Avg_n){
      buf1_i = 0;
      current_rms_mA = rms(buf1, Avg_n);
      power_avg_W = current_rms_mA*voltage_rms_V/1000;
      aenergy_W_s += power_avg_W*Avg_n*Ts/1000;
    }
    
  }
   
}


int rms(int buf[], int n){
  unsigned long long int val = 0;
  for (int i = 0; i < n; i++){
    val += pow(buf[i], 2);
  }
  return sqrt(val)/sqrt(n);
}

int mean(int buf[], unsigned int n){
  long long int val = 0;
  for (int i = 0; i < n; i++){
    val += buf[i];
  }
  return val/n;
}
