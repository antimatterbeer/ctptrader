#include <unistd.h>

#include <variant>

#include <toml.hpp>

#include <app/ctpMG.hpp>
#include <app/stgManager.hpp>
#include <base/msg.hpp>
#include <base/stg.hpp>

using namespace ctptrader;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " config.toml" << std::endl;
    return 1;
  }

  auto res = toml::parse_file(argv[1]);
  if (!res.is_table()) {
    std::cout << "Error: " << argv[1] << " is not a valid toml file"
              << std::endl;
    return 1;
  }
  auto config = res.as_table();
  auto data_folder = (*config)["data_folder"].value_or<std::string>("");
  auto market_channel = (*config)["market_channel"].value_or<std::string>("");
  if (data_folder.empty() || market_channel.empty()) {
    std::cerr << "Error: data_folder or market_channel is not specified"
              << std::endl;
  }
  DataService().Init(data_folder);
  auto pid = fork();
  if (pid == 0) {
    app::StgManager sm(market_channel);
    sm.Init(*(*config)["stgManager"].as_table());
    sm.Start();
    return 0;
  } else {
    app::CtpMG mg(market_channel);
    mg.Init(*(*config)["ctpMG"].as_table());
    mg.Start();
  }
  return 0;
}