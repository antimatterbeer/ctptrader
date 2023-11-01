#include <unistd.h>
#include <variant>

#include <toml.hpp>

#include <base/context.hpp>
#include <base/msg.hpp>
#include <base/stg.hpp>

using namespace ctptrader;

class Runner {
public:
  ~Runner() {
    for (auto s : strategies_) {
      delete s;
    }
  }

  void Init(const std::string &folder_path) {
    ctx_.Init(folder_path);
    for (auto s : strategies_) {
      s->SetContext(&ctx_);
      s->Init();
    }
  }

  void AddStrategy(base::IStrategy *s) { strategies_.push_back(s); }

  void OnStatic(const base::Static &st) {
    for (auto s : strategies_) {
      if (s->IsInstrumentInterested(st.id_))
        s->OnStatic(st);
    }
    ctx_.OnStatic(st);
  }

  void OnBar(const base::Bar &bar) {
    for (auto s : strategies_) {
      if (s->IsInstrumentInterested(bar.id_))
        s->OnBar(bar);
    }
    ctx_.OnBar(bar);
  }

  void OnBalance(const base::Balance &bal) {
    for (auto s : strategies_) {
      if (s->IsAccountInterested(bal.id_))
        s->OnBalance(bal);
    }
    ctx_.OnBalance(bal);
  }

private:
  base::Context ctx_;
  std::vector<base::IStrategy *> strategies_;
};

int main(int argc, char *argv[]) { return 0; }