#pragma once

#include <unordered_map>
#include <vector>
#include <type_traits>

#include <base/def.hpp>

namespace ctptrader::core {

/**
 * @brief A class template for managing a collection of data objects with unique
 * IDs and names.
 *
 * @tparam T The type of data object to manage. It must have an int field id_
 * and a string field name_.
 */
template <typename T> class RefCenter {
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

} // namespace ctptrader::core