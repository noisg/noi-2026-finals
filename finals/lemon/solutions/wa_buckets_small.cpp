#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include "lemon.h"
 
static int g_n, g_pos;
static bool g_seen_lemon, g_lemon_in_set;
static std::set<int> g_last_set;
static int g_median, g_xor30;
static int g_count_a, g_count_b, g_eat_m;
static int g_eaten_a, g_eaten_b, g_eaten_m;
 
std::string init(int subtask, int n, std::vector<int> p) {
    g_n = n; g_pos = 0;
    g_seen_lemon = false; g_lemon_in_set = false;
    g_count_a = 0; g_count_b = 0; g_eat_m = 0;
    g_eaten_a = 0; g_eaten_b = 0; g_eaten_m = 0;
 
    // Tail size = 30 (Buckets 14, 1, 15)
    int K = 30, start = n - K + 1;
    g_last_set.clear();
    std::vector<int> last_sorted;
    for (int i = start; i <= n; i++) {
        g_last_set.insert(p[i]);
        last_sorted.push_back(p[i]);
    }
    std::sort(last_sorted.begin(), last_sorted.end());
 
    g_median = last_sorted[14]; 
    g_xor30 = 0;
    for (int x : last_sorted) g_xor30 ^= x;
 
    std::string B;
    for (int bit = 8; bit >= 0; bit--) B.push_back(((g_median >> bit) & 1) ? '1' : '0');
    for (int bit = 8; bit >= 0; bit--) B.push_back(((g_xor30 >> bit) & 1) ? '1' : '0');
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
            
            // Skip states where Y >= 29 (Reserved for fallback)
            int invalid[] = {479, 478, 463, 447};
            for (int inv : invalid) {
                if (v >= inv) v++;
            }
            
            g_count_a = std::min(14, v / 32);
            int rem = v % 32;
            g_eat_m = rem / 16;
            g_count_b = std::min(15, rem % 16);
        }
        return false;
    }
 
    if (!g_last_set.count(id)) return false;
    
    // Fallback: Lemon is in the tail, eat everything else (Y = 29)
    if (g_lemon_in_set) return true;
    if (!g_seen_lemon) return true;
 
    // Standard Encoding
    if (id == g_median) {
        if (g_eat_m && !g_eaten_m) { g_eaten_m = 1; return true; }
        return false;
    }
    if (id < g_median) {
        if (g_eaten_a < g_count_a) { g_eaten_a++; return true; }
        return false;
    } else {
        if (g_eaten_b < g_count_b) { g_eaten_b++; return true; }
        return false;
    }
}
 
int answer(int subtask, int n, std::string b, std::vector<int> uneaten) {
    int median = 0, xor30 = 0;
    for (int i = 0; i < 9; i++) median = (median << 1) | (b[i] - '0');
    for (int i = 9; i < 18; i++) xor30 = (xor30 << 1) | (b[i] - '0');
 
    std::set<int> uneaten_set(uneaten.begin(), uneaten.end());
    std::vector<int> eaten;
    for (int i = 1; i <= n; i++) {
        if (!uneaten_set.count(i)) eaten.push_back(i);
    }
 
    // Fallback Check: Exactly 29 eaten means lemon was in the tail
    if (eaten.size() == 29) {
        int xor_eaten = 0;
        for (int x : eaten) xor_eaten ^= x;
        int candidate = xor30 ^ xor_eaten;
        if (candidate >= 1 && candidate <= n && uneaten_set.count(candidate)) return candidate;
    }
 
    int count_a = 0, count_b = 0, m_eaten = 0;
    for (int x : eaten) {
        if (x < median) count_a++;
        else if (x > median) count_b++;
        else m_eaten = 1;
    }
 
    int v = count_a * 32 + m_eaten * 16 + count_b;
    
    // Reverse mapping
    int invalid_asc[] = {447, 463, 478, 479}; 
    for (int inv : invalid_asc) {
        if (v > inv) v--;
    }
 
    return v + 1;
}