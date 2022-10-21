
// JR: VERY Important ADC2 pins cannot be used when the WiFi is being used, and some of the ADC1 pins are also blocked for some reason, 32 is ok...

#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
// #include "soc/rtc.h" No usado

#define WLAN_SSID       "ssid"
#define WLAN_PASS       "pass"
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                  
#define AIO_USERNAME    "username"
#define AIO_KEY         "key"
#define PUB_INTERVAL    2

unsigned long currentmicros;
unsigned long previousmicros = micros();
unsigned long zerointerval = previousmicros;
unsigned long interval;
unsigned long debounce = 35000;
double avg_period;
long period = 1000000;

uint32_t reed_rotations = 0;
bool reed_flag;
// uint64_t current;
// uint64_t previous = rtc_time_get();
// uint64_t cycles_passed;
// double curr_interval = 0;
// double prev_interval = 0;
double avg_time_ms = 0;
double anemo_speed = 0;
double pub_speed = 0;
double prev_speed = 0;
/* uint32_t period_in_us = rtc_clk_cal(RTC_CAL_RTC_MUX,10);  //ten cycle average
/* create a hardware timer */
/* hw_timer_t * timer = NULL; */
bool timer_flag;
int counter = 0;
bool pub_flag = 1;
bool print_flag = 1;
int pub_secs = 0; // in seconds
int result = 0;
char the_buffer[20] = {'\0'};
char received_char = '\0';

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);       
Adafruit_MQTT_Publish anemo_data = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/anemo_test");

void IRAM_ATTR reed_isr() {
  reed_flag = 1;
}

void IRAM_ATTR timer_isr(){
  timer_flag = 1;
}

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(2, INPUT);
  attachInterrupt(2, &reed_isr, RISING);
  //Serial.println("Start GPIO interrupt on PIN#2...");  
    /* Use 1st timer of 4 */
  /* 1 tick take 1/(80MHZ/80) = 1us so we set divider 80 and count up */
  /** timer = timerBegin(0, 80, true); **/
  /* Attach timer_isr function to our timer */
  /** timerAttachInterrupt(timer, &timer_isr, true);  **/
  /* Set alarm to call onTimer function every second 1 tick is 1us
  => 1 second is 1000000us */
  /* Repeat the alarm (third parameter) */
  /** timerAlarmWrite(timer, 1000000, true); **/
  /* Start an alarm */
  /** timerAlarmEnable(timer); **/
  //Serial.println("Start timer..");
  menu();
}

char telemetry[15] = {'\0'};

void loop() {
  if (Serial.available()) received_char = Serial.read();
  if (received_char == 0x1B) menu();
  if(reed_flag){
    reed_flag = 0;
    if (micros() - previousmicros > debounce){
      counter++;   
      currentmicros = micros();
      interval += currentmicros - previousmicros;
      previousmicros = currentmicros;
    }
    // current = rtc_time_get(); //get current RTC time cycles
    // cycles_passed = current - previous; //calculate how many cycles have passed since previous reed switch closing   
    // prev_interval = curr_interval; //save previous interval in case the 1sec interruption ocurrs
    // curr_interval += (double) ((rtc_time_slowclk_to_us(cycles_passed,period_in_us))/1000.0);  //calculate current interval based on cycle period of rtc     
    // previous = current; //save previous cycles for next interruption    
  }  
  if (micros() - zerointerval >= period){  //if one second has passed
    zerointerval = micros();
      if (counter == 0){
        anemo_speed = 0;
      }else{
        if (counter == 1 & interval > 2500000){ //below 2500cycles, ie below 0.45m/s report zero
          interval = 0; 
          anemo_speed = 0;       
        }else{
          avg_period = (interval / counter)*0.001;
          anemo_speed = (1000/avg_period)*2.5*0.44704;
          interval = 0;
          //avg_time_ms = curr_interval / counter;  //take the average of the overall interval between the rotations
          //anemo_speed = (1000/avg_time_ms)*2.5*0.44704;  //calculate speed according to the manufacturer
          //curr_interval = curr_interval - prev_interval;  //set the interval to the time between last rotation and the one second trigger         
        }        
        counter = 0;  //clear the counter to restart the speed calculation    
      }
    sprintf(telemetry, "S%05.2fN", anemo_speed); //create telemetry string before publising in MTTQ 
    /*the string has the S as firsts character, then 5 spaces for the velocity including two before the dot and two after the dot, then
    * the status of the MTTQ transmission 
    */   
    strcpy(the_buffer, "00.00");
    if(pub_flag){
      pub_secs++;
      pub_speed += anemo_speed;
      if (pub_secs == PUB_INTERVAL){ //only publish according to the preset interval
        pub_speed /= pub_secs;
        pub_secs = 0;       
        MQTT_connect();
        if(pub_speed != prev_speed){ //only publish if the value is different from the previous
          if (! anemo_data.publish(pub_speed)) {
            telemetry[6] = 'E';
            strcpy(the_buffer, "00.00");
          }else{
            telemetry[6] = 'S';
            sprintf(the_buffer, "%05.2f", pub_speed);
          }
          prev_speed = pub_speed;
          pub_speed = 0;
        }
      }else{
        strcpy(the_buffer, "00.00");
      }
    }    
    strcat(telemetry, the_buffer);
    telemetry[12] = '\r';
    telemetry[13] = '\n';
    if (print_flag) Serial.print(telemetry); 
  }
}

void MQTT_connect(){
  int8_t ret;
  if (mqtt.connected()){
    return;
  }
  Serial.print("Connecting to MQTT...");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0){
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);
    retries--;
    if (retries == 0){
      while (1);
    }    
  }
  Serial.println("MQTT Connected!");
}

void menu(){
  Serial.setTimeout(10000);
  Serial.println("Do you want to publish on Adafruit.io (y/n)?");
  while (!(received_char == 0x79 | received_char == 0x6E)){
    if (Serial.available()) received_char = Serial.read();
  }
  if (received_char == 0x79){ //if a "y" is received
    pub_flag = 1;
    Serial.print("Publish interval: ");
    Serial.print(PUB_INTERVAL);
    Serial.println(" seconds");
    Serial.print("Connecting to... ");
    Serial.println(WLAN_SSID);
    WiFi.begin(WLAN_SSID, WLAN_PASS);  
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println();
    Serial.println("WiFi connected");
    Serial.println("IP address: "); 
    Serial.println(WiFi.localIP());  
  }else{
    pub_flag = 0;
  }
}
