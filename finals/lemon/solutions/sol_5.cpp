#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include "lemon.h"

static std::vector<int> g_perm;
static int g_n;
static std::vector<int> g_last9;
static std::vector<int> g_last9_sorted;
static std::set<int> g_last9_set;
static bool g_seen_lemon, g_lemon_in_set, g_ate;
static int g_lemon_pos;
static std::set<int> g_eat_set;
static int g_pos;

static int comb_table[10][10];

static void init_comb() {
    for (int i = 0; i <= 9; i++) {
        comb_table[i][0] = 1;
        for (int j = 1; j <= i; j++) comb_table[i][j] = comb_table[i-1][j-1] + comb_table[i-1][j];
        for (int j = i+1; j <= 9; j++) comb_table[i][j] = 0;
    }
}

static int rank_subset(const std::vector<int>& s) {
    int k = (int)s.size();
    int r = 0;
    for (int i = 0; i < k; i++) r += comb_table[s[i]][i + 1];
    return r;
}

static std::vector<int> unrank_subset(int k, int r) {
    std::vector<int> s(k);
    int cur = 8;
    for (int i = k - 1; i >= 0; i--) {
        while (comb_table[cur][i + 1] > r) cur--;
        s[i] = cur;
        r -= comb_table[cur][i + 1];
        cur--;
    }
    return s;
}

static int global_rank(int k, int r_k) {
    int offset = 0;
    for (int j = 2; j < k; j++) offset += comb_table[9][j];
    return offset + r_k;
}

static std::pair<int,int> global_unrank(int v) {
    int offset = 0;
    for (int k = 2; k <= 9; k++) {
        if (v < offset + comb_table[9][k]) return {k, v - offset};
        offset += comb_table[9][k];
    }
    return {9, v}; 
}

std::string init(int subtask, int n, std::vector<int> p) {
    init_comb();
    g_perm = p; g_n = n; g_pos = 0;
    g_seen_lemon = false; g_lemon_in_set = false; g_ate = false; g_lemon_pos = -1;

    int start = n - 8;
    g_last9.clear(); g_last9_sorted.clear(); g_last9_set.clear(); g_eat_set.clear();
    for (int i = start; i <= n; i++) {
        g_last9.push_back(p[i]); g_last9_set.insert(p[i]);
    }
    g_last9_sorted.assign(g_last9_set.begin(), g_last9_set.end());

    std::string B; B.reserve(81);
    for (int i = 0; i < 9; i++) {
        int val = g_last9[i];
        for (int bit = 8; bit >= 0; bit--) B.push_back(((val >> bit) & 1) ? '1' : '0');
    }
    return B;
}

bool receive_fruit(int id, bool is_lemon) {
    int pos = g_pos++;
    if (is_lemon) {
        g_seen_lemon = true;
        g_lemon_pos = pos;
        if (g_last9_set.count(id)) {
            g_lemon_in_set = true;
        } else {
            std::vector<int> non_last;
            for (int i = 1; i <= g_n; i++) if (!g_last9_set.count(i)) non_last.push_back(i);
            int rank = (int)(std::lower_bound(non_last.begin(), non_last.end(), id) - non_last.begin());
            auto [k, r_k] = global_unrank(rank);
            std::vector<int> indices = unrank_subset(k, r_k);
            for (int idx : indices) g_eat_set.insert(g_last9_sorted[idx]);
        }
        return false;
    }

    if (g_lemon_in_set) {
        if (g_seen_lemon && !g_ate && pos > g_lemon_pos) { g_ate = true; return true; }
        return false;
    }
    if (g_last9_set.count(id) && g_eat_set.count(id)) return true;
    return false;
}

int answer(int subtask, int n, std::string b, std::vector<int> uneaten) {
    init_comb();
    std::vector<int> last9_perm(9);
    for (int i = 0; i < 9; i++) {
        int val = 0;
        for (int bit = 8; bit >= 0; bit--) val |= (b[i * 9 + (8 - bit)] - '0') << bit;
        last9_perm[i] = val;
    }
    std::set<int> last9_set(last9_perm.begin(), last9_perm.end());
    std::vector<int> last9_sorted(last9_set.begin(), last9_set.end());
    std::set<int> uneaten_set(uneaten.begin(), uneaten.end());

    std::vector<int> eaten_from_last9;
    std::vector<int> eaten_indices; 
    for (int i = 0; i < 9; i++) {
        if (!uneaten_set.count(last9_sorted[i])) {
            eaten_from_last9.push_back(last9_sorted[i]);
            eaten_indices.push_back(i);
        }
    }

    int num_eaten = (int)eaten_from_last9.size();
    if (num_eaten <= 1) {
        if (num_eaten == 1) {
            int eaten_id = eaten_from_last9[0];
            for (int i = 0; i < 9; i++) {
                if (last9_perm[i] == eaten_id) return (i > 0) ? last9_perm[i - 1] : last9_perm[0];
            }
        }
        return last9_perm[8];
    }

    int v = global_rank(num_eaten, rank_subset(eaten_indices));
    std::vector<int> non_last;
    for (int i = 1; i <= n; i++) if (!last9_set.count(i)) non_last.push_back(i);
    return (v < (int)non_last.size()) ? non_last[v] : non_last[0];
}