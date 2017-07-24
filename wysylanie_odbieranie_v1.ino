#include <ESP8266WiFi.h>

const char* ssid     = "router";
const char* password = "haslo123";

const char* host = "192.168.1.101";

String inData = "";
String finalString = "";
boolean boolReadSerial = true;
boolean boolSend = false;
void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  if(boolReadSerial == true && boolSend == false)
    serialRead();

  if(boolSend == true)
    sendTom(finalString);
  
}

void serialRead(){
  if(Serial.available() >0){   
      char inChar = Serial.read();      
      
      if(inChar == '\n'){
      Serial.println("i mamy stringa:  " + inData);
      inData.trim();
      finalString = inData;
      boolSend = true;   
      }     
      if(inChar != '\n' && inChar != '\r')
      inData += inChar;
  }
}

void sendTom(String val){
  boolReadSerial = false;
  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 8080;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = "/test/test";
  url += "?naz=podworko";
  url += "&stan=" + val;
  url += "&kto=przechowalnia";

  Serial.print("Requesting URL: ");
  Serial.println(url);
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 500) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      inData = "";
    finalString = "";
    boolReadSerial = true;
    boolSend = false;
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
    client.stop();
    inData = "";
    finalString = "";
    boolReadSerial = true;
    boolSend = false;
  }
  

  Serial.println();
  Serial.println("closing connection");
  inData = "";
  finalString = "";
  boolReadSerial = true;
  boolSend = false;
}

