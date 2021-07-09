#pragma once

#ifndef JSON_SIZE
#define JSON_SIZE 4096
#endif

#include <ArduinoJson.h>
#include <HTTPSClient.h>

class APIDisplayer {
public:
  virtual uniuno::Request get_request() const;
  virtual void display(StaticJsonDocument<JSON_SIZE> &doc);
};