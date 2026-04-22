#include "lemon.h"
#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include <numeric>

using namespace std;

static vector<int> g_valid_masks;

static void build_masks() {
    if (!g_valid_masks.empty()) return;
    for (int i = 0; i < 512; i++) {
        if (__builtin_popcount(i) != 8) g_valid_masks.push_back(i);
    }
}

struct AuxNode { int left = -1, right = -1, depth = 0, leaf_rank = -1, gap_idx = -1; bool is_leaf = false; };

static int g_N;
static int g_step;
static int g_lemon_id;
static int g_lemon_pos;
static bool g_lemon_seen;
static vector<int> g_last_9;

struct TrieNode { int left = -1, right = -1; };

static vector<TrieNode> a_trie;
static vector<AuxNode> a_aux;
static vector<int> a_weights;
static int a_leaf_count, a_internal_count;
static vector<int> a_internal_nodes;

static int compress_trie(int u, int depth) {
    if (depth == 9) {
        int id = a_aux.size(); a_aux.push_back(AuxNode());
        a_aux[id].is_leaf = true; a_aux[id].depth = 9; return id;
    }
    if (a_trie[u].left != -1 && a_trie[u].right == -1) return compress_trie(a_trie[u].left, depth + 1);
    if (a_trie[u].left == -1 && a_trie[u].right != -1) return compress_trie(a_trie[u].right, depth + 1);
    int L = compress_trie(a_trie[u].left, depth + 1);
    int R = compress_trie(a_trie[u].right, depth + 1);
    int id = a_aux.size(); a_aux.push_back(AuxNode());
    a_aux[id].is_leaf = false; a_aux[id].depth = depth; a_aux[id].left = L; a_aux[id].right = R;
    return id;
}

static void a_inorder(int u) {
    if (a_aux[u].is_leaf) { a_aux[u].leaf_rank = a_leaf_count++; return; }
    a_inorder(a_aux[u].left);
    a_aux[u].gap_idx = a_internal_count;
    a_internal_nodes[a_internal_count] = u;
    a_internal_count++;
    a_inorder(a_aux[u].right);
}

static void preorder_weights(int u) {
    if (a_aux[u].is_leaf) return;
    int L = a_aux[u].left;
    if (!a_aux[L].is_leaf) { a_weights.push_back(a_aux[L].depth - a_aux[u].depth); preorder_weights(L); }
    int R = a_aux[u].right;
    if (!a_aux[R].is_leaf) { a_weights.push_back(a_aux[R].depth - a_aux[u].depth); preorder_weights(R); }
}

