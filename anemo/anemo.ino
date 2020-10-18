/**
 * @file pump.ino
 * @author Alejandro Arroyo
 * @author Sergio Solorzano
 * @author Juan J. Rojas 
 * @date 02 Oct 2020
 * @brief Main loop for pump controller
 * @par Institution:
 * SETECLab. Instituto Tecnologico de Costa Rica.
 * @par mail:
 * juan.rojas@tec.ac.cr
 * @par Git repository:
 * https://github.com/Setec-Lab/gst_eps
 */

// JR: VERY Important ADC2 pins cannot be used when the WiFi is being used, and some of the ADC1 pins are also blocked for some reason, 32 is ok...

#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "soc/rtc.h"

#define WLAN_SSID       "RojasMatamoros"
#define WLAN_PASS       "needuhuru"
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                  
#define AIO_USERNAME    "JUANJOROJASH"
#define AIO_KEY         "aio_WrBp37mZc3UAxXUr73RNh7SB9LQf"

uint32_t reel_rotations = 0;
bool reel_flag;
uint64_t current;
uint64_t previous;
uint64_t cycles_passed;
double ms_passed = 0;
double prev_ms_passed = 0;
double avg_time_ms = 0;
double curr_speed;
double prev_speed;
uint32_t period_in_us = rtc_clk_cal(RTC_CAL_RTC_MUX,10);  //ten cycle average
/* create a hardware timer */
hw_timer_t * timer = NULL;
bool timer_flag;
int counter = 0;
bool pub_flag = 0;


WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);       
Adafruit_MQTT_Publish anemo_data = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/anemo_test");

void IRAM_ATTR reel_isr() {
  if(counter == 0){
    previous = rtc_time_get();
    ms_passed = 0;
  }else{
    current = rtc_time_get();
    cycles_passed = current - previous;
    previous = current;
    prev_ms_passed = ms_passed;  
    ms_passed += (double) ((rtc_time_slowclk_to_us(cycles_passed,period_in_us))/1000.0);
  }
  counter++;
}

void IRAM_ATTR timer_isr(){
  timer_flag = 1;
}

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  pinMode(2, INPUT);
  attachInterrupt(2, &reel_isr, RISING);
  Serial.println("Start GPIO interrupt on PIN#2...");  
    /* Use 1st timer of 4 */
  /* 1 tick take 1/(80MHZ/80) = 1us so we set divider 80 and count up */
  timer = timerBegin(0, 80, true);
  /* Attach timer_isr function to our timer */
  timerAttachInterrupt(timer, &timer_isr, true);
  /* Set alarm to call onTimer function every second 1 tick is 1us
  => 1 second is 1000000us */
  /* Repeat the alarm (third parameter) */
  timerAlarmWrite(timer, 1000000, true);
  /* Start an alarm */
  timerAlarmEnable(timer);
  Serial.println("Start timer..");
}



void loop() {
  MQTT_connect(); 
  if (timer_flag){  //if one second has passed
    timer_flag = 0; //clear the flag 
    Serial.print("ms_passed: ");
    Serial.println(ms_passed);
    Serial.print("counter: ");
    Serial.println(counter);
      if (counter < 2){
        curr_speed = 0;   
      }else{
        avg_time_ms = ms_passed / counter;
        curr_speed = (1000/avg_time_ms)*2.5*0.44704;  
        ms_passed = ms_passed - prev_ms_passed; //keep only last interval  
        counter = 1;      
      }
    Serial.print("Speed: ");
    Serial.println(curr_speed);
    if((curr_speed != prev_speed)&(pub_flag)){
      if (! anemo_data.publish(curr_speed)) {
        Serial.println(F("Failed"));
      }else{
        Serial.println(F("OK!"));
      }
    }
    prev_speed = curr_speed;    
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
