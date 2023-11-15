#pragma once

#include <concepts>
#include <string_view>

#include <boost/circular_buffer.hpp>
#include <spdlog/spdlog.h>
#include <toml.hpp>

#include <base/msg.hpp>
#include <base/ref.hpp>

namespace ctptrader::core {

template <typename T>
concept HasID = requires(T t) {
  { t.id_ } -> std::convertible_to<base::ID>;
};

template <typename T>
concept HasNameAndID = requires(T t) {
  { t.id_ } -> std::convertible_to<base::ID>;
  { t.name_ } -> std::convertible_to<std::string_view>;
};

/**
 * @brief A circular buffer that stores data of type T with a fixed size N for
 * each ID.
 *
 * @tparam T The type of data to be stored in the buffer. It must have an int
 * field id_.
 * @tparam N The fixed size of the circular buffer for each ID.
 */
template <HasID T, size_t N> class BufCenter {
public:
  /**
   * @brief Resizes the buffer to the specified size.
   *
   * @param size The new size of the buffer.
   */
  void Resize(size_t size) {
    chunks_.resize(size, boost::circular_buffer<T>(N));
  }

  /**
   * @brief Checks if a value with the given ID exists in the buffer.
   *
   * @param id The ID of the value to check for.
   * @return true if a value with the given ID exists in the buffer, false
   * otherwise.
   */
  [[nodiscard]] bool HasValue(base::ID id) const {
    return chunks_[id].size() > 0;
  }

  /**
   * @brief Checks if there is a previous element in the buffer for the given
   * ID.
   *
   * @param id The ID to check for a previous element.
   * @return True if there is a previous element, false otherwise.
   */
  [[nodiscard]] bool HasPrev(base::ID id) const {
    return chunks_[id].size() > 1;
  }

  /**
   * @brief Pushes a value into the buffer with the corresponding ID.
   *
   * @param value The value to be pushed into the buffer.
   */
  void PushBack(const T &value) { chunks_[value.id_].push_back(value); }

  /**
   * @brief Returns a reference to the last element in the buffer with the
   * specified ID.
   *
   * @param id The ID of the buffer to retrieve the last element from.
   * @return const T& A reference to the last element in the buffer.
   */
  [[nodiscard]] const T &Back(base::ID id) const { return chunks_[id].back(); }

  /**
   * Returns a const reference to the previous element in the buffer for the
   * given ID.
   *
   * @param id The ID of the element to get the previous element for.
   * @return A const reference to the previous element in the buffer for the
   * given ID.
   */
  [[nodiscard]] const T &Prev(base::ID id) const {
    return chunks_[id][chunks_[id].size() - 2];
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
    return chunks_[id][n];
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
    return chunks_[id][chunks_[id].size() - n - 1];
  }

  /**
   * @brief Returns the size of the buffer with the given ID.
   *
   * @param id The ID of the buffer to get the size of.
   * @return The size of the buffer with the given ID.
   */
  [[nodiscard]] size_t Size(base::ID id) const { return chunks_[id].size(); }

  /**
   * @brief Returns the maximum number of elements that the buffer can hold.
   *
   * @return The maximum number of elements that the buffer can hold.
   */
  [[nodiscard]] static size_t Capacity() { return N; }

  /**
   * @brief Returns the number of chunks in the buffer center.
   *
   * @return The number of chunks in the buffer center.
   */
  [[nodiscard]] size_t Count() const { return chunks_.size(); }

private:
  std::vector<boost::circular_buffer<T>> chunks_;
};

/**
 * @brief A class template for managing a collection of data objects with unique
 * IDs and names.
 *
 * @tparam T The type of data object to manage. It must have an int field id_
 * and a string field name_.
 */
template <HasNameAndID T> class RefCenter {

public:
  /**
   * @brief Checks if the given ID is valid.
   *
   * @param id The ID to check.
   * @return true if the ID is valid, false otherwise.
   */
  bool HasID(base::ID id) const { return id >= 0 && id < Count(); }

  /**
   * Checks if a reference with the given name exists in the reference center.
   *
   * @param name The name of the reference to check for.
   * @return True if a reference with the given name exists, false otherwise.
   */
  bool HasName(const std::string &name) const { return id_map_.contains(name); }

  /**
   * @brief Retrieves the ID associated with the given name.
   *
   * @param name The name to retrieve the ID for.
   * @return The ID associated with the given name.
   */
  base::ID GetID(const std::string &name) const {
    if (const auto it = id_map_.find(name); it == id_map_.end()) {
      return -1;
    } else {
      return it->second;
    }
  }

  /**
   * @brief Returns a constant reference to the object with the specified ID.
   *
   * @param id The ID of the object to retrieve.
   * @return const T& A constant reference to the object with the specified ID.
   */
  const T &Get(base::ID id) const { return vec_[id]; }

  /**
   * @brief Returns a reference to the object with the specified name.
   *
   * This function retrieves the object with the specified name from the
   * reference center and returns a reference to it. If the object does not
   * exist, a std::out_of_range exception is thrown.
   *
   * @param name The name of the object to retrieve.
   * @return const T& A reference to the object with the specified name.
   * @throws std::out_of_range If the object with the specified name does not
   * exist.
   */
  const T &Get(const std::string &name) const { return Get(GetID(name)); }

  /**
   * Returns the number of elements in the vector.
   *
   * @return The number of elements in the vector.
   */
  int Count() const { return vec_.size(); }

  /**
   * @brief Loads data from a CSV file.
   *
   * This function reads data from a CSV file with the given filename and loads
   * it into the reference center.
   *
   * @param filename The name of the CSV file to load.
   * @return True if the data was successfully loaded, false otherwise.
   */
  bool LoadFromCsv(std::string_view filename);

  friend class Context;

private:
  std::vector<T> vec_;
  std::unordered_map<std::string, base::ID> id_map_;
};

class Clock {
public:
  Clock() { last_ts_ = base::Timestamp::Now(); }
  void Init(bool sim_test) { sim_test_ = sim_test; }
  void SetTime(const base::Timestamp &ts) { last_ts_ = ts; }
  base::Timestamp Now() {
    return sim_test_ ? last_ts_ : base::Timestamp::Now();
  }

private:
  bool sim_test_{false};
  base::Timestamp last_ts_;
};

using StaticCenter = BufCenter<base::Static, 1>;

using BarCenter = BufCenter<base::Bar, 240>;

using DepthCenter = BufCenter<base::Depth, 2>;

using BalanceCenter = BufCenter<base::Balance, 2>;

using InstrumentCenter = RefCenter<base::Instrument>;

using UnderlyingCenter = RefCenter<base::Underlying>;

using AccountCenter = RefCenter<base::Account>;

using CalendarCenter = RefCenter<base::CalendarDate>;

/**
 * @brief The Context class represents the trading context, which contains
 * various data buffers and references.
 */
class Context {
public:
  Context()
      : logger_(spdlog::default_logger()) {}

