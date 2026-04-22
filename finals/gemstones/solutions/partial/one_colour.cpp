#include <bits/stdc++.h>
using namespace std;

int n, q;

int main() {
	ios::sync_with_stdio(0);
	cin.tie(0);
	cin >> n >> q;
	for (int i = 1, a; i <= n; i++) {
		cin >> a;
	}
	for (int i = 1, l, r; i <= q; i++) {
		cin >> l >> r;
		cout << (r - l + 1) % 2 << '\n';
	}
}
