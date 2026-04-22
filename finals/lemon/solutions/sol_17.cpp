#include "lemon.h"
#include <vector>
#include <string>
#include <algorithm>
#include <set>

using namespace std;

// ==========================================
// SHARED UTILITIES
// ==========================================

static int gcd(int a, int b) {
    while (b) { a %= b; swap(a, b); }
    return a;
}

static vector<int> g_valid_masks;

static void build_masks() {
    if (!g_valid_masks.empty()) return;
    for (int i = 0; i < 512; i++) {
        // Exclude mask size 8. This guarantees exactly 8 pairs with 2 eaten 
        // and 1 pair with 1 eaten uniquely means "Lemon is in T U P".
        // Remaining 503 masks are enough for the 482 IDs outside the pairs.
        if (__builtin_popcount(i) != 8) {
            g_valid_masks.push_back(i);
        }
    }
}

static int pair_id[9][505];
static int pair_elements[9][255][2];
static bool families_built = false;

static void build_families(int n) {
    if (families_built) return;
    vector<int> k_vals;
    
    // Insert user's requested superincreasing k values
    int preferred_k[] = {1, 3, 7, 13, 27, 53, 107, 213};
    for (int k : preferred_k) {
        if (gcd(k, n) == 1) k_vals.push_back(k);
    }
    
    // Fill up to 9 valid coprime k's just in case
    int cur_k = 1;
    while (k_vals.size() < 9) {
        if (gcd(cur_k, n) == 1) {
            bool used = false;
            for (int x : k_vals) if (x == cur_k) used = true;
            if (!used) k_vals.push_back(cur_k);
        }
        cur_k++;
    }

    for (int f = 0; f < 9; f++) {
        // Family 0: k=1, S=0. Family 1: k=1, S=1. Families 2..8: k=k[1..7], S=0.
        int k = (f == 1) ? k_vals[0] : k_vals[(f == 0) ? 0 : f - 1];
        int S = (f == 1) ? 1 : 0;
        
        for (int m = 0; m < n / 2; m++) {
            int u = (S + 2LL * m * k) % n;
            int v = (S + (2LL * m + 1) * k) % n;
            pair_elements[f][m][0] = u;
            pair_elements[f][m][1] = v;
            pair_id[f][u] = m;
            pair_id[f][v] = m;
        }
    }
    families_built = true;
}

// ==========================================
// ALICE'S STATE & LOGIC
// ==========================================

static int g_N;
static int g_f;
static set<int> g_T;
static set<int> g_P;
static vector<int> g_sorted_pairs;

static bool g_seen_lemon;
static bool g_lemon_in_TP;
static int g_lemon_id;
static int g_pos;
static int g_mask;

string init(int subtask, int n, vector<int> p) {
    build_masks(); 
    build_families(n);
    g_N = n;
    g_pos = 0;
    g_seen_lemon = false;
    g_lemon_in_TP = false;
    g_lemon_id = -1;
    g_T.clear(); g_P.clear(); g_sorted_pairs.clear();

    vector<int> T_vec;
    for (int i = n - 9 + 1; i <= n; i++) {
        T_vec.push_back(p[i] - 1); // map to 0-based
        g_T.insert(p[i] - 1);
    }

    // 1. Find a valid pairing family
    g_f = 0;
    for (int f = 0; f < 9; f++) {
        set<int> pids;
        bool ok = true;
        for (int x : T_vec) {
            if (pids.count(pair_id[f][x])) { ok = false; break; }
            pids.insert(pair_id[f][x]);
        }
        if (ok) { g_f = f; break; }
    }

    // 2. Resolve P set and sort the pairs to synchronize with Bob
    for (int x : T_vec) {
        int pid = pair_id[g_f][x];
        g_sorted_pairs.push_back(pid);
        int u = pair_elements[g_f][pid][0];
        int v = pair_elements[g_f][pid][1];
        if (u == x) g_P.insert(v);
        else g_P.insert(u);
    }
    sort(g_sorted_pairs.begin(), g_sorted_pairs.end());

    // 3. Send 4-bit representation of the chosen family
    string B = "";
    for (int i = 3; i >= 0; i--) B += ((g_f >> i) & 1) ? '1' : '0';
    return B;
}

