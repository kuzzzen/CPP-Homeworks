#ifndef __PROGTEST__

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <set>
#include <map>
#include <list>
#include <utility>
#include <vector>
#include <memory>

using namespace std;

struct CCoord {
public:
    explicit CCoord(int x = 0, int y = 0) : m_X(x), m_Y(y) {}
    int m_X, m_Y;
};


#endif /* __PROGTEST__ */

class CShape {
public:
    explicit CShape(const int id, const char type) : m_Id(id), m_Type(type) {}
    virtual ~CShape() = default;
    [[nodiscard]] virtual bool hasPoint(const CCoord & point) const = 0;
    [[nodiscard]] virtual unique_ptr<CShape> getPtr() const = 0;
    friend class CScreen;
protected:
    int m_Id;
    char m_Type;
};

class CRectangle : public CShape {
public:
    CRectangle (const int id, const int x1, const int y1, const int x2, const int y2): CShape(id, 'r'), m_x1(x1), m_y1(y1), m_x2(x2), m_y2(y2) {}
    [[nodiscard]] bool hasPoint(const CCoord & point) const override {
        auto x_min = min(m_x1, m_x2), x_max = max(m_x1, m_x2);
        auto y_min = min(m_y1, m_y2), y_max = max(m_y1, m_y2);
        return (point.m_X >= x_min && point.m_X <= x_max && point.m_Y >= y_min && point.m_Y <= y_max);
    }
    [[nodiscard]] unique_ptr<CShape> getPtr() const override {
        return make_unique<CRectangle>(*this);
    }
private:
    int m_x1, m_y1, m_x2, m_y2;
};

class CCircle : public CShape {
public:
    CCircle (const int id, const int x, const int y, const int r) : CShape(id, 'c'), m_x(x), m_y(y), m_r(r) {}
    [[nodiscard]] bool hasPoint(const CCoord & point) const override {
        return pow(point.m_X - m_x, 2) + pow(point.m_Y - m_y, 2) <= pow(m_r, 2);
    }
    [[nodiscard]] unique_ptr<CShape> getPtr() const override {
        return make_unique<CCircle>(*this);
    }
private:
    int m_x, m_y, m_r;
};

class CTriangle : public CShape {
public:
    CTriangle (const int id, const CCoord & a, const CCoord & b, const CCoord & c) : CShape(id, 't'), xa(a.m_X), xb(b.m_X), xc(c.m_X), ya(a.m_Y), yb(b.m_Y), yc(c.m_Y) {}
    [[nodiscard]] bool hasPoint(const CCoord & point) const override {
        long px = point.m_X, py = point.m_Y;
        long side1 = ((xa-px)*(yb-ya)-(ya-py)*(xb-xa));
        long side2 = ((xb-px)*(yc-yb)-(yb-py)*(xc-xb));
        long side3 = ((xc-px)*(ya-yc)-(yc-py)*(xa-xc));
        return ((side1 == side2 && side1 == side3 && side1 == 0)
        || (side1 > 0 && side2 > 0 && side3 > 0)
        || (side1 < 0 && side2 < 0 && side3 < 0));
    }
    [[nodiscard]] unique_ptr<CShape> getPtr() const override {
        return make_unique<CTriangle>(*this);
    }
private:
        long xa, xb, xc, ya, yb, yc;
};

class CPolygon : public CShape {
public:
    CPolygon (const int id, const CCoord * b, const CCoord * e) : CShape(id, 'p'), coords(b, e) {}

    template<typename Iter>
    CPolygon (const int id, const Iter b, const Iter e) : CShape(id, 'p') { coords.assign(b, e); }

    CPolygon(const int id, const initializer_list<CCoord> &coordList) : CShape(id, 'p'), coords{coordList} {}

    template<typename Head, typename... Tail>
    CPolygon(const int id, const Head h, const Tail... tail) : CShape(id, 'p'), coords{tail...} { coords.push_back(h); }

    [[nodiscard]] unique_ptr<CShape> getPtr() const override {
        return make_unique<CPolygon>(*this);
    }

    [[nodiscard]] bool hasPoint(const CCoord &point) const override {
        long numVertPoints = (long) coords.size();
        if (numVertPoints == 3) return CTriangle(this->m_Id, coords[0], coords[1], coords[2]).hasPoint(point);

        long px = point.m_X, py = point.m_Y;
        long j = numVertPoints - 1;
        bool res = false;
        for (long i = 0; i < numVertPoints; i++) {
            long xi = coords[i].m_X, xj = coords[j].m_X, yi = coords[i].m_Y, yj = coords[j].m_Y;
            if ((yi > py) == (yj > py)) {
                j = i;
                continue;
            }
            auto cross = (xj - xi) * (py - yi) / (yj - yi) + xi;
            if (px < cross) res = !res;
            j = i;
        }
        return res;
    }
private:
    vector<CCoord> coords;
};

