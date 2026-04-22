#include <bits/stdc++.h>
using namespace std;

#define mp make_pair
#define mt make_tuple

using ii = pair<int, int>;
using iii = tuple<int, int, int>;

const int B = 600, PUSH_BACK = 0, PUSH_FRONT = 1, POP_BACK = 2, POP_FRONT = 3;
int n, q, a[1000005], ans[500005];
iii qu[500005];
deque<int> d;
stack<ii> st;

void init() {
	//~ cout << "INIT\n";
	d.clear();
	while (!st.empty()) {
		st.pop();
	}
}

int get_answer() {
	//~ cout << "GA\n";
	return d.size();
}

void snapshot() {
	//~ cout << "SNAPSHOT\n";
	while (!st.empty()) {
		st.pop();
	}
}

void rollback() {
	//~ cout << "ROLLBACK\n";
	while (!st.empty()) {
		auto [inst, x] = st.top();
		st.pop();
		if (inst == PUSH_BACK) {
			d.push_back(x);
		} else if (inst == PUSH_FRONT) {
			d.push_front(x);
		} else if (inst == POP_BACK) {
			d.pop_back();
		} else {
			d.pop_front();
		}
	}
}

void insert_left(int x) {
	//~ cout << "INS LEFT " << x << '\n';
	if (!d.empty() && d.back() == a[x]) {
		st.emplace(PUSH_BACK, a[x]);
		d.pop_back();
	} else {
		st.emplace(POP_BACK, -1);
		d.push_back(a[x]);
	}
}

void insert_right(int x) {
	//~ cout << "INS RIGHT " << x << '\n';
	if (!d.empty() && d.front() == a[x]) {
		st.emplace(PUSH_FRONT, a[x]);
		d.pop_front();
	} else {
		st.emplace(POP_FRONT, -1);
		d.push_front(a[x]);
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
