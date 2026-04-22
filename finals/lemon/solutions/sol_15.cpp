#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include "lemon.h"

static int g_n, g_pos;
static bool g_seen_lemon, g_lemon_in_set;
static std::set<int> g_last_set;
static int g_sep1, g_sep2, g_xor15;
static int g_count_a, g_count_b, g_count_c, g_eat_s1, g_eat_s2;
static int g_eaten_a, g_eaten_b, g_eaten_c, g_eaten_s1, g_eaten_s2;

std::string init(int subtask, int n, std::vector<int> p) {
    g_n = n; g_pos = 0; g_seen_lemon = false; g_lemon_in_set = false;
    g_count_a = g_count_b = g_count_c = g_eat_s1 = g_eat_s2 = 0;
    g_eaten_a = g_eaten_b = g_eaten_c = g_eaten_s1 = g_eaten_s2 = 0;

    // Tail size = 15 (Buckets 5, 1, 4, 1, 4)
    int K = 15, start = n - K + 1;
    g_last_set.clear();
    std::vector<int> last_sorted;
    for (int i = start; i <= n; i++) {
        g_last_set.insert(p[i]); last_sorted.push_back(p[i]);
    }
    std::sort(last_sorted.begin(), last_sorted.end());

    g_sep1 = last_sorted[5];
    g_sep2 = last_sorted[10];
    g_xor15 = 0;
    for (int x : last_sorted) g_xor15 ^= x;

    std::string B;
    auto encode9 = [&](int val) {
        for (int bit = 8; bit >= 0; bit--) B.push_back(((val >> bit) & 1) ? '1' : '0');
    };
    encode9(g_sep1); encode9(g_sep2); encode9(g_xor15);
    return B;
}

bool receive_fruit(int id, bool is_lemon) {
    g_pos++;
    if (is_lemon) {
        g_seen_lemon = true;
        if (g_last_set.count(id)) {
            g_lemon_in_set = true;
        } else {
            // Encode Absolute ID
            int v = id - 1;
            
            // Skip states where Y >= 14 (Reserved for fallback)
            int invalid[] = {499, 549, 589, 594, 598, 599};
            for (int inv : invalid) {
                if (v >= inv) v++;
            }
            
            g_count_a = v / 100;
            int rem = v % 100;
            g_eat_s1 = rem / 50;
            rem %= 50;
            g_count_b = rem / 10;
            rem %= 10;
            g_eat_s2 = rem / 5;
            g_count_c = rem % 5;
        }
        return false;
    }

    if (!g_last_set.count(id)) return false;
    
    // Fallback: Lemon is in the tail, eat everything else (Y = 14)
    if (g_lemon_in_set) return true;
    if (!g_seen_lemon) return true;

    // Standard Encoding
    if (id == g_sep1) { if (g_eat_s1 && !g_eaten_s1) { g_eaten_s1 = 1; return true; } return false; }
    if (id == g_sep2) { if (g_eat_s2 && !g_eaten_s2) { g_eaten_s2 = 1; return true; } return false; }
    if (id < g_sep1) { if (g_eaten_a < g_count_a) { g_eaten_a++; return true; } return false; }
    if (id < g_sep2) { if (g_eaten_b < g_count_b) { g_eaten_b++; return true; } return false; }
    if (g_eaten_c < g_count_c) { g_eaten_c++; return true; }
    return false;
}

int answer(int subtask, int n, std::string b, std::vector<int> uneaten) {
    auto decode9 = [&](int start) {
        int val = 0;
        for (int i = start; i < start + 9; i++) val = (val << 1) | (b[i] - '0');
        return val;
    };
    int sep1 = decode9(0), sep2 = decode9(9), xor15 = decode9(18);

    std::set<int> uneaten_set(uneaten.begin(), uneaten.end());
    std::vector<int> eaten;
    for (int i = 1; i <= n; i++) {
        if (!uneaten_set.count(i)) eaten.push_back(i);
    }

    // Fallback Check: Exactly 14 eaten means lemon was in the tail
    if (eaten.size() == 14) {
        int xor_eaten = 0;
        for (int x : eaten) xor_eaten ^= x;
        int candidate = xor15 ^ xor_eaten;
        if (candidate >= 1 && candidate <= n && uneaten_set.count(candidate)) return candidate;
    }

    int ca = 0, cb = 0, cc = 0, ms1 = 0, ms2 = 0;
    for (int x : eaten) {
        if (x == sep1) ms1 = 1;
        else if (x == sep2) ms2 = 1;
        else if (x < sep1) ca++;
        else if (x < sep2) cb++;
        else cc++;
    }

    int v = ca * 100 + ms1 * 50 + cb * 10 + ms2 * 5 + cc;
    
    // Reverse mapping (Process descending)
    int invalid_desc[] = {599, 598, 594, 589, 549, 499};
    for (int inv : invalid_desc) {
        if (v > inv) v--;
    }

    return v + 1;
}