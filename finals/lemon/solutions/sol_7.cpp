#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include "lemon.h"

static int g_n, g_start, g_pos;
static bool g_seen_lemon, g_lemon_in_set;
static int g_keep_a_id, g_keep_b_id;
static std::set<int> g_last_set;
static std::vector<int> g_bucketA, g_bucketB;

std::string init(int subtask, int n, std::vector<int> p) {
    g_n = n; g_start = n - 42; g_pos = 0; // Tail size = 43
    g_seen_lemon = false; g_lemon_in_set = false;
    g_keep_a_id = -1; g_keep_b_id = -1;

    g_last_set.clear();
    std::vector<int> last43;
    for (int i = g_start; i <= n; i++) {
        last43.push_back(p[i]); g_last_set.insert(p[i]);
    }
    std::sort(last43.begin(), last43.end());
    // Buckets 21 and 22 provide exactly 462 states (>= N-K = 457)
    g_bucketA.assign(last43.begin(), last43.begin() + 21);
    g_bucketB.assign(last43.begin() + 21, last43.end());

    int xor_a = 0, xor_b = 0;
    for (int x : g_bucketA) xor_a ^= x;
    for (int x : g_bucketB) xor_b ^= x;

    std::string B;
    for (int bit = 8; bit >= 0; bit--) B.push_back(((xor_a >> bit) & 1) ? '1' : '0');
    for (int bit = 8; bit >= 0; bit--) B.push_back(((xor_b >> bit) & 1) ? '1' : '0');
    return B;
}

bool receive_fruit(int id, bool is_lemon) {
    g_pos++;
    if (is_lemon) {
        g_seen_lemon = true;
        if (g_last_set.count(id)) { g_lemon_in_set = true; }
        else {
            std::vector<int> non_last;
            for (int i = 1; i <= g_n; i++) if (!g_last_set.count(i)) non_last.push_back(i);
            int rank = (int)(std::lower_bound(non_last.begin(), non_last.end(), id) - non_last.begin());
            g_keep_a_id = g_bucketA[rank / 22];
            g_keep_b_id = g_bucketB[rank % 22];
        }
        return false;
    }
    if (!g_last_set.count(id)) return false;
    if (g_seen_lemon) {
        if (g_lemon_in_set) return true;
        return (id != g_keep_a_id && id != g_keep_b_id);
    }
    return true; 
}

int answer(int subtask, int n, std::string b, std::vector<int> uneaten) {
    int xor_a = 0, xor_b = 0;
    for (int i = 0; i < 9; i++) xor_a = (xor_a << 1) | (b[i] - '0');
    for (int i = 9; i < 18; i++) xor_b = (xor_b << 1) | (b[i] - '0');

    std::set<int> uneaten_set(uneaten.begin(), uneaten.end());
    std::vector<int> eaten;
    for (int i = 1; i <= n; i++) if (!uneaten_set.count(i)) eaten.push_back(i);

    if (eaten.size() >= 42) {
        int xor_eaten = 0;
        for (int x : eaten) xor_eaten ^= x;
        return xor_a ^ xor_b ^ xor_eaten;
    }

    int xor_eaten = 0;
    for (int x : eaten) xor_eaten ^= x;
    int target = xor_a ^ xor_b ^ xor_eaten; 

    std::vector<int> prefix_xor(42, 0);
    for (int i = 0; i < 41; i++) prefix_xor[i + 1] = prefix_xor[i] ^ eaten[i];

    for (int x : uneaten) {
        if (x == 0) continue;
        int y = x ^ target;
        if (y <= 0 || y > n || y == x || !uneaten_set.count(y) || x > y) continue;

        int pos_x = (int)(std::lower_bound(eaten.begin(), eaten.end(), x) - eaten.begin());
        int pos_y = (int)(std::lower_bound(eaten.begin(), eaten.end(), y) - eaten.begin());
        int y_merged = pos_y + 1;

        int xor_check;
        if (pos_x >= 21) xor_check = prefix_xor[21];
        else if (y_merged >= 21) xor_check = prefix_xor[20] ^ x;
        else xor_check = prefix_xor[19] ^ x ^ y;

        if (xor_check != xor_a) continue;

        std::vector<int> last43_sorted = eaten;
        last43_sorted.push_back(x); last43_sorted.push_back(y);
        std::sort(last43_sorted.begin(), last43_sorted.end());

        int keep_a = -1, keep_b = -1;
        for (int i = 0; i < 21; i++) if (last43_sorted[i] == x || last43_sorted[i] == y) { keep_a = last43_sorted[i]; break; }
        for (int i = 21; i < 43; i++) if (last43_sorted[i] == x || last43_sorted[i] == y) { keep_b = last43_sorted[i]; break; }

        if (keep_a == -1 || keep_b == -1) continue; 

        int r_a = -1, r_b = -1;
        for (int i = 0; i < 21; i++) if (last43_sorted[i] == keep_a) { r_a = i; break; }
        for (int i = 21; i < 43; i++) if (last43_sorted[i] == keep_b) { r_b = i - 21; break; }

        int v = r_a * 22 + r_b;
        std::set<int> last43_set(last43_sorted.begin(), last43_sorted.end());
        std::vector<int> non_last;
        for (int i = 1; i <= n; i++) if (!last43_set.count(i)) non_last.push_back(i);

        if (v >= 0 && v < (int)non_last.size()) return non_last[v];
    }
    return uneaten[1];
}