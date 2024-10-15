#include "account.hpp"

#include <iostream>
#include <format>

Account::Account() : m_name{}, m_rate_pa{0.0}, m_balance{0.0}, m_time{0} {}

// Daisy-chain style construction.

Account &Account::name(const std::string &name)
{
    m_name = name;
    return *this;
}

// For 12.3% use 12.3 rate_pa
Account &Account::rate_pa(double rate_pa)
{
    m_rate_pa = rate_pa;
    return *this;
}

Account &Account::balance(double balance)
{
    m_balance = balance;
    return *this;
}

Account &Account::time(time_t time)
{
    m_time = time;
    return *this;
}

Account &Account::show_transaction(bool show_transaction)
{
    m_show_transaction = show_transaction;
    return *this;
}

//

const std::string &Account::name() const { return m_name; }

double Account::balance() const { return m_balance; }

double Account::rate_pa() const { return m_rate_pa; }

void Account::transact(double amount, time_t time, const std::string &description)
{
    if (time < m_time)
    {
        throw std::runtime_error("Error: time attempted to go backwards");
    }
    double rate_factor_pa = 1 + abs(m_rate_pa / 100.0);
    double part_of_year = (time - m_time) / m_seconds_pa;
    double interest_factor_interval = std::pow(rate_factor_pa, part_of_year) - 1;
    if (m_rate_pa < 0)
    {
        interest_factor_interval *= -1.0;
    }
    double interest = m_balance * interest_factor_interval;
    m_balance += interest + amount;
    m_time = time;

    if (std::abs(amount) > 0.01 && m_show_transaction)
    {
        std::cout << std::format("   {:10} Amt: {:7.2f}, {}\n", m_name, amount, description);
    }
}
