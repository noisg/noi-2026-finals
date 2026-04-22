#include "lemon.h"
#include <vector>
#include <string>
#include <algorithm>
#include <set>

using namespace std;

static vector<int> g_valid_masks;

static void build_masks() {
    if (!g_valid_masks.empty()) return;
    for (int i = 0; i < 512; i++) {
        // Safely exclude only mask size 8. Unlocks 503 valid combinations.
        if (__builtin_popcount(i) != 8) g_valid_masks.push_back(i);
    }
}

// DP to count unlabelled strict binary trees with edge weights >= 1
static long long dp[10][10] = {0}; 
static bool dp_built = false;

static void build_dp() {
    if (dp_built) return;
    
    // Base case: 1 internal node (both children are leaves)
    for (int b = 0; b <= 8; b++) dp[1][b] = 1;
    
    for (int n = 2; n <= 8; n++) {
        for (int b = 1; b <= 8; b++) {
            long long total = 0;
            // Iterate over size of left internal subtree
            for (int s1 = 0; s1 <= n - 1; s1++) {
                int s2 = n - 1 - s1;
                
                long long left_ways = 0;
                if (s1 == 0) left_ways = 1; 
                else for (int w = 1; w <= b; w++) left_ways += dp[s1][b - w];
                
                long long right_ways = 0;
                if (s2 == 0) right_ways = 1; 
                else for (int w = 1; w <= b; w++) right_ways += dp[s2][b - w];
                
                total += left_ways * right_ways;
            }
            dp[n][b] = total;
        }
    }
    dp_built = true;
}

static long long get_ways(int n, int b) {
    if (n == 0) return 1;
    long long res = 0;
    for (int w = 1; w <= b; w++) res += dp[n][b - w];
    return res;
}

struct TrieNode { int left = -1, right = -1; };
struct AuxNode { int left = -1, right = -1, depth = 0, leaf_rank = -1, size_internal = 0; bool is_leaf = false; };

static int g_N, g_step, g_lemon_id, g_lemon_pos;
static bool g_lemon_seen;
static vector<int> g_last_9;
static vector<TrieNode> a_trie;
static vector<AuxNode> a_aux;

static int compress_trie(int u, int depth) {
    if (depth == 9) {
        int id = a_aux.size(); a_aux.push_back(AuxNode());
        a_aux[id].is_leaf = true; a_aux[id].depth = 9; a_aux[id].size_internal = 0; return id;
    }
    if (a_trie[u].left != -1 && a_trie[u].right == -1) return compress_trie(a_trie[u].left, depth + 1);
    if (a_trie[u].left == -1 && a_trie[u].right != -1) return compress_trie(a_trie[u].right, depth + 1);
    
    int L = compress_trie(a_trie[u].left, depth + 1);
    int R = compress_trie(a_trie[u].right, depth + 1);
    
    int id = a_aux.size(); a_aux.push_back(AuxNode());
    a_aux[id].is_leaf = false; a_aux[id].depth = depth; a_aux[id].left = L; a_aux[id].right = R;
    a_aux[id].size_internal = 1 + a_aux[L].size_internal + a_aux[R].size_internal;
    return id;
}

static long long encode_node(int u, int n, int b) {
    if (n == 1) return 0;
    
    long long idx = 0;
    int L_child = a_aux[u].left;
    int R_child = a_aux[u].right;
    int actual_s1 = a_aux[L_child].size_internal;
    int actual_s2 = a_aux[R_child].size_internal;
    int actual_w1 = (actual_s1 > 0) ? (a_aux[L_child].depth - a_aux[u].depth) : 0;
    int actual_w2 = (actual_s2 > 0) ? (a_aux[R_child].depth - a_aux[u].depth) : 0;

    for (int s1 = 0; s1 <= n - 1; s1++) {
        int s2 = n - 1 - s1;
        long long L_tot = get_ways(s1, b);
        long long R_tot = get_ways(s2, b);
        
        if (s1 == actual_s1) {
            long long l_idx = 0;
            if (s1 > 0) {
                for (int w1 = 1; w1 < actual_w1; w1++) l_idx += dp[s1][b - w1];
                l_idx += encode_node(L_child, s1, b - actual_w1);
            }
            
            long long r_idx = 0;
            if (s2 > 0) {
                for (int w2 = 1; w2 < actual_w2; w2++) r_idx += dp[s2][b - w2];
                r_idx += encode_node(R_child, s2, b - actual_w2);
            }
            
            idx += l_idx * R_tot + r_idx;
            return idx;
        } else {
            idx += L_tot * R_tot;
        }
    }
    return idx;
}

