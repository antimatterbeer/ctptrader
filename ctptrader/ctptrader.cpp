#include <string_view>
#include <variant>
#include <vector>

#include <toml.hpp>

#include <base/msg.hpp>
#include <core/ctx.hpp>
#include <core/stg.hpp>
#include <util/proxy.hpp>

using namespace ctptrader;

class CtpTrader {
public:
  ~CtpTrader() {}

  bool Init(std::string_view config_path) {
    auto res = toml::parse_file(config_path);
    if (!res.is_table()) {
      std::cerr << "Failed to parse config file.\n";
      return false;
    }
    auto config = res.as_table();
    auto data_folder = (*config)["data_folder"].value<std::string>();
    if (!data_folder.has_value()) {
      std::cerr << "Failed to get data folder.\n";
      return false;
    }
    if (!ctx_.Init(data_folder.value())) {
      std::cerr << "Failed to initialize context.\n";
      return false;
    }
    auto stgs = (*config)["stg"].as_array();
    for (const auto &c : *stgs) {
      auto tb = c.as_table();
      auto name = (*tb)["name"].value<std::string>();
      auto libpath = (*tb)["libpath"].value<std::string>();
      stgs_.emplace_back(name.value(), libpath.value());
      stgs_.back().Instance().SetContext(&ctx_);
      stgs_.back().Instance().Init(*tb);
      ctx_.Logger()->info("Loaded strategy: {}", name.value());
    }
    return true;
  }

  void OnMsg(const base::Msg &msg) { std::visit(*this, msg); }

  void operator()(const base::Static &st) {
    for (auto &s : stgs_) {
      if (s.Instance().WatchesInstrument(st.id_)) {
        s.Instance().OnStatic(st);
      }
    }
    ctx_.OnStatic(st);
  }

  void operator()(const base::Bar &bar) {
    for (auto &s : stgs_) {
      if (s.Instance().WatchesInstrument(bar.id_)) {
        s.Instance().OnBar(bar);
      }
    }
    ctx_.OnBar(bar);
  }

  void operator()(const base::Depth &depth) {
    for (auto &s : stgs_) {
      if (s.Instance().WatchesInstrument(depth.id_)) {
        s.Instance().OnDepth(depth);
      }
    }
    ctx_.OnDepth(depth);
  }

  void operator()(const base::Balance &bal) {
    for (auto &s : stgs_) {
      if (s.Instance().WatchesAccount(bal.id_)) {
        s.Instance().OnBalance(bal);
      }
    }
    ctx_.OnBalance(bal);
  }

private:
  core::Context ctx_;
  std::vector<util::Proxy<core::IStrategy>> stgs_;
};

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " <config_file>\n";
    return 1;
  }
  CtpTrader trader;
  if (!trader.Init(argv[1])) {
    return 1;
  }
  base::Balance bal{0, 1000000, 1000000};
  trader.OnMsg(bal);
  return 0;
}