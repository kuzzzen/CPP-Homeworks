#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <memory>

using namespace std;
#endif /* __PROGTEST__ */

#define all(x) (x).begin(), (x).end()

//  struct representing a company
struct Company {
    string name, addr, id;
    unsigned int invoice_sum = 0;
};

//  compares in lowercase without modifying whole string
bool iStrCmp(const string &left, const string &right) {
    return equal(all(left), all(right), [](char a, char b) {
        return tolower(a) == tolower(b);
    });
}

//  turns whole string to lowercase
string toLowerStr(string str) {
    for_each(all(str), [](char &c) {
        c = (char) tolower(c);
    });
    return str;
}

//  compares Company structs by name and address
bool companyCmpByNA(const Company &left, const Company &right) {
    return toLowerStr(left.name) < toLowerStr(right.name) ||
           (iStrCmp(left.name, right.name) && toLowerStr(left.addr) < toLowerStr(right.addr));
}

//  compares Company structs by id
bool companyCmpById(const Company &left, const Company &right) {
    return left.id < right.id;
}


class CVATRegister {
private:
    vector<Company> companies;  //  sorted by name and address
    vector<Company> companies_by_id;    // additional vector sorted by id for faster access
    vector<unsigned int> all_invoices;  // vector of invoices for fast median search
public:
    CVATRegister() = default;

    ~CVATRegister() = default;

    // adds new company to both vectors using insert
    bool newCompany(const string &name, const string &addr, const string &taxID) {
        // check for presence:
        for (const auto &company: companies)
            if (company.id == taxID || (iStrCmp(company.name, name) && iStrCmp(company.addr, addr)))
                return false;

        Company tmp{name, addr, taxID};

        auto it = lower_bound(all(companies), tmp, companyCmpByNA);
        auto it_id = lower_bound(all(companies_by_id), tmp, companyCmpById);

        companies.insert(it, tmp);
        companies_by_id.insert(it_id, tmp);
        return true;
    }

    // removes company from the register by name and address (from both vectors)
    bool cancelCompany(const string &name, const string &addr) {
        auto init_s = companies.size();
        eraseByNA(name, addr, companies);
        eraseByNA(name, addr, companies_by_id);
        return companies.size() != init_s;
    }

    // removes company from the register by id (from both vectors)
    bool cancelCompany(const string &taxID) {
        auto init_s = companies.size();
        eraseById(taxID, companies);
        eraseById(taxID, companies_by_id);
        return companies.size() != init_s;
    }

    //  adds amount to company's income by name and address
    bool invoice(const string &name, const string &addr, unsigned int amount) {
        Company tmp{name, addr, ""}; // struct object to use in lower_bound for companies
        auto ind = lower_bound(all(companies), tmp, companyCmpByNA) - companies.begin();
        if (iStrCmp(addr, companies[ind].addr) && iStrCmp(name, companies[ind].name)) {
            createNewInvoice(companies[ind], amount);
            return true;
        }
        return false;
    }

    //  adds amount to company's income by id
    bool invoice(const string &taxID, unsigned int amount) {
        Company tmp{"", "", taxID}; // struct object to use in lower_bound for companies_by_id
        auto ind = lower_bound(all(companies_by_id), tmp, companyCmpById) - companies_by_id.begin();
        if (taxID == companies_by_id[ind].id) {
            createNewInvoice(companies_by_id[ind], amount);
            return true;
        }
        return false;
    }

    //  used to find company's income
    bool audit(const string &name, const string &addr, unsigned int &sumIncome) const {
        Company tmp{name, addr, ""};  // used for lower_bound in companies
        auto ind = lower_bound(all(companies), tmp, companyCmpByNA) - companies.begin();
        if (iStrCmp(addr, companies[ind].addr) && iStrCmp(name, companies[ind].name)) {
            sumIncome = companies[ind].invoice_sum;
            // adding income from companies_by_id as well:
            Company tmp_id{"", "", companies[ind].id}; // used for lower_bound in companies_by_id
            auto by_id = lower_bound(all(companies_by_id), tmp_id, companyCmpById) - companies_by_id.begin();
            sumIncome += companies_by_id[by_id].invoice_sum;
            return true;
        }
        return false;
    }

    //  used to find company's income
    bool audit(const string &taxID, unsigned int &sumIncome) const {
        Company tmp{"", "", taxID};  // used for lower_bound in companies_by_id
        auto ind = lower_bound(all(companies_by_id), tmp, companyCmpById) - companies_by_id.begin();
        if (taxID == companies_by_id[ind].id) {
            sumIncome = companies_by_id[ind].invoice_sum;
            // adding income from companies by name and address as well:
            Company tmp_na{companies_by_id[ind].name, companies_by_id[ind].addr,""};  // used for lower_bound in companies
            auto by_na = lower_bound(all(companies), tmp_na, companyCmpByNA) - companies.begin();
            sumIncome += companies[by_na].invoice_sum;
            return true;
        }
        return false;
    }

    // finds first company from alphabetically-sorted vector "companies"
    bool firstCompany(string &name, string &addr) const {
        if (companies.empty()) return false;
        name = companies[0].name;
        addr = companies[0].addr;
        return true;
    }

    // finds company that is right next to the one passed in "name" and "addr"
    bool nextCompany(string &name, string &addr) const {
        if (companies.empty()) return false;
        for (long unsigned int i = 0; i < companies.size(); i++)
            if (iStrCmp(name, companies[i].name) && iStrCmp(addr, companies[i].addr) && companies.size() > i + 1) {
                name = companies[i + 1].name;
                addr = companies[i + 1].addr;
                return true;
            }
        return false;
    }

