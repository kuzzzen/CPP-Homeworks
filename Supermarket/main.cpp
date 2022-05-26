#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <queue>
#include <stack>
#include <deque>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>

using namespace std;
#endif /* __PROGTEST__ */

class CDate {
    friend class CSupermarket;
public:
    CDate(uint16_t year, uint16_t month, uint16_t day) : _year(year), _month(month), _day(day) {}
    bool operator < (const CDate &date) const;
    bool operator == (const CDate &date) const;
private:
    uint16_t _year, _month, _day;
};

bool CDate::operator<(const CDate &date) const {
    return (this->_year < date._year
            || (this->_year == date._year && this->_month < date._month)
            || (this->_year == date._year && this->_month == date._month && this->_day < date._day));;
}

bool CDate::operator == (const CDate &date) const {
    return (this->_year == date._year && this->_month == date._month && this->_day == date._day);
}

struct Item {
    mutable int _count;
    CDate _expiryDate;
};

bool operator < (const Item &left, const Item &right) { return left._expiryDate < right._expiryDate; }
bool operator == (const Item &left, const Item &right) { return left._expiryDate == right._expiryDate; }

class CSupermarket {
public:
    CSupermarket() = default;
    CSupermarket& store(const string &name, const CDate &expiryDate, const int &count);
    void sell (list<pair<string,int>> &lst);
    list<pair<string, int>> expired (const CDate &date) const;
    static bool differentByOne (const string &left, const string &right);
private:
    unordered_map<string, set<Item>> items;
    int sellItems(const string& name, int left_to_sell);
};

bool CSupermarket::differentByOne(const string &left, const string &right) {
    if (left.size() != right.size()) return false;
    auto first_mismatch = mismatch(left.begin(), left.end(), right.begin());
    advance(first_mismatch.first, 1);
    advance(first_mismatch.second, 1);
    return (mismatch(first_mismatch.first, left.end(), first_mismatch.second).first == left.end());
}

CSupermarket& CSupermarket::store(const string &name, const CDate &expiryDate, const int &count) {
    Item tmp{count, expiryDate};
    if(items.count(name)) {
        auto it = items[name].find(tmp);
        if (it == items[name].end())
            items[name].insert(tmp);
        else
            it->_count += count;
    } else {
        items[name] = set<Item>();
        items[name].insert(tmp);
    }
    return *this;
}



void CSupermarket::sell(list<pair<string, int>> &lst) {
    int sold = 0;
    unordered_set<string> to_rem;
//     debug
//    for (auto &itemMapPair: items) {
//        cout << itemMapPair.first << " : ";
//        for (const auto &el: itemMapPair.second)
//            cout << el._expiryDate._day << "-" <<  el._expiryDate._month << "-" <<  el._expiryDate._year << ", " << el._count << endl;
//        cout << "\n\n";
//    }
//
    for (auto& p: lst) {
        if (items.count(p.first)) {
            sold = sellItems(p.first, p.second);
            p.second -= sold;
            if (items[p.first].empty())
                to_rem.insert(p.first);
        } else {
            string name_to_del; int count_to_del = INT32_MAX; bool sell = false;
            for (auto& itemMapPair : items) {
                auto diff = differentByOne(itemMapPair.first, p.first);
                if (diff && name_to_del.empty() && count_to_del == INT32_MAX) {
                    name_to_del = itemMapPair.first;
                    count_to_del = p.second;
                    sell = true;
                } else if (diff && !(name_to_del.empty() && count_to_del == INT32_MAX)) {
                    sell = false;
                    break;
                }
            }
            if (sell) {
                sold = sellItems(name_to_del, count_to_del);
                p.second -= sold;
            }
            if (items[name_to_del].empty())
                to_rem.insert(name_to_del);
        }
    }
    lst.erase(remove_if(lst.begin(), lst.end(), [&](auto &pr) {
        return pr.second <= 0;
    }), lst.end());

    for (const auto &el: to_rem)
        items.erase(el);
}

list<pair<string, int>> CSupermarket::expired(const CDate &date) const {
    list<pair<string, int>> exp;
    // debug
//    for (auto &itemMapPair: items) {
//        cout << itemMapPair.first << " : ";
//        for (const auto &el: itemMapPair.second)
//            cout << el._expiryDate._day << "-" <<  el._expiryDate._month << "-" <<  el._expiryDate._year << ", " << el._count << endl;
//        cout << "\n\n";
//    }
    //
    for (auto &itemMapPair: items) {
        auto tmpCnt = 0;
        for (auto rit : itemMapPair.second) {
            if (!(rit._expiryDate < date)) {
                break;
            } else {
                tmpCnt += rit._count;
            }
        }
        if (tmpCnt > 0)
            exp.emplace_back(itemMapPair.first, tmpCnt);
    }
    exp.sort ([] (auto const &left, auto const &right) {
        return !(left.second < right.second);
    });
    return exp;
}

int CSupermarket::sellItems(const string& name, int left_to_sell) {
    int sold = 0;
    while (left_to_sell != 0) {
        auto oldest = items[name].begin();
        if (oldest == items[name].end()) {
            break;
        }
        if (oldest->_count <= left_to_sell) {
            sold += oldest->_count;
            left_to_sell -= oldest->_count;
            items[name].erase(oldest);
        } else {
            sold += left_to_sell;
            oldest->_count -= left_to_sell;
            left_to_sell = 0;
        }
    }
    return sold;
}


