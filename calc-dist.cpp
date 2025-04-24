#include "account.hpp"
#include "amount_indexed.hpp"

#include <cmath>
#include <ctime>

#include <string>
#include <format>
#include <iostream>

namespace
{

    // Doesn't handle when DOM is in 29-31 and doesn't exist in the next month.
    // Bug: DoM increases with repeated calls.
    static time_t add_month(time_t timestamp)
    {
        struct tm datetime = *localtime(&timestamp);
        datetime.tm_isdst = 0; // Not daylight saving
        datetime.tm_mon++;
        if (datetime.tm_mon > 11)
        {
            datetime.tm_year++;
            datetime.tm_mon -= 12;
        }
        timestamp = mktime(&datetime);
        return timestamp;
    }

    static time_t first_of_month(time_t timestamp)
    {
        struct tm datetime = *localtime(&timestamp);
        datetime.tm_isdst = 0; // Not daylight saving
        datetime.tm_sec = 0;
        datetime.tm_min = 0;
        datetime.tm_hour = 0;
        if (datetime.tm_mday != 1)
        {
            datetime.tm_mday = 1;
            datetime.tm_mon++;
            if (datetime.tm_mon > 11)
            {
                datetime.tm_year++;
                datetime.tm_mon -= 12;
            }
        }
        timestamp = mktime(&datetime);
        return timestamp;
    }

    void print_date(std::ostream &ostream, time_t timestamp)
    {
        struct tm datetime = *localtime(&timestamp);
        ostream << std::format("{:04}-{:02}-{:02}",
                               datetime.tm_year + 1900, datetime.tm_mon + 1, datetime.tm_mday);
    }

    struct FinancialStrategy
    {
        // Used for display e.g. employment, retired
        std::string name;

        // Monthly contribution to mortgage or super
        // Note: Try some higher values to see what happens to your retirement date
        std::optional<AmountIndexed> repayment_pm;

        // Monthly employer contribution to super.
        // NOTE: reduce this amount by taxes paid by super fund.
        // Have a look at your super transactions; note the emp contributions and corresponding taxes.
        std::optional<AmountIndexed> emp_super_contribution_pm;

        // Yearly personal contribution to super.
        // Note: Consider claiming an income deduction at tax time.
        // Note: Consider topping up to contributions cap MINUS the
        // employer contributions throughout the year.
        // Note: Consider making this contribution just prior EoFY
        // to reduce time out of pocket, hopefully before tax refund.
        std::optional<AmountIndexed> personal_super_contribution_pa;

        // Yearly taxes paid by super account for personal contribution to super.
        std::optional<AmountIndexed> personal_super_contribution_tax_pa;

        // Yearly tax refund (or debt)
        // Note: Personal super contributions may allow you to reduce your taxable
        // income, which may reduce tax.
        std::optional<AmountIndexed> tax_refund_pa;

        // When retired the amount of your monthly wage.
        // This is a negative amount.
        std::optional<AmountIndexed> retirement_super_transaction_pm;

        std::optional<double> retirement_balance_requirement;
    };

    std::ostream &operator<<(std::ostream &os, const FinancialStrategy &rhs)
    {
        os << std::format("Financial Strategy: {}\n",
                          rhs.name);
        if (rhs.repayment_pm)
        {
            os << std::format("  repayment_pm: {:.2f}\n",
                              rhs.repayment_pm->value());
        }
        if (rhs.emp_super_contribution_pm)
        {
            os << std::format("  emp_super_contribution_pm: {:.2f}\n",
                              rhs.emp_super_contribution_pm->value());
        }
        if (rhs.personal_super_contribution_pa)
        {
            os << std::format("  personal_super_contribution_pa: {:.2f}\n",
                              rhs.personal_super_contribution_pa->value());
        }
        if (rhs.personal_super_contribution_tax_pa)
        {
            os << std::format("  personal_super_contribution_tax_pa: {:.2f}\n",
                              rhs.personal_super_contribution_tax_pa->value());
        }
        if (rhs.tax_refund_pa)
        {
            os << std::format("  tax_refund_pa: {:.2f}\n",
                              rhs.tax_refund_pa->value());
        }
        if (rhs.retirement_super_transaction_pm)
        {
            os << std::format("  retirement_super_transaction_pm: {:.2f}\n",
                              rhs.retirement_super_transaction_pm->value());
        }
        if (rhs.retirement_balance_requirement)
        {
            os << std::format("  retirement_balance_requirement: {:.2f}\n",
                              *rhs.retirement_balance_requirement);
        }
        return os;
    }

