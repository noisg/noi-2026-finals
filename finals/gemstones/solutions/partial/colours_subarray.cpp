#include <bits/stdc++.h>
using namespace std;
 
int n, q, a[1000005], lef[1000005], rig[1000005], pf[1000005];
 
int main() {
	ios::sync_with_stdio(0);
	cin.tie(0);
	cin >> n >> q;
	for (int i = 1; i <= n; i++) {
		cin >> a[i];
		if (a[i] != a[i - 1]) {
			lef[i] = i;
		} else {
			lef[i] = lef[i - 1];
		}
	}
	for (int i = n; i >= 1; i--) {
		if (a[i] != a[i + 1]) {
			rig[i] = i;
		} else {
			rig[i] = rig[i + 1];
		}
	}
	for (int i = 1; i <= n; i++) {
		if (a[i] != a[i + 1]) {
			pf[i] = pf[i - 1] + (i - lef[i] + 1) % 2;
		} else {
			pf[i] = pf[i - 1];
		}
	}
	for (int i = 1, l, r; i <= q; i++) {
		cin >> l >> r;
		if (rig[l] >= r) {
			cout << (r - l + 1) % 2 << '\n';
		} else {
			int x = rig[l] + 1, y = lef[r] - 1;
			cout << (x - l) % 2 + (r - y) % 2 + pf[y] - pf[x - 1] << '\n';
		}
	}
}