class CScreen {
public:
    CScreen() = default;

    void add(const CShape & shape) {
        shapes.emplace_back(shape.getPtr());
    }

    [[nodiscard]] vector<int> test(int x, int y) const {
        vector<int> ids;
        for (const auto &shape: shapes) {
            if (shape->hasPoint(CCoord(x, y)))
                ids.emplace_back(shape->m_Id);
        }
        return ids;
    }

    void optimize() {}
private:
    vector <unique_ptr<CShape>> shapes;
};


#ifndef __PROGTEST__

int main() {
    CScreen s;
    s.add(CTriangle(3, CCoord(0, 0), CCoord(0, 20), CCoord(-10, 0)));
    s.test(0, 20);

    CScreen s0;
    s0.add(CRectangle(1, 10, 20, 30, 40));
    s0.add(CRectangle(2, 20, 10, 40, 30));
    s0.add(CTriangle(3, CCoord(10, 20), CCoord(20, 10), CCoord(30, 30)));
    s0.optimize();
    assert (s0.test(0, 0) == (vector<int>{}));
    assert (s0.test(21, 21) == (vector<int>{1, 2, 3}));
    assert (s0.test(16, 17) == (vector<int>{3}));
    assert (s0.test(30, 22) == (vector<int>{1, 2}));
    assert (s0.test(35, 25) == (vector<int>{2}));

    CScreen s1;
    s1.add(CCircle(1, 10, 10, 15));
    s1.add(CCircle(2, 30, 10, 15));
    s1.add(CCircle(3, 20, 20, 15));
    s1.optimize();
    assert (s1.test(0, 0) == (vector<int>{1}));
    assert (s1.test(15, 15) == (vector<int>{1, 3}));
    assert (s1.test(20, 11) == (vector<int>{1, 2, 3}));
    assert (s1.test(32, 8) == (vector<int>{2}));

    CScreen s2;
    CCoord vertex21[] = {CCoord(10, 0), CCoord(20, 20), CCoord(30, 20), CCoord(40, 0)};
    s2.add(CPolygon(1, vertex21, vertex21 + 4));
    CCoord vertex22[] = {CCoord(20, 10), CCoord(10, 20), CCoord(25, 30), CCoord(40, 20), CCoord(30, 10)};
    s2.add(CPolygon(2, vertex22, vertex22 + 5));
    s2.optimize();
    assert (s2.test(25, 15) == (vector<int>{1, 2}));
    assert (s2.test(25, 25) == (vector<int>{2}));
    assert (s2.test(15, 3) == (vector<int>{1}));
    assert (s2.test(11, 10) == (vector<int>{}));

    CScreen s3;
    std::initializer_list<CCoord> vertex31 = {CCoord(10, 0), CCoord(20, 20), CCoord(30, 20), CCoord(40, 0)};
    s3.add(CPolygon(1, vertex31.begin(), vertex31.end()));
    std::list<CCoord> vertex32 = {CCoord(20, 10), CCoord(10, 20), CCoord(25, 30), CCoord(40, 20), CCoord(30, 10)};
    s3.add(CPolygon(2, vertex32.begin(), vertex32.end()));
    s3.optimize();
    assert (s3.test(25, 15) == (vector<int>{1, 2}));
    assert (s3.test(25, 25) == (vector<int>{2}));
    assert (s3.test(15, 3) == (vector<int>{1}));
    assert (s3.test(11, 10) == (vector<int>{}));
    s3.add(CPolygon(3, std::initializer_list<CCoord>{CCoord(10, 0), CCoord(20, 20), CCoord(30, 20), CCoord(40, 0)}));
    s3.add(CPolygon(4, {CCoord(20, 10), CCoord(10, 20), CCoord(25, 30), CCoord(40, 20), CCoord(30, 10)}));
    s3.add(CPolygon(5, CCoord(20, 10), CCoord(10, 20), CCoord(25, 30), CCoord(40, 20), CCoord(30, 10)));
    s3.optimize();
    assert (s3.test(25, 15) == (vector<int>{1, 2, 3, 4, 5}));
    assert (s3.test(25, 25) == (vector<int>{2, 4, 5}));
    assert (s3.test(15, 3) == (vector<int>{1, 3}));
    assert (s3.test(11, 10) == (vector<int>{}));

    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
