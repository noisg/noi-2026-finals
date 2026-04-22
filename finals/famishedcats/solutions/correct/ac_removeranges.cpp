#include <bits/stdc++.h>
using namespace std;

using ll = long long;

/*
    Lazy segment tree:
    - range add
    - global minimum query
*/
struct SegTree {
    int n;
    vector<ll> mn, lz;

    SegTree() : n(0) {}
    SegTree(int n_) { init(n_); }

    void init(int n_) {
        n = n_;
        mn.assign(4 * n + 5, 0);
        lz.assign(4 * n + 5, 0);
    }

    void build(int p, int l, int r, const vector<ll>& a) {
        if (l == r) {
            mn[p] = a[l];
            return;
        }
        int m = (l + r) >> 1;
        build(p << 1, l, m, a);
        build(p << 1 | 1, m + 1, r, a);
        mn[p] = min(mn[p << 1], mn[p << 1 | 1]);
    }

    inline void apply(int p, ll v) {
        mn[p] += v;
        lz[p] += v;
    }

    inline void push(int p) {
        if (lz[p] != 0) {
            apply(p << 1, lz[p]);
            apply(p << 1 | 1, lz[p]);
            lz[p] = 0;
        }
    }

    void add(int p, int l, int r, int ql, int qr, ll v) {
        if (ql > r || qr < l) return;
        if (ql <= l && r <= qr) {
            apply(p, v);
            return;
        }
        push(p);
        int m = (l + r) >> 1;
        add(p << 1, l, m, ql, qr, v);
        add(p << 1 | 1, m + 1, r, ql, qr, v);
        mn[p] = min(mn[p << 1], mn[p << 1 | 1]);
    }

    inline ll getMin() const {
        return mn[1];
    }
};

struct Cand {
    ll delta;
    int i; // 1-indexed left endpoint
};

int main() {
    int n;
    ll x;
    cin>>n>>x;

    vector<ll> d(n + 1), f(n + 1), g(n + 1);
    for (int i = 1; i <= n; ++i) cin>>d[i];//In.readInt(d[i]);
    for (int i = 1; i <= n; ++i) cin>>f[i];//In.readInt(f[i]);

    int m = n >> 1;
    vector<Cand> cand;
    g = f;

    // Step 1: greedily do every beneficial swap.
    for (int i = 1; i <= m; ++i) {
        int j = n - i + 1;
        if (g[i] < g[j]) {
            ll delta = g[j] - g[i];
            swap(g[i], g[j]);
            cand.push_back({delta, i});
        }
    }

    // Simulate with all beneficial swaps to get the largest reachable D.
    int D = 0;
    ll fuel = x;
    vector<ll> margin(max(2, n), 0); // margin[k] for k = 1..D-1
    for (int i = 1; i <= n; ++i) {
        if (fuel < d[i]) break;
        fuel -= d[i];
        fuel += g[i];
        D = i;
        if (i < n) margin[i] = fuel - d[i + 1];
    }

    // If only house 1 is reachable, 0 swaps always suffices.
    if (D <= 1) {
        cout<<D<<' '<<0;
        return 0;
    }

    // Build segment tree over prefix margins 1..D-1.
    // Reachability to D is equivalent to all these margins staying >= 0.
    vector<ll> base(D, 0); // 1..D-1 used
    for (int k = 1; k <= D - 1; ++k) base[k] = margin[k];

    SegTree st(D - 1);
    st.build(1, 1, D - 1, base);

    // Step 2: try to undo swaps in ascending delta order.
    // Tiebreak by larger i first: shorter interval first, which often "looks stronger".
    sort(cand.begin(), cand.end(), [&](const Cand& a, const Cand& b) {
        if (a.delta != b.delta) return a.delta < b.delta;
        return a.i > b.i;
    });

    int S = (int)cand.size();

    for (auto &c : cand) {
        int i = c.i;
        ll delta = c.delta;

        // Undoing pair i removes delta from all prefix constraints in [i, n-i].
        int L = i;
        int R = min(n - i, D - 1);

        // If this swap never matters before D, always undo it.
        if (L > R) {
            --S;
            continue;
        }

        st.add(1, 1, D - 1, L, R, -delta);

        // Keep the undo only if D is still reachable.
        if (st.getMin() >= 0) {
            --S;
        } else {
            st.add(1, 1, D - 1, L, R, +delta);
        }
    }

    cout<<D<<' '<<S;
    return 0;
}