#include "logger.hpp"

using namespace ctptrader::logger;

extern "C" {

Logger *creator() { return new Logger(); }

void deleter(Logger *p) { delete p; }

}