#include <ESP8266WiFi.h>
#include <TimeLib.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char *ssid = "*****";
const char *password = "*****";
MDNSResponder mdns;
ESP8266WebServer server(80);

String webPage = "",webPage1 = "";

void turn_off(){
  Serial.println("turning off!!!!!!");
  digitalWrite(12, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(1000);   
  digitalWrite(13, LOW);  // Turn the LED off by making the voltage HIGH
  delay(500);   
  digitalWrite(13, HIGH);  // Turn the LED off by making the voltage HIGH
  
}

void turn_on(){
  Serial.println("turning on!!!!!!");
  digitalWrite(13, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(1000);   
  digitalWrite(12, LOW);  // Turn the LED off by making the voltage HIGH
  delay(500);   
  digitalWrite(12, HIGH);  // Turn the LED off by making the voltage HIGH
  
}


void setup() {
  webPage += "<h1>ESP8266 Web Server</h1><p>Socket #1 <a href=\"socket1On\"><button>ON</button></a>&nbsp;<a href=\"socket1Off\"><button>OFF</button></a></p>";
  webPage += "<p>Socket #2 <a href=\"socket2On\"><button>ON</button></a>&nbsp;<a href=\"socket2Off\"><button>OFF</button></a></p>";


  pinMode(LED_BUILTIN, OUTPUT);
    pinMode(12, OUTPUT);     
  pinMode(13, OUTPUT);
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);

  Serial.begin(115200); 
  Serial.println(); 
  initWifi();
  
}
int day_, year_, hour_, minute_, second_; 
String month_, str("Time until watering: "), res;
int target_hour = 22, target_minutes = 59, duration = 5;
void handle_timers(){
  if (hour() == target_hour && minute() == target_minutes){
   turn_on();
   for(int m = 0; m < duration; m++)
    for(int s = 0; s < 60; s++){
      Serial.print(m);
      Serial.print("min ");     
      Serial.print(s);
      Serial.println("sec");
      delay(1000);
    }
   turn_off();
  }
  else{
    int tt = target_hour*60 + target_minutes;
    int cur = hour()*60 + minute();
    int w = (tt - cur);
    w = (w+24*60)%(24*60);
    res = str + w/60 + "hours " + w%60 + "minutes...";
    Serial.println(res);
//    Serial.print("Time until watering: ");
//    Serial.print((target_hour - hour())%24);
//    Serial.print("hours ");  
//    Serial.print((target_minutes - minute())%60);
//    Serial.println("minutes...");
  }
}

//void handle_timers(){
//  if (second() < 5){
//   turn_on();
//   delay(5000);
//   turn_off();
//  }
//}

void loop() {
  delay(500);
  digitalWrite(LED_BUILTIN, LOW); 
  if (year_ < 2017){
    Serial.println("Quarying time...");
    Serial.println(getTime());
  }
  else{
    digitalClockDisplay();
  }
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);

  handle_timers();

  server.handleClient();
}

String getTime() {
  WiFiClient client;
  while (!!!client.connect("google.com", 80)) {
    Serial.println("connection failed, retrying...");
  }

  client.print("HEAD / HTTP/1.1\r\n\r\n");
 
  while(!!!client.available()) {
     yield();
  }

  while(client.available()){
    if (client.read() == '\n') {    
      if (client.read() == 'D') {    
        if (client.read() == 'a') {    
          if (client.read() == 't') {    
            if (client.read() == 'e') {    
              if (client.read() == ':') {   
                client.read();
                String theDate = client.readStringUntil('\r');
                client.stop();
                //Serial.println(theDate.substring(17, 19));
                //Serial.println(theDate.substring(20, 22));
                //  Serial.println(theDate.substring(23, 25));
                year_ = theDate.substring(12, 16).toInt();
                month_ = theDate.substring(8, 11);
                day_ = theDate.substring(5, 7).toInt();
                hour_ = theDate.substring(17, 19).toInt()+3;
                minute_ = theDate.substring(20, 22).toInt();
                second_ = theDate.substring(23, 25).toInt();
                
                  setTime(hour_,minute_,second_,day_,1,year_);
                  //setTime(12,11,1,1,1,2017);
                  //setTime(14, 27, 00, 14, 12, 2015);
                
                return theDate;
              }
            }
          }
        }
      }
    }
  }
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  Serial.print(" ");
  Serial.print(minute());
  Serial.print(" ");
  Serial.print(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void initWifi() {
   Serial.print("Connecting to ");
   Serial.print(ssid);
      WiFi.begin(ssid, password);
   
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  
  server.on("/", [](){
    server.send(200, "text/html", webPage+"<h1>INFO: " + res + "</h1>");
  });
  server.on("/socket1On", [](){
    server.send(200, "text/html", webPage);
    turn_on();
    delay(1000);
  });
  server.on("/socket1Off", [](){
    server.send(200, "text/html", webPage);
    turn_off();
    delay(1000); 
  });
  server.begin();
  Serial.println("HTTP server started");

} 
