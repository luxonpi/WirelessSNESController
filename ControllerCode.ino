#include <Arduino.h>
#include "GameControllers.h"
#include "WiFi.h"
#include <esp_now.h>

//shared pins between all controllers
const int LATCH_PIN = 17;
const int CLOCK_PIN = 18;
const int DATA_PIN_0 = 16;  

GameControllers controllers;
// 7C:9E:BD:4A:60:FC
uint8_t broadcastAddress[] = {0x7C, 0x9E, 0xBD, 0x4A, 0x60, 0xFC};

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
 // Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}


esp_now_peer_info_t peerInfo;


void setup(){

  Serial.begin(115200); //prepare serial for text output 
  Serial.print("SETUP");

  //initialize shared pins
  controllers.init(LATCH_PIN, CLOCK_PIN); 
  //activate first controller ans set the type to SNES
  controllers.setController(0, GameControllers::SNES, DATA_PIN_0);

   // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }


}

void loop()
{
  controllers.poll(); //read all controllers at once

  int code =controllers.GetCode(0);
  if(code!=0){
    //Serial.println(code);
  }

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &code, sizeof(int));
   
  if (result == ESP_OK) {
    //Serial.println("Sent with success");
  }
  else {
   // Serial.println("Error sending the data");
  }

  delay(20);  //read controller just ~20times a second

}