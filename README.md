# mysqlpp

Minimal RAII MySQL client for modern C++.

`mysqlpp` provides a small, explicit wrapper around mysql-connector-cpp
with clean connection management and transaction support.

Not header-only. Requires MySQL Connector/C++.

## Download

https://vixcpp.com/registry/pkg/gaspardkirira/mysqlpp

## Why mysqlpp?

Working directly with mysql-connector-cpp can be verbose and
error-prone:

-   Manual connection lifecycle
-   Repeated error handling
-   No RAII transaction safety
-   No clear abstraction boundary

This library provides:

-   Explicit connection management
-   RAII transactions
-   Simple `exec()` and `query()` helpers
-   Clean exception model
-   No global singletons

No hidden magic. No ORM. No reflection.

Just a thin, deterministic wrapper.

## Installation

### Using Vix Registry

``` bash
vix add gaspardkirira/mysqlpp
vix deps
```

### Manual

``` bash
git clone https://github.com/GaspardKirira/mysqlpp.git
```

Add the `include/` directory to your project.

## Dependency

You must install the mysql-connector-cpp development package.

On Ubuntu:

``` bash
sudo apt install libmysqlcppconn-dev
```

On other systems, install MySQL Connector/C++ (8.x recommended).

## Quick Examples

### Basic Connection

``` cpp
#include <mysqlpp/mysqlpp.hpp>
#include <iostream>

int main()
{
  mysqlpp::Config cfg{
    "tcp://127.0.0.1:3306",
    "root",
    "root",
    "test"
  };

  mysqlpp::Client db(cfg);

  try
  {
    db.connect();
    std::cout << "Connected\n";
  }
  catch (const mysqlpp::DatabaseError& e)
  {
    std::cerr << e.what() << "\n";
  }
}
```

### Insert and Select

``` cpp
#include <mysqlpp/mysqlpp.hpp>
#include <iostream>

int main()
{
  mysqlpp::Config cfg{
    "tcp://127.0.0.1:3306",
    "root",
    "root",
    "test"
  };

  mysqlpp::Client db(cfg);
  db.connect();

  db.exec("CREATE TABLE IF NOT EXISTS users("
          "id INT AUTO_INCREMENT PRIMARY KEY,"
          "name VARCHAR(100))");

  db.exec("INSERT INTO users(name) VALUES('Alice')");

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
```

### Transaction (RAII)

``` cpp
#include <mysqlpp/mysqlpp.hpp>

int main()
{
  mysqlpp::Config cfg{
    "tcp://127.0.0.1:3306",
    "root",
    "root",
    "test"
  };

  mysqlpp::Client db(cfg);
  db.connect();

  mysqlpp::Client::Transaction tx(db);

  db.exec("INSERT INTO users(name) VALUES('Transactional User')");

  tx.commit();
}
```

If `commit()` is not called, rollback is automatic.

## API Overview

``` cpp
mysqlpp::Config
mysqlpp::Client

Client::connect()
Client::is_connected()
Client::exec(sql)
Client::query(sql)

mysqlpp::Client::Transaction
Transaction::commit()
Transaction::rollback()
```

## Design Principles

-   Explicit over implicit
-   No global state
-   RAII for safety
-   Thin wrapper over official connector
-   No ORM
-   No schema abstraction

This library is intentionally minimal.

If you need:

-   Query builders
-   ORM mapping
-   Schema reflection
-   Async drivers

Build those on top.

## Tests

Run:

``` bash
vix build
vix tests
```

Tests verify:

-   Configuration construction
-   Connection state logic
-   Basic API compile correctness

Examples require a running MySQL server.

## License

MIT License
Copyright (c) Gaspard Kirira

