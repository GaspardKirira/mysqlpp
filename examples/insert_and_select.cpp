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

    db.exec("CREATE TABLE IF NOT EXISTS users ("
            "id INT AUTO_INCREMENT PRIMARY KEY,"
            "name VARCHAR(100))");

    db.exec("INSERT INTO users(name) VALUES('Alice')");
    db.exec("INSERT INTO users(name) VALUES('Bob')");

    auto result = db.query("SELECT id, name FROM users");

    while (result->next())
    {
      std::cout
          << result->getInt("id")
          << " - "
          << result->getString("name")
          << "\n";
    }
  }
  catch (const mysqlpp::DatabaseError &e)
  {
    std::cerr << "Database error: " << e.what() << "\n";
  }
}

int main()
{
  run_example();
  return 0;
}
