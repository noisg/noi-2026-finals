#include <bits/stdc++.h>
using namespace std;
using namespace std;
typedef long long ll;
typedef pair<ll, ll> pll;

#pragma GCC optimize("O3,unroll-loops")
#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")

int simulate(long long x, vector<long long>& d, vector<long long>& f, int n) {
    long long fuel = x; int best = 0;
    for (int i = 1; i <= n; i++) {
        if (fuel < d[i]) break;
        fuel -= d[i]; fuel += f[i]; best = i;
    }
    return best; //no swaps
}


pll bruteForce(ll N, long long x, vector<long long>& d, vector<long long>& f) {
    if (N >= 60) {N = 60;} //so it TLEs
	pll res = make_pair(-1, -1);
	for (ll i = 0; i < (1 << N/2); i++){
		ll noBits = 0;
		for (ll bit = 1, l = 1, r = N; bit <= i; bit <<= 1, l++, r--){
			if ( (i & bit) == 0) continue;
			noBits++; swap( f[l], f[r] );
		}
		
		ll fuelStop = simulate( x, d, f, N );
		
		if ( fuelStop > res.first ) 	res = make_pair( fuelStop, noBits );
		else if (fuelStop == res.first) res.second = min( noBits, res.second );
		
		for (ll bit = 1, l = 1, r = N; bit <= i; bit <<= 1, l++, r--){
			if ( (i & bit) == 0) continue;
			swap( f[l], f[r] );
		}
	}
	return res;
}


// ============================================================
// STRESS TEST
// ============================================================
int main() {
    long long N, X; cin >> N >> X;
    vector<long long> D, F; D.resize(N + 1); F.resize(N + 1);
    for (int i = 1; i <= N; i++) cin >> D[i];
    for (int i = 1; i <= N; i++) cin >> F[i];
    pair<int,int> res = bruteForce(
		N, X, D, F
    );
    cout << res.first <<  ' ' << res.second << '\n';
    return 0;
}
