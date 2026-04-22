#include <bits/stdc++.h>
using namespace std;

int n, q, nodes, a[1000005], pos[1000005], dep[1000005], anc[25][1000005];
stack<int> st;
map<int, int> children[1000005];

int lca(int u, int v) {
	if (dep[u] > dep[v]) {
		swap(u, v);
	}
	for (int k = 20; k >= 0; k--) {
		if (dep[v] - (1 << k) >= dep[u]) {
			v = anc[k][v];
		}
	}
	if (u == v) {
		return u;
	}
	for (int k = 20; k >= 0; k--) {
		if (anc[k][u] != anc[k][v]) {
			u = anc[k][u];
			v = anc[k][v];
		}
	}
	return anc[0][u];
}

int dist(int u, int v) {
	return dep[u] + dep[v] - 2 * dep[lca(u, v)];
}

int main() {
	ios::sync_with_stdio(0);
	cin.tie(0);
	cin >> n >> q;
	for (int i = 1; i <= n; i++) {
		cin >> a[i];
		if (!st.empty() && st.top() == a[i]) {
			st.pop();
			pos[i] = anc[0][pos[i - 1]];
		} else {
			st.push(a[i]);
			auto it = children[pos[i - 1]].find(a[i]);
			if (it == children[pos[i - 1]].end()) {
				pos[i] = ++nodes;
				children[pos[i - 1]][a[i]] = pos[i];
				dep[pos[i]] = dep[pos[i - 1]] + 1;
				anc[0][pos[i]] = pos[i - 1];
			} else {
				pos[i] = it->second;
			}
		}
	}
	for (int k = 1; k <= 20; k++) {
		for (int i = 1; i <= n; i++) {
			anc[k][i] = anc[k - 1][anc[k - 1][i]];
		}
	}
	for (int i = 1, l, r; i <= q; i++) {
		cin >> l >> r;
		cout << dist(pos[l - 1], pos[r]) << '\n';
	}
}
