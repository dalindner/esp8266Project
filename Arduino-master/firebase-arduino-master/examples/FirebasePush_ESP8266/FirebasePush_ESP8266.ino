//
// Copyright 2015 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// FirebasePush_ESP8266 is a sample that push a new timestamp to firebase
// on each reset.

#include <Firebase.h>

// create firebase client.
Firebase fbase = Firebase("flickering-fire-1849.firebaseio.com")
                   .auth("7fr042il8Dfcb7pT2SJeQOzNih01bqgYTqv3AYHy");

void setup() {
  Serial.begin(9600);

  // connect to wifi.
  WiFi.begin("eric_phone", "3192412548");
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

String asdf = "kitchen";

   // add a new entry.
  //FirebasePush push2 = fbase.push("/logs", "{\"Room\": \"test\"}");

  String command = "{\"Room\": \"" + asdf + "\"}";
  
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
}

void loop() {
}
