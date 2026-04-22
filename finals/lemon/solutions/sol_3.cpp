#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include "lemon.h"

static std::vector<int> g_perm;
static int g_n;
static int g_lemon_pos;
static std::vector<int> g_last32;
static std::vector<int> g_last32_sorted;
static std::set<int> g_last32_set;
static bool g_lemon_in_last32;
static int g_encode_val;
static bool g_seen_lemon;
static bool g_ate;
static int g_pos_counter;
static int g_eat_i, g_eat_j;
static int g_eaten_count;

std::string init(int subtask, int n, std::vector<int> p) {
    g_perm = p; g_n = n; g_seen_lemon = false; g_ate = false;
    g_pos_counter = 0; g_lemon_pos = -1; g_eaten_count = 0;

    g_last32.clear(); g_last32_sorted.clear(); g_last32_set.clear();
    int start = n - 31;
    for (int i = start; i <= n; i++) {
        g_last32.push_back(p[i]);
        g_last32_set.insert(p[i]);
    }
    g_last32_sorted = g_last32;
    std::sort(g_last32_sorted.begin(), g_last32_sorted.end());

    std::string B; B.reserve(32 * 9);
    for (int i = 0; i < 32; i++) {
        int val = g_last32[i];
        for (int bit = 8; bit >= 0; bit--) B.push_back(((val >> bit) & 1) ? '1' : '0');
    }

    g_eat_i = -1; g_eat_j = -1;
    g_lemon_in_last32 = false; g_encode_val = -1;
    return B;
}

bool receive_fruit(int id, bool is_lemon) {
    int pos = g_pos_counter++;
    int start = g_n - 32;

    if (is_lemon) {
        g_lemon_pos = pos;
        g_seen_lemon = true;
        if (pos >= start) {
            g_lemon_in_last32 = true;
        } else {
            std::vector<int> non_last32;
            non_last32.reserve(g_n - 32);
            for (int i = 1; i <= g_n; i++) {
                if (g_last32_set.find(i) == g_last32_set.end()) non_last32.push_back(i);
            }
            int rank = (int)(std::lower_bound(non_last32.begin(), non_last32.end(), id) - non_last32.begin());
            g_encode_val = rank;
            int v = rank;
            g_eat_j = 1;
            while ((g_eat_j + 1) * g_eat_j / 2 <= v) g_eat_j++;
            g_eat_i = v - g_eat_j * (g_eat_j - 1) / 2;
        }
        return false;
    }

    if (g_lemon_in_last32) {
        if (g_seen_lemon && !g_ate && pos > g_lemon_pos) {
            g_ate = true; return true;
        }
        return false;
    }

    if (pos >= start && g_seen_lemon && !g_lemon_in_last32) {
        int idx = (int)(std::lower_bound(g_last32_sorted.begin(), g_last32_sorted.end(), id) - g_last32_sorted.begin());
        if (idx == g_eat_i || idx == g_eat_j) {
            if (g_eaten_count < 2) {
                g_eaten_count++; return true;
            }
        }
    }
    return false;
}

int answer(int subtask, int n, std::string b, std::vector<int> uneaten) {
    std::vector<int> last32_perm(32);
    for (int i = 0; i < 32; i++) {
        int val = 0;
        for (int bit = 8; bit >= 0; bit--) val |= (b[i * 9 + (8 - bit)] - '0') << bit;
        last32_perm[i] = val;
    }

    std::vector<int> last32_sorted = last32_perm;
    std::sort(last32_sorted.begin(), last32_sorted.end());
    std::set<int> last32_set(last32_sorted.begin(), last32_sorted.end());
    std::set<int> uneaten_set(uneaten.begin(), uneaten.end());
    
    std::vector<int> eaten_from_last32;
    for (int x : last32_sorted) {
        if (uneaten_set.find(x) == uneaten_set.end()) eaten_from_last32.push_back(x);
    }

    if (eaten_from_last32.size() <= 1) {
        if (eaten_from_last32.size() == 1) {
            int eaten_id = eaten_from_last32[0];
            for (int i = 0; i < 32; i++) {
                if (last32_perm[i] == eaten_id) return (i > 0) ? last32_perm[i - 1] : last32_perm[0];
            }
        }
        return last32_perm[31];
    }

    int idx_i = (int)(std::lower_bound(last32_sorted.begin(), last32_sorted.end(), eaten_from_last32[0]) - last32_sorted.begin());
    int idx_j = (int)(std::lower_bound(last32_sorted.begin(), last32_sorted.end(), eaten_from_last32[1]) - last32_sorted.begin());
    if (idx_i > idx_j) std::swap(idx_i, idx_j);

    int v = idx_j * (idx_j - 1) / 2 + idx_i;

    std::vector<int> non_last32;
    non_last32.reserve(n - 32);
    for (int i = 1; i <= n; i++) {
        if (last32_set.find(i) == last32_set.end()) non_last32.push_back(i);
    }
    return (v < (int)non_last32.size()) ? non_last32[v] : non_last32[0];
}