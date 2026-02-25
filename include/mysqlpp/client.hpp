/**
 * @file client.hpp
 * @brief Minimal RAII MySQL client wrapper (mysql-connector-cpp).
 */
#pragma once

#include "mysqlpp/exception.hpp"

#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <mysql_driver.h>

#include <memory>
#include <string>
#include <utility>

namespace mysqlpp
{
  /**
   * @brief Connection configuration.
   */
  struct Config
  {
    std::string host; ///< Example: "tcp://127.0.0.1:3306"
    std::string user;
    std::string password;
    std::string database;
  };

  /**
   * @brief RAII MySQL client.
   *
   * - No global singleton.
   * - Explicit connect().
   * - Simple exec/query helpers.
   */
  class Client final
  {
  public:
    explicit Client(Config cfg)
        : cfg_(std::move(cfg))
    {
    }

    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;

    Client(Client &&) noexcept = default;
    Client &operator=(Client &&) noexcept = default;

    /**
     * @brief Connect to MySQL if not already connected.
     */
    void connect()
    {
      if (conn_)
        return;

      try
      {
        auto *driver = sql::mysql::get_mysql_driver_instance();
        std::unique_ptr<sql::Connection> c(driver->connect(cfg_.host, cfg_.user, cfg_.password));
        c->setSchema(cfg_.database);
        conn_ = std::move(c);
      }
      catch (const sql::SQLException &e)
      {
        throw DatabaseError(std::string("mysql connect failed: ") + e.what());
      }
    }

    /**
     * @brief Check connection state.
     */
    bool is_connected() const noexcept
    {
      return static_cast<bool>(conn_);
    }

    /**
     * @brief Get underlying connection (connects if needed).
     */
    sql::Connection &connection()
    {
      ensure_connected();
      return *conn_;
    }

    /**
     * @brief Execute INSERT/UPDATE/DELETE and return affected rows.
     */
    int exec(const std::string &sql_text)
    {
      ensure_connected();
      try
      {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(conn_->prepareStatement(sql_text));
        return stmt->executeUpdate();
      }
      catch (const sql::SQLException &e)
      {
        throw DatabaseError(std::string("mysql exec failed: ") + e.what());
      }
    }

    /**
     * @brief Execute a SELECT-like query and return a ResultSet.
     *
     * Note: ResultSet lifetime can be tied to statement lifetime in some connector builds.
     * For robust usage, build higher-level APIs that keep statement ownership.
     */
    std::unique_ptr<sql::ResultSet> query(const std::string &sql_text)
    {
      ensure_connected();
      try
      {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(conn_->prepareStatement(sql_text));
        return std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
      }
      catch (const sql::SQLException &e)
      {
        throw DatabaseError(std::string("mysql query failed: ") + e.what());
      }
    }

    /**
     * @brief RAII transaction (rollback on destruction if not committed).
     */
    class Transaction final
    {
    public:
      explicit Transaction(Client &c)
          : c_(c)
      {
        c_.ensure_connected();
        c_.conn_->setAutoCommit(false);
      }

      Transaction(const Transaction &) = delete;
      Transaction &operator=(const Transaction &) = delete;

      ~Transaction()
      {
        if (!done_)
        {
          try
          {
            c_.conn_->rollback();
            c_.conn_->setAutoCommit(true);
          }
          catch (...)
          {
          }
        }
      }

      void commit()
      {
        c_.conn_->commit();
        c_.conn_->setAutoCommit(true);
        done_ = true;
      }

      void rollback()
      {
        c_.conn_->rollback();
        c_.conn_->setAutoCommit(true);
        done_ = true;
      }

    private:
      Client &c_;
      bool done_{false};
    };

  private:
    void ensure_connected()
    {
      if (!conn_)
      {
        connect();
      }
    }

  private:
    Config cfg_;
    std::unique_ptr<sql::Connection> conn_;
  };
}