  /**
   * @brief Initializes the context with the specified configuration file.
   *
   * @param config_path The path to the configuration file.
   * @return true if initialization succeeds, false otherwise.
   */
  bool Init(std::string_view config_path);

  /**
   * @brief Callback function for handling static data.
   *
   * @param st The static data to be handled.
   */
  void OnStatic(const base::Static &st) { st_center_.PushBack(st); }

  /**
   * @brief Callback function for receiving market depth data.
   *
   * @param depth The market depth data received.
   */
  void OnDepth(const base::Depth &depth) { depth_center_.PushBack(depth); }

  /**
   * @brief Callback function for receiving bar data.
   *
   * @param bar The bar data received.
   */
  void OnBar(const base::Bar &bar) { bar_center_.PushBack(bar); }

  /**
   * @brief Callback function for balance update.
   *
   * @param bal The updated balance information.
   */
  void OnBalance(const base::Balance &bal) { bal_center_.PushBack(bal); }

  /**
   * @brief Returns the static data buffer.
   *
   * @return The static data buffer.
   */
  const StaticCenter &GetStaticCenter() const { return st_center_; }

  /**
   * @brief Returns the bar data buffer.
   *
   * @return The bar data buffer.
   */
  const BarCenter &GetBarCenter() const { return bar_center_; }

  /**
   * @brief Returns the depth data buffer.
   *
   * @return The depth data buffer.
   */
  const DepthCenter &GetDepthCenter() const { return depth_center_; }

  /**
   * @brief Returns the balance data buffer.
   *
   * @return The balance data buffer.
   */
  const BalanceCenter &GetBalanceCenter() const { return bal_center_; }

  /**
   * @brief Returns the account data reference.
   *
   * @return The account data reference.
   */
  const AccountCenter &GetAccountCenter() const { return acc_center_; }

  /**
   * @brief Returns a reference to the calendar date.
   *
   * @return The calendar date reference.
   */
  const CalendarCenter &GetCalendarCenter() const { return cal_center_; }

  /**
   * @brief Returns the underlying data reference.
   *
   * @return The underlying data reference.
   */
  const UnderlyingCenter &GetUnderlyingCenter() const { return uly_center_; }

  /**
   * @brief Returns the instrument data reference.
   *
   * @return The instrument data reference.
   */
  const InstrumentCenter &GetInstrumentCenter() const { return ins_center_; }

  std::shared_ptr<spdlog::logger> GetLogger() const { return logger_; }

  const Clock &GetClock() const { return clock_; }

private:
  std::shared_ptr<spdlog::logger> logger_;
  StaticCenter st_center_;
  BarCenter bar_center_;
  DepthCenter depth_center_;
  BalanceCenter bal_center_;
  InstrumentCenter ins_center_;
  UnderlyingCenter uly_center_;
  AccountCenter acc_center_;
  CalendarCenter cal_center_;
  Clock clock_;
};

} // namespace ctptrader::core