/**
 * @file exception.hpp
 * @brief mysqlpp exceptions.
 */
#pragma once

#include <stdexcept>
#include <string>

namespace mysqlpp
{
  /**
   * @brief Database error thrown by mysqlpp.
   */
  class DatabaseError final : public std::runtime_error
  {
  public:
    explicit DatabaseError(const std::string &msg)
        : std::runtime_error(msg)
    {
    }
  };
}
