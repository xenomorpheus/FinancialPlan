#pragma once

// An extremely simplified representation of a financial transactional account.
// On construction optionally set the name, rate_pa, balance, time,
// show_transaction.

#include <string>

class Account
{
public:
    Account();

    ~Account() = default;

    // Daisy-chain methods for construction

    Account &name(const std::string &name);

    // For 12.3% use 12.3 rate_pa
    Account &rate_pa(double rate_pa);

    Account &balance(double balance);

    Account &time(time_t time);

    // When transact() is called and the transaction amount is non-zero then print the transaction details on one line.
    Account &show_transaction(bool show_transaction);

    // Getters

    const std::string &name() const;

    double balance() const;

    double rate_pa() const;

    //

    // Apply the transaction to the account at the time specified.
    // Also the balance is updated with interest using the interest rate and time since last transaction.
    // Time may not go backwards.
    // Optionally print the transaction, see show_transaction();
    void transact(double amount, time_t time, const std::string &description = "");

private:
    const double m_seconds_pa{365.25 * 24 * 60 * 60};
    std::string m_name;
    double m_rate_pa;
    double m_balance;
    time_t m_time;
    bool m_show_transaction;
};