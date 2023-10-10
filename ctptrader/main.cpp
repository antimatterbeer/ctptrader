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
  auto data_folder = (*config)["data_folder"].value<std::string>();
  if (!data_folder.has_value()) {
    std::cout << "Error: data_folder is not specified" << std::endl;
    return 1;
  }

  auto pid = fork();
  if (pid == 0) {
    DataService().Init(data_folder.value());
    app::StgManager sm("shm_ctptrader_md");
    sm.Init(*config);
    sm.Start();
    return 0;
  } else {
    DataService().Init(data_folder.value());
    app::CtpMG mg("shm_ctptrader_md");
    mg.Init(*config);
    mg.Start();
  }
  return 0;
}