#pragma once

#include <concepts>
#include <queue>

#include <base/msg.hpp>
#include <base/timestamp.hpp>

namespace ctptrader::core {

template <typename T>
concept HasUpdateTime = requires(T t) {
  { t.update_time_ } -> std::same_as<base::Timestamp>;
};

class IReader {
public:
  class Compare {
  public:
    bool operator()(const IReader *lhs, const IReader *rhs) const noexcept {
      return lhs->NextUpdateTime() > rhs->NextUpdateTime();
    }
  };
  virtual ~IReader() = default;
  [[nodiscard]] virtual base::Timestamp NextUpdateTime() const = 0;
  [[nodiscard]] virtual bool Empty() const = 0;
  [[nodiscard]] virtual base::Msg Read() = 0;
};

class MsgIter {
public:
  [[nodiscard]] bool Empty() const { return readers_.empty(); }
  [[nodiscard]] base::Msg Next() {
    auto reader = readers_.top();
    readers_.pop();
    auto msg = reader->Read();
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