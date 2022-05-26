#ifndef __PROGTEST__

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdexcept>

using namespace std;
#endif /* __PROGTEST__ */

//=================================================================================================
// a dummy exception class, keep this implementation
class InvalidDateException : public invalid_argument {
public:
    InvalidDateException()
            : invalid_argument("invalid date or format") {
    }
};

//=================================================================================================
// date_format manipulator - a dummy implementation. Keep this code unless you implement your
// own working manipulator.
ios_base &( *date_format(const char *fmt))(ios_base &x) {
    return [](ios_base &ios) -> ios_base & { return ios; };
}

//=================================================================================================
class CDate {
private:
    int _year, _month, _day;
public:
    CDate() = delete;

    static bool isValidDate(CDate date);
    CDate(const int &year, const int &month, const int &day) : _year(year), _month(month), _day(day) {
        if (!isValidDate(*this))
            throw InvalidDateException();
    }
    CDate operator += (const int &days);
    CDate operator -= (const int &days);
    CDate operator + (const int &days);
    CDate operator - (const int &days);
    size_t operator - (const CDate & date);
    bool operator == (const CDate & date) const;
    bool operator != (const CDate & date) const;
    bool operator < (const CDate & date) const;
    bool operator <= (const CDate & date) const;
    bool operator > (const CDate & date) const;
    bool operator >= (const CDate & date) const;
    CDate operator ++ ();
    CDate operator -- ();
    CDate operator ++ (int);
    CDate operator -- (int);
    friend ostream & operator << (ostream &ost, const CDate &date);
    friend istream & operator >> (istream & is, CDate &date);
    static string zfill(const int &num);
    static int getDaysInMonth(int year, int month);
    static bool isLeap(int year);
};

CDate CDate::operator += (const int &days) {
    if (days < 0)
        return *this -= abs(days);
    this->_day += days;
    while(this->_day > getDaysInMonth(this->_year, this->_month)) {
        this->_day -= getDaysInMonth(this->_year, this->_month);
        this->_month += 1;
        if (this->_month == 13) {
            this->_month = 1;
            this->_year += 1;
        }
    }
    return *this;
}

CDate CDate::operator -= (const int &days) {
    if (days < 0)
        return *this += abs(days);
    this->_day -= days;
    while(this->_day <= 0) {
        this->_month -= 1;
        if (this->_month == 0) {
            this->_month = 12;
            this->_year -= 1;
        }
        this->_day += getDaysInMonth(this->_year, this->_month);
    }
    return *this;
}

CDate CDate::operator + (const int &days) {
    CDate tmp(*this);
    tmp += days;
    return tmp;
}

CDate CDate::operator - (const int &days) {
    CDate tmp(*this);
    tmp -= days;
    return tmp;
}

size_t CDate::operator - (const CDate &date) {
    size_t res = 0;
    CDate g(*this), l(date);
    if (date > *this) {
        CDate buff (*this);
        g = l;
        l = buff;
    } else if (*this == date)
        return 0;
    for (int i = l._year; i < g._year; i++)
        res += 365 + isLeap(i);
    for (int i = 0; i < g._month; i++)
        res += getDaysInMonth(g._year, i);
    for (int i = 0; i < l._month; i++)
        res -= getDaysInMonth(l._year, i);
    res += g._day - l._day;
    return res;
}

bool CDate::operator == (const CDate &date) const {
    return (this->_year == date._year && this->_month == date._month && this->_day == date._day);
}

bool CDate::operator != (const CDate &date) const {
    return !(*this == date);
}

bool CDate::operator < (const CDate &date) const {
    return (this->_year < date._year
            || (this->_year == date._year && this->_month < date._month)
            || (this->_year == date._year && this->_month == date._month && this->_day < date._day));;
}

bool CDate::operator <= (const CDate &date) const {
    return (*this < date || *this == date);
}

bool CDate::operator > (const CDate &date) const {
    return !(*this <= date);
}

bool CDate::operator >= (const CDate &date) const {
    return !(*this < date);
}

