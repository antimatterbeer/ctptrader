#pragma once

#include <iostream>
#include <type_traits>
#include <vector>

#include <boost/circular_buffer.hpp>

#include <base/def.hpp>
#include <base/msg.hpp>
#include <base/ref.hpp>

namespace ctptrader::base {

template <typename T, size_t N> class DataBuffer {
  static_assert(std::is_same<decltype(T::id_), ID>::value,
                "T must have an int field instrument_id_");

public:
  void Resize(size_t size) {
    buffer_.resize(size, boost::circular_buffer<T>(N));
  }
  [[nodiscard]] bool HasValue(ID id) const { return buffer_[id].size() > 0; }
  [[nodiscard]] bool HasPrev(ID id) const { return buffer_[id].size() > 1; }
  void Push(const T &value) { buffer_[value.id_].push_back(value); }
  [[nodiscard]] const T &Back(ID id) const { return buffer_[id].back(); }
  [[nodiscard]] const T &Prev(ID id) const {
    return buffer_[id][buffer_[id].size() - 2];
  }

private:
  std::vector<boost::circular_buffer<T>> buffer_;
};

template <typename T> class DataRef {
  static_assert(std::is_same<decltype(T::id_), ID>::value,
                "T must have an int field id_");
  static_assert(std::is_same<decltype(T::name_), std::string>::value,
                "T must have a string field name_");

public:
  bool LoadFromCsv(const std::string &filename);
  bool HasValue(ID id) const { return id > 0 && id < Size(); }
  bool HasValue(const std::string &name) const {
    return id_map_.count(name) > 0;
  }
  ID GetID(const std::string &name) const {
    auto it = id_map_.find(name);
    if (it == id_map_.end()) {
      return -1;
    } else {
      return it->second;
    }
  }
  const T &Get(ID id) const { return vec_[id]; }
  const T &Get(const std::string &name) const { return Get(GetID(name)); }
  int Size() const { return vec_.size(); }

private:
  std::vector<T> vec_;
  std::unordered_map<std::string, ID> id_map_;
};

class Context {
public:
  bool Init(const std::string &folder_path) {
    if (!instrument_.LoadFromCsv(folder_path + "instrument.csv")) {
      std::cerr << "Failed to load instrument.csv.\n";
      return false;
    }
    if (!account_.LoadFromCsv(folder_path + "account.csv")) {
      std::cerr << "Failed to load account.csv.\n";
      return false;
    }
    static_.Resize(instrument_.Size());
    bar_.Resize(instrument_.Size());
    depth_.Resize(instrument_.Size());
    balance_.Resize(account_.Size());
    return true;
  }
  const DataBuffer<Static, 1> &StaticCenter() const { return static_; }
  const DataBuffer<Bar, 240> &BarCenter() const { return bar_; }
  const DataBuffer<Depth, 2> &DepthCenter() const { return depth_; }
  const DataBuffer<Balance, 2> &BalanceCenter() const { return balance_; }
  const DataRef<Instrument> &InstrumentCenter() const { return instrument_; }
  const DataRef<Account> &AccountCenter() const { return account_; }

  void OnStatic(const Static &st) { static_.Push(st); }
  void OnBar(const Bar &bar) { bar_.Push(bar); }
  void OnDepth(const Depth &depth) { depth_.Push(depth); }
  void OnBalance(const Balance &bal) { balance_.Push(bal); }

private:
  DataBuffer<Static, 1> static_;
  DataBuffer<Bar, 240> bar_;
  DataBuffer<Depth, 2> depth_;
  DataBuffer<Balance, 2> balance_;
  DataRef<Instrument> instrument_;
  DataRef<Account> account_;
};

} // namespace ctptrader::base