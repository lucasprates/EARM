//Energy and Reachability Managament of Mining Rigs - Using Wemos D1 (ESP8266)
//Coded by Lucas Quintiliano

//Sensors connected: (a) High Voltage Relay #1 (b) High Voltage Relay #2

//Instructions:
//We check reachability of (1) Outside server to guarantee internet access, and (2) ping the local mining rig to check if it is still stable and reliable
//Then, after that, we turn the High Voltage relay #1 and/or #2 On+Off to get internet access back on, or, reboot the computer system

#include <Arduino.h>
#include <ESP8266mDNS.h>
//Wifi Client Libraries
#include <ESP8266WiFi.h>
//Web Server Libraries
#include <ESP8266WebServer.h>
//Ping Libraries
#include <ESP8266Ping.h>
//Timestamp Libraries
#include <TimeLib.h>

//Wifi variables and definitions (TO SET UP)
const char* ssid = "YourSSID";
const char* password = "YourPSWD";

//Mining Rig #1 variable
IPAddress rigTestIP (192, 168, 0, 254); //IP of mining rig is "XXX.XXX.XXX.XXX"

//Internet Reachability Host addresses
IPAddress internetHostAddress (8, 8, 8, 8); //Google DNS #1: 8.8.8.8
IPAddress internetHostAddress2 (8, 8, 4, 4); //Google DNS #2: 8.8.4.4
String internetHostAddress3 = "www.amazon.com"; //Generic Website - Amazon's main host: www.amazon.com

//Wemos D1 Config variables
const int led = 13;

//High Voltage Relay #1 variables and definitions - Mining Rig Test #1 Power Input
int PIN_REL1 = D6; //digital pin where data of Relay sensor #1 as INPUT from (D6) at wemos D1 board
bool read_REL1; //value read from PIR sensor #1 

//High Voltage Relay #2 variables and definitions - Router and Internet Modem Power Input
int PIN_REL2 = D7; //digital pin where data of Relay sensor #2 as INPUT from (D7) at wemos D1 board
bool read_REL2; //value read from PIR sensor #2

//Timestamp related variables
time_t lastCheckTimeDate = -1; 
time_t lastTimeMiningRigWasDown = -1; 
time_t lastTimeInternetWasDown = -1; 
int checkCount = 0;
int rigCount = 0;
int wwwCount = 0;

//Reachability Check Loop variables
int reachabilityLoopCheckTime = 60; //in seconds
bool miningRigIsReachable = true;
int miningRigBootCycleTime = 5; //in minutes
bool internetIsReachable = true;
int internetReachabilityBootCycleTime = 5; //in minutes


//Wemos HTTP Server's variables and definitions
ESP8266WebServer server(80);
#define MAXNUMBER_ARGS 5 //Maximum Number of Arguments passing thru GET method with URI
String userAccessGrantPassword = "YourUserPasswordToGrantAccess";

void setup(void){
  Serial.begin(115200);  

  WiFi.mode(WIFI_STA);

  //Blue LED setup code
  pinMode(LED_BUILTIN, OUTPUT); //Blue LED from Wifi

  //Detector Relay components setup code
  //Relay Sensor #1
  pinMode(PIN_REL1, OUTPUT);

  //Relay Sensor #2
  pinMode(PIN_REL2, OUTPUT);

  delay(500);

  //Tell the current state of Relay Sensor #1 (On)
  read_REL1 = false;
  changeRelayState(1, false);
  delay(500);

  //Tell the current state of Relay Sensor #2 (On)
  read_REL2 = false;
  changeRelayState(2, false);
  delay(500);

  lastCheckTimeDate = now(); 
  lastTimeMiningRigWasDown = now(); 
  lastTimeInternetWasDown = now(); 

  delay(500);

  // Wait for Wi-fi Connection setup code
  connectToWifi(); 
  
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
  
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  
  server.on("/rebootMiningRig", handleClientRequest);
  server.on("/rebootInternet", handleClientRequest);
  server.on("/checkMiningRigReachability", handleClientRequest);
  server.on("/checkInternetReachability", handleClientRequest);
  server.on("/getLogFile", handleClientRequest);
  
  server.begin();

  Serial.println("EARM - Mining - Rig Test #1 - Wemos D1 - Boot OK");
}

void loop(void){
  //HTTP server
  server.handleClient();
  
  //check if last time it checked reachability was before 1 minute ago
  if((now() - lastCheckTimeDate) > reachabilityLoopCheckTime){
    Serial.printf("Checking Reachability - Turn #%d\n", checkCount);
    checkCount+=1;
    //if it was, then lets do the checking again
    
    //check if Internet is NOT reachable
    internetIsReachable = isInternetReachable();
    if(internetIsReachable == false){
      Serial.println("Internet is NOT Reachable!");
      //check if internet was reachable for more than 5 minutes ago
      if((now() - lastTimeInternetWasDown) > (60*internetReachabilityBootCycleTime)){
        Serial.printf("Web Router energy reboot - Turn #%d\n", wwwCount);
        wwwCount+=1;
        switchRelay(2);
        lastTimeInternetWasDown = now(); 
      }
    }else{
      Serial.println("Internet is Reachable!");
    }
    
    //check if mining rig is NOT reachable
    miningRigIsReachable = isMiningRigReachable();
    if((miningRigIsReachable == false) && (internetIsReachable == true)){
      Serial.println("Mining Rig is NOT Reachable while internet is OK!");
      //check if mining rig was reachable for more than 5 minutes ago
      if((now() - lastTimeMiningRigWasDown) > (60*miningRigBootCycleTime)){
        Serial.printf("Mining Rig energy reboot - Turn #%d\n", rigCount);
        rigCount+=1;
        switchRelay(1);
        lastTimeMiningRigWasDown = now(); 
      }
    }else{
      Serial.println("Mining Rig is Reachable!");
    }

    lastCheckTimeDate = now();
    Serial.println("");
  }
  
}