bool CDate::isLeap(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int CDate::getDaysInMonth(int year, int month) {
    int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return days[month-1] + (month == 2 && isLeap(year) ? 1 : 0);
}

string CDate::zfill(const int &num) {
    return (num < 10 ? "0" : "") + to_string(num);
}

CDate CDate::operator++(int) {
    CDate tmp(*this);
    *this += 1;
    return tmp;
}

CDate CDate::operator--(int) {
    CDate tmp(*this);
    *this -= 1;
    return tmp;
}

CDate CDate::operator++() {
    *this += 1;
    return *this;
}

CDate CDate::operator--() {
    *this -= 1;
    return *this;
}

bool CDate::isValidDate(CDate date) {
    return !(date._month < 1 || date._month > 12 || date._day < 1 || date._day > getDaysInMonth(date._year, date._month));
}

ostream &operator<<(ostream &ost, const CDate &date) {
    return ost << date._year << "-" << CDate::zfill(date._month) << "-" << CDate::zfill(date._day);
}

istream &operator>>(istream &is, CDate &date) {
    bool success = true;
    int y, m, d;
    is >> y;
    if(is.get() != '-') success = false;
    is >> m;
    if(is.get() != '-') success = false;
    is >> d;
    if (m < 1 || m > 12 || d < 1 || d > CDate::getDaysInMonth(y, m))
        success = false;
    if(!success)
        is.setstate(ios::failbit);
    else {
        date._year = y;
        date._month = m;
        date._day = d;
    }
    return is;
}


#ifndef __PROGTEST__

int main() {
    ostringstream oss;
    istringstream iss;

    CDate a(2000, 1, 2);
    CDate b(2010, 2, 3);
    CDate c(2004, 2, 10);
    oss.str("");
    oss << a;
    assert (oss.str() == "2000-01-02");
    oss.str("");
    oss << b;
    assert (oss.str() == "2010-02-03");
    oss.str("");
    oss << c;
    assert (oss.str() == "2004-02-10");
    a = a + 1500;
    oss.str("");
    oss << a;
    assert (oss.str() == "2004-02-10");
    b = b - 2000;
    oss.str("");
    oss << b;
    assert (oss.str() == "2004-08-13");
    assert (b - a == 185);
    assert ((b == a) == false);
    assert ((b != a) == true);
    assert ((b <= a) == false);
    assert ((b < a) == false);
    assert ((b >= a) == true);
    assert ((b > a) == true);
    assert ((c == a) == true);
    assert ((c != a) == false);
    assert ((c <= a) == true);
    //
//    oss.str("");
//    oss << c;
//    cout << oss.str() << endl;
//    oss.str("");
//    oss << a;
//    cout << oss.str() << endl;
    //
    assert ((c < a) == false);
    assert ((c >= a) == true);
    assert ((c > a) == false);
    a = ++c;
    oss.str("");
    oss << a << " " << c;
    assert (oss.str() == "2004-02-11 2004-02-11");
    a = --c;
    oss.str("");
    oss << a << " " << c;
    assert (oss.str() == "2004-02-10 2004-02-10");
    a = c++;
    oss.str("");
    oss << a << " " << c;
    assert (oss.str() == "2004-02-10 2004-02-11");
    a = c--;
    oss.str("");
    oss << a << " " << c;
    assert (oss.str() == "2004-02-11 2004-02-10");
    iss.clear();
    iss.str("2015-09-03");
    assert ((iss >> a));
    oss.str("");
    oss << a;
    assert (oss.str() == "2015-09-03");
    a = a + 70;
    oss.str("");
    oss << a;
    assert (oss.str() == "2015-11-12");

    CDate d(2000, 1, 1);
    try {
        CDate e(2000, 32, 1);
        assert ("No exception thrown!" == nullptr);
    }
    catch (...) {
    }
    iss.clear();
    iss.str("2000-12-33");
    assert (!(iss >> d));
    oss.str("");
    oss << d;
    assert (oss.str() == "2000-01-01");
    iss.clear();
    iss.str("2000-11-31");
    assert (!(iss >> d));
    oss.str("");
    oss << d;
    assert (oss.str() == "2000-01-01");
    iss.clear();
    iss.str("2000-02-29");
    assert ((iss >> d));
    oss.str("");
    oss << d;
    assert (oss.str() == "2000-02-29");
    iss.clear();
    iss.str("2001-02-29");
    assert (!(iss >> d));
    oss.str("");
    oss << d;
    assert (oss.str() == "2000-02-29");

    //-----------------------------------------------------------------------------
    // bonus test examples
    //-----------------------------------------------------------------------------
//    CDate f(2000, 5, 12);
//    oss.str("");
//    oss << f;
//    assert (oss.str() == "2000-05-12");
//    oss.str("");
//    oss << date_format("%Y/%m/%d") << f;
//    assert (oss.str() == "2000/05/12");
//    oss.str("");
//    oss << date_format("%d.%m.%Y") << f;
//    assert (oss.str() == "12.05.2000");
//    oss.str("");
//    oss << date_format("%m/%d/%Y") << f;
//    assert (oss.str() == "05/12/2000");
//    oss.str("");
//    oss << date_format("%Y%m%d") << f;
//    assert (oss.str() == "20000512");
//    oss.str("");
//    oss << date_format("hello kitty") << f;
//    assert (oss.str() == "hello kitty");
//    oss.str("");
//    oss << date_format("%d%d%d%d%d%d%m%m%m%Y%Y%Y%%%%%%%%%%") << f;
//    assert (oss.str() == "121212121212050505200020002000%%%%%");
//    oss.str("");
//    oss << date_format("%Y-%m-%d") << f;
//    assert (oss.str() == "2000-05-12");
//    iss.clear();
//    iss.str("2001-01-1");
//    assert (!(iss >> f));
//    oss.str("");
//    oss << f;
//    assert (oss.str() == "2000-05-12");
//    iss.clear();
//    iss.str("2001-1-01");
//    assert (!(iss >> f));
//    oss.str("");
//    oss << f;
//    assert (oss.str() == "2000-05-12");
//    iss.clear();
//    iss.str("2001-001-01");
//    assert (!(iss >> f));
//    oss.str("");
//    oss << f;
//    assert (oss.str() == "2000-05-12");
//    iss.clear();
//    iss.str("2001-01-02");
//    assert ((iss >> date_format("%Y-%m-%d") >> f));
//    oss.str("");
//    oss << f;
//    assert (oss.str() == "2001-01-02");
//    iss.clear();
//    iss.str("05.06.2003");
//    assert ((iss >> date_format("%d.%m.%Y") >> f));
//    oss.str("");
//    oss << f;
//    assert (oss.str() == "2003-06-05");
//    iss.clear();
//    iss.str("07/08/2004");
//    assert ((iss >> date_format("%m/%d/%Y") >> f));
//    oss.str("");
//    oss << f;
//    assert (oss.str() == "2004-07-08");
//    iss.clear();
//    iss.str("2002*03*04");
//    assert ((iss >> date_format("%Y*%m*%d") >> f));
//    oss.str("");
//    oss << f;
//    assert (oss.str() == "2002-03-04");
//    iss.clear();
//    iss.str("C++09format10PA22006rulez");
//    assert ((iss >> date_format("C++%mformat%dPA2%Yrulez") >> f));
//    oss.str("");
//    oss << f;
//    assert (oss.str() == "2006-09-10");
//    iss.clear();
//    iss.str("%12%13%2010%");
//    assert ((iss >> date_format("%%%m%%%d%%%Y%%") >> f));
//    oss.str("");
//    oss << f;
//    assert (oss.str() == "2010-12-13");
//
//    CDate g(2000, 6, 8);
//    iss.clear();
//    iss.str("2001-11-33");
//    assert (!(iss >> date_format("%Y-%m-%d") >> g));
//    oss.str("");
//    oss << g;
//    assert (oss.str() == "2000-06-08");
//    iss.clear();
//    iss.str("29.02.2003");
//    assert (!(iss >> date_format("%d.%m.%Y") >> g));
//    oss.str("");
//    oss << g;
//    assert (oss.str() == "2000-06-08");
//    iss.clear();
//    iss.str("14/02/2004");
//    assert (!(iss >> date_format("%m/%d/%Y") >> g));
//    oss.str("");
//    oss << g;
//    assert (oss.str() == "2000-06-08");
//    iss.clear();
//    iss.str("2002-03");
//    assert (!(iss >> date_format("%Y-%m") >> g));
//    oss.str("");
//    oss << g;
//    assert (oss.str() == "2000-06-08");
//    iss.clear();
//    iss.str("hello kitty");
//    assert (!(iss >> date_format("hello kitty") >> g));
//    oss.str("");
//    oss << g;
//    assert (oss.str() == "2000-06-08");
//    iss.clear();
//    iss.str("2005-07-12-07");
//    assert (!(iss >> date_format("%Y-%m-%d-%m") >> g));
//    oss.str("");
//    oss << g;
//    assert (oss.str() == "2000-06-08");
//    iss.clear();
//    iss.str("20000101");
//    assert ((iss >> date_format("%Y%m%d") >> g));
//    oss.str("");
//    oss << g;
//    assert (oss.str() == "2000-01-01");

    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
