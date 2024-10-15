#include "account.hpp"

#include <stdexcept>
#include <iostream>
#include <format>

namespace
{

    void should_return_zero_balance_on_construction()
    {
        std::cout << __func__ << "\n";
        auto acc = Account();
        double got = acc.balance();
        if (std::abs(got) > 0.001)
        {
            throw std::runtime_error(std::format("Error: got {:6.2f}", got));
        }
    }

    void should_return_transaction_amount_for_first_transaction()
    {
        std::cout << __func__ << "\n";
        const double expected = 12.34;
        auto acc = Account();
        acc.transact(expected, 0);
        double got = acc.balance();
        if (std::abs(got - expected) > 0.001)
        {
            throw std::runtime_error(std::format("Error: got {:6.2f}", got));
        }
    }

    void should_handle_interest()
    {
        std::cout << __func__ << "\n";
        const time_t seconds_pa{static_cast<time_t>(365.25 * 24 * 60 * 60)};
        auto acc = Account().balance(100.0).rate_pa(12.34);
        acc.transact(0.0, seconds_pa);
        double got = acc.balance();
        if (std::abs(got - 112.34) > 0.001)
        {
            throw std::runtime_error(std::format("Error: got {:6.2f}", got));
        }
    }

    void should_handle_negative_interest()
    {
        std::cout << __func__ << "\n";
        const time_t seconds_pa{static_cast<time_t>(365.25 * 24 * 60 * 60)};
        auto acc = Account().balance(100.0).rate_pa(-12.34);
        acc.transact(0.0, seconds_pa);
        double got = acc.balance();
        if (std::abs(got - 87.66) > 0.001)
        {
            throw std::runtime_error(std::format("Error: got {:6.2f}", got));
        }
    }

} // namespace

int main(int argc, char *argv[])
{
    should_return_zero_balance_on_construction();
    should_return_transaction_amount_for_first_transaction();
    should_handle_interest();
    should_handle_negative_interest();
    exit(0);
}