static long long encode_full_tree(int root_id) {
    long long idx = 0;
    int r_d = a_aux[root_id].depth;
    for (int d = 0; d < r_d; d++) idx += dp[8][8 - d];
    idx += encode_node(root_id, 8, 8 - r_d);
    return idx;
}

string init(int subtask, int n, vector<int> p) {
    build_masks(); build_dp();
    g_N = n; g_step = 0; g_lemon_id = -1; g_lemon_pos = -1; g_lemon_seen = false; g_last_9.clear();
    a_trie.assign(1, TrieNode()); a_aux.clear();

    int xor_sum = 0;
    for (int i = n - 8; i <= n; i++) { g_last_9.push_back(p[i]); xor_sum ^= p[i]; }
    vector<int> sorted_9 = g_last_9; sort(sorted_9.begin(), sorted_9.end());

    for (int val : sorted_9) {
        int u = 0;
        for (int b = 8; b >= 0; b--) {
            int bit = (val >> b) & 1;
            if (bit == 0) { if (a_trie[u].left == -1) { a_trie[u].left = a_trie.size(); a_trie.push_back(TrieNode()); } u = a_trie[u].left; }
            else { if (a_trie[u].right == -1) { a_trie[u].right = a_trie.size(); a_trie.push_back(TrieNode()); } u = a_trie[u].right; }
        }
    }
    
    int root_id = compress_trie(0, 0);
    
    // OPTIMIZATION 1: Keep only the lower 8 bits of the XOR
    int xor8 = xor_sum & 255;
    
    long long tree_index = encode_full_tree(root_id);
    long long V = tree_index * 256LL + xor8;
    
    // OPTIMIZATION 2: Variable length bitstring
    long long map_val = V + 1;
    string B = "";
    bool found_msb = false;
    for (int i = 35; i >= 0; i--) { 
        if ((map_val >> i) & 1) {
            if (!found_msb) found_msb = true; // Drop leading '1'
            else B += '1';
        } else {
            if (found_msb) B += '0';
        }
    }
    return B;
}

bool receive_fruit(int id, bool is_lemon) {
    int pos = g_step++;
    if (pos < g_N - 9) {
        if (is_lemon) { g_lemon_seen = true; g_lemon_id = id; g_lemon_pos = pos; }
        return false; 
    }
    if (g_lemon_seen && g_lemon_pos < g_N - 9) {
        vector<int> sorted_9 = g_last_9; sort(sorted_9.begin(), sorted_9.end());
        int rank = find(sorted_9.begin(), sorted_9.end(), id) - sorted_9.begin();
        return ((g_valid_masks[g_lemon_id - 1] >> rank) & 1); 
    }
    return !is_lemon; 
}

// ==========================================
// BOB'S STATE & LOGIC
// ==========================================
static vector<AuxNode> b_aux;
static int b_leaf_counter;

