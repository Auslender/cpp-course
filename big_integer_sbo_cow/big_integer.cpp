//
// Created by maria on 28.05.16.
//
#include <iostream>
#include <sstream>
#include "big_integer.h"
#include <algorithm>

big_integer::big_integer(){
    this->pos = 1;
    this->number.push_back(0);
}

big_integer::big_integer(big_integer const &other) :
        pos (other.pos),
        number (other.number)
{}

big_integer::big_integer(int n)
{
    int64_t a = n;
    this->pos = true;
    if (a < 0) {
        this->pos = false;
        a = -a;
    }
    size_t sz = 1;
    while (a) {
        number.resize(sz);
        number[sz - 1] = a % BASE;
        sz++;
        a /= BASE;
    }
}

big_integer::big_integer(std::string const &str)
{
    bool sign = true;
    std::string s = str;
    this->pos = true;
    if (s[0] == '-') {
        sign = false;
    }
    big_integer power = 1;
    *this = 0;
    for (int32_t i = str.size() - 1; i >= !sign; i--)
    {
        *this += power.mul_long_short(str[i] - '0');
        power = power.mul_long_short(10);
    }
    this->pos = sign;
}

big_integer::~big_integer()
{

}

static inline void increase_size(my_container &a, uint32_t need)
{
    while (a.size() <= need) {
        a.push_back(0);
    }
}

big_integer& big_integer::add(big_integer const &b)
{
    int64_t carry = 0;
    for (size_t i = 0; i < b.number.size() || carry; i++) {
        increase_size(this->number, i);
        this->number[i] = (this->number[i] + (i < b.number.size() ? b.number[i] : 0) + carry);
        carry = 0;
        if (this->number[i] >= BASE) {
            this->number[i] -= BASE;
            carry = 1;
        }
    }
    return *this;
}

big_integer& big_integer::sub(big_integer const &b)
{
    int64_t borrow = 0;
    for (size_t i = 0; i < b.number.size() || borrow; i++) {
        increase_size(this->number, i);
        this->number[i] = this->number[i] - (i < b.number.size() ? b.number[i] : 0) - borrow;
        borrow = 0;
        if (this->number[i] < 0) {
            this->number[i] += BASE;
            borrow = 1;
        }
    }
    return *this;
}

int abs_comp(big_integer const &a, big_integer const &b)
{
    big_integer c = a;
    big_integer d = b;
    c.pos = 1;
    d.pos = 1;
    if (c > d)
        return 1;
    else if (c == d)
        return 0;
    else
        return -1;
}

std::string to_string(big_integer const &a)
{
    std::string str = "";
    big_integer ac = a, temp;
    ac.pos = true;
    while (ac != 0) {
        temp = (ac % 10);
        str = str + (char)(temp.number[0] + '0');
        ac /= 10;
    }
    std::reverse(str.begin(), str.end());
    if (!a.pos && str != "")
        str = "-" + str;
    if (str == "")
        str = "0";
    return str;
}

std::ostream &operator<<(std::ostream &s, big_integer const &a)
{
    return s << to_string(a);
}

big_integer &big_integer::operator=(big_integer const &other)
{
    pos = other.pos;
    number = my_container(other.number);
    return *this;
}

big_integer &big_integer::operator+=(big_integer const &rhs)
{
    if (this->pos == rhs.pos) {
        this->add(rhs);
    }
    else if (abs_comp(*this, rhs) >= 0) {
        this->sub(rhs);
    }
    else {
        big_integer temp = rhs;
        temp.sub(*this);
        *this = temp;
    }
    return *this;
}

big_integer &big_integer::operator-=(big_integer const &rhs)
{
    return *this += (-rhs);
}

big_integer &big_integer::operator++()
{
    *this += 1;
    return *this;
}

big_integer big_integer::operator++(int)
{
    big_integer ret = *this;
    ++*this;
    return ret;
}

big_integer &big_integer::operator--()
{
    *this -= 1;
    return *this;
}

big_integer big_integer::operator--(int)
{
    big_integer ret = *this;
    --*this;
    return ret;
}

bool operator==(big_integer const &a, big_integer const &b)
{
    size_t maxs = std::max(a.number.size(), b.number.size());
    bool zero = 0;
    for (size_t i = 0; i < maxs; i++)
    {
        if ((i >= a.number.size() && b.number[i]) || (i >= b.number.size() && a.number[i]) ||
            (i < a.number.size() && i < b.number.size() && a.number[i] != b.number[i]))
            return false;
        zero |= (i < a.number.size() ? a.number[i] : 0) | (i < b.number.size() ? b.number[i] : 0);
    }
    if (zero && a.pos != b.pos) {
        return false;
    }
    return true;
}