    // Optionally apply a transaction to an Account.
    void transact(Account &account, std::optional<AmountIndexed> amount, time_t timestamp, const std::string &description = "")
    {
        if (amount)
        {
            account.transact(amount->value_at(timestamp), timestamp, description);
        }
    }

    // As per transact(), but when prefer to reindex amounts periodically e.g. yearly
    void transact_no_indexation(Account &account, std::optional<AmountIndexed> amount, time_t timestamp, const std::string &description = "")
    {
        if (amount)
        {
            account.transact(amount->value(), timestamp, description);
        }
    }

}; // namespace

int main(int argc, char *argv[])
{
    // Get the timestamp for the current date and time
    time_t timestamp;
    time(&timestamp);
    timestamp = first_of_month(timestamp);

    //===================================================================
    // Config area

    constexpr double pay_pa{XXX}; // update by CPI each year
    constexpr double pay_take_home_pa{XXX}; // after tax and medicare levy
    constexpr double cpi_pa = XXX; // Consider using 20+ year average
    constexpr double super_personal_contrib_cap_pa = XXX; // 2024- financial year
    constexpr double emp_super_rate{XXX}; // 11.5% superannuation
    constexpr double emp_super_contribution_pa = pay_pa * emp_super_rate;
    constexpr double super_top_up_pa = super_personal_contrib_cap_pa - emp_super_contribution_pa;

    constexpr bool show_transactions{true}; // print transaction details when mortgage or super is updated
    constexpr bool show_heading_and_months{true};

    std::cout << std::format("Pay p.a.                    : {:>10.2f}\n", pay_pa);
    std::cout << std::format("Take home Pay p.a.          : {:>10.2f}\n", pay_take_home_pa);
    std::cout << std::format("CPI p.a.                    : {:>10.2f}\n", cpi_pa);
    std::cout << std::format("Super personal contrib cap  : {:>10.2f}\n", super_personal_contrib_cap_pa);
    std::cout << std::format("Employer Super rate         : {:>10.3f}\n", emp_super_rate);
    std::cout << std::format("Employer Super contrib p.a. : {:>10.2f}\n", emp_super_contribution_pa);
    std::cout << std::format("Super top up p.a.           : {:>10.2f}\n", super_top_up_pa);


    // Consider long term interest rate_pa, but further REDUCE the rate_pa to account for fees.
    auto mortgage = Account().name("mortgage").rate_pa(-XXX).balance(XXX).time(timestamp).show_transaction(show_transactions);

    // Consider long term return rate_pa, but further REDUCE the rate_pa to account for fees.
    auto super = Account().name("super").rate_pa(XXX - XXX).balance(XXX).time(timestamp).show_transaction(show_transactions);

    FinancialStrategy employed = {
        .name = "employed",
        .repayment_pm = std::make_optional<AmountIndexed>(pay_take_home_pa/12.0 - XXX, timestamp, cpi_pa),
        .emp_super_contribution_pm = std::make_optional<AmountIndexed>(emp_super_contribution_pa/12.0*0.XXX, timestamp, cpi_pa),
        .personal_super_contribution_pa = std::make_optional<AmountIndexed>(super_top_up_pa, timestamp, cpi_pa),
        .personal_super_contribution_tax_pa = std::make_optional<AmountIndexed>(super_top_up_pa*-0.XXX, timestamp, cpi_pa),
        .tax_refund_pa = std::make_optional<AmountIndexed>(super_personal_contrib_cap_pa *.XXX + XXX, timestamp, cpi_pa),
        .retirement_balance_requirement = XXX,     // XX years at XXXk indexed from 2024.
    };

    FinancialStrategy retired = {
        .name = "retired",
        .retirement_super_transaction_pm = std::make_optional<AmountIndexed>(-XXX / 12.0, timestamp, cpi_pa),
    };
    //===================================================================

    if (mortgage.rate_pa() >= 0.0)
    {
        std::cerr << std::format("WARNING: {} has non-negative interest rate {}\n", mortgage.name(), mortgage.rate_pa());
    }
    if (super.rate_pa() <= 0.0)
    {
        std::cerr << std::format("WARNING: {} has non-positive interest rate {}\n", super.name(), super.rate_pa());
    }

    FinancialStrategy strategy = employed;

    std::cout << std::format("Mortgage Int. Rate : {:>10.2f}\n", mortgage.rate_pa());
    std::cout << std::format("Super Int. Rate    : {:>10.2f}\n", super.rate_pa());

    std::cout << "\n"
                 "Defining financial strategies:\n\n"
              << employed << "\n\n"
              << retired << "\n\n"
              << "Starting strategy: " << strategy.name << "\n\n";

    if (show_heading_and_months)
    {
        std::cout << std::format("\n\n{:10}: {:>10} : {:>10}: {:>10}", "Date", mortgage.name(), super.name(), "Net") << "\n";
    }

    // Main Monthly loop
    while (true)
    {
        transact_no_indexation(mortgage.balance() < 0.0 ? mortgage : super, strategy.repayment_pm, timestamp, "monthly contribution");
        if (mortgage.balance() > 0.0)
        {
            super.transact(mortgage.balance(), timestamp, "Mortgage paid!!! transfer mortgage balance into super");
            mortgage.time(timestamp).balance(0.0);
        }

        transact_no_indexation(super, strategy.emp_super_contribution_pm, timestamp, "emp super contrib, after contribution tax removed");
        transact(super, strategy.retirement_super_transaction_pm, timestamp, "monthly transaction");

        struct tm datetime = *localtime(&timestamp);
        if (datetime.tm_mon == 5) // June
        {
            if (strategy.personal_super_contribution_pa)
            {
                mortgage.transact(-strategy.personal_super_contribution_pa->value_at(timestamp), timestamp, "yearly transfer from mortgage to super");
            }
            transact(super, strategy.personal_super_contribution_pa, timestamp, "yearly personal contrib");
            transact(super, strategy.personal_super_contribution_tax_pa, timestamp, "yearly personal contrib tax");
        }
        else if (datetime.tm_mon == 6) // July - New Financial Year
        {
            transact(mortgage, strategy.tax_refund_pa, timestamp, "tax refund");

            // Update for CPI
            if (strategy.repayment_pm)
            {
                strategy.repayment_pm->reindex(timestamp);
            }
            if (strategy.emp_super_contribution_pm)
            {
                strategy.emp_super_contribution_pm->reindex(timestamp);
            }
        }

        double balance = mortgage.balance() + super.balance();
        if (show_heading_and_months)
        {
            print_date(std::cout, timestamp);
            std::cout
                << ": " << std::format("{:10.2f} : {:10.2f}: {:10.2f}", mortgage.balance(), super.balance(), balance) << "\n";
        }

        // Only change strategy at end of month when all transfers have been completed.

        if (strategy.retirement_balance_requirement && strategy.retirement_balance_requirement && balance > *strategy.retirement_balance_requirement)
        {
            print_date(std::cout, timestamp);
            std::cout << "  Retirement starts!!! ================================\n";
            strategy = retired;
            std::cout << strategy << "\n\n";
        }
        if (strategy.name == retired.name && balance < 0)
        {
            print_date(std::cout, timestamp);
            std::cout << "  Retirement ends!!! ================================\n";
            break;
        }
        timestamp = add_month(timestamp);
    }
}
