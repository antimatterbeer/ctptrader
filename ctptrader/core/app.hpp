#pragma once

#include <boost/noncopyable.hpp>

#include <core/ctx.hpp>

namespace ctptrader::core {

/**
 * @brief Interface for an application.
 */
class IApp : public boost::noncopyable {
public:
  virtual ~IApp() = default;

  /**
   * @brief Initializes the application.
   * @param global_config The global configuration.
   * @param app_config The application configuration.
   * @return True if initialization succeeded, false otherwise.
   */
  virtual bool Init(toml::table &global_config, toml::table &app_config) = 0;

  /**
   * @brief Runs the application.
   */
  virtual void Run() = 0;

protected:
  Context ctx_; /**< The context of the application. */
};

}; // namespace ctptrader::core