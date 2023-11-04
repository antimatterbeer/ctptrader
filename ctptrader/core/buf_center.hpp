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
  /**
   * @brief Resizes the buffer to the specified size.
   *
   * @param size The new size of the buffer.
   */
  void Resize(size_t size) {
    buffer_.resize(size, boost::circular_buffer<T>(N));
  }

  /**
   * @brief Checks if a value with the given ID exists in the buffer.
   *
   * @param id The ID of the value to check for.
   * @return true if a value with the given ID exists in the buffer, false
   * otherwise.
   */
  [[nodiscard]] bool HasValue(base::ID id) const {
    return buffer_[id].size() > 0;
  }

  /**
   * @brief Checks if there is a previous element in the buffer for the given
   * ID.
   *
   * @param id The ID to check for a previous element.
   * @return True if there is a previous element, false otherwise.
   */
  [[nodiscard]] bool HasPrev(base::ID id) const {
    return buffer_[id].size() > 1;
  }

  /**
   * @brief Pushes a value into the buffer with the corresponding ID.
   *
   * @param value The value to be pushed into the buffer.
   */
  void Push(const T &value) { buffer_[value.id_].push_back(value); }

  /**
   * @brief Returns a reference to the last element in the buffer with the
   * specified ID.
   *
   * @param id The ID of the buffer to retrieve the last element from.
   * @return const T& A reference to the last element in the buffer.
   */
  [[nodiscard]] const T &Back(base::ID id) const { return buffer_[id].back(); }

  /**
   * Returns a const reference to the previous element in the buffer for the
   * given ID.
   *
   * @param id The ID of the element to get the previous element for.
   * @return A const reference to the previous element in the buffer for the
   * given ID.
   */
  [[nodiscard]] const T &Prev(base::ID id) const {
    return buffer_[id][buffer_[id].size() - 2];
  }

  /**
   * @brief Returns a const reference to the nth element in the buffer with the
   * given ID.
   *
   * @param id The ID of the buffer to get the nth element from.
   * @param n The index of the element to get.
   * @return A const reference to the nth element in the buffer with the given
   * ID.
   */
  [[nodiscard]] const T &Nth(base::ID id, size_t n) const {
    return buffer_[id][n];
  }

  /**
   * @brief Returns a const reference to the nth element in the buffer with the
   * given ID, counting from the back.
   *
   * @param id The ID of the buffer to get the nth element from.
   * @param n The index of the element to get, counting from the back.
   * @return A const reference to the nth element in the buffer with the given
   * ID, counting from the back.
   */
  [[nodiscard]] const T &ReverseNth(base::ID id, size_t n) const {
    return buffer_[id][buffer_[id].size() - n - 1];
  }

  const auto Iterator(base::ID id) const { return buffer_[id].begin(); }

  /**
   * @brief Returns the size of the buffer with the given ID.
   *
   * @param id The ID of the buffer to get the size of.
   * @return The size of the buffer with the given ID.
   */
  [[nodiscard]] size_t Size(base::ID id) const { return buffer_[id].size(); }

  /**
   * @brief Returns the maximum number of elements that the buffer can hold.
   *
   * @return The maximum number of elements that the buffer can hold.
   */
  [[nodiscard]] size_t Capacity() const { return N; }

  /**
   * @brief Returns the number of elements in the buffer.
   *
   * @return The number of elements in the buffer.
   */
  [[nodiscard]] size_t Count() const { return buffer_.size(); }

private:
  std::vector<boost::circular_buffer<T>> buffer_;
};

} // namespace ctptrader::core