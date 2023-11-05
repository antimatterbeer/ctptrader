#include <filesystem>

#include <core/ctx.hpp>

namespace ctptrader::core {

bool Context::Init(std::string_view data_folder) {
  std::filesystem::path folder(data_folder);
  if (!std::filesystem::is_directory(folder)) {
    std::cerr << "Data folder not found: " << folder << std::endl;
    return false;
  }

  if (!cal_center_.LoadFromCsv((folder / "calendar.csv").string())) {
    std::cerr << "Failed to load calendar from csv file: " << folder
              << std::endl;
    return false;
  }

  if (!acc_center_.LoadFromCsv((folder / "account.csv").string())) {
    std::cerr << "Failed to load account from csv file: " << folder
              << std::endl;
    return false;
  }
  if (!uly_center_.LoadFromCsv((folder / "underlying.csv").string())) {
    std::cerr << "Failed to load underlying from csv file: " << folder
              << std::endl;
    return false;
  }
  if (!ins_center_.LoadFromCsv((folder / "instrument.csv").string())) {
    std::cerr << "Failed to load instrument from csv file: " << folder
              << std::endl;
    return false;
  }
  for (auto i = 0; i < ins_center_.Count(); ++i) {
    auto uly_id = uly_center_.GetID(ins_center_.Get(i).underlying_);
    ins_center_.vec_[i].underlying_id_ = uly_id;
  }

  st_center_.Resize(ins_center_.Count());
  bar_center_.Resize(ins_center_.Count());
  depth_center_.Resize(ins_center_.Count());
  bal_center_.Resize(acc_center_.Count());
  return true;
}

} // namespace ctptrader::core