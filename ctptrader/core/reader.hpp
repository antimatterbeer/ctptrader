#pragma once

#include <concepts>
#include <queue>

#include <base/msg.hpp>
#include <base/timestamp.hpp>

namespace ctptrader::core {

class IReader {
public:
  class Compare {
  public:
    bool operator()(const IReader *lhs, const IReader *rhs) const noexcept {
      return lhs->PeekTimestamp() > rhs->PeekTimestamp();
    }
  };
  virtual ~IReader() = default;
  [[nodiscard]] virtual base::Timestamp PeekTimestamp() const = 0;
  [[nodiscard]] virtual bool Empty() const = 0;
  [[nodiscard]] virtual base::Msg Pop() = 0;
};

class MsgIter {
public:
  [[nodiscard]] bool Empty() const { return readers_.empty(); }
  [[nodiscard]] base::Msg Next() {
    auto reader = readers_.top();
    readers_.pop();
    auto msg = reader->Pop();
    if (reader->Empty()) {
      delete reader;
    } else {
      readers_.push(reader);
    }
    return msg;
  }
  void AddReader(IReader *reader) { readers_.push(reader); }

private:
  std::priority_queue<IReader *, std::vector<IReader *>, IReader::Compare>
      readers_;
};

} // namespace ctptrader::core