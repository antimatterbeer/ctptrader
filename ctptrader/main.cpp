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
    delete context_;
    for (auto strategy : strategies_) {
      delete strategy;
    }
  }

  void Init(const std::vector<std::string> &symbols) {
    context_ = new base::Context(symbols);
    for (auto strategy : strategies_) {
      strategy->SetContext(context_);
      strategy->Init();
    }
  }

  void AddStrategy(base::IStrategy *strategy) {
    strategies_.push_back(strategy);
  }

  void OnStatic(const base::Static &st) {
    for (auto strategy : strategies_) {
      if (strategy->IsInterested(st.instrument_id_))
        strategy->OnStatic(st);
    }
    context_->OnStatic(st);
  }

  void OnBar(const base::Bar &bar) {
    for (auto strategy : strategies_) {
      if (strategy->IsInterested(bar.instrument_id_))
        strategy->OnBar(bar);
    }
    context_->OnBar(bar);
  }

private:
  base::Context *context_;
  std::vector<base::IStrategy *> strategies_;
};

int main(int argc, char *argv[]) { return 0; }