bool operator!=(big_integer const &a, big_integer const &b)
{
    return !(a == b);
}

bool operator<(big_integer const &a, big_integer const &b)
{
    int32_t maxs = std::max(a.number.size(), b.number.size());
    if (a == b)
        return false;
    if (a.pos != b.pos) {
        return !a.pos;
    }
    for (size_t i = maxs - 1; i >= 0; i--) {
        if ((i >= b.number.size() && a.number[i]) || (i < a.number.size() && i < b.number.size() && a.number[i] > b.number[i]))
            return false;
        else if (((i >= a.number.size()) && b.number[i]) || (i < a.number.size() && i < b.number.size() && a.number[i] < b.number[i]))
            return true;
    }
    return false;
}

bool operator>(big_integer const &a, big_integer const &b)
{
    return b < a;
}

bool operator<=(big_integer const &a, big_integer const &b)
{
    return (a == b || a < b);
}

bool operator>=(big_integer const &a, big_integer const &b)
{
    return b <= a;
}

big_integer big_integer::operator-() const
{
    big_integer neg = *this;
    neg.pos = !neg.pos;
    return neg;
}

big_integer big_integer::operator+() const
{
    big_integer t = *this;
    return t;
}

big_integer operator-(big_integer a, big_integer const &b)
{
    return a -= b;
}

big_integer operator+(big_integer a, big_integer const &b)
{
    return a += b;
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    if (rhs.number.size() == 1) {
        this->mul_long_short(rhs.number[0]);
    }
    big_integer ans;
    ans.number.resize(this->number.size() + rhs.number.size());
    int64_t t, carry;
    for (size_t i = 0; i < this->number.size(); i++) {
        carry = 0;
        for (size_t j = 0; j < rhs.number.size() || carry; j++) {
            t = ans.number[i + j] + number[i] * (j < rhs.number.size() ? rhs.number[j] : 0) + carry;
            ans.number[i + j] = (t % BASE);
            carry = t / BASE;
        }
    }
    ans.pos = (pos == rhs.pos);
    *this = ans;
    return *this;
}

big_integer big_integer::mul_long_short(int64_t d) const
{
    big_integer res = *this;
    int64_t carry = 0;
    int64_t curr;
    for (size_t i = 0; (i < res.number.size()) || carry; i++) {
        curr = ((i < res.number.size()) ? res.number[i] : 0) * d;
        curr += carry;
        increase_size(res.number, i);
        res.number[i] = curr;
        carry = 0;
        if (res.number[i] >= BASE) {
            carry = res.number[i] / BASE;
            res.number[i] %= BASE;
        }
        curr = 0;
    }
    return res;
}


big_integer operator*(big_integer a, big_integer const &b)
{
    return a *= b;
}

big_integer big_integer::div_long_short(int64_t d)
{
    int64_t carry = 0;
    for (int32_t i = this->number.size() - 1; i >= 0; i--) {
        int64_t temp = this->number[i] + carry * BASE;
        this->number[i] = temp / d;
        carry = temp % d;
    }
    return *this;
}


int64_t big_integer::div(big_integer const& b) {
    int64_t l = 0, r = BASE, mid;
    while (l < r - 1) {
        mid = (l + r) / 2;
        if (mid * b <= *this)
            l = mid;
        else
            r = mid;
    }
    *this -= l * b;
    return l;
}

inline void shift_v(big_integer& a) {
    for (int32_t i = a.number.size() - 1; i + 1 >= 0; i--) {
        increase_size(a.number, i + 1);
        if (i >= 0)
            a.number[i + 1] = a.number[i];
        else
            a.number[i + 1] = 0;
    }
}

big_integer& big_integer::operator/=(big_integer const& rhs)
{   if (rhs.number.size() == 1) {
        if (rhs < 0) {
            pos = !pos;
        }
        this->div_long_short(rhs.number.back());
        return *this;
    }
    bool q1 = this->pos, q2 = rhs.pos;
    big_integer rh = rhs;
    rh.pos = 1;
    big_integer res = 0;
    big_integer r = 0;
    for (int32_t i = this->number.size() - 1; i >= 0; i--)
    {
        if (r >= rh) {
            res.add(r.div(rh));
        }
        shift_v(res);
        shift_v(r);
        r.add((int)(this->number[i]));
    }
    if (r >= rh)
        res.add(r.div(rh));
    *this = res;
    this->pos = (q1 == q2);
    return *this;
}


