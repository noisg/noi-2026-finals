#include <bits/stdc++.h> 
#include <ext/pb_ds/assoc_container.hpp> 
#include <ext/pb_ds/tree_policy.hpp> 
using namespace std; 
   
using str = string; 
using ll = long long; 
using ld = long double; 
   
#define int long long 
#define show(x) cerr << (#x) << " = " << (x) << '\n'; 
#define output_vector(v) for(auto &x : v){cout << x << " ";}cout << '\n'; 
#define debug_vector(v) for(auto &x : v){cerr << x << " ";}cerr << '\n'; 
#define output_pairvector(v) for(auto &x : v){cout << x.first << " " << x.second << '\n';} 
#define vt vector 
#define pii pair<int,int> 
#define piii pair<pair<int,int>, pair<int,int>> 
#define discretize(x) sort(x.begin(), x.end()); x.erase(unique(x.begin(), x.end()), x.end()); 
#define all(x) x.begin(), x.end() 
#define rall(x) x.rbegin(), x.rend() 
#define sz(x) ll(x.size()) 
#define fir first 
#define sec second 
   
/* 
   
inline int readint() { 
    int x=0; char ch=getchar_unlocked(); bool s=1; 
    while(ch<'0'||ch>'9'){if(ch=='-')s=0;ch=getchar_unlocked();} 
    while(ch>='0'&&ch<='9'){x=(x<<3)+(x<<1)+ch-'0';ch=getchar_unlocked();} 
    return s?x:-x; 
} 
   
*/
   
//#define large 
   
//#ifdef large 
    //#define cin fin 
    //ifstream fin("inpt.txt"); 
//#else 
//#endif 
   
const int INF = LLONG_MAX/20; 
const int mod = 676767676;
void solve() { 
    int n, k;
    cin >> n >> k;
	vector<int> p(n);
	for(int i=0; i<n; ++i)
		cin >> p[i];
	str s; cin >> s;
	map<int, int> freq;
	vector<pii> even, odd;
	for(int i=0; i<n; ++i) {
		auto temp = make_pair(p[i], (s[i] == 'L' ? 0 : 1));
		freq[p[i]]++;
		if(p[i] % 2 == 0)
			even.push_back(temp);
		else
			odd.push_back(temp);
	}
	int ans = 0;
	for(auto &[key, val] : freq) {
		ans += val * (val-1) / 2;
	}
	//show(ans);
	sort(all(even));
	sort(all(odd));
	
	// Process even
	int ptr = 1;
	int cnt = 0;
	for(int i=0; i<sz(even); ++i) {
		if(even[i].sec == 1) {
			while(ptr < sz(even) && even[ptr].fir <= even[i].fir + 2*k) {
				cnt += (even[ptr].sec == 0 && ptr > i);
				ptr++;
			}
			//if(i == 2) show(cnt);
			ans += cnt;
		} else {
			if(cnt > 0) --cnt;
		}
		//show(i); show (ans);
	}
	//show(ans);
	// Process odd
	ptr = 1, cnt = 0;
	for(int i=0; i<sz(odd); ++i) {
		if(odd[i].sec == 1) {
			while(ptr < sz(odd) && odd[ptr].fir <= odd[i].fir + 2*k) {
				cnt += (odd[ptr].sec == 0 && ptr > i);
				ptr++;
			}
			ans += cnt;
		} else {
			if(cnt > 0) --cnt;
		}
	}
	
	
	cout << ans << '\n';

} 
   
   
signed main() { 
    ios::sync_with_stdio(0); 
    cin.tie(0); 
    //freopen("output.txt", "w", stdout); 
    //freopen("input.txt", "r", stdin); 
      
    ll T = 1; 
    //cin >> T; 
      
    while(T--)  
        solve(); 
} 
