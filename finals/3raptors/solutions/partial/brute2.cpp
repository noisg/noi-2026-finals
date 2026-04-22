#include <bits/stdc++.h>
using namespace std;
int c[200005];
main(){
    ios_base::sync_with_stdio(0); cin.tie(0);
    int n, k; cin >> n >> k;
    for (int i = 1; i <= n; i++) cin >> c[i];
    int ans = 0;
    for (int mid = n; mid >= 1; mid--){
        int cnt[4]; cnt[1] = cnt[2] = cnt[3] = 0;
        for (int i = 1; i <= mid - 1; i++){
            cnt[c[i]]++;
        }
        for (int i = mid; i <= n; i++){
            cnt[c[i]]++;
            int mini = min({cnt[1], cnt[2], cnt[3]});
            int maxi = max({cnt[1], cnt[2], cnt[3]});
            if (maxi - mini <= k){
                cout << mid << '\n';
                return 0;
            }
            cnt[c[i - mid + 1]]--;
        }
    }
    cout << ans << '\n';
}
