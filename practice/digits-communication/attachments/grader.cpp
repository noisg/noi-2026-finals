#include "digits.h"
#include <iostream>
#include <string>
#include <vector>

/**
 * Returns the number of distinct digits in a given string,
 * or -1 if a non-digit character is found
 */
int count_distinct_digits(const std::string &s) {
    int d = 0;
    bool found[10];
    std::fill(found, found + 10, false);

    for (char ch : s) {
        if (!('0' <= ch && ch <= '9')) {
            return -1;
        }

        if (!found[ch - '0']) {
            found[ch - '0'] = true;
            ++d;
        }
    }

    return d;
}

int main() {
    int T, K, D = 0;
    std::vector<int> V;
    std::vector<std::string> S;

    std::cin >> T >> K;
    for (int i = 0; i < T; ++i) {
        int v;
        std::cin >> v;
        V.push_back(v);
    }

    // First run: Call send_number only
    for (int i = 0; i < T; ++i) {
        std::string s = send_number(K, V[i]);
        S.push_back(s);

        if ((int)s.length() > K) {
            std::cout << "Wrong Answer: String " << s
                      << " exceeds maximum allowed length k = " << K << "\n";
            return -1;
        }

        int d = count_distinct_digits(s);
        if (d == -1) {
            std::cout << "Wrong Answer: String " << s
                      << " contains a non-digit character\n";
            return -1;
        }
        D = std::max(D, d);
    }

    // Second run: Call receive_number only
    for (int i = 0; i < T; ++i) {
        int v = receive_number(K, S[i]);
        if (v != V[i]) {
            std::cout << "Wrong Answer: Got " << v << ", "
                      << "but expected " << V[i] << "\n";
            return -1;
        }
    }

    // Success
    std::cout << "D = " << D << "\n";
    return 0;
}
