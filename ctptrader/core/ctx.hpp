#pragma once

#include <string_view>
#include <type_traits>
#include <vector>

#include <boost/circular_buffer.hpp>
#include <spdlog/spdlog.h>
#include <toml.hpp>

#include <base/msg.hpp>
#include <base/ref.hpp>

namespace ctptrader::core {

/**
 * @brief A circular buffer that stores data of type T with a fixed size N for
 * each instrument ID.
 *
 * @tparam T The type of data to be stored in the buffer. It must have an int
 * field instrument_id_.
 * @tparam N The fixed size of the circular buffer for each instrument ID.
 */
template <typename T, size_t N> class DataBuffer {
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

/**
 * @brief A class template for managing a collection of data objects with unique
 * IDs and names.
 *
 * @tparam T The type of data object to manage. It must have an int field id_
 * and a string field name_.
 */
template <typename T> class DataRef {
  static_assert(std::is_same<decltype(T::id_), base::ID>::value,
                "T must have an int field id_");
  static_assert(std::is_same<decltype(T::name_), std::string>::value,
                "T must have a string field name_");

public:
  bool HasValue(base::ID id) const { return id > 0 && id < Size(); }

  bool HasValue(const std::string &name) const {
    return id_map_.count(name) > 0;
  }

  base::ID GetID(const std::string &name) const {
    auto it = id_map_.find(name);
    if (it == id_map_.end()) {
      return -1;
    } else {
      return it->second;
    }
  }

  const T &Get(base::ID id) const { return vec_[id]; }

  const T &Get(const std::string &name) const { return Get(GetID(name)); }

  int Size() const { return vec_.size(); }

  bool LoadFromCsv(std::string_view filename);

  friend class Context;

private:
  std::vector<T> vec_;
  std::unordered_map<std::string, base::ID> id_map_;
};

/**
 * @brief The Context class represents the trading context, which contains
 * various data buffers and references.
 */
class Context {
public:
  Context()
      : logger_(spdlog::default_logger()) {}

  /**
   * @brief Initializes the context with the specified data folder.
   *
   * @param data_folder The path to the data folder.
   * @return true if initialization succeeds, false otherwise.
   */
  bool Init(std::string_view config_path);

  /**
   * @brief Callback function for handling static data.
   *
   * @param st The static data to be handled.
   */
  void OnStatic(const base::Static &st) { st_buf_.Push(st); }

  /**
   * @brief Callback function for receiving market depth data.
   *
   * @param depth The market depth data received.
   */
  void OnDepth(const base::Depth &depth) { depth_buf_.Push(depth); }

  /**
   * @brief Callback function for receiving bar data.
   *
   * @param bar The bar data received.
   */
  void OnBar(const base::Bar &bar) { bar_buf_.Push(bar); }

  /**
   * @brief Callback function for balance update.
   *
   * @param bal The updated balance information.
   */
  void OnBalance(const base::Balance &bal) { bal_buf_.Push(bal); }

  /**
   * @brief Returns the static data buffer.
   *
   * @return The static data buffer.
   */
  const DataBuffer<base::Static, 1> &StaticBuf() const { return st_buf_; }

  /**
   * @brief Returns the bar data buffer.
   *
   * @return The bar data buffer.
   */
  const DataBuffer<base::Bar, 240> &BarBuf() const { return bar_buf_; }

  /**
   * @brief Returns the depth data buffer.
   *
   * @return The depth data buffer.
   */
  const DataBuffer<base::Depth, 2> &DepthBuf() const { return depth_buf_; }

  /**
   * @brief Returns the balance data buffer.
   *
   * @return The balance data buffer.
   */
  const DataBuffer<base::Balance, 2> &BalanceBuf() const { return bal_buf_; }

  /**
   * @brief Returns the account data reference.
   *
   * @return The account data reference.
   */
  const DataRef<base::Account> &AccountRef() const { return acc_ref_; }

  /**
   * @brief Returns a reference to the calendar date.
   *
   * @return The calendar date reference.
   */
  const DataRef<base::CalendarDate> &CalendarRef() const { return cal_ref_; }

  /**
   * @brief Returns the underlying data reference.
   *
   * @return The underlying data reference.
   */
  const DataRef<base::Underlying> &UnderlyingRef() const { return uly_ref_; }

  /**
   * @brief Returns the instrument data reference.
   *
   * @return The instrument data reference.
   */
  const DataRef<base::Instrument> &InstrumentRef() const { return ins_ref_; }

  const std::shared_ptr<spdlog::logger> Logger() const { return logger_; }

private:
  std::shared_ptr<spdlog::logger> logger_;
  DataBuffer<base::Static, 1> st_buf_;
  DataBuffer<base::Bar, 240> bar_buf_;
  DataBuffer<base::Depth, 2> depth_buf_;
  DataBuffer<base::Balance, 2> bal_buf_;
  DataRef<base::Instrument> ins_ref_;
  DataRef<base::Underlying> uly_ref_;
  DataRef<base::Account> acc_ref_;
  DataRef<base::CalendarDate> cal_ref_;
};

} // namespace ctptrader::core