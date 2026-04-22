#include <bits/stdc++.h>
#pragma GCC optimize("Ofast")
using namespace std;
using ll = long long;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    long long k;
    if (!(cin >> n >> k)) return 0;
    vector<long long> p(n);
    for (int i = 0; i < n; ++i) cin >> p[i];
    string d; cin >> d;

    long long ans = 0;

    // simulate days 0..k
    for (long long day = 0; day <= k; ++day) {
        unordered_map<long long, pair<int,int>> cnt;
        cnt.reserve(n * 2);

        for (int i = 0; i < n; ++i) {
            long long pos = (d[i] == 'L') ? (p[i] - day) : (p[i] + day);
            if (d[i] == 'L') cnt[pos].second++;
            else cnt[pos].first++;
        }

        for (auto &it : cnt) {
            long long r = it.second.first;
            long long l = it.second.second;
            // R-L pairs meeting this day
            ans += r * l;
            // at day 0, also count same-direction pairs at same starting pos
            if (day == 0) {
                ans += r * (r - 1) / 2;
                ans += l * (l - 1) / 2;
            }
        }
    }

    cout << ans << '\n';
    return 0;
}