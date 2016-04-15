

/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>

#include <ESP8266WebServer.h>
#include <EEPROM.h>

#include <Firebase.h>

extern "C" {
#include "user_interface.h"
}


// create firebase client.
Firebase fbase = Firebase("flickering-fire-1849.firebaseio.com")
                   .auth("7fr042il8Dfcb7pT2SJeQOzNih01bqgYTqv3AYHy");



/* Set these to your desired credentials. */
const char *ssid = "ESPap";
const char *password = "thereisnospoon";
String savedSSID = "";
String savedPass = "";
String leftRoom = "";
String rightRoom = "";
boolean set = false;
boolean push = false;
String roomToPush = "";
//boolean light = true;

ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */


void PinInt0(){
  Serial.println("PIN0 RISING");
  Serial.println("Going to " + rightRoom);
  detachInterrupt(0);
  detachInterrupt(2);
  attachInterrupt(digitalPinToInterrupt(2), PinInt22, FALLING);
}

void PinInt2(){
  Serial.println("PIN2 RISING");
  Serial.println("Going to " + leftRoom); 
  detachInterrupt(2); 
  detachInterrupt(0);
  attachInterrupt(digitalPinToInterrupt(0), PinInt02, FALLING);
}

void PinInt02(){
  Serial.println("Send " + leftRoom);
  detachInterrupt(0);

  if(savedSSID != ""){
    push = true;
    roomToPush = leftRoom;
    return;
  }
}

void PinInt22(){
  Serial.println("Send " + rightRoom);
  detachInterrupt(2);

  if(savedSSID != ""){
    push = true;
    roomToPush = rightRoom;
    return;
  }
}


 
void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected</h1>");
}
void login() {
  Serial.println("here");
  String msg;
  String content = "<html><body bgcolor='#000000'>";
  content += "<div style='margin-top:10%;margin-left:10%;background:rgba(255,255,255,.4);border-radius:25px;border:2px solid #a1a1a1;padding:50px;margin-right:10%'>"; 
  content += "<form action='/login' method='POST'><h1>To log in, please use : admin/admin</h1><br>";
  content += "<h2>User:</h2><input type='text' name='USERNAME' placeholder='user name' style='width:100%' required><br>";
  content += "<h2>Password:</h2><input type='password' name='PASSWORD' placeholder='password' style='width:100%' required><br>";
  content += "<h2>SSID:</h2><input type='password' name='SSID' placeholder='password' style='width:100%' required><br>";
  content += "<h2>SSID Password:</h2><input type='password' name='SSIDPASSWORD' placeholder='password' style='width:100%' required><br>";
  content += "<h2>Room To The Left:</h2><select name='LEFTROOM' style='width:100%'><option value='kitchen'>Kitchen</option><option value='bedroom'>Bedroom</option><option value='livingRoom'>Living Room</option><option value='bathRoom'>Bath Room</option></select><br>";
  content += "<h2>Room To The Right:</h2><select name='RIGHTROOM' style='width:100%'><option value='kitchen'>Kitchen</option><option value='bedroom'>Bedroom</option><option value='livingRoom'>Living Room</option><option value='bathRoom'>Bath Room</option></select><br>";
  content += "<br><input type='submit' name='SUBMIT' value='Submit' style='width:100%'></form>" + msg + "<br>";
  content += "</div></body></html>";
  server.send(200, "text/html", content);
}

void handleLogin() {
  String user = server.arg("USERNAME");
  String pass = server.arg("PASSWORD");
  String ssid = server.arg("SSID");
  String ssidPass = server.arg("SSIDPASSWORD");
  String lRoomIn = server.arg("LEFTROOM");
  String rRoomIn = server.arg("RIGHTROOM");
  if(user == "admin" && pass == "admin"){
    String content = "<html><body><a'>Log In Worked, Switching To Detect Mode</a><br><p> Welcome:" + user + "</p></body></html>";
    server.send(200, "text/html", content);
    
    set = true;
    savedSSID = ssid;
    savedPass = ssidPass;
    leftRoom = lRoomIn;
    rightRoom = rRoomIn;

    EEPROM.begin(512);
    // write a 0 to all 512 bytes of the EEPROM
    for (int i = 0; i < 512; i++){
      EEPROM.write(i, 0);
    }

    
    Serial.println("writing eeprom ssid:");
    for (int i = 0; i < savedSSID.length(); ++i){
        EEPROM.write(i, savedSSID[i]);
        Serial.print("Wrote: ");
        Serial.println(savedSSID[i]); 
    }
    Serial.println("writing eeprom pass:"); 
    for (int i = 0; i < savedPass.length(); ++i){
        EEPROM.write(32+i, savedPass[i]);
        Serial.print("Wrote: ");
        Serial.println(savedPass[i]); 
    }    
    Serial.println("writing room left:"); 
    for (int i = 0; i < leftRoom.length(); ++i){
        EEPROM.write(96+i, leftRoom[i]);
        Serial.print("Wrote: ");
        Serial.println(leftRoom[i]); 
    }    
    Serial.println("writing room right:"); 
    for (int i = 0; i < rightRoom.length(); ++i){
        EEPROM.write(116+i, rightRoom[i]);
        Serial.print("Wrote: ");
        Serial.println(rightRoom[i]); 
    }    
      
    EEPROM.commit();

    delay(500);
    abort();

  }
  else{
    String content = "<html><body><a href='/'>Log In Failed</a><br><p>" + user + "</p><br><p>" + pass + "</p></body></html>";
    server.send(200, "text/html", content);
  }
}


