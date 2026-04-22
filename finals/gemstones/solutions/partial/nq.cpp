#include <bits/stdc++.h>
using namespace std;

int n, q, a[1000005];

int main() {
	ios::sync_with_stdio(0);
	cin.tie(0);
	cin >> n >> q;
	for (int i = 1; i <= n; i++) {
		cin >> a[i];
	}
	for (int i = 1, l, r; i <= q; i++) {
		cin >> l >> r;
		stack<int> st;
		for (int j = l; j <= r; j++) {
			if (!st.empty() && st.top() == a[j]) {
				st.pop();
			} else {
				st.push(a[j]);
			}
		}
		cout << st.size() << '\n';
	}
}
