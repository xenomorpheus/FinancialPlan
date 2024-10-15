#pragma once

// AmountIndexed represents amounts that may be effected by CPI and passage of time.
// Normally to get the value the value_at(some_timestamp) method is used.
// Alternatively if a value is updated periodically (e.g. yearly) then use
// the value() method, but ensure you call reindex(some_timestamp) every period.

#include "time.h"
#include <iostream>
#include <format>

class AmountIndexed
{

public:
    AmountIndexed(double amount, time_t time, double cpi_pa);

    ~AmountIndexed() = default;

    AmountIndexed &operator+=(const AmountIndexed &rhs)
    {
        m_amount += rhs.value_at(m_time);
        return *this;
    }

    // update the internal value to the supplied time using the CPI
    void reindex(time_t time);

    // return the value considering time and CPI
    double value_at(time_t time) const;

    // return the value at construction, or updated value if reindex has been used
    double value() const;

private:
    double m_amount;
    time_t m_time;
    double m_cpi_pa;
};

std::ostream &operator<<(std::ostream &os, const AmountIndexed &rhs);