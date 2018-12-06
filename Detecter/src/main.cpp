#include <M5Stack.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>

#include "key.h"

const String SEND_MESSAGE = "https://slack.com/api/chat.postMessage";
const String channel = "channel=DEHC05B4N";
const String as_user = "as_user=false";

const int PIN = 36;
bool prev = false;
int count = 0;

const int loop_timeout = 5000; //[ms]
hw_timer_t *timer = NULL;

TaskHandle_t wtcdog;
xSemaphoreHandle xSemaphore = NULL;
bool measuring = false;

void reset()
{
  esp_restart();
}

void connectWifi()
{

  M5.Lcd.print("Connecting to ");

  M5.Lcd.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {

    delay(500);

    M5.Lcd.print(".");
  }

  M5.Lcd.println("\nWiFi connected.");

  M5.Lcd.println("IP address: ");

  M5.Lcd.println(WiFi.localIP());
}

void disconnectWifi()
{
  WiFi.disconnect();
  Serial.println("disconnected!");
}

String post_slack(String host, String params)
{
  HTTPClient http;

  if (!http.begin(host, CA))
  {
    return String("begin failed!");
  }

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Accept", "*/*");
  //M5.Lcd.println(params);

  int httpCode = http.POST(params);
  M5.lcd.println(String("httpCode: ") + httpCode);
  String result = "";

  if (httpCode < 0)
  {
    result = http.errorToString(httpCode);
  }
  else
  {
    result = http.getString();
  }

  http.end();
  return result;
}

String get_robot(String host)
{
  HTTPClient http;

  if (!http.begin(host))
  {
    return String("begin failed!");
  }

  int httpCode = http.GET();
  String result = "";

  if (httpCode < 0)
  {
    result = http.errorToString(httpCode);
  }
  else
  {
    result = http.getString();
  }

  http.end();
  return result;
}

void begin_measure(void)
{
  while (xSemaphoreTake(xSemaphore, portMAX_DELAY) != pdTRUE)
    ;
  measuring = true;
  xSemaphoreGive(xSemaphore);
}

void end_measure(void)
{
  while (xSemaphoreTake(xSemaphore, portMAX_DELAY) != pdTRUE)
    ;
  measuring = false;
  xSemaphoreGive(xSemaphore);
}

void measure_time(void *pvParameters)
{
  bool prev = false;
  bool now = false;
  int count = 0;
  portTickType last_time = xTaskGetTickCount();
  while (1)
  {
    vTaskDelayUntil(&last_time, 1000 / portTICK_RATE_MS); // every 100 ms

    while (xSemaphoreTake(xSemaphore, portMAX_DELAY) != pdTRUE)
      ;
    now = measuring;
    xSemaphoreGive(xSemaphore);

    if (!prev && now) // pos_edge
      count = 0;

    if (now)
      count++;

    if (count >= 6) // 6 sec
      reset();

    prev = now;
  }
}

void setup()
{
  M5.begin();
  delay(100);
  connectWifi();

  if (!MDNS.begin("ESP32_Browser"))
  {
    Serial.println("Error setting up MDNS responder!");
    while (1)
    {
      delay(1000);
    }
  }

  M5.Lcd.setTextFont(4);
  M5.Lcd.setTextColor(TFT_YELLOW);

  pinMode(PIN, INPUT);

  xSemaphore = xSemaphoreCreateMutex();
  xTaskCreatePinnedToCore(measure_time, "measure_time", 4096, NULL, 1, &wtcdog, 0);
  xSemaphoreGive(xSemaphore);
}

void loop()
{
  M5.Lcd.clear();
  M5.Lcd.setCursor(0, 0);

  int sens = analogRead(PIN);
  bool now = sens > 3000;
  bool pos_edge = now && !prev;
  prev = now;

  if (pos_edge)
  {
    M5.Lcd.println("POST");
    count++;
    String text = String() + "text=!!!DENGER!!! (" + count + " times!)";
    post_slack(SEND_MESSAGE, token + "&" + channel + "&" + text + "&" + as_user);

    begin_measure();
    M5.Lcd.println("GET");
    auto ip = MDNS.queryHost("kirimasyaro", 1000);
    if (ip.toString() != "0.0.0.0")
    {
      String host = "http://" + ip.toString() + "/";
      M5.Lcd.println("ADDRESS: " + ip.toString());
      String r2 = get_robot(host);
      M5.Lcd.println(r2);
    }
    else
    {
      M5.Lcd.println("Robot is not found!");
    }
    end_measure();
  }

  M5.Lcd.println(String("Value: ") + sens);
  delay(500);
  M5.update();
}
