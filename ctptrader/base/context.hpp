#pragma once

#include <vector>

#include <boost/circular_buffer.hpp>

#include <base/def.hpp>
#include <base/msg.hpp>

namespace ctptrader::base {
template <typename T, size_t N> class DataCenter {
public:
  DataCenter(size_t size)
      : data_(size, boost::circular_buffer<T>(N)) {}

  bool HasValue(ID id) const { return data_[id].size() > 0; }
  bool HasPrev(ID id) const { return data_[id].size() > 1; }
  void Push(ID id, const T &value) { data_[id].push_back(value); }
  const T &Back(ID id) const { return data_[id].back(); }
  const T &Prev(ID id) const { return data_[id][data_[id].size() - 2]; }
  size_t Size(ID id) const { return data_[id].size(); }

private:
  std::vector<boost::circular_buffer<T>> data_;
};

class Context {
public:
  Context(const std::vector<std::string> &symbols)
      : symbols_(symbols)
      , static_data_(symbols.size())
      , bar_data_(symbols.size())
      , depth_data_(symbols.size()) {
    for (size_t i = 0; i < symbols.size(); ++i) {
      symbol_to_id_[symbols[i]] = i;
    }
  }

  ID GetID(const std::string &symbol) const {
    auto it = symbol_to_id_.find(symbol);
    if (it == symbol_to_id_.end()) {
      return -1;
    }
    return it->second;
  }

  const std::vector<std::string> &GetSymbols() const { return symbols_; }
  
  const Static &GetStatic(ID id) const { return static_data_[id]; }

  const DataCenter<Bar, 240> &GetBarData() const { return bar_data_; }

  const DataCenter<Depth, 2> &GetDepthData() const { return depth_data_; }

  void OnStatic(const Static &st) { static_data_[st.instrument_id_] = st; }

  void OnBar(const Bar &bar) { bar_data_.Push(bar.instrument_id_, bar); }

  void OnDepth(const Depth &depth) {
    depth_data_.Push(depth.instrument_id_, depth);
  }

private:
  const std::vector<std::string> symbols_;
  std::vector<Static> static_data_;
  DataCenter<Bar, 240> bar_data_;
  DataCenter<Depth, 2> depth_data_;
  std::unordered_map<std::string, size_t> symbol_to_id_;
};

} // namespace ctptrader::base