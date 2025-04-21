#include "amount_indexed.hpp"

#include <cstdlib>
#include <cmath>

namespace
{

    const double year_seconds = (365.35 * 24 * 3600); // approximately

}

AmountIndexed::AmountIndexed(double amount, time_t time, double cpi_pa) : amount_{amount}, time_{time}, cpi_pa_{cpi_pa} {}

void AmountIndexed::reindex(time_t time)
{
    amount_ = value_at(time);
    time_ = time;
}

double AmountIndexed::value_at(time_t time) const
{
    double years = (time - time_) / year_seconds;
    return amount_ * std::pow(1.0 + cpi_pa_ / 100.0, years);
}

double AmountIndexed::value() const
{
    return amount_;
}

std::ostream &operator<<(std::ostream &os, const AmountIndexed &rhs)
{
    os << std::format("{:.2f}",
                      rhs.value());
    return os;
}