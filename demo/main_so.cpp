#include "logger.hpp"

using namespace ctptrader::demo;

extern "C" {

Logger::Ptr creator() { return new Logger(); }

void deleter(Logger::Ptr p) { delete p; }
}