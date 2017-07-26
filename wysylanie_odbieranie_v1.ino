#include <ESP8266WiFi.h>

const char* ssid     = "router";
const char* password = "haslo123";

const char* host = "192.168.1.101";
WiFiServer server(80);

String inData = "";
String inDataState = "";
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

  server.begin();
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  fromWiFi();
  
  if(boolReadSerial == true && boolSend == false)
    serialRead();

  if(boolSend == true)
    sendTom(inDataState, finalString);

  
}

void fromWiFi(){
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data

  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  //client.stop();
  
  client.flush();

  client.print("koniec");
  delay(1);
}

int i = 1;

void serialRead(){
  if(Serial.available() >0){   
      char inChar = Serial.read();   

      if(inChar == '\n'){
        Serial.println("i mamy stringa:  " + inData);
        inData.trim();
        finalString = String(inData[0]) + String(inData[1]) + String(inData[2]);
        inDataState = inData[4];
        Serial.println("nasz final: " + finalString);
        Serial.println("nasz stan: " + inDataState);
        boolSend = true;   
      }     
      if(inChar != '\n' && inChar != '\r')
        inData += inChar;
        
 
       
      
//      if(inChar == '\n'){
//      Serial.println("i mamy stringa:  " + inData);
//      inData.trim();
//      finalString = inData;
//      boolSend = true;   
//      }     
//      if(inChar != '\n' && inChar != '\r' && inChar != ',' && i < 3)
//      
//      
//      if(i == 4){
//        inDataState = inChar;
//        Serial.println("mamy cos: " + inDataState);
//        i = 0;        
//      }
//      if(i < 4)
//      i++;         
//      }    
}
}

void sendTom(String val, String valWhere){
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
  url += "?naz=" + valWhere;
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
  i = 1;
}

