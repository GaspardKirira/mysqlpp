#include <mysqlpp/mysqlpp.hpp>
#include <iostream>

static void run_example()
{
  mysqlpp::Config cfg;
  cfg.host = "tcp://127.0.0.1:3306";
  cfg.user = "root";
  cfg.password = "root";
  cfg.database = "test";

  mysqlpp::Client client(cfg);

  try
  {
    client.connect();
    std::cout << "Connected successfully\n";
  }
  catch (const mysqlpp::DatabaseError &e)
  {
    std::cerr << "Connection failed: " << e.what() << "\n";
  }
}

int main()
{
  run_example();
  return 0;
}
