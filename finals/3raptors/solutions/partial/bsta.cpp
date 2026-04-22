#include <bits/stdc++.h>
using namespace std;
int c[200005];
bool check(int mid, int n, int k){
    int cnt[4]; cnt[1] = cnt[2] = cnt[3] = 0;
    for (int i = 1; i <= mid - 1; i++){
        cnt[c[i]]++;
    }
    for (int i = mid; i <= n; i++){
        cnt[c[i]]++;
        int mini = min({cnt[1], cnt[2], cnt[3]});
        int maxi = max({cnt[1], cnt[2], cnt[3]});
        if (maxi - mini <= k){
           return true;
        }
        cnt[c[i - mid + 1]]--;
    }
    return false;
}
main(){
    ios_base::sync_with_stdio(0); cin.tie(0);
    int n, k; cin >> n >> k;
    for (int i = 1; i <= n; i++) cin >> c[i];
    int ans = 0;
    int mini = 0;
    int maxi = n + 1;
    while (maxi - mini > 1){
        int mid = (maxi + mini) / 2;
        if (check(mid, n, k)) mini = mid;
        else maxi = mid;
    }
    cout << mini << '\n';
}
