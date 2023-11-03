#pragma once

#include <unistd.h>

#include <boost/circular_buffer.hpp>

#include <base/def.hpp>

namespace ctptrader::core {

/**
 * @brief A circular buffer that stores data of type T with a fixed size N for
 * each instrument ID.
 *
 * @tparam T The type of data to be stored in the buffer. It must have an int
 * field instrument_id_.
 * @tparam N The fixed size of the circular buffer for each instrument ID.
 */
template <typename T, size_t N> class BufCenter {
  static_assert(std::is_same<decltype(T::id_), base::ID>::value,
                "T must have an int field instrument_id_");

public:
  void Resize(size_t size) {
    buffer_.resize(size, boost::circular_buffer<T>(N));
  }

  [[nodiscard]] bool HasValue(base::ID id) const {
    return buffer_[id].size() > 0;
  }

  [[nodiscard]] bool HasPrev(base::ID id) const {
    return buffer_[id].size() > 1;
  }

  void Push(const T &value) { buffer_[value.id_].push_back(value); }

  [[nodiscard]] const T &Back(base::ID id) const { return buffer_[id].back(); }

  [[nodiscard]] const T &Prev(base::ID id) const {
    return buffer_[id][buffer_[id].size() - 2];
  }

  [[nodiscard]] size_t Size() const { return buffer_.size(); }

private:
  std::vector<boost::circular_buffer<T>> buffer_;
};

} // namespace ctptrader::core