    //  searches for median invoice amount
    [[nodiscard]] unsigned int medianInvoice() const {
        if (all_invoices.empty()) return 0;
        // as vector is sorted, we can just take the middle value:
        return all_invoices[all_invoices.size() / 2];
    }


    /*      HELPER FUNCTIONS :      */


    //  function to erase from vector by name and address
    static void eraseByNA(const string &name, const string &addr, vector<Company> &vec) {
        vec.erase(remove_if(all(vec), [&](Company const &company) {
            return iStrCmp(addr, company.addr) && iStrCmp(name, company.name);
        }), vec.end());
    }

    //  function to erase from vector by id
    static void eraseById(const string &taxID, vector<Company> &vec) {
        vec.erase(remove_if(all(vec), [&](Company const &company) {
            return taxID == company.id;
        }), vec.end());
    }

    //  adds an invoice to a given company's income sum and the vector of all invoices
    void createNewInvoice(Company &company, const unsigned int amount) {
        company.invoice_sum += amount;

        //  insert an invoice into the vector of invoices:
        auto it_inv = lower_bound(all(all_invoices), amount);
        all_invoices.insert(it_inv, amount);
    }

};


#ifndef __PROGTEST__

int main() {
    string name, addr;
    unsigned int sumIncome;

    CVATRegister b1;
    assert (b1.newCompany("ACME", "Thakurova", "666/666"));
    assert (b1.newCompany("ACME", "Kolejni", "666/666/666"));
    assert (b1.newCompany("Dummy", "Thakurova", "123456"));
    assert (b1.invoice("666/666", 2000));
    assert (b1.medianInvoice() == 2000);
    assert (b1.invoice("666/666/666", 3000));
    assert (b1.medianInvoice() == 3000);
    assert (b1.invoice("123456", 4000));
    assert (b1.medianInvoice() == 3000);
    assert (b1.invoice("aCmE", "Kolejni", 5000));
    assert (b1.medianInvoice() == 4000);
    assert (b1.audit("ACME", "Kolejni", sumIncome) && sumIncome == 8000);
    assert (b1.audit("123456", sumIncome) && sumIncome == 4000);
    assert (b1.firstCompany(name, addr) && name == "ACME" && addr == "Kolejni");
    assert (b1.nextCompany(name, addr) && name == "ACME" && addr == "Thakurova");
    assert (b1.nextCompany(name, addr) && name == "Dummy" && addr == "Thakurova");
    assert (!b1.nextCompany(name, addr));
    assert (b1.cancelCompany("ACME", "KoLeJnI"));
    assert (b1.medianInvoice() == 4000);
    assert (b1.cancelCompany("666/666"));
    assert (b1.medianInvoice() == 4000);
    assert (b1.invoice("123456", 100));
    assert (b1.medianInvoice() == 3000);
    assert (b1.invoice("123456", 300));
    assert (b1.medianInvoice() == 3000);
    assert (b1.invoice("123456", 200));
    assert (b1.medianInvoice() == 2000);
    assert (b1.invoice("123456", 230));
    assert (b1.medianInvoice() == 2000);
    assert (b1.invoice("123456", 830));
    assert (b1.medianInvoice() == 830);
    assert (b1.invoice("123456", 1830));
    assert (b1.medianInvoice() == 1830);
    assert (b1.invoice("123456", 2830));
    assert (b1.medianInvoice() == 1830);
    assert (b1.invoice("123456", 2830));
    assert (b1.medianInvoice() == 2000);
    assert (b1.invoice("123456", 3200));
    assert (b1.medianInvoice() == 2000);
    assert (b1.firstCompany(name, addr) && name == "Dummy" && addr == "Thakurova");
    assert (!b1.nextCompany(name, addr));
    assert (b1.cancelCompany("123456"));
    assert (!b1.firstCompany(name, addr));

    CVATRegister b2;
    assert (b2.newCompany("ACME", "Kolejni", "abcdef"));
    assert (b2.newCompany("Dummy", "Kolejni", "123456"));
    assert (!b2.newCompany("AcMe", "kOlEjNi", "1234"));
    assert (b2.newCompany("Dummy", "Thakurova", "ABCDEF"));
    assert (b2.medianInvoice() == 0);
    assert (b2.invoice("ABCDEF", 1000));
    assert (b2.medianInvoice() == 1000);
    assert (b2.invoice("abcdef", 2000));
    assert (b2.medianInvoice() == 2000);
    assert (b2.invoice("aCMe", "kOlEjNi", 3000));
    assert (b2.medianInvoice() == 2000);
    assert (!b2.invoice("1234567", 100));
    assert (!b2.invoice("ACE", "Kolejni", 100));
    assert (!b2.invoice("ACME", "Thakurova", 100));
    assert (!b2.audit("1234567", sumIncome));
    assert (!b2.audit("ACE", "Kolejni", sumIncome));
    assert (!b2.audit("ACME", "Thakurova", sumIncome));
    assert (!b2.cancelCompany("1234567"));
    assert (!b2.cancelCompany("ACE", "Kolejni"));
    assert (!b2.cancelCompany("ACME", "Thakurova"));
    assert (b2.cancelCompany("abcdef"));
    assert (b2.medianInvoice() == 2000);
    assert (!b2.cancelCompany("abcdef"));
    assert (b2.newCompany("ACME", "Kolejni", "abcdef"));
    assert (b2.cancelCompany("ACME", "Kolejni"));
    assert (!b2.cancelCompany("ACME", "Kolejni"));

    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
