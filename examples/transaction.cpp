#include <mysqlpp/mysqlpp.hpp>
#include <iostream>

static void run_example()
{
  mysqlpp::Config cfg{
      "tcp://127.0.0.1:3306",
      "root",
      "root",
      "test"};

  mysqlpp::Client db(cfg);

  try
  {
    db.connect();

    mysqlpp::Client::Transaction tx(db);

    db.exec("INSERT INTO users(name) VALUES('Transactional User')");

    // Simulate condition
    bool everything_ok = true;

    if (everything_ok)
    {
      tx.commit();
      std::cout << "Transaction committed\n";
    }
    else
    {
      tx.rollback();
      std::cout << "Transaction rolled back\n";
    }
  }
  catch (const mysqlpp::DatabaseError &e)
  {
    std::cerr << "Transaction error: " << e.what() << "\n";
  }
}

int main()
{
  run_example();
  return 0;
}
