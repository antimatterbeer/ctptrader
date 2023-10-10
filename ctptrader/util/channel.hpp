#pragma once

#include <string_view>
#include <tuple>

#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/lockfree/spsc_queue.hpp>

namespace ctptrader::util {

namespace bip = boost::interprocess;

template <typename T, size_t Size> class ShmSpscQueue {
  using QueueType =
      boost::lockfree::spsc_queue<T, boost::lockfree::capacity<Size>>;
  using AllocatorType =
      bip::allocator<T, bip::managed_shared_memory::segment_manager>;

public:
  ShmSpscQueue(std::string_view name, size_t size, bool readonly)
      : name_(name), segment_(CreateSegment(name, size, readonly)),
        allocator_(segment_->get_segment_manager()),
        queue_(segment_->find_or_construct<QueueType>("queue")()) {}
  ~ShmSpscQueue() { bip::shared_memory_object::remove(name_.c_str()); }

private:
  static std::unique_ptr<bip::managed_shared_memory>
  CreateSegment(std::string_view name, size_t size, bool readonly) {
    if (readonly) {
      return std::make_unique<bip::managed_shared_memory>(bip::open_only,
                                                          name.data());
    } else {
      return std::make_unique<bip::managed_shared_memory>(bip::open_or_create,
                                                          name.data(), size);
    }
  }

protected:
  const std::string name_;
  std::unique_ptr<bip::managed_shared_memory> segment_;
  AllocatorType allocator_;
  QueueType *queue_;
};

template <typename T, size_t Size>
class ShmSpscReader : private ShmSpscQueue<T, Size> {
public:
  ShmSpscReader(std::string_view name)
      : ShmSpscQueue<T, Size>(name, Size * sizeof(T) * 2, false) {}
  bool Read(T &value) { return this->queue_->pop(value); }
  bool Empty() const { return this->queue_->empty(); }
};

template <typename T, size_t Size>
class ShmSpscWriter : private ShmSpscQueue<T, Size> {
public:
  ShmSpscWriter(std::string_view name)
      : ShmSpscQueue<T, Size>(name, Size * sizeof(T) * 2, false) {}
  bool Write(const T &value) { return this->queue_->push(value); }
  bool Full() const { return this->queue_->full(); }
};

} // namespace ctptrader::util