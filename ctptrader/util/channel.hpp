#pragma once

#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/lockfree/spsc_queue.hpp>

#include <util/readerwriterqueue.h>

namespace ctptrader::util {

namespace bip = boost::interprocess;

template <typename T, size_t Size> class ShmSpscQueue {
  using QueueType =
      boost::lockfree::spsc_queue<T, boost::lockfree::capacity<Size>>;
  using AllocatorType =
      bip::allocator<T, bip::managed_shared_memory::segment_manager>;

public:
  ShmSpscQueue(const std::string_view name, size_t size, bool readonly)
      : name_(name)
      , segment_(CreateSegment(name, size, readonly))
      , allocator_(segment_->get_segment_manager())
      , queue_(segment_->find_or_construct<QueueType>("queue")()) {}
  ~ShmSpscQueue() { bip::shared_memory_object::remove(name_.c_str()); }

private:
  static std::unique_ptr<bip::managed_shared_memory>
  CreateSegment(const std::string_view name, size_t size, bool readonly) {
    if (readonly) {
      return std::make_unique<bip::managed_shared_memory>(bip::open_only,
                                                          name.data());
    }
    return std::make_unique<bip::managed_shared_memory>(bip::open_or_create,
                                                        name.data(), size);
  }

protected:
  const std::string name_;
  std::unique_ptr<bip::managed_shared_memory> segment_;
  AllocatorType allocator_;
  QueueType *queue_;
};

template <typename T, size_t Size> class ShmSpscReader : ShmSpscQueue<T, Size> {
public:
  explicit ShmSpscReader(const std::string_view name)
      : ShmSpscQueue<T, Size>(name, Size * sizeof(T) * 2, false) {}
  bool Read(T &value) { return this->queue_->pop(value); }
  [[nodiscard]] bool Empty() const { return this->queue_->empty(); }
};

template <typename T, size_t Size> class ShmSpscWriter : ShmSpscQueue<T, Size> {
public:
  explicit ShmSpscWriter(const std::string_view name)
      : ShmSpscQueue<T, Size>(name, Size * sizeof(T) * 2, false) {}
  bool Write(const T &value) { return this->queue_->push(value); }
};

template <typename T, size_t N> class ShmReaderWriterQueue {
  using QueueType = moodycamel::ReaderWriterQueue<T>;
  using AllocatorType =
      bip::allocator<T, bip::managed_shared_memory::segment_manager>;

public:
  explicit ShmReaderWriterQueue(const std::string_view name, size_t size,
                                bool readonly)
      : name_(name)
      , segment_(CreateSegment(name, size, readonly))
      , allocator_(segment_->get_segment_manager())
      , queue_(segment_->find_or_construct<QueueType>("queue")(size)) {}
  ~ShmReaderWriterQueue() { bip::shared_memory_object::remove(name_.c_str()); }

private:
  static std::unique_ptr<bip::managed_shared_memory>
  CreateSegment(const std::string_view name, size_t size, bool readonly) {
    if (readonly) {
      return std::make_unique<bip::managed_shared_memory>(bip::open_only,
                                                          name.data());
    }
    return std::make_unique<bip::managed_shared_memory>(bip::open_or_create,
                                                        name.data(), size);
  }

protected:
  const std::string name_;
  std::unique_ptr<bip::managed_shared_memory> segment_;
  AllocatorType allocator_;
  QueueType *queue_;
};

template <typename T, size_t N> class ShmReader : ShmReaderWriterQueue<T, N> {
public:
  explicit ShmReader(const std::string_view name)
      : ShmReaderWriterQueue<T, N>(name, N * sizeof(T) * 2, false) {}
  bool Read(T &value) { return this->queue_->try_dequeue(value); }
  [[nodiscard]] bool Empty() const { return this->queue_->size_approx() == 0; }
};

template <typename T, size_t N> class ShmWriter : ShmReaderWriterQueue<T, N> {
public:
  explicit ShmWriter(const std::string_view name)
      : ShmReaderWriterQueue<T, N>(name, N * sizeof(T) * 2, false) {}
  bool Write(const T &value) { return this->queue_->try_enqueue(value); }
};

} // namespace ctptrader::util