int testWifi(void) {
  int c = 0;
  Serial.println("Waiting for Wifi to connect");  
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED) { return(20); } 
    delay(1000);
    Serial.print(WiFi.status());    
    c++;
  }
  Serial.println("Connect timed out, opening AP");
  return(10);
} 


void accessPoint() {
  
  ssid = "ESPap";
  password = "thereisnospoon";

  Serial.println("Configuring access point...");
  //ESP8266WebServer server(80);
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", login);
  server.on("/login", handleLogin);
  server.begin();
  Serial.println("HTTP server started");

  //pinMode(LED_BUILTIN, OUTPUT);
  
}

void setup() {

  Serial.begin(115200);
  EEPROM.begin(512);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println("Startup");
  // read eeprom for ssid and pass
  Serial.println("Reading EEPROM ssid");
  String esid;
  delay(1000);
  
  for (int i = 0; i < 32; ++i){
      esid += char(EEPROM.read(i));
  }
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");
  String epass = "";
  for (int i = 32; i < 96; ++i){
      epass += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  Serial.println(epass);  
  
  String eLeft = "";
  for (int i = 96; i < 116; ++i){
      eLeft += char(EEPROM.read(i));
  }
  leftRoom = eLeft.c_str();
  Serial.print("Left Room: ");
  Serial.println(leftRoom);  

  String eRight = "";
  for (int i = 116; i < 136; ++i){
      eRight += char(EEPROM.read(i));
  }
  rightRoom = eRight.c_str();
  Serial.print("Right Room: ");
  Serial.println(rightRoom);  
  
  EEPROM.end();

  if ( esid.length() > 1 ) {
    // test esid 
    
    WiFi.begin(esid.c_str(), epass.c_str());
    if ( testWifi() == 20 ) { 
      savedSSID = esid.c_str();
      savedPass = epass.c_str();
      set = true;
    }
    else{
      accessPoint();
    }
  }

  pinMode(0, INPUT);
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(0), PinInt0, RISING);
  attachInterrupt(digitalPinToInterrupt(2), PinInt2, RISING);
}



void loop() {

    //light = !light;
    //digitalWrite(LED_BUILTIN, !light);

    server.handleClient();
    delay(500);
    
    if(set){
      WiFi.mode(WIFI_STA);
      //WiFi.disconnect();
      Serial.println("SSID: " + savedSSID);
      Serial.println("SSID PASS: " + savedPass);
      Serial.println("LEFT: " + leftRoom);
      Serial.println("RIGHT: " + rightRoom);
      set = false;
    }
    else{
      Serial.println("Waiting to be set...");
    }
    
    if(push == true){
      if(digitalRead(0) == 0 && digitalRead(2) == 0){
        noInterrupts();
        push = false;
      
        String command = "{\"Room\": \"" + roomToPush + "\"}";
        
        FirebasePush push2 = fbase.push("/logs", command);
        if (push2.error()) {
            Serial.println("Firebase push failed");
            Serial.println(push2.error().message());  
            return;
        }
      
        // print key.
        Serial.println(push2.name());
      
        String key = push2.name().substring(9,29);
        Serial.println(key);
         
         FirebasePush push = fbase.push("/logs/"+key, "{\".sv\": \"timestamp\"}");
        
        if (push.error()) {
            Serial.println("Firebase push failed");
            Serial.println(push.error().message());  
            return;
        }
      
        // print key.
        Serial.println(push.name());
      
        // get all entries.
        FirebaseGet get = fbase.get("/logs");
        if (get.error()) {
            Serial.println("Firebase get failed");
            Serial.println(push.error().message());  
            return;
        }
        // print json.
        Serial.println(get.json());
      
        attachInterrupt(digitalPinToInterrupt(0), PinInt0, RISING);
        attachInterrupt(digitalPinToInterrupt(2), PinInt2, RISING);
        interrupts();




        //WiFi.disconnect();
      }
    }
    
    //Serial.println("GPIO PIN0: " + String(digitalRead(0)));
    //Serial.println("GPIO PIN2: " + String(digitalRead(2)));

}

void pushToFireBase(String roomToPush){
  

  return;
}







