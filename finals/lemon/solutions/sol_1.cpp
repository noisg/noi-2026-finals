#include <vector>
#include <string>
#include <algorithm>
#include "lemon.h"

static std::vector<int> g_perm;
static bool g_seen_lemon;
static bool g_ate;

std::string init(int subtask, int n, std::vector<int> p) {
    g_perm = p;
    g_seen_lemon = false;
    g_ate = false;
    std::string B;
    B.reserve(n * 9);
    for (int i = 1; i <= n; i++) {
        for (int bit = 8; bit >= 0; bit--) {
            B.push_back(((p[i] >> bit) & 1) ? '1' : '0');
        }
    }
    return B;
}

bool receive_fruit(int id, bool is_lemon) {
    if (is_lemon) {
        g_seen_lemon = true;
        return false;
    }
    if (g_seen_lemon && !g_ate) {
        g_ate = true;
        return true;
    }
    return false;
}

int answer(int subtask, int n, std::string b, std::vector<int> uneaten) {
    std::vector<int> perm(n);
    for (int i = 0; i < n; i++) {
        int val = 0;
        for (int bit = 8; bit >= 0; bit--) {
            val |= (b[i * 9 + (8 - bit)] - '0') << bit;
        }
        perm[i] = val;
    }

    std::vector<bool> present(n + 1, false);
    for (int x : uneaten) present[x] = true;

    for (int i = 0; i < n; i++) {
        if (!present[perm[i]]) {
            return perm[i - 1];
        }
    }
    return perm[n - 1];
}