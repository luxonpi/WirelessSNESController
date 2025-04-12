#include <esp_now.h>
#include <WiFi.h>

// Q8 DATA OUT Green
// Clock Purple
// Latch Red

int code=0;

int DATAPINS[12] = {26,25,23,22,21,19,18,17,16,15,14,13};

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

  memcpy(&code, incomingData, sizeof(int));
  //Serial.println(code);
  writeToShiftRegister();

}

void writeToShiftRegister(){

  for(int k=0; k<12; k++){
    int mask =  1 << k;
    int masked_n = code & mask;
    int thebit = masked_n >> k;

    if(thebit==1){

      digitalWrite(DATAPINS[k], LOW);


    }else{

      digitalWrite(DATAPINS[k], HIGH);

    }

  }




}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  Serial.println("Setup");

  for(int i=0; i<12; i++){

    pinMode(DATAPINS[i], OUTPUT);

  }
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {

}