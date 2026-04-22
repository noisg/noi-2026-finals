#include <bits/stdc++.h>
using namespace std;
int c[200005];
main(){
    ios_base::sync_with_stdio(0); cin.tie(0);
    int n, k; cin >> n >> k;
    for (int i = 1; i <= n; i++) cin >> c[i];
    int ans = 0;
    for (int i = 1; i <= n; i++){
        int cnt[4]; cnt[1] = cnt[2] = cnt[3] = 0;
        for (int j = i; j <= n; j++){
            cnt[c[j]]++;
            int mini = min({cnt[1], cnt[2], cnt[3]});
            int maxi = max({cnt[1], cnt[2], cnt[3]});
            if (maxi - mini <= k){
                ans = max(ans, j - i + 1);
            }
        }
    }
    cout << ans << '\n';
}