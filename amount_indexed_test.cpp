#include "amount_indexed.hpp"

#include <stdexcept>
#include <iostream>
#include <format>
#include <sstream>

namespace
{

    const double year_seconds = (365.35 * 24 * 3600); // approximately

    void should_return_initial_amount_at_inital_time()
    {
        std::cout << __func__ << "\n";
        double initial_amount = 123.45;
        time_t time = 4567;
        auto amount = AmountIndexed(initial_amount, time, 6.78);
        double got = amount.value_at(time);
        if (std::abs(initial_amount - got) > 0.001)
        {
            throw std::runtime_error(std::format("Error: got {:6.2f}", got));
        }
    }

    void should_account_for_cpi()
    {
        std::cout << __func__ << "\n";
        double initial_amount = 100.0;
        time_t time = 14567; // nothing special
        auto amount = AmountIndexed(initial_amount, time, 13.45);
        double got = amount.value_at(time + year_seconds);
        if (std::abs(113.45 - got) > 0.001)
        {
            throw std::runtime_error(std::format("Error: got {:6.2f}", got));
        }
    }

    void should_be_streamable()
    {
        std::cout << __func__ << "\n";
        double initial_amount = 100.0;
        time_t time = 14567; // nothing special
        auto amount = AmountIndexed(initial_amount, time, 13.45);
        std::ostringstream stream;
        stream << amount;

        auto got = stream.str();
        if (got != "100.00")
        {
            throw std::runtime_error(std::format("Error: got {:}", got));
        }
    }

} // namespace

int main(int argc, char *argv[])
{
    should_return_initial_amount_at_inital_time();
    should_account_for_cpi();
    should_be_streamable();
    exit(0);
}