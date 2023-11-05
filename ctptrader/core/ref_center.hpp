#pragma once

#include <type_traits>
#include <unordered_map>
#include <vector>

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
  /**
   * @brief Checks if the given ID is valid.
   *
   * @param id The ID to check.
   * @return true if the ID is valid, false otherwise.
   */
  bool HasID(base::ID id) const { return id > 0 && id < Count(); }

  /**
   * Checks if a reference with the given name exists in the reference center.
   *
   * @param name The name of the reference to check for.
   * @return True if a reference with the given name exists, false otherwise.
   */
  bool HasName(const std::string &name) const {
    return id_map_.count(name) > 0;
  }

  /**
   * @brief Retrieves the ID associated with the given name.
   *
   * @param name The name to retrieve the ID for.
   * @return The ID associated with the given name.
   */
  base::ID GetID(const std::string &name) const {
    auto it = id_map_.find(name);
    if (it == id_map_.end()) {
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

} // namespace ctptrader::core