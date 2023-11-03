#include <string_view>

#include <toml.hpp>

#include <app/market.hpp>
#include <app/strategy.hpp>

using namespace ctptrader;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " <config_file>\n";
    return 1;
  }
  toml::table config;
  config = toml::parse_file(argv[1]);
  auto global_config = *config["global"].as_table();
  auto market_config = *config["market"].as_table();
  auto strategy_config = *config["strategy"].as_table();
  auto pid = fork();
  if (pid == 0) {
    app::start_strategy(global_config, strategy_config);
  } else {
    app::start_market(global_config, market_config);
  }
  return 0;
}
