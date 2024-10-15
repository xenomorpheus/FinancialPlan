#include "amount_indexed.hpp"

#include <cstdlib>
#include <cmath>

namespace
{

    const double year_seconds = (365.35 * 24 * 3600); // approximately

}

AmountIndexed::AmountIndexed(double amount, time_t time, double cpi_pa) : m_amount{amount}, m_time{time}, m_cpi_pa{cpi_pa} {}

void AmountIndexed::reindex(time_t time)
{
    m_amount = value_at(time);
    m_time = time;
}

double AmountIndexed::value_at(time_t time) const
{
    double years = (time - m_time) / year_seconds;
    return m_amount * std::pow(1.0 + m_cpi_pa / 100.0, years);
}

double AmountIndexed::value() const
{
    return m_amount;
}

std::ostream &operator<<(std::ostream &os, const AmountIndexed &rhs)
{
    os << std::format("{:.2f}",
                      rhs.value());
    return os;
}