string init(int subtask, int n, vector<int> p) {
    build_masks(); g_N = n; g_step = 0; g_lemon_id = -1; g_lemon_pos = -1; g_lemon_seen = false;
    g_last_9.clear(); a_trie.assign(1, TrieNode()); a_aux.clear(); a_weights.clear();
    a_leaf_count = 0; a_internal_count = 0; a_internal_nodes.assign(8, 0);

    int xor_sum = 0;
    for (int i = n - 8; i <= n; i++) { g_last_9.push_back(p[i]); xor_sum ^= p[i]; }
    vector<int> sorted_9 = g_last_9;
    sort(sorted_9.begin(), sorted_9.end());

    for (int val : sorted_9) {
        int u = 0;
        for (int b = 8; b >= 0; b--) {
            int bit = (val >> b) & 1;
            if (bit == 0) { if (a_trie[u].left == -1) { a_trie[u].left = a_trie.size(); a_trie.push_back(TrieNode()); } u = a_trie[u].left; } 
            else { if (a_trie[u].right == -1) { a_trie[u].right = a_trie.size(); a_trie.push_back(TrieNode()); } u = a_trie[u].right; }
        }
    }
    
    int root_id = compress_trie(0, 0);
    int root_depth = a_aux[root_id].depth;
    a_inorder(root_id);
    
    vector<int> P(8); iota(P.begin(), P.end(), 0);
    sort(P.begin(), P.end(), [&](int a, int b) {
        int u = a_internal_nodes[a], v = a_internal_nodes[b];
        if (a_aux[u].depth != a_aux[v].depth) return a_aux[u].depth > a_aux[v].depth; 
        return a < b;
    });
    
    long long perm_val = 0;
    vector<int> avail(8); iota(avail.begin(), avail.end(), 0);
    for (int x : P) {
        int idx = find(avail.begin(), avail.end(), x) - avail.begin();
        perm_val = perm_val * avail.size() + idx;
        avail.erase(avail.begin() + idx);
    }
    
    preorder_weights(root_id);
    string B = "";
    for (int i = 15; i >= 0; i--) B += ((perm_val >> i) & 1) ? '1' : '0';
    for (int i = 2; i >= 0; i--) B += ((root_depth >> i) & 1) ? '1' : '0';
    for (int i = 0; i < 7; i++) {
        int w = (i < (int)a_weights.size()) ? a_weights[i] - 1 : 0;
        for (int j = 2; j >= 0; j--) B += ((w >> j) & 1) ? '1' : '0';
    }
    for (int i = 8; i >= 0; i--) B += ((xor_sum >> i) & 1) ? '1' : '0';
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

static vector<AuxNode> b_aux;

int answer(int subtask, int n, string b, vector<int> uneaten) {
    build_masks();
    
    long long perm_val = 0;
    for (int i = 0; i < 16; i++) perm_val = (perm_val << 1) | (b[i] - '0');
    
    int root_depth = 0;
    for (int i = 0; i < 3; i++) root_depth = (root_depth << 1) | (b[16 + i] - '0');
    
    vector<int> b_weights(7);
    for (int w = 0; w < 7; w++) {
        int weight = 0;
        for (int i = 0; i < 3; i++) weight = (weight << 1) | (b[19 + w * 3 + i] - '0');
        b_weights[w] = weight + 1;
    }
    
    int expected_xor = 0;
    for (int i = 0; i < 9; i++) expected_xor = (expected_xor << 1) | (b[40 + i] - '0');
    
    vector<int> E; int xor_e = 0, p_idx = 1;
    for (int i = 1; i <= n; i++) {
        if (p_idx < (int)uneaten.size() && uneaten[p_idx] == i) p_idx++;
        else { E.push_back(i); xor_e ^= i; }
    }
    
    if (E.size() == 8) return expected_xor ^ xor_e;
    
    long long v = perm_val; vector<int> L_idx(8);
    for (int i = 1; i <= 8; i++) { L_idx[8 - i] = v % i; v /= i; }
    vector<int> avail(8); iota(avail.begin(), avail.end(), 0);
    vector<int> P(8);
    for (int i = 0; i < 8; i++) { P[i] = avail[L_idx[i]]; avail.erase(avail.begin() + L_idx[i]); }
    
    b_aux.clear(); b_aux.resize(9);
    vector<int> parent(17); iota(parent.begin(), parent.end(), 0);
    
    auto find_root = [&](int i) { while (i != parent[i]) i = parent[i]; return i; };
    for (int i = 0; i < 9; i++) { b_aux[i].is_leaf = true; b_aux[i].depth = 9; b_aux[i].leaf_rank = i; }
    
    int root_id = -1;
    for (int p : P) {
        int left_root = find_root(p), right_root = find_root(p + 1);
        int new_node = b_aux.size(); b_aux.push_back(AuxNode());
        b_aux[new_node].is_leaf = false; b_aux[new_node].left = left_root; b_aux[new_node].right = right_root;
        parent[left_root] = new_node; parent[right_root] = new_node; root_id = new_node;
    }
    
    b_aux[root_id].depth = root_depth; int w_idx = 0;
    auto assign_depths = [&](auto& self, int u) -> void {
        if (b_aux[u].is_leaf) return;
        int L = b_aux[u].left;
        if (!b_aux[L].is_leaf) { b_aux[L].depth = b_aux[u].depth + b_weights[w_idx++]; self(self, L); }
        int R = b_aux[u].right;
        if (!b_aux[R].is_leaf) { b_aux[R].depth = b_aux[u].depth + b_weights[w_idx++]; self(self, R); }
    };
    if (root_id != -1) assign_depths(assign_depths, root_id);
    
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