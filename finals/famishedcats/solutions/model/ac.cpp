#include <bits/stdc++.h>
using namespace std;
#define int long long
int N;

int d[500010];
int f[500010];
int ff[500010];
int pref[500010];
int pref2[500010];
int x;

priority_queue<int> pq;

int32_t main(){
    cin >> N >> x;
    for(int a=1; a<=N; a++){
        cin >> d[a];
    }
    for(int a=1; a<=N; a++){
        cin >> f[a];
    }
    for(int a=1; a<=N/2; a++){
        ff[a] = max(f[a], f[N - a + 1]);
        ff[N - a + 1] = f[a] + f[N - a + 1] - ff[a];
        //cout << ff[a] << ' ';
    }
    //cout << '\n';     
    int t = f[1] + (x - d[1]);
    int tt = ff[1] + (x - d[1]);
    
    int ans = -1;
    pref[1] = t - d[2];
    pref2[1] = tt - d[2];
    for(int a=2; a<=N-1; a++){
        pref[a] = pref[a-1] + f[a] - d[a+1];
        pref2[a] = pref2[a-1] + ff[a] - d[a+1];
    }
    bool found = false;
    for(int a=1; a<=N-1; a++){
        if(pref2[a] < 0){
            found = true;
            ans = a;
            break;
        }
    }
    if(!found){
        ans = N;
    }

    
    int sofar = 0;
    int cntswap = 0;
    for(int a=1; a<=min(N/2, ans-1); a++){
        //cout << a << ": \n";
        int pos1 = a;
        int pos2 = N - a;
        if(pos1 > ans){
            break;
        }
        if(f[N - a + 1] > f[a]){
            //cout << f[N - a + 1] << ' ' << f[a] << '\n';
            pq.push(f[N - a + 1] - f[a]);
        }
        int recover = pref[pos1];
        if(pos2+1 <= ans){
            recover = min(recover, pref[pos2]);
        }
        //cout << pref[pos1] << ' ' << pref[pos2] << ' ' << recover << '\n';
        if(recover >= 0 || recover + sofar >= 0) continue;
        else{
            while(recover + sofar < 0){
                sofar += pq.top();
                //cout << "CHOSEN: " << pq.top() << '\n';
                pq.pop();
                cntswap++;
            }
        }
    }
    cout << ans << ' ' << cntswap;
}