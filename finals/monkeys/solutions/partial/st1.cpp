#include <bits/stdc++.h>
//#include "includeall.h"
//#include <ext/pb_ds/assoc_container.hpp>
//#include <ext/pb_ds/tree_policy.hpp>
#define endl '\n'
#define f first
#define s second
#define pb push_back
#define mp make_pair
#define lb lower_bound
#define ub upper_bound
#define input(x) scanf("%lld", &x);
#define input2(x, y) scanf("%lld%lld", &x, &y);
#define input3(x, y, z) scanf("%lld%lld%lld", &x, &y, &z);
#define input4(x, y, z, a) scanf("%lld%lld%lld%lld", &x, &y, &z, &a);
#define print(x, y) printf("%lld%c", x, y);
#define show(x) cerr << #x << " is " << x << endl;
#define show2(x,y) cerr << #x << " is " << x << " " << #y << " is " << y << endl;
#define show3(x,y,z) cerr << #x << " is " << x << " " << #y << " is " << y << " " << #z << " is " << z << endl;
#define all(x) x.begin(), x.end()
#define discretize(x) sort(x.begin(), x.end()); x.erase(unique(x.begin(), x.end()), x.end());
#define FOR(i, x, n) for (ll i =x; i<=n; ++i) 
#define RFOR(i, x, n) for (ll i =x; i>=n; --i) 
using namespace std;
mt19937_64 rnd(chrono::steady_clock::now().time_since_epoch().count());
//using namespace __gnu_pbds;
//#define ordered_set tree<int, null_type, less<int>, rb_tree_tag, tree_order_statistics_node_update>
//#define ordered_multiset tree<int, null_type, less_equal<int>, rb_tree_tag, tree_order_statistics_node_update>
typedef long long ll;
typedef long double ld;
typedef pair<ld, ll> pd;
typedef pair<ll, string> psl;
typedef pair<ll, ll> pi;
typedef pair<ll, pi> pii;
typedef pair<pi, pi> piii;


int main()
{
	ll n, k;
	cin >> n >> k;
	vector<ll> p(n);
	unordered_map<ll, ll> hh;
	for (ll i=0; i<n; ++i)  
	{
		cin >> p[i];
		hh[p[i]]++;
	}
	string d;
	cin >> d;
	ll ans = 0;
	for (pi u: hh) ans += u.s * (u.s-1)/2;
	cout << ans << endl;
	return 0;
}


