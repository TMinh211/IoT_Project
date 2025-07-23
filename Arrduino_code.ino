#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

ESP8266WiFiMulti WiFiMulti;

#define BUZZER_PIN D2
#define PIR_PIN D5

unsigned long lastActionTime = 0;
const unsigned long actionInterval = 5000;  // Mỗi 5 giây thực hiện lại nếu còn chuyển động

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  pinMode(PIR_PIN, INPUT);

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("Nger", "103203@@");

  Serial.println("\n📡 Đang kết nối WiFi...");
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n✅ Đã kết nối WiFi");
}

void loop() {
  int currentPir = digitalRead(PIR_PIN);

  if (currentPir == HIGH) {
    // Kiểm tra thời gian để tránh spam
    if (millis() - lastActionTime >= actionInterval || lastActionTime == 0) {
      Serial.println("👀 Phát hiện chuyển động! Gửi cảnh báo và chụp ảnh...");

      // Gửi yêu cầu HTTP
      if (WiFiMulti.run() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;

        String url = "http://172.20.10.2:8080/canh-bao?id=esp8266";
        Serial.println("🌐 Gửi cảnh báo đến server...");

        if (http.begin(client, url)) {
          int httpCode = http.GET();

          if (httpCode > 0) {
            Serial.printf("✅ Server phản hồi mã: %d\n", httpCode);
            String payload = http.getString();
            Serial.println("📥 Phản hồi từ Flask: " + payload);

            // Dù phản hồi có gì, ta vẫn kích buzzer để đồng bộ
            Serial.println("🔔 Kích hoạt buzzer...");
            digitalWrite(BUZZER_PIN, HIGH);
            delay(500);
            digitalWrite(BUZZER_PIN, LOW);
            delay(5000); 
          } else {
            Serial.printf("❌ GET thất bại: %s\n", http.errorToString(httpCode).c_str());
          }

          http.end();
        } else {
          Serial.println("❌ Không kết nối được server");
        }
      }

      lastActionTime = millis();  // Cập nhật thời gian
    }
  } else {
    // Không có chuyển động → reset timer
    lastActionTime = 0;
    digitalWrite(BUZZER_PIN, LOW);
  }

  delay(100);  // kiểm tra liên tục mỗi 100ms
}
