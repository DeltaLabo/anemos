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
#define AIO_KEY         "aio_CBBh14Rm8hA3L30YDKEurb44vl7E"

uint32_t reel_rotations = 0;
bool reel_flag;
uint64_t current;

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);       
Adafruit_MQTT_Publish anemo_data = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/anemo_test");
Adafruit_MQTT_Subscribe timefeed = Adafruit_MQTT_Subscribe(&mqtt, "time/seconds");

void IRAM_ATTR reel_isr() {
  reel_flag = 1;
  current = rtc_time_get();
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
  attachInterrupt(2, reel_isr, RISING);
}

uint64_t previous;
uint64_t cycles_passed;
double ms_passed = 0;
 //period_in_us;
int counter;
double anemo_speed;
uint32_t period_in_us = rtc_clk_cal(RTC_CAL_RTC_MUX,10);  //ten cycle average

void loop() {
  MQTT_connect(); 
  if (reel_flag){
    counter++;
    reel_flag = 0;
    cycles_passed = current - previous;
    previous = current;   
    ms_passed += (double) ((rtc_time_slowclk_to_us(cycles_passed,period_in_us))/1000.0);
  }
  if (counter == 10){
    ms_passed = ms_passed / counter;
    anemo_speed = (1000/ms_passed)*2.5*0.44704;  
    ms_passed = 0;
    counter = 0;
    Serial.print("Speed: ");
    Serial.println(anemo_speed);
    if (! anemo_data.publish(anemo_speed)) {
      Serial.println(F("Failed"));
    }else{
      Serial.println(F("OK!"));
    }    
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