static int decode_node(int n, int b, long long idx, int current_depth) {
    int u = b_aux.size(); b_aux.push_back(AuxNode());
    b_aux[u].depth = current_depth;
    
    if (n == 1) {
        b_aux[u].is_leaf = false; b_aux[u].size_internal = 1;
        
        int L = b_aux.size(); b_aux.push_back(AuxNode());
        b_aux[L].is_leaf = true; b_aux[L].depth = 9; b_aux[L].leaf_rank = b_leaf_counter++;
        
        int R = b_aux.size(); b_aux.push_back(AuxNode());
        b_aux[R].is_leaf = true; b_aux[R].depth = 9; b_aux[R].leaf_rank = b_leaf_counter++;
        
        b_aux[u].left = L; b_aux[u].right = R;
        return u;
    }
    
    b_aux[u].is_leaf = false; b_aux[u].size_internal = n;
    
    int target_s1 = -1;
    long long l_idx = 0, r_idx = 0, R_tot = 0;
    
    for (int s1 = 0; s1 <= n - 1; s1++) {
        int s2 = n - 1 - s1;
        long long L_t = get_ways(s1, b);
        long long R_t = get_ways(s2, b);
        
        if (idx < L_t * R_t) {
            target_s1 = s1; R_tot = R_t;
            l_idx = idx / R_t; r_idx = idx % R_t;
            break;
        } else {
            idx -= L_t * R_t;
        }
    }
    
    int s2 = n - 1 - target_s1;
    
    // Decode Left Child
    if (target_s1 == 0) {
        int L = b_aux.size(); b_aux.push_back(AuxNode());
        b_aux[L].is_leaf = true; b_aux[L].depth = 9; b_aux[L].leaf_rank = b_leaf_counter++;
        b_aux[u].left = L;
    } else {
        int w1 = 1;
        while (w1 <= b) {
            if (l_idx < dp[target_s1][b - w1]) break;
            l_idx -= dp[target_s1][b - w1];
            w1++;
        }
        b_aux[u].left = decode_node(target_s1, b - w1, l_idx, current_depth + w1);
    }
    
    // Decode Right Child
    if (s2 == 0) {
        int R = b_aux.size(); b_aux.push_back(AuxNode());
        b_aux[R].is_leaf = true; b_aux[R].depth = 9; b_aux[R].leaf_rank = b_leaf_counter++;
        b_aux[u].right = R;
    } else {
        int w2 = 1;
        while (w2 <= b) {
            if (r_idx < dp[s2][b - w2]) break;
            r_idx -= dp[s2][b - w2];
            w2++;
        }
        b_aux[u].right = decode_node(s2, b - w2, r_idx, current_depth + w2);
    }
    
    return u;
}

static int decode_full_tree(long long idx) {
    int r_d = 0;
    while (r_d <= 8) {
        if (idx < dp[8][8 - r_d]) break;
        idx -= dp[8][8 - r_d];
        r_d++;
    }
    return decode_node(8, 8 - r_d, idx, r_d);
}

// Helps determine MSB deduction when Aux root diverges at bit 8
static int count_leaves(int u) {
    if (b_aux[u].is_leaf) return 1;
    return count_leaves(b_aux[u].left) + count_leaves(b_aux[u].right);
}

int answer(int subtask, int n, string b, vector<int> uneaten) {
    build_masks(); build_dp();
    
    // Inverse the variable-length bitstring
    long long map_val = 1;
    for (char c : b) {
        map_val = (map_val << 1) | (c - '0');
    }
    long long V = map_val - 1;
    
    long long tree_index = V / 256LL;
    int expected_xor_8bit = V % 256;
    
    vector<int> E; int xor_e = 0, p_idx = 1;
    for (int i = 1; i <= n; i++) {
        if (p_idx < (int)uneaten.size() && uneaten[p_idx] == i) p_idx++;
        else { E.push_back(i); xor_e ^= i; }
    }
    
    b_aux.clear(); b_leaf_counter = 0;
    int root_id = decode_full_tree(tree_index);
    
    // Fallback: Lemon is in the tail (we ate 8 other fruits)
    if (E.size() == 8) {
        int L_msb = 0;
        
        
        // If segment tree does not split at MSB, all items (including the Lemon) share the same MSB as the eaten fruits.
        if (b_aux[root_id].depth > 0) {
            L_msb = (E[0] >> 8) & 1;
        } 
        // If it DOES split at MSB, infer Lemon's side mathematically via population counts
        else {
            int c0 = 0;
            for (int x : E) {
                if (((x >> 8) & 1) == 0) c0++;
            }
            int S_L = count_leaves(b_aux[root_id].left);
            L_msb = (S_L > c0) ? 0 : 1;
        }
        
        int L_lower = expected_xor_8bit ^ (xor_e & 255);
        return (L_msb << 8) | L_lower;
    }
    
    // Normal routing via subset paths
    int missing_mask = 0;
    for (int x : E) {
        int curr = root_id;
        while (!b_aux[curr].is_leaf) {
            int split_bit = 8 - b_aux[curr].depth;
            if (((x >> split_bit) & 1) == 0) curr = b_aux[curr].left;
            else curr = b_aux[curr].right;
        }
        missing_mask |= (1 << b_aux[curr].leaf_rank);
    }
    
    for (int i = 0; i < (int)g_valid_masks.size(); i++) if (g_valid_masks[i] == missing_mask) return i + 1; 
    return 1; 
}