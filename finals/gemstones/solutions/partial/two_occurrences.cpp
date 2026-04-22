#include <bits/stdc++.h>
using namespace std;

using ii = pair<int, int>;

#define eb emplace_back

int n, q, a[1000005], rig[1000005], ft[1000005], ans[500005];
vector<ii> qu[1000005];
stack<int> st;

int ls(int x) {
	return x & -x;
}

void ft_upd(int p, int v) {
	for (p = n - p + 1; p <= n; p += ls(p)) {
		ft[p] += v;
	}
}

int ft_qry(int p) {
	int r = 0;
	for (p = n - p + 1; p; p -= ls(p)) {
		r += ft[p];
	}
	return r;
}

int main() {
	ios::sync_with_stdio(0);
	cin.tie(0);
	cin >> n >> q;
	for (int i = 1; i <= n; i++) {
		cin >> a[i];
		if (!st.empty() && a[st.top()] == a[i]) {
			rig[i] = st.top();
			st.pop();
		} else {
			st.push(i);
		}
	}
	for (int i = 1, l, r; i <= q; i++) {
		cin >> l >> r;
		qu[r].eb(l, i);
	}
	for (int i = 1; i <= n; i++) {
		if (rig[i] > 0) {
			ft_upd(rig[i], 2);
		}
		for (auto [l, idx] : qu[i]) {
			ans[idx] = (i - l + 1) - ft_qry(l);
		}
	}
	for (int i = 1; i <= q; i++) {
		cout << ans[i] << '\n';
	}
}
