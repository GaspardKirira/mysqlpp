#include <mysqlpp/mysqlpp.hpp>
#include <cassert>
#include <string>

int main()
{
  mysqlpp::Config cfg;
  cfg.host = "tcp://127.0.0.1:3306";
  cfg.user = "root";
  cfg.password = "root";
  cfg.database = "test";

  mysqlpp::Client c(cfg);
  assert(!c.is_connected());

  // We don't call connect() here to avoid requiring a running DB in CI.
  return 0;
}
