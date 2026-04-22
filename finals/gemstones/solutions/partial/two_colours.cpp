#include <bits/stdc++.h>
using namespace std;

#define mp make_pair

int n, q, a[1000005];

pair<int, bool> merge(pair<int, bool> lhs, pair<int, bool> rhs) {
	if ((lhs.second ^ (lhs.first % 2 == 0)) == rhs.second) {
		int len_del = min(lhs.first, rhs.first);
		return mp(max(lhs.first, rhs.first) - len_del, lhs.second ^ (len_del == lhs.first));
	}
	return mp(lhs.first + rhs.first, lhs.second);
}

struct node {
	node *left, *right;
	int S, E;
	pair<int, bool> val;
	node(int _s, int _e) : S(_s), E(_e) {
		if (S == E) {
			val = mp(1, a[S] == 1);
			return;
		}
		int M = (S + E) / 2;
		left = new node(S, M);
		right = new node(M + 1, E);
		val = merge(left->val, right->val);
	}
	pair<int, bool> qry(int l, int r) {
		if (l > E || r < S) {
			return mp(0, 0);
		}
		if (l <= S && E <= r) {
			return val;
		}
		return merge(left->qry(l, r), right->qry(l, r));
	}
} *root;

int main() {
	ios::sync_with_stdio(0);
	cin.tie(0);
	cin >> n >> q;
	for (int i = 1; i <= n; i++) {
		cin >> a[i];
	}
	root = new node(1, n);
	for (int i = 1, l, r; i <= q; i++) {
		cin >> l >> r;
		cout << root->qry(l, r).first << '\n';
	}
}
