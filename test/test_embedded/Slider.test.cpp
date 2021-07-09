#include <Future.h>
#include <Slider.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <optional>

using namespace testing;
using namespace uniuno;

class ScreenMock : public Screen {
public:
  MOCK_METHOD((Future<void, void>), load, (), (override));
};

TEST(
    Slider,
    test_next_slide_returns_future_when_there_is_no_new_screen_being_loaded_right_now) {
  ScreenMock screen1;
  Slider slider({screen1});

  EXPECT_CALL(screen1, load())
      .WillRepeatedly(Return(Future<void, void>::resolve()));

  ASSERT_TRUE(slider.next_screen());
}

TEST(
    Slider,
    test_next_slide_does_not_return_future_when_there_is_other_screen_being_loaded_already) {
  auto pending_future =
      Future<void, void>([]() { return AsyncResult<void>::pending(); });
  ScreenMock screen1;
  ScreenMock screen2;
  Slider slider({screen1, screen2});

  EXPECT_CALL(screen1, load()).WillRepeatedly(Return(pending_future));
  EXPECT_CALL(screen2, load()).Times(0);

  auto future = slider.next_screen();
  future->poll();
  future->poll();

  ASSERT_FALSE(slider.next_screen());
}

TEST(
    Slider,
    test_next_slide_goes_to_the_first_screen_when_the_last_one_has_been_loaded) {
  ScreenMock screen1;
  ScreenMock screen2;
  Slider slider({screen1, screen2});

  {
    InSequence s;

    EXPECT_CALL(screen1, load())
        .WillOnce(Return(Future<void, void>::resolve()));
    EXPECT_CALL(screen2, load())
        .WillRepeatedly(Return(Future<void, void>::resolve()));
    EXPECT_CALL(screen1, load())
        .WillOnce(Return(Future<void, void>::resolve()));
  };

  slider.next_screen()->poll();
  slider.next_screen()->poll();
  slider.next_screen()->poll();
}