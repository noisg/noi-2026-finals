#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    long long x;
    cin >> n >> x;

    vector<long long> d(n + 2), f(n + 2);
    for (int i = 1; i <= n; i++) cin >> d[i];
    for (int i = 1; i <= n; i++) cin >> f[i];

    // Step 1: Find F by simulating with all beneficial swaps
    vector<long long> fs(f);
    for (int i = 1; i <= n / 2; i++) {
        int j = n - i + 1;
        if (fs[j] > fs[i]) swap(fs[i], fs[j]);
    }

    long long fuel = x;
    int F = 0;
    for (int i = 1; i <= n; i++) {
        if (fuel < d[i]) break;
        fuel -= d[i];
        fuel += fs[i];
        F = i;
    }

    if (F == 0) {
        cout << "0 0\n";
        return 0;
    }

    // Step 2: deficit[k] = min starting fuel to reach house k (before collecting)
    vector<long long> deficit(n + 2, 0);
    long long cumD = 0, cumF = 0;
    for (int k = 1; k <= F; k++) {
        cumD += d[k];
        deficit[k] = cumD - cumF;
        cumF += f[k];
    }

    vector<long long> need(n + 2, 0);
    for (int k = 1; k <= F; k++)
        need[k] = max(0LL, deficit[k] - x);

    // Step 3: O(n^2) DP
    // Process pairs outside-in: i = 1, 2, ..., n/2
    // At step i, positions a=i+1 and b=n-i+1 are "locked":
    //   their coverage = sum of swaps chosen from pairs 1..i ONLY
    //
    // dp_need[j] = min "max need unsatisfied" over all locked positions, using j swaps
    // dp_cov[j]  = max coverage when attaining dp_need[j] (tiebreak: prefer higher coverage)

    int max_j = n / 2;
    vector<long long> pair_delta(max_j + 1, 0);
    for (int i = 1; i <= n / 2; i++) {
        int p = n - i + 1;
        if (f[p] > f[i]) pair_delta[i] = f[p] - f[i];
    }

    const long long INF  =  4e18;
    const long long NINF = -4e18;

    vector<long long> dp_need(max_j + 1, INF);
    vector<long long> dp_cov (max_j + 1, NINF);
    dp_need[0] = 0;
    dp_cov[0]  = 0;

    auto better = [](long long mn1, long long cv1, long long mn2, long long cv2) {
        return mn1 < mn2 || (mn1 == mn2 && cv1 > cv2);
    };

    vector<long long> nn(max_j + 1), nc(max_j + 1);

    for (int i = 1; i <= n / 2; i++) {
        int a = i + 1, b = n - i + 1;
        long long new_need = max(a <= F ? need[a] : 0LL,
                                 b <= F ? need[b] : 0LL);

        fill(nn.begin(), nn.end(), INF);
        fill(nc.begin(), nc.end(), NINF);

        for (int j = 0; j <= max_j; j++) {
            if (dp_cov[j] == NINF) continue;
            long long mn = dp_need[j], cov = dp_cov[j];

            // Don't swap pair i
            long long mn1 = max(mn, max(0LL, new_need - cov));
            if (better(mn1, cov, nn[j], nc[j])) {
                nn[j] = mn1; nc[j] = cov;
            }

            // Swap pair i (if valid)
            if (pair_delta[i] > 0 && j + 1 <= max_j) {
                long long nco = cov + pair_delta[i];
                long long mn2 = max(mn, max(0LL, new_need - nco));
                if (better(mn2, nco, nn[j+1], nc[j+1])) {
                    nn[j+1] = mn2; nc[j+1] = nco;
                }
            }
        }

        swap(dp_need, nn);
        swap(dp_cov,  nc);
    }

    // Find min j s.t. max need = 0
    for (int j = 0; j <= max_j; j++) {
        if (dp_cov[j] != NINF && dp_need[j] == 0) {
            cout << F << " " << j << "\n";
            return 0;
        }
    }

    cout << F << " -1\n"; // shouldn't reach here
    return 0;
}