#include <bits/stdc++.h>
using namespace std;

#define mp make_pair
#define mt make_tuple

using ii = pair<int, int>;
using iii = tuple<int, int, int>;

const int B = 600, PUSH_BACK = 0, PUSH_FRONT = 1, POP_BACK = 2, POP_FRONT = 3;
int n, q, l_ptr, r_ptr, a[1000005], d[2000005], ans[500005];
iii qu[500005];
stack<ii> st;

void init() {
	l_ptr = n + 1;
	r_ptr = n;
	while (!st.empty()) {
		st.pop();
	}
}

int get_answer() {
	return r_ptr - l_ptr + 1;
}

void snapshot() {
	while (!st.empty()) {
		st.pop();
	}
}

void rollback() {
	while (!st.empty()) {
		auto [inst, x] = st.top();
		st.pop();
		if (inst == PUSH_BACK) {
			d[++r_ptr] = x;
		} else if (inst == PUSH_FRONT) {
			d[--l_ptr] = x;
		} else if (inst == POP_BACK) {
			r_ptr--;
		} else {
			l_ptr++;
		}
	}
}

void insert_left(int x) {
	if (l_ptr <= r_ptr && d[r_ptr] == a[x]) {
		st.emplace(PUSH_BACK, a[x]);
		r_ptr--;
	} else {
		st.emplace(POP_BACK, -1);
		d[++r_ptr] = a[x];
	}
}

void insert_right(int x) {
	if (l_ptr <= r_ptr && d[l_ptr] == a[x]) {
		st.emplace(PUSH_FRONT, a[x]);
		l_ptr++;
	} else {
		st.emplace(POP_FRONT, -1);
		d[--l_ptr] = a[x];
	}
}

int main() {
	ios::sync_with_stdio(0);
	cin.tie(0);
	cin >> n >> q;
	for (int i = 1; i <= n; i++) {
		cin >> a[i];
	}
	init();
	snapshot();
	for (int i = 1, l, r; i <= q; i++) {
		cin >> l >> r;
		qu[i] = mt(l, r, i);
	}
	sort(qu + 1, qu + 1 + q, [](const auto &lhs, const auto &rhs) {
		return mp(get<0>(lhs) / B, get<1>(lhs)) < mp(get<0>(rhs) / B, get<1>(rhs));
	});
	for (int i = 1; i <= q; i++) {
		auto [l, r, idx] = qu[i];
		if (r - l + 1 <= B + 1) {
			for (int j = l; j <= r; j++) {
				insert_right(j);
			}
			ans[idx] = get_answer();
			rollback();
		}
	}
	int last_bucket = -1;
	for (int i = 1, l_b = -1, r_b = -1; i <= q; i++) {
		auto [l, r, idx] = qu[i];
		if (r - l + 1 <= B + 1) {
			continue;
		}
		int bucket = l / B;
		if (bucket != last_bucket) {
			init();
			l_b = (bucket + 1) * B;
			r_b = r;
			for (int j = l_b; j <= r_b; j++) {
				insert_right(j);
			}
			last_bucket = bucket;
		}
		while (r_b < r) {
			insert_right(++r_b);
		}
		snapshot();
		for (int j = l_b - 1; j >= l; j--) {
			insert_left(j);
		}
		ans[idx] = get_answer();
		rollback();
	}
	for (int i = 1; i <= q; i++) {
		cout << ans[i] << '\n';
	}
}