bool receive_fruit(int id, bool is_lemon) {
    int x = id - 1;
    bool in_T = g_T.count(x);
    bool in_P = g_P.count(x);
    
    // If we've hit the tail fruits and haven't seen the lemon yet, 
    // it unequivocally guarantees the lemon is in T.
    if (g_pos == g_N - 9 && !g_seen_lemon) {
        g_lemon_in_TP = true;
    }
    g_pos++;
    
    if (is_lemon) {
        g_seen_lemon = true;
        g_lemon_id = x;
        if (in_T || in_P) {
            g_lemon_in_TP = true;
        } else {
            // Lemon outside T U P. Compute target rank among valid fruits
            int rank = 0;
            for (int i = 0; i < g_N; i++) {
                if (g_T.count(i) || g_P.count(i)) continue;
                if (i == x) break;
                rank++;
            }
            g_mask = g_valid_masks[rank];
        }
        return false; // Contestant must not eat the lemon
    }
    
    if (g_lemon_in_TP) {
        // CASE 1: Lemon in T U P. Eat all 17 OTHER fruits in T U P.
        if (in_T || in_P) return true;
        return false;
    } else {
        // CASE 2: Lemon outside. Eat all of P, and a mapped subset of T.
        if (in_P) return true;
        
        if (in_T) {
            int pid = pair_id[g_f][x];
            int bit_idx = -1;
            for (int i = 0; i < 9; i++) {
                if (g_sorted_pairs[i] == pid) { bit_idx = i; break; }
            }
            return (g_mask >> bit_idx) & 1;
        }
        return false;
    }
}

// ==========================================
// BOB'S STATE & LOGIC
// ==========================================

int answer(int subtask, int n, string b, vector<int> uneaten) {
    build_masks(); 
    build_families(n);
    
    // 1. Decode family ID
    int f = 0;
    for (int i = 0; i < 4; i++) {
        f = (f << 1) | (b[i] - '0');
    }
    
    // 2. Identify all missing fruits
    set<int> eaten;
    for (int i = 0; i < n; i++) eaten.insert(i);
    for (int id : uneaten) if(id) eaten.erase(id - 1);
    
    int eaten_count[255] = {0};
    for (int x : eaten) eaten_count[pair_id[f][x]]++;
    
    int c2 = 0, c1 = 0;
    for (int i = 0; i < n / 2; i++) {
        if (eaten_count[i] == 2) c2++;
        else if (eaten_count[i] == 1) c1++;
    }
    
    // 3. CASE 1 Check: Lemon was in T U P
    // This state is impossible to accidentally hit in Case 2 due to `__builtin_popcount != 8`
    if (c2 == 8 && c1 == 1) {
        for (int i = 0; i < n / 2; i++) {
            if (eaten_count[i] == 1) {
                int u = pair_elements[f][i][0];
                int v = pair_elements[f][i][1];
                if (!eaten.count(u)) return u + 1;
                if (!eaten.count(v)) return v + 1;
            }
        }
    }
    
    
    // 4. CASE 2: Reconstruct the Mask and target Rank
    vector<int> sp;
    for (int i = 0; i < n / 2; i++) {
        if (eaten_count[i] >= 1) sp.push_back(i);
    }
    sort(sp.begin(), sp.end()); // Automatically sorted by pair ID, syncing with Alice
    
    int mask = 0;
    for (int i = 0; i < 9; i++) {
        if (eaten_count[sp[i]] == 2) {
            mask |= (1 << i);
        }
    }
    
    int rank = -1;
    for (int i = 0; i < (int)g_valid_masks.size(); i++) {
        if (g_valid_masks[i] == mask) {
            rank = i;
            break;
        }
    }
    
    // 5. Build T U P set dynamically
    set<int> TP;
    for (int pid : sp) {
        TP.insert(pair_elements[f][pid][0]);
        TP.insert(pair_elements[f][pid][1]);
    }
    
    // 6. Navigate to the absolute ID using the rank among remaining candidates
    for (int i = 0; i < n; i++) {
        if (TP.count(i)) continue;
        if (rank == 0) return i + 1;
        rank--;
    }
    
    return 1; // Fallback
}