#include "digits.h"
#include <string>
#include <algorithm>

std::string send_number(int k, int v) {
    std::string s = "";
    --v;
    int mod = (k == 9) ? 10 : 2;

    while (v) {
        char ch = '0' + (v % mod);
        s.push_back(ch);
        v /= mod;
    }

    std::reverse(s.begin(), s.end());
    return s;
}

int receive_number(int k, std::string s) {
    int v = 0;
    int mod = (k == 9) ? 10 : 2;

    for (int i = 0; i < (int)s.length(); ++i) {
        v = v * mod + s[i] - '0';
    }

    return ++v;
}
