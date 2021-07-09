#include <APIDisplayer.h>
#include <APIScreen.h>
#include <Arduino.h>
#include <ESP8266WiFiAdapter.h>
#include <Executor.h>
#include <HTTPSClient.h>
#include <LittleFS.h>
#include <SSD1306OLED.h>
#include <Slider.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

using namespace uniuno;

class WeatherAPIDisplayer : public APIDisplayer {
public:
  WeatherAPIDisplayer(OLED *o) : oled(o) {}

  Request get_request() const override {
    return Request::build(
        Request::GET, PSTR("https://api.openweathermap.org/data/2.5/"
                           "weather?q=London&appid="
                           "3146fc6c08b27745a6d12ee9384bd05d&units=metric"));
  }

  void display(StaticJsonDocument<JSON_SIZE> &doc) override {
    oled->display_text(doc["weather"][0]["description"], {0, 0}, 1);

    char temp[10];
    sprintf(temp, "%.0f%c%c", (float)doc["main"]["temp"], 248, 'C');
    this->oled->display_text(temp, {0, 24}, 2, false);
    this->oled->display_text("London", {0, 48}, 1, false);
  }

private:
  OLED *oled;
};

class StockIndexAPIDisplayer : public APIDisplayer {
public:
  StockIndexAPIDisplayer(OLED *o) : oled(o) {}

  Request get_request() const override {
    return Request::build(Request::GET,
                          PSTR("https://query1.finance.yahoo.com/v7/finance/"
                               "spark?symbols=%5EGSPC&range=1d&interval=5m"));
  }

  void display(StaticJsonDocument<JSON_SIZE> &doc) override {
    this->oled->display_text("S&P500", {0, 0}, 2);

    char temp[10];
    float price =
        doc["spark"]["result"][0]["response"][0]["meta"]["regularMarketPrice"];
    sprintf(temp, "$%.2f", price);
    this->oled->display_text(temp, {0, 24}, 2, false);
  }

private:
  OLED *oled;
};

class BTCAPIDisplayer : public APIDisplayer {
public:
  BTCAPIDisplayer(OLED *o) : oled(o) {}

  Request get_request() const override {
    return Request::build(
        Request::GET,
        PSTR("https://api.coindesk.com/v1/bpi/currentprice.json"));
  }

  void display(StaticJsonDocument<JSON_SIZE> &doc) override {
    this->oled->display_text("1 BTC", {0, 0}, 2);

    char temp[10];
    float price = doc["bpi"]["USD"]["rate_float"];
    sprintf(temp, "$%.2f", price);
    this->oled->display_text(temp, {0, 24}, 2, false);
  }

private:
  OLED *oled;
};

class DogeAPIDisplayer : public APIDisplayer {
public:
  DogeAPIDisplayer(OLED *o) : oled(o) {}

  Request get_request() const override {
    return Request::build(
        Request::GET, PSTR("https://api.cryptonator.com/api/ticker/doge-usd"));
  }

  void display(StaticJsonDocument<JSON_SIZE> &doc) override {
    this->oled->display_text("1 Dogecoin", {0, 0}, 2);

    char temp[10];
    float price = doc["ticker"]["price"];
    sprintf(temp, "$%.2f", price);
    this->oled->display_text(temp, {0, 24}, 2, false);
  }

private:
  OLED *oled;
};

// create ESP8266 built-in classes <-> non-blocking wifi adapter
ESP8266WiFiAdapter wifi_adapter;
// create non-blocking wifi connector
WiFiConnector wifi_connector(&wifi_adapter);
// create non-blocking https client
HTTPSClient https_client(&wifi_connector, &LittleFS);
// create executor (future resolver queue handler)
Executor executor;

// create display
Adafruit_SSD1306 ssd1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
// create ssd1306 <-> OLED adapter
SSD1306OLED oled(&ssd1306);

// create single JSON document and allocate memory for it on the stack
StaticJsonDocument<JSON_SIZE> doc;

// create displayers
WeatherAPIDisplayer weather_displayer(&oled);
StockIndexAPIDisplayer stock_displayer(&oled);
BTCAPIDisplayer btc_displayer(&oled);
DogeAPIDisplayer doge_displayer(&oled);

// create slider screens
APIScreen<HTTPSClient> weather_screen(https_client, doc, weather_displayer);
APIScreen<HTTPSClient> stock_screen(https_client, doc, stock_displayer);
APIScreen<HTTPSClient> btc_screen(https_client, doc, btc_displayer);
APIScreen<HTTPSClient> doge_screen(https_client, doc, doge_displayer);

// create slider with screens
Slider slider({weather_screen, stock_screen, btc_screen, doge_screen});

void setup() {
  // Serial monitor initalization
  Serial.begin(115200);
  delay(2000);

  // set pin D5 to wait for input
  pinMode(D5, INPUT_PULLUP);

  // Initialize 128x64 display
  if (!ssd1306.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
  }
  ssd1306.clearDisplay();
  ssd1306.display();

  // Initialize file system (to read SSL certs from the cert store)
  LittleFS.begin();

  // Add WiFi access point
  wifi_connector.add_access_point("myssid", "mypassword");
}

bool is_button_pressed() { return digitalRead(D5) == LOW; }

void loop() {
  if (is_button_pressed()) {
    // go to the next screen
    auto future = slider.next_screen();

    // check if screen is not loading already
    if (future) {
      // display info when screen starts to load data from API
      oled.display_text("Loading...", {0, 32}, 2);

      // add future to the execution queue to resolve it
      executor.execute(future.value(), [](Error e) {
        // in case future is rejected display error on the screen

        // TODO: fix error string going out of scope so it's not possible to
        // retrieve it here
        oled.display_text("Error", {0, 32}, 2);
      });
    }

    // neccesary delay for the button to work correctly
    delay(250);
  }
}