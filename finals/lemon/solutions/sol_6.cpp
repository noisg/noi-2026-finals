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
        // Exclude mask size 8 to safely unlock 503 valid states.
        if (__builtin_popcount(i) != 8) g_valid_masks.push_back(i);
    }
}

static int g_N;
static int g_step;
static int g_lemon_id;
static int g_lemon_pos;
static bool g_lemon_seen;
static vector<int> g_last_9;

struct TrieNode { int left = -1, right = -1; };
struct AuxNode { int left = -1, right = -1, depth = 0, leaf_rank = -1; bool is_leaf = false; };

static vector<TrieNode> a_trie;
static vector<AuxNode> a_aux;
static string a_structure;
static vector<int> a_weights;

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

static void generate_euler(int u) {
    if (a_aux[u].is_leaf) return;
    a_structure += "1";
    if (!a_aux[a_aux[u].left].is_leaf) {
        a_weights.push_back(a_aux[a_aux[u].left].depth - a_aux[u].depth);
        generate_euler(a_aux[u].left);
    }
    a_structure += "0";
    a_structure += "1";
    if (!a_aux[a_aux[u].right].is_leaf) {
        a_weights.push_back(a_aux[a_aux[u].right].depth - a_aux[u].depth);
        generate_euler(a_aux[u].right);
    }
    a_structure += "0";
}

string init(int subtask, int n, vector<int> p) {
    build_masks();
    g_N = n; g_step = 0; g_lemon_id = -1; g_lemon_pos = -1; g_lemon_seen = false; g_last_9.clear();
    a_trie.assign(1, TrieNode()); a_aux.clear(); a_structure = ""; a_weights.clear();

    int xor_sum = 0;
    for (int i = n - 8; i <= n; i++) {
        g_last_9.push_back(p[i]); xor_sum ^= p[i];
    }
    vector<int> sorted_9 = g_last_9;
    sort(sorted_9.begin(), sorted_9.end());

    for (int val : sorted_9) {
        int u = 0;
        for (int b = 8; b >= 0; b--) {
            int bit = (val >> b) & 1;
            if (bit == 0) {
                if (a_trie[u].left == -1) { a_trie[u].left = a_trie.size(); a_trie.push_back(TrieNode()); }
                u = a_trie[u].left;
            } else {
                if (a_trie[u].right == -1) { a_trie[u].right = a_trie.size(); a_trie.push_back(TrieNode()); }
                u = a_trie[u].right;
            }
        }
    }
    
    int root_id = compress_trie(0, 0);
    int root_depth = a_aux[root_id].depth;
    generate_euler(root_id);

    string B = a_structure; 
    for (int i = 2; i >= 0; i--) B += ((root_depth >> i) & 1) ? '1' : '0';
    for (int i = 0; i < 7; i++) {
        int w = a_weights[i] - 1; 
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
        vector<int> sorted_9 = g_last_9;
        sort(sorted_9.begin(), sorted_9.end());
        int rank = find(sorted_9.begin(), sorted_9.end(), id) - sorted_9.begin();
        return ((g_valid_masks[g_lemon_id - 1] >> rank) & 1); 
    }
    return !is_lemon; 
}

static vector<AuxNode> b_aux;
static string b_struct;
static vector<int> b_weights;
static int s_idx, w_idx, leaf_counter;

static void rebuild_aux(int u) {
    if (b_struct[s_idx] == '1') {
        if (b_struct[s_idx + 1] == '0') {
            int L = b_aux.size(); b_aux.push_back(AuxNode());
            b_aux[L].is_leaf = true; b_aux[L].depth = 9; b_aux[L].leaf_rank = leaf_counter++; 
            b_aux[u].left = L; s_idx += 2;
        } else {
            int L = b_aux.size(); b_aux.push_back(AuxNode());
            b_aux[L].is_leaf = false; b_aux[L].depth = b_aux[u].depth + b_weights[w_idx++];
            b_aux[u].left = L; s_idx++; rebuild_aux(L); s_idx++; 
        }
    }
    if (b_struct[s_idx] == '1') {
        if (b_struct[s_idx + 1] == '0') {
            int R = b_aux.size(); b_aux.push_back(AuxNode());
            b_aux[R].is_leaf = true; b_aux[R].depth = 9; b_aux[R].leaf_rank = leaf_counter++; 
            b_aux[u].right = R; s_idx += 2;
        } else {
            int R = b_aux.size(); b_aux.push_back(AuxNode());
            b_aux[R].is_leaf = false; b_aux[R].depth = b_aux[u].depth + b_weights[w_idx++];
            b_aux[u].right = R; s_idx++; rebuild_aux(R); s_idx++; 
        }
    }
}

int answer(int subtask, int n, string b, vector<int> uneaten) {
    build_masks();
    b_struct = b.substr(0, 32);
    
    int root_depth = 0;
    for (int i = 0; i < 3; i++) root_depth = (root_depth << 1) | (b[32 + i] - '0');
    
    b_weights.clear();
    for (int w = 0; w < 7; w++) {
        int weight = 0;
        for (int i = 0; i < 3; i++) weight = (weight << 1) | (b[35 + w * 3 + i] - '0');
        b_weights.push_back(weight + 1);
    }
    
    int expected_xor = 0;
    for (int i = 0; i < 9; i++) expected_xor = (expected_xor << 1) | (b[56 + i] - '0');
    
    vector<int> E; int xor_e = 0, p_idx = 1;
    for (int i = 1; i <= n; i++) {
        if (p_idx < (int)uneaten.size() && uneaten[p_idx] == i) p_idx++;
        else { E.push_back(i); xor_e ^= i; }
    }
    
    if (E.size() == 8) return expected_xor ^ xor_e;
    
    b_aux.clear(); int root_id = b_aux.size(); b_aux.push_back(AuxNode());
    b_aux[root_id].depth = root_depth;
    s_idx = 0; w_idx = 0; leaf_counter = 0;
    rebuild_aux(root_id);
    
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
    
    for (int i = 0; i < (int)g_valid_masks.size(); i++) {
        if (g_valid_masks[i] == missing_mask) return i + 1; 
    }
    return 1;
}