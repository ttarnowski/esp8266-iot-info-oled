#pragma once

#include <APIDisplayer.h>
#include <ArduinoJson.h>
#include <HTTPSClient.h>
#include <Screen.h>
#include <memory>

using namespace uniuno;

template <class HTTPSClient> class APIScreen : public Screen {
public:
  explicit APIScreen(HTTPSClient &c, StaticJsonDocument<JSON_SIZE> &sjd,
                     APIDisplayer &d)
      : https_client(c), doc(sjd), displayer(d) {}

  Future<void, void> load() override {
    return this->https_client.send_request(this->displayer.get_request())
        .and_then([this](std::shared_ptr<Response> res) {
          deserializeJson(doc, res->body->readString());
          this->displayer.display(this->doc);
        });
  }

private:
  HTTPSClient &https_client;
  StaticJsonDocument<JSON_SIZE> &doc;
  APIDisplayer &displayer;
};
