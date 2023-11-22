#include <gtest/gtest.h>
#include <thread>

#include <util/channel.hpp>

namespace {

using namespace ctptrader::util;

TEST(ShmSpscQueueTest, ReadWrite) {
  constexpr size_t kQueueSize = 1024;
  constexpr size_t kNumMessages = 1000;
  constexpr char kQueueName[] = "test_queue";

  // Create writer process
  ShmSpscWriter<int, kQueueSize> writer(kQueueName);
  for (size_t i = 0; i < kNumMessages; ++i) {
    ASSERT_TRUE(writer.Write(i));
  }

  // Create reader process
  ShmSpscReader<int, kQueueSize> reader(kQueueName);
  for (size_t i = 0; i < kNumMessages; ++i) {
    int value;
    while (!reader.Read(value)) {
      std::this_thread::yield();
    }
    ASSERT_EQ(value, i);
  }
}

TEST(ShmSpscQueueTest, Empty) {
  constexpr size_t kQueueSize = 1024;
  constexpr char kQueueName[] = "test_queue_empty";

  // Create reader process
  ShmSpscReader<int, kQueueSize> reader(kQueueName);
  ASSERT_TRUE(reader.Empty());

  // Create writer process
  ShmSpscWriter<int, kQueueSize> writer(kQueueName);
  ASSERT_TRUE(writer.Write(42));

  // Check that reader is not empty
  while (reader.Empty()) {
    std::this_thread::yield();
  }
  ASSERT_FALSE(reader.Empty());
}

TEST(ShmReaderWriterTest, ReadWrite) {
  constexpr size_t kQueueSize = 1024;
  constexpr size_t kNumMessages = 1000;
  constexpr char kQueueName[] = "test_queue";

  // Create writer process
  ShmWriter<int, kQueueSize> writer(kQueueName);
  for (size_t i = 0; i < kNumMessages; ++i) {
    ASSERT_TRUE(writer.Write(i));
  }

  // Create reader process
  ShmReader<int, kQueueSize> reader(kQueueName);
  for (size_t i = 0; i < kNumMessages; ++i) {
    int value;
    while (!reader.Read(value)) {
      std::this_thread::yield();
    }
    ASSERT_EQ(value, i);
  }
}

} // namespace
