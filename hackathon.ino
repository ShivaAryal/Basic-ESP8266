#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <SPI.h>

#define D0 16
#define D1 5 
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define D7 13
#define D8 15
#define D9 3
#define D10 1

const char* ssid     = "try";
const char* password = "throwException";

const char* host = "192.168.43.154";
const int httpPort = 80;
char charVar[15];

struct UserData {
  int id;
  //char username[32];
  //char password[32];
  char street[10];    ///connected to D0
  char kitchen[10];  //connected to D1
  char hallway[10];     //D2
  char livingroom[10];          //D3
  char fan[10];         //D4
  char bedroom[10];      //D5
  int hourTime; 
};

UserData userData;

void setup(){
  pinMode(A0,INPUT);
  pinMode(D0,OUTPUT);
  pinMode(D1,OUTPUT);
  pinMode(D2,OUTPUT);
  pinMode(D3,OUTPUT);
  pinMode(D4,OUTPUT);
  pinMode(D10,INPUT);
  Serial.begin(115200);
  //delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("WiFi connected"); 
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  delay(2000);
  String line;
  
  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // We now create a URI for the request
  String url = "/hackathon/get.php?id=2";

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server.
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + 
                 "Host:" + host + "\r\n" + 
                "User-Agent:Mozilla/5.0(Windows NT 6.3; WOW64; rv:48.0) Gecko/20100101 Firefox/48.0" +
                "Accept:text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8" +
                "Connection:close\r\n\r\n");
  
  delay(10);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    line = client.readStringUntil('\r');
   // Serial.print(line);
  }
  Serial.println();
  Serial.println("----------------------JSON PARSER-------------------------");

  char* charBuf = (char*)malloc(sizeof(char)*1024);  //dynamic memory allocation avoid most of ESP watch dog exeptions
  line.toCharArray(charBuf,1024);
  if (parseUserData(charBuf, &userData)) {
    Serial.println("printing data");
    //printUserData(&userData);       
}
  Serial.println("-------------------------------------------------------"); 
  free(charBuf); // remember to free dynamic memory 
  Serial.println("closing connection");
}


bool parseUserData(char* content, struct UserData* userData) {
  // Compute optimal size of the JSON buffer according to what we need to parse.
//   This is only required if you use StaticJsonBuffer.
    Serial.println("parsing data");
   const size_t BUFFER_SIZE =
     JSON_OBJECT_SIZE(8)     // the root object has 8 elements
      + JSON_OBJECT_SIZE(5)   // the "address" object has 5 elements
      + JSON_OBJECT_SIZE(2)   // the "geo" object has 2 elements
      + JSON_OBJECT_SIZE(3);  // the "company" object has 3 elements

  // Allocate a temporary memory pool on the stack
   //StaticJsonBuffer<200> jsonBuffer;
  // If the memory pool is too big for the stack, use this instead:
  DynamicJsonBuffer jsonBuffer(BUFFER_SIZE);

  JsonObject& root = jsonBuffer.parseObject(content);

  if (!root.success()) {
    Serial.println("JSON parsing failed!");
    return false;
  }
  
  root.prettyPrintTo(Serial);   //print raw JSON data

  // Here were copy the strings we're interested in
  //userData->id = root["id"];
  //strcpy(userData->username, root["username"]);
  //strcpy(userData->password, root["password"]);
  //Serial.println(root["Kitchen"]);
  strcpy(userData->kitchen, root["Kitchen"]);
  strcpy(userData->bedroom, root["Bedroom"]);
  strcpy(userData->hallway, root["Hallway"]);
  strcpy(userData->livingroom, root["Livingroom"]);
  strcpy(userData->fan, root["Fan"]);
  strcpy(userData->street, root["Street"]);
  userData->hourTime=(int)root["hour"];
  Serial.println(userData->hourTime);
  Serial.println("Copying success");
  Serial.println("---------------------------");
  Serial.println(userData->kitchen);
  Serial.println("---------------------------");
  
  if(userData->hourTime>17&&userData->hourTime<=24){
    int j=map(userData->hourTime,0,24,0,255);}
  else if(userData->hourTime>=1&&userData->hourTime<6){
    int j=map(userData->hourTime,24,0,255,0);}
  }
  if(strcmp(userData->street,"1")==0){
    analogWrite(D0,j);
  }else{
    analogWrite(D0,LOW);
  }

  int any = analogRead(A0);
  if(!strcmp(userData->livingroom,"1")){
    if (any>10){
      Serial.println("working");
    }else{
      Serial.println("not Working");
    } 
  }


  if(!strcmp(userData->kitchen,"1")){
    digitalWrite(D1,HIGH);  
  }
  else{
    digitalWrite(D1,LOW);
  }

  if(!strcmp(userData->hallway,"1")){
    digitalWrite(D2,HIGH);  
  }
  else{
    digitalWrite(D2,LOW);
  }

  if(!strcmp(userData->bedroom,"1")){
    digitalWrite(D3,HIGH);  
  }
  else{
    digitalWrite(D3,LOW);
  }

  if(!strcmp(userData->livingroom,"1")){
    digitalWrite(D4,HIGH);  
  }
  else{
    digitalWrite(D4,LOW);
}
  
void printUserData(const struct UserData* userData){
  Serial.println("");
//  Serial.print("id = ");
//  Serial.println(userData->id);
  
  Serial.print("kitchen = ");
  Serial.println(userData->kitchen);

  Serial.print("bedroom = ");
  Serial.println(userData->bedroom);

  Serial.print("hallway = ");
  Serial.println(userData->hallway);

  Serial.print("livingroom = ");
  Serial.println(userData->livingroom);

  Serial.print("fan = ");
  Serial.println(userData->fan);

  Serial.print("street = ");
  Serial.println(userData->street);
}

