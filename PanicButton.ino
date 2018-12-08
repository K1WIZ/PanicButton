/*
 * WiFi Emergency Panic Button using ESP8266
 * 
 * John Rogers
 *
 */
 
#include <ESP8266WiFi.h>

// Replace with your SSID and Password
const char* ssid     = "MyNetwork";
const char* password = "P@ssword";

// Replace with your unique IFTTT URL resource
const char* resource = "/trigger/panic_button/with/key/someKey-324d23ec2cedwec";

// How your resource variable should look like, but with your own API KEY (that API KEY below is just an example):
//const char* resource = "/trigger/button_pressed/with/key/nAZjOphL3d-ZO4N3k64-1A7gTlNSrxMJdmqy";

// Maker Webhooks IFTTT
const char* server = "maker.ifttt.com";

void setup() {
  Serial.begin(115200); 
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);  // turn off LED
  initWifi();
  makeIFTTTRequest();

  // Deep sleep mode until RESET pin is connected to a LOW signal (pushbutton is pressed)
  ESP.deepSleep(0);
}

void loop() {
  // sleeping so wont get here
}

// Establish a Wi-Fi connection with your router
void initWifi() {
  Serial.print("Connecting to: "); 
  Serial.print(ssid);
  WiFi.begin(ssid, password);  

  int timeout = 10 * 4; // 10 seconds
  while(WiFi.status() != WL_CONNECTED  && (timeout-- > 0)) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");

  if(WiFi.status() != WL_CONNECTED) {
     Serial.println("Failed to connect, going back to sleep");
  }

  Serial.print("WiFi connected in: "); 
  Serial.print(millis());
  Serial.print(", IP address: "); 
  Serial.println(WiFi.localIP());
}

// Make an HTTP request to the IFTTT web service
void makeIFTTTRequest() {
  Serial.print("Connecting to "); 
  Serial.print(server);
  
  WiFiClient client;
  int retries = 5;
  while(!!!client.connect(server, 80) && (retries-- > 0)) {
    Serial.print(".");
  }
  Serial.println();
  if(!!!client.connected()) {
     Serial.println("Failed to connect, going back to sleep");
  }
  
  Serial.print("Request resource: "); 
  Serial.println(resource);
  client.print(String("GET ") + resource + 
                  " HTTP/1.1\r\n" +
                  "Host: " + server + "\r\n" + 
                  "Connection: close\r\n\r\n");
 
                  
  int timeout = 5 * 10; // 5 seconds             
  while(!!!client.available() && (timeout-- > 0)){
    delay(100);
  }
  if(!!!client.available()) {
     Serial.println("No response, going back to sleep");
  }
  while(client.available()){
    Serial.write(client.read());
  }
  
  Serial.println("\nclosing connection");
    // PROVIDE SOME VISUAL INDICATION MESSAGE WAS SENT BY LIGHTING LED
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED on to provide visual confirmation
    delay(10000);                       // wait for 10 seconds
    digitalWrite(LED_BUILTIN, HIGH);  // turn off LED
  client.stop();
}