#ifndef __PROGTEST__

int main() {
    CSupermarket s;
    s.store("bread", CDate(2016, 4, 30), 100)
            .store("butter", CDate(2016, 5, 10), 10)
            .store("beer", CDate(2016, 8, 10), 50)
            .store("bread", CDate(2016, 4, 25), 100)
            .store("okey", CDate(2016, 7, 18), 5);

    list<pair<string, int>> l0 = s.expired(CDate(2018, 4, 30));
    assert(l0.size() == 4);
    assert((l0 == list<pair<string, int> >{{"bread",  200},
                                           {"beer",   50},
                                           {"butter", 10},
                                           {"okey",   5}}));
//
    list<pair<string, int>> l1{{"bread",  2},
                               {"Coke",   5},
                               {"butter", 20}};
    s.sell(l1);
    assert(l1.size() == 2);
    assert((l1 == list<pair<string, int> >{{"Coke",   5},
                                           {"butter", 10}}));

    list<pair<string, int>> l2 = s.expired(CDate(2016, 4, 30));
//    for (auto &el : l2)
//        cout << el.first << " - " << el.second << endl;
//    cout << l2.size() << endl;
    assert(l2.size() == 1);
    assert((l2 == list<pair<string, int> >{{"bread", 98}}));
//
    list<pair<string, int>> l3 = s.expired(CDate(2016, 5, 20));
    assert(l3.size() == 1);
    assert((l3 == list<pair<string, int> >{{"bread", 198}}));

    list<pair<string, int>> l4{{"bread", 105}};
    s.sell(l4);
    assert(l4.size() == 0);
    assert((l4 == list<pair<string, int> >{}));

    list<pair<string, int>> l5 = s.expired(CDate(2017, 1, 1));
    assert(l5.size() == 3);
    assert((l5 == list<pair<string, int> >{{"bread", 93},
                                           {"beer",  50},
                                           {"okey",  5}}));

    s.store("Coke", CDate(2016, 12, 31), 10);

    list<pair<string, int>> l6{{"Cake",  1},
                               {"Coke",  1},
                               {"cake",  1},
                               {"coke",  1},
                               {"cuke",  1},
                               {"Cokes", 1}};
    s.sell(l6);
    assert(l6.size() == 3);
    assert((l6 == list<pair<string, int> >{{"cake",  1},
                                           {"cuke",  1},
                                           {"Cokes", 1}}));

    list<pair<string, int>> l7 = s.expired(CDate(2017, 1, 1));
    assert(l7.size() == 4);
    assert((l7 == list<pair<string, int> >{{"bread", 93},
                                           {"beer",  50},
                                           {"Coke",  7},
                                           {"okey",  5}}));

    s.store("cake", CDate(2016, 11, 1), 5);

    list<pair<string, int>> l8{{"Cake", 1},
                               {"Coke", 1},
                               {"cake", 1},
                               {"coke", 1},
                               {"cuke", 1}};
    s.sell(l8);
    assert(l8.size() == 2);
    assert((l8 == list<pair<string, int> >{{"Cake", 1},
                                           {"coke", 1}}));

    list<pair<string, int>> l9 = s.expired(CDate(2017, 1, 1));
    assert(l9.size() == 5);
    assert((l9 == list<pair<string, int> >{{"bread", 93},
                                           {"beer",  50},
                                           {"Coke",  6},
                                           {"okey",  5},
                                           {"cake",  3}}));

    list<pair<string, int>> l10{{"cake", 15},
                                {"Cake", 2}};
    s.sell(l10);
    assert(l10.size() == 2);
    assert((l10 == list<pair<string, int> >{{"cake", 12},
                                            {"Cake", 2}}));

    list<pair<string, int>> l11 = s.expired(CDate(2017, 1, 1));
    assert(l11.size() == 4);
    assert((l11 == list<pair<string, int> >{{"bread", 93},
                                            {"beer",  50},
                                            {"Coke",  6},
                                            {"okey",  5}}));

    list<pair<string, int>> l12{{"Cake", 4}};
    s.sell(l12);
//    for (auto &el: l12)
//        cout << el.first << " - " << el.second << endl;
    assert(l12.size() == 0);
    assert((l12 == list<pair<string, int> >{}));

    list<pair<string, int>> l13 = s.expired(CDate(2017, 1, 1));
    assert(l13.size() == 4);
    assert((l13 == list<pair<string, int> >{{"bread", 93},
                                            {"beer",  50},
                                            {"okey",  5},
                                            {"Coke",  2}}));

    list<pair<string, int>> l14{{"Beer", 20},
                                {"Coke", 1},
                                {"bear", 25},
                                {"beer", 10}};
    s.sell(l14);
    assert(l14.size() == 1);
    assert((l14 == list<pair<string, int> >{{"beer", 5}}));

    s.store("ccccb", CDate(2019, 3, 11), 100)
            .store("ccccd", CDate(2019, 6, 9), 100)
            .store("dcccc", CDate(2019, 2, 14), 100);

    list<pair<string, int>> l15{{"ccccc", 10}};
    s.sell(l15);
    assert(l15.size() == 1);
    assert((l15 == list<pair<string, int> >{{"ccccc", 10}}));
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
