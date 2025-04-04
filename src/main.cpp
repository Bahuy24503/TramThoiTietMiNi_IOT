#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>
#include <secret.h>

#define SDA_PIN 21 // Chân SDA của ESP32
#define SCL_PIN 22 // Chân SCL của ESP32

// Kết nối WiFi
const char *WIFI_SSID = "Wokwi-GUEST";
const char *WIFI_PASSWORD = ""; // Không có mật khẩu

// Thông tin Telegram Bot

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

LiquidCrystal_I2C lcd(0x27, 20, 4); // Địa chỉ I2C của LCD
unsigned long lastCheck = 0;        // Thời gian kiểm tra tin nhắn

void setup()
{
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN); // Khởi tạo giao tiếp I2C
  lcd.begin(20, 4);
  lcd.backlight();

  // Kết nối WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Đang kết nối WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nĐã kết nối WiFi");

  // Cấu hình kết nối TLS cho Telegram
  client.setInsecure();

  lcd.setCursor(0, 0);
  lcd.print("ESP32 Ready!");
}

void loop()
{
  // Sinh giá trị ngẫu nhiên
  float temperature = (esp_random() % 1501 + 1500) / 100.0;
  float humidity = (esp_random() % 701 + 300) / 10.0;
  float pressure = (esp_random() % 71 + 980);

  // Hiển thị lên LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Nhiet Do: ");
  lcd.print(temperature);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Do Am: ");
  lcd.print(humidity);
  lcd.print(" %");

  lcd.setCursor(0, 2);
  lcd.print("Ap Suat: ");
  lcd.print(pressure);
  lcd.print(" hPa");

  delay(4000); // Cập nhật mỗi 4 giây

  // Kiểm tra tin nhắn Telegram mỗi 5 giây
  if (millis() - lastCheck > 5000)
  {
    lastCheck = millis();
    int newMessages = bot.getUpdates(bot.last_message_received + 1);

    while (newMessages)
    {
      for (int i = 0; i < newMessages; i++)
      {
        String chat_id = bot.messages[i].chat_id;
        String text = bot.messages[i].text;

        Serial.println("Tin nhắn từ Telegram: " + text);

        if (text == "/getdata")
        {
          String message = "📊 Dữ liệu thời tiết:\n";
          message += "🌡 Nhiệt độ: " + String(temperature) + " °C\n";
          message += "💧 Độ ẩm: " + String(humidity) + " %\n";
          message += "🔵 Áp suất: " + String(pressure) + " hPa";

          bot.sendMessage(chat_id, message, "");
        }
      }
      newMessages = bot.getUpdates(bot.last_message_received + 1);
    }
  }
}
