#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
const char *ssid = "AirMac Time Capsule"; // Enter your WiFi name
const char *password = "xiaopenbaka233";  // Enter WiFi password
const char *mqtt_broker = "192.168.1.134";
const int mqtt_port = 1883;

char chaValues[150];
//char rainValues[20];
String SValues;
String Tmsg;
String Pmsg;
String Amsg;
String Hmsg;
String Rmsg;
//String rain="is raining!";
//String norain="is not raining!";


// Create the sensor instance
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C


WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // Set software serial baud to 115200;
  Serial.begin(115200);
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("Connecting to public emqx mqtt broker.....");
    if (client.connect("esp8266-client")) {
      Serial.println("Public emqx mqtt broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  //connecting to bme280 
  Serial.println(F("BME280 test"));
  bool status;
  status = bme.begin(0x76);
  if (!status)
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1)
      ;
    Serial.println("-- Default Test --");

    Serial.println();
  }
  //water sensor set
  pinMode(D5,INPUT);
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}

void printValues() {
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");
    
    Serial.print("Pressure = ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
}
void seedValues(){
  int n=digitalRead(D5);
  Tmsg="\"Temperature\":"+String(bme.readTemperature())+",";
  Pmsg="\"Pressure\":"+String(bme.readPressure() / 100.0F)+",";
  Amsg="\"Approx.Altitude\":"+String(bme.readAltitude(SEALEVELPRESSURE_HPA))+",";
  Hmsg="\"Humidity\":"+String(bme.readHumidity())+",";
  if(n==LOW){
    Rmsg="\"rain\":1";
  }else Rmsg="\"rain\":0";
  SValues="{"+Tmsg+Pmsg+Amsg+Hmsg+Rmsg+"}";
  //SValues=Tmsg+Pmsg;
}
void loop() {
  client.loop();
  //printValues();
  seedValues();
  SValues.toCharArray(chaValues,150);
  client.publish("esp8266/test",chaValues, true);
  client.subscribe("esp8266/test");
  /*int n=digitalRead(D5);
  if(n==LOW)
  {
    //Serial.println("is raining");
    rain.toCharArray(rainValues,20);
    client.publish("esp8266/rain",rainValues,true);
    client.subscribe("esp8266/rain");
  }
  else{
    norain.toCharArray(rainValues,20);
    client.publish("esp8266/rain",rainValues,true);
    client.subscribe("esp8266/rain");
  }*/
  delay(5000);
}