big_integer operator/(big_integer a, big_integer const& b)
{
    return (a /= b);
}

big_integer& big_integer::operator%=(big_integer const& rhs)
{
    *this = *this - *this / rhs * rhs;
    return *this;
}

big_integer operator%(big_integer a, big_integer const& b)
{
    return (a %= b);
}

big_integer &big_integer::operator<<=(int rhs)
{
    int shift = rhs / POWER;
    int64_t s = this->number.size();
    increase_size(this->number, s + shift);
    for (int32_t i = s - 1; i >= 0 && shift; i--) {
        this->number[i + shift] = this->number[i];
        this->number[i] = 0;
    }
    *this *= (1 << (rhs % POWER));
    return *this;
}

big_integer operator<<(big_integer a, int b)
{
    return a <<= b;
}

big_integer &big_integer::operator>>=(int rhs) {
    int shift = rhs / POWER;
    bool ch = 0;
    if ( this->number[0] % 2 == 1 && *this < 0) {
        *this -= 1;
    }
    for (size_t i = shift; i < this->number.size(); i++) {
        if (number[i - shift] > 0) ch = true;
        number[i - shift] = number[i];
        if (shift) number[i] = 0;
    }
    *this /= (1 << (rhs % POWER));
    if (!pos && ch) {
        *this -= 1;
    }
    return *this;
}

big_integer operator>>(big_integer a, int b)
{
    return a >>= b;
}

big_integer big_integer::operator~() const
{
    big_integer ans = *this;
    return -ans - 1;
}

big_integer invert_bytes(big_integer const& a)
{
    big_integer inv = a;
    for (size_t i = 0; i < inv.number.size(); i++) {
        inv.number[i] = ~inv.number[i];
    }
    return inv;
}

big_integer additional_code(big_integer const& a)
{
    big_integer ad = a;
    if (!a.pos) {
        ad = invert_bytes(ad);
        ad.pos = 1;
        ad++;
    }
    return ad;
}

big_integer& big_integer::operator&=(big_integer const &rhs)
{
    big_integer l = additional_code(*this);
    big_integer r = additional_code(rhs);
    size_t maxs = std::max(l.number.size(), r.number.size());
    for (size_t i = 0; i < maxs; i++) {
        increase_size(this->number, i);
        this->number[i] = ((i < l.number.size()) ? l.number[i] : !this->pos) & ((i < r.number.size()) ? r.number[i] : !rhs.pos);
    }
    if (!this->pos & !rhs.pos) {
        *this = invert_bytes(*this);
        *this -=1;
    }
    if (*this < 0 && rhs > 0) {
        *this = -*this;
    }
    return *this;
}

big_integer operator&(big_integer a, big_integer const& b)
{
    return (a &= b);
}

big_integer& big_integer::operator|=(big_integer const &rhs)
{
    big_integer l = additional_code(*this);
    big_integer r = additional_code(rhs);
    size_t maxs = std::max(l.number.size(), r.number.size());
    for (size_t i = 0; i < maxs; i++) {
        increase_size(this->number, i);
        this->number[i] = ((i < l.number.size()) ? l.number[i] : !this->pos) | ((i < r.number.size()) ? r.number[i] : !rhs.pos);
    }
    if ((!this->pos) | (!rhs.pos)) {
        *this = invert_bytes(*this);
        *this -= 1;
    }
    if (*this >= 0 && rhs < 0) {
        *this = -*this - 2;
    }
    return *this;
}

big_integer operator|(big_integer a, big_integer const& b)
{
    return (a |= b);
}

big_integer& big_integer::operator^=(big_integer const &rhs)
{
    big_integer l = additional_code(*this);
    big_integer r = additional_code(rhs);
    size_t maxs = std::max(l.number.size(), r.number.size());
    for (size_t i = 0; i < maxs; i++) {
        increase_size(this->number, i);
        this->number[i] = ((i < l.number.size()) ? l.number[i] : !this->pos) ^ ((i < r.number.size()) ? r.number[i] : !rhs.pos);
    }
    if ((!this->pos) ^ (!rhs.pos)) {
        *this = invert_bytes(*this);
        *this -= 1;
    }
    if (*this >= 0 && rhs < 0) {
        *this = -*this - 2;
    }
    if (*this < 0 && rhs > 0) {
        *this = -*this;
    }
    return *this;
}

big_integer operator^(big_integer a, big_integer const& b)
{
    return (a ^= b);
}


/*int main() {
    big_integer c;
    big_integer a = 4;
    big_integer b = 7;
    a = b;
    std::cout << (a == b);
}*/