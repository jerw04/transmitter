#include <ESP8266WiFi.h>
#include <espnow.h>
#include <String.h>

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress1[] = {0xE0, 0x98, 0x06, 0x92, 0x3A, 0xF9};
uint8_t broadcastAddress2[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Structure example to send data
// Must match the receiver structure
typedef struct test_struct {
  uint8_t mac_addr[6];
} test_struct;

// Create a struct_message called test to store variables to be sent
test_struct test;

unsigned long lastTime = 0;  
unsigned long timerDelay = 2000;  // send readings timer

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  char macStr[18];
  Serial.print("Packet to:");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
         mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  // Set device as a Wi-Fi Station
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress1, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  esp_now_add_peer(broadcastAddress2, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

}
 
void loop() {
  if ((millis() - lastTime) > timerDelay) {
    // Set values to send
    test.mac_addr[0] = WiFi.macAddress()[0];
    test.mac_addr[1] = WiFi.macAddress()[1];
    test.mac_addr[2] = WiFi.macAddress()[2];
    test.mac_addr[3] = WiFi.macAddress()[3];
    test.mac_addr[4] = WiFi.macAddress()[4];
    test.mac_addr[5] = WiFi.macAddress()[5];

    // Send message via ESP-NOW
    esp_now_send(0, (uint8_t *) &test, sizeof(test));
    
    lastTime = millis();
  }
}