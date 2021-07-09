#include <APIScreen.h>
#include <Future.h>
#include <HTTPSClient.h>
#include <Screen.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>

using namespace uniuno;
using namespace testing;

class TestAPIDisplayer : public APIDisplayer {
public:
  MOCK_METHOD(Request, get_request, (), (const override));
  MOCK_METHOD(void, display, (StaticJsonDocument<JSON_SIZE> & doc), (override));
};

class HTTPSClientInterface {
public:
  virtual ~HTTPSClientInterface() {}
  virtual Future<void, std::shared_ptr<Response>>
  send_request(Request request) = 0;
};

class HTTPSClientMock : public HTTPSClientInterface {
public:
  MOCK_METHOD((Future<void, std::shared_ptr<Response>>), send_request,
              (Request request), (override));
};

TEST(APIScreen,
     test_load_requests_the_data_using_displayer_and_parse_the_json_response) {
  StaticJsonDocument<JSON_SIZE> doc;
  HTTPSClientMock https_client;
  TestAPIDisplayer api_displayer;
  APIScreen<HTTPSClientMock> api_screen(https_client, doc, api_displayer);
  Request expected_request = Request::build(Request::GET, "https://test.com");
  auto expected_response = std::make_shared<Response>(
      Response{200, std::make_shared<ResponseStream>()});

  EXPECT_CALL(api_displayer, get_request())
      .WillRepeatedly(Return(expected_request));
  EXPECT_CALL(api_displayer, display(Eq(std::ref(doc))));
  EXPECT_CALL(
      https_client,
      send_request(AllOf(Field(&Request::url, Eq(expected_request.url)),
                         Field(&Request::method, Eq(expected_request.method)))))
      .WillRepeatedly(Return(
          Future<void, std::shared_ptr<Response>>::resolve(expected_response)));

  auto future = api_screen.load();

  while (future.poll().is_pending())
    ;
}