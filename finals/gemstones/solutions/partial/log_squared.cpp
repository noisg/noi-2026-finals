#include <iostream>
#include <vector>
#include <map>
#include <array>

using namespace std;

struct Query {
    int l, r, id;
};

int n, q;
vector<int> c;
vector<int> ans;
vector<int> pos_node_left;
vector<int> pos_node_right;

struct Node {
    int parent;
    int parent_char;
    map<int, int> children;
    int depth;
};

void solve(int s, int e, const vector<Query>& queries) {
    if (queries.empty()) return;
    
    // Base case: a single gemstone cannot be reduced
    if (s == e) {
        for (const auto& qry : queries) {
            ans[qry.id] = 1;
        }
        return;
    }
    
    int m = s + (e - s) / 2;
    vector<Query> left_q, right_q, cross_q;
    
    for (const auto& qry : queries) {
        if (qry.r <= m) left_q.push_back(qry);
        else if (qry.l > m) right_q.push_back(qry);
        else cross_q.push_back(qry);
    }
    
    // Process queries that cross the midpoint
    if (!cross_q.empty()) {
        int max_nodes = e - s + 2;
        vector<Node> nodes(max_nodes);
        int node_cnt = 0;
        
        nodes[0].depth = 0;
        nodes[0].parent = 0;
        nodes[0].parent_char = -1;
        
        // Build Trie for the left half (suffix)
        int curr = 0;
        for (int k = m; k >= s; --k) {
            int char_k = c[k];
            // If the character matches the edge to the parent, pop the stack
            if (curr != 0 && nodes[curr].parent_char == char_k) {
                curr = nodes[curr].parent;
            } else {
                // Otherwise, push to the stack (traverse to or create a child)
                auto it = nodes[curr].children.find(char_k);
                if (it == nodes[curr].children.end()) {
                    int nxt = ++node_cnt;
                    nodes[nxt].parent = curr;
                    nodes[nxt].parent_char = char_k;
                    nodes[nxt].depth = nodes[curr].depth + 1;
                    nodes[curr].children[char_k] = nxt;
                    curr = nxt;
                } else {
                    curr = it->second;
                }
            }
            pos_node_left[k] = curr;
        }
        
        // Build Trie for the right half (prefix)
        curr = 0;
        for (int k = m + 1; k <= e; ++k) {
            int char_k = c[k];
            if (curr != 0 && nodes[curr].parent_char == char_k) {
                curr = nodes[curr].parent;
            } else {
                auto it = nodes[curr].children.find(char_k);
                if (it == nodes[curr].children.end()) {
                    int nxt = ++node_cnt;
                    nodes[nxt].parent = curr;
                    nodes[nxt].parent_char = char_k;
                    nodes[nxt].depth = nodes[curr].depth + 1;
                    nodes[curr].children[char_k] = nxt;
                    curr = nxt;
                } else {
                    curr = it->second;
                }
            }
            pos_node_right[k] = curr;
        }
        
        // Build binary lifting table iteratively
        vector<array<int, 20>> up(node_cnt + 1);
        for (int i = 0; i < 20; ++i) up[0][i] = 0;
        
        for (int i = 1; i <= node_cnt; ++i) {
            up[i][0] = nodes[i].parent;
            for (int j = 1; j < 20; ++j) {
                up[i][j] = up[up[i][j-1]][j-1];
            }
        }
        
        // LCA function
        auto get_lca = [&](int u, int v) {
            if (nodes[u].depth < nodes[v].depth) swap(u, v);
            int diff = nodes[u].depth - nodes[v].depth;
            for (int j = 0; j < 20; ++j) {
                if ((diff >> j) & 1) {
                    u = up[u][j];
                }
            }
            if (u == v) return u;
            for (int j = 19; j >= 0; --j) {
                if (up[u][j] != up[v][j]) {
                    u = up[u][j];
                    v = up[v][j];
                }
            }
            return up[u][0];
        };
        
        // Answer cross queries
        for (const auto& qry : cross_q) {
            int u = pos_node_left[qry.l];
            int v = pos_node_right[qry.r];
            int lca = get_lca(u, v);
            ans[qry.id] = nodes[u].depth + nodes[v].depth - 2 * nodes[lca].depth;
        }
    }
    
    // Recurse for remaining queries
    solve(s, m, left_q);
    solve(m + 1, e, right_q);
}

int main() {
    // Optimize standard I/O operations for competitive programming
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    if (!(cin >> n >> q)) return 0;
    
    c.resize(n + 1);
    for (int i = 1; i <= n; ++i) {
        cin >> c[i];
    }
    
    vector<Query> queries(q);
    for (int i = 0; i < q; ++i) {
        cin >> queries[i].l >> queries[i].r;
        queries[i].id = i;
    }
    
    ans.resize(q);
    pos_node_left.resize(n + 1);
    pos_node_right.resize(n + 1);
    
    solve(1, n, queries);
    
    for (int i = 0; i < q; ++i) {
        cout << ans[i] << "\n";
    }
    
    return 0;
}
