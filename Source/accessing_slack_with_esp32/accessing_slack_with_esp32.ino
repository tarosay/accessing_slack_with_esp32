#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "UriCode.h"

#define BAURATE 9600
#define CHANNEL "xxxxxxxxxxx"                                             //スラックのチャンネルIDです
#define TAKEN "xoxb-xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"  //スラックアプリ用に取得したトークンです
#define SSID "xxxxxxx"                                                    //WiFiのSSIDです
#define PASSWORD "xxxxxxxxxx"                                             //WiFiのパスワードです
#define MAXCHARBYTE 2048

char Recives[MAXCHARBYTE];
StaticJsonDocument<4096> Docs;

void setup() {
  Serial.begin(BAURATE);
  Connection();
  WriteSlackChnnel("テストの書き込みです。\r\n");
  ReadSlackChnnel();
  Serial.println((const char*)Recives);
}

void loop() {
}

//*************************
// WiFiに接続します
//*************************
void Connection() {
  int cnt;
  if (WiFi.isConnected()) {
    WiFi.disconnect();
    delay(3000);
  }

  while (!WiFi.isConnected()) {
    Serial.print("Waiting connect to WiFi: ");
    WiFi.begin(SSID, PASSWORD);

    cnt = 0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      cnt += 1;
      if (cnt == 10) {
        Serial.println("Conncet failed!");
        break;
      }
    }
  }
  Serial.print("\nIP address: ");
  Serial.println(WiFi.localIP());
}

//*************************
// スラックに書き込みます
//*************************
void WriteSlackChnnel(const char* data) {
  int len = strlen((const char*)data);
  unsigned char send[3 * len];

  URI_enCode((const unsigned char*)data, send);

  WiFiClientSecure client;
  client.setInsecure();  //skip verification
  if (!client.connect("slack.com", 443)) {
    Serial.println("Connection failed!");
    return;
  } else {
    client.print("GET https://slack.com/api/chat.postMessage?channel=" CHANNEL "&text=");
    client.print((const char*)send);
    client.println("&pretty=1 HTTP/1.0");
    client.println("Host: slack.com");
    client.println("Authorization: Bearer " TAKEN);
    client.println("Connection: close");
    client.println();

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        break;
      }
    }

    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }
    client.stop();
  }
}

//*************************
// スラックにから読み込んでRecives[]配列に書きだします
// limit=1にしているので、最新の書き込みを1つだれ読み出します。
//*************************
void ReadSlackChnnel() {
  WiFiClientSecure client;
  client.setInsecure();  //skip verification
  if (!client.connect("slack.com", 443)) {
    Serial.println("Connection failed!");
    Recives[0] = 0;
    return;
  } else {
    client.println("GET https://slack.com/api/conversations.history?channel=" CHANNEL "&limit=1 HTTP/1.0");
    client.println("Host: slack.com");
    client.println("Authorization: Bearer " TAKEN);
    client.println("Connection: close");
    client.println();

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        break;
      }
    }

    int cnt = 0;
    while (client.available()) {
      char c = client.read();
      Recives[cnt] = c;

      cnt++;
      if (cnt >= MAXCHARBYTE) {
        Recives[0] = 0;
        client.stop();
        return;
      }
    }
    Recives[cnt] = 0;
    client.stop();
  }
  Serial.println((const char*)Recives);

  DeserializationError error = deserializeJson(Docs, Recives, MAXCHARBYTE);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  bool ok = Docs["ok"];
  if (!ok) {
    return;
  }

  JsonObject message = Docs["messages"][0];
  const char* message_text = message["text"];
  strcpy(Recives, message_text);
}
