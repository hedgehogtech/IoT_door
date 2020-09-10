#include <ESP8266WiFi.h>
//#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WLAN_SSID       "" //add wifi ssid
#define WLAN_PASS       "" //add wifi password

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    ""                    //add adafruit IO user id
#define AIO_KEY         ""                    //add adafruit IO key

WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish door = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/door");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe door = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/door");

void MQTT_connect();

const int buttonPin = 4;  // the number of the reed sensor to be connected to
int buttonState = 0;
bool dooropen = true;
bool hold = true;
int count = 0;


void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi access point.
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
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&door);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected). 
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &door) {
      Serial.print(F("Got: "));
      Serial.println((char *)door.lastread);
    }
  }

 
  //  Serial.print(F("\nSending value "));
  //  if (! door.publish(count++)) {
  //    Serial.println(F("Failed"));
  //  } else {
  //    Serial.println(F("OK!"));
  //  }

  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  
  //input starts
  dooropen = digitalRead(buttonPin);
  
  //Serial.println(dooropen);
//  if (dooropen == HIGH) {
//    // turn LED on
//    Serial.println("door is open");
//  } 
//  
//  else {
//    Serial.println("door is closed");
//  }

  if(dooropen!=hold)
  {
    Serial.println(count);
    
    //Serial.println("door state changed");
    if (dooropen==true){
       Serial.println("Door Opened");
         Serial.print(F("\nSending door opened "));
          Serial.print(1);
         Serial.print("..."); // you can change the 1 to door open , depends if you are using ifttt or direct messages
            if (! door.publish("1")) {
              Serial.println(F("Failed"));
              MQTT_connect();
            } else {
              Serial.println(F("OK!"));
            }
     
      }
    
    if (dooropen==false)
    {
          Serial.println("Door Closed");
          //Serial.print(F("\nSending door info "));
          //Serial.print("...");
          //if (! door.publish("0")) {
          //  Serial.println(F("Failed"));
          //  MQTT_connect();
          //} else {
          //  Serial.println(F("OK!"));
          //}
      
      }
    
    delay(500);
    //count++;
    }
  hold = dooropen;
}

// Function to connect and reconnect as necessary to the MQTT server.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