void connectToWifi(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  if(WiFi.status() == WL_CONNECTED){
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

//HTTP Server functions - HandleRoot "/" page request as an ID of what this board is
void handleRoot() {
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
  delay(1000);
  server.send(200, "text/plain", "Hello from EARM - Mining Rig #1 - Wemos D1");
  digitalWrite(LED_BUILTIN, HIGH);
}

//HTTP Server functions - HandleRoot anyother page request as notfound
void handleNotFound(){
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
  delay(1000);
  String message = "EARM - Mining Rig #1 - Wemos D1 - File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(LED_BUILTIN, HIGH);
}

//HTTP Server functions - Handle actions that a client can do with this EaRM application
void handleClientRequest(){
  //HTTP Address command syntax: http://IPADDRESS:port/uri?name=value&name2=value2
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level)
  delay(1000);

  String message = "EARM - Mining Rig #1 - Wemos D1 - Client Request\n\n";
  if(server.method() == HTTP_GET){
    //get all arguments sent via HTTP GET protocol
    String argNames[MAXNUMBER_ARGS];
    String argValues[MAXNUMBER_ARGS];
    for (uint8_t i=0; i<server.args(); i++){
      argNames[i] = server.argName(i);
      argValues[i] = server.arg(i);
    } 

    //check password sent
    String passwordInput = getValueOfArgument("pass", argNames, argValues, server.args());
    if((passwordInput != "") && (passwordInput == userAccessGrantPassword)){
      //password is correct, so move on
      String uriStr = server.uri();
      message += "URI: " + uriStr + "\n";

      //check if URI user chose has a protocol related
      if(uriStr == "/rebootMiningRig"){
        switchRelay(1);
        message += "Response: Minig Rig asked to Reboot - Rebooting";
      }else if(uriStr == "/rebootInternet"){
        switchRelay(2);
        message += "Response: Internet asked to Reboot - Rebooting";
      }else if(uriStr == "/checkMiningRigReachability"){
        int elapsedRigCheckTime = now() - lastCheckTimeDate;
        if(miningRigIsReachable == true){
          message += "Response: Mining Rig IS Reachable - Last check was " + String(elapsedRigCheckTime);
        }else{
          message += "Response: Mining Rig is NOT Reachable - Last check was " + String(elapsedRigCheckTime);
        }
        message += " second(s) ago";
      }else if(uriStr == "/checkInternetReachability"){
        int elapsedInternetCheckTime = now() - lastCheckTimeDate;
        if(miningRigIsReachable == true){
//          Serial.printf("Response: Internet IS Reachable - Last check was %d", elapsedInternetCheckTime);
          message += "Response: Internet IS Reachable - Last check was " + String(elapsedInternetCheckTime);
        }else{
          message += "Response: Internet is NOT Reachable - Last check was " + String(elapsedInternetCheckTime);
        }
        message += " second(s) ago";
      }else if(uriStr == "/getLogFile"){
        message += "Get Log File in TXT format Not Implemented Yet\n";
      }
    }else{
      message += "Something is not right: Out of the Pass protocol.\n";
    }
  }
  
  server.send(404, "text/plain", message);
  digitalWrite(LED_BUILTIN, HIGH);
}

String getValueOfArgument(String argument, String names[], String values[], int numberOfArguments){
  for (int i=0; i<numberOfArguments; i++){    
    if(names[i] == argument){
      return values[i];
    }
  }
  return "";
}

//Change relay state
void changeRelayState(int relayNumber, bool newStateOfRelay){
  int relayChosen = -1;
  if(relayNumber == 1){
    relayChosen = PIN_REL1;
  }
  if(relayNumber == 2){
    relayChosen = PIN_REL2;
  }

  if(relayChosen != -1){
    if(newStateOfRelay){
      digitalWrite(relayChosen, HIGH);
      if(relayNumber == 1){
        read_REL1 = true;
      }else{
        read_REL2 = true;
      }
    }else{
      digitalWrite(relayChosen, LOW);
      if(relayNumber == 1){
        read_REL1 = false;
      }else{
        read_REL2 = false;
      }
    }
  }
}

//Change the State of the selected relay from ON to OFF, and then after 2 seconds, from OFF to ON Again
void switchRelay(int relayNumber){
  changeRelayState(relayNumber, true);
  delay(10000);
  changeRelayState(relayNumber, false);
  delay(500);
}

//Function to check if Mining Rig is Still On and with LAN access
bool isMiningRigReachable(){
  return Ping.ping(rigTestIP, 5);
}

//Function to check if Internet is Still Reachble by the current router/modem and internet provider
bool isInternetReachable(){
  if(Ping.ping(internetHostAddress, 5) == true){
    return true;
  }else if(Ping.ping(internetHostAddress2, 5) == true){
    return true;
  }else if(Ping.ping(internetHostAddress2, 5) == true){
    return true;
  }

  return false;
}
