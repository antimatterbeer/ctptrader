#include <variant>

#include <gtest/gtest.h>

#include <base/def.hpp>
#include <base/msg.hpp>
#include <util/channel.hpp>

using namespace ctptrader;

TEST(ChannelTest, ShmSpscQueue) {
  util::ShmSpscWriter<base::Msg, 20> writer("test");
  util::ShmSpscReader<base::Msg, 20> reader("test");
  base::Msg msg;
  base::Bar bar{base::Date(20230101),
                base::Timestamp::FromString("2023-01-01 09:00:00"),
                0,
                100,
                100,
                100,
                100,
                100,
                100};
  writer.Write(bar);
  ASSERT_TRUE(reader.Read(msg));
  ASSERT_TRUE(std::holds_alternative<base::Bar>(msg));
}