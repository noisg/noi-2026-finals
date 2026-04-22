#include <bits/stdc++.h>
using namespace std;
// I don’t want to read the room anymore; I’m unpredictable, that’s me; I’m just being myself  ~ Siyeon, #QWER
#define pii pair<int, int>
const int INF = 1e9;
struct node{
    int s, e, m, v;
    node *l, *r;
    node(int s, int e): s(s), e(e), m((s+e)/2), v(INF){
        if (s != e){
            l = new node(s, m);
            r = new node(m+1, e);
        }
    }
    void upd(int x, int val){
        if (s==e) {
            v = val;
            return;
        }
        if (x <= m) l->upd(x, val);
        else r->upd(x, val);
        v = min(l->v, r->v);
    }
    int qwer(int x, int y){
        if (s == x && e == y) return v;
        if (y <= m) return l->qwer(x, y);
        else if (x > m) return r->qwer(x, y);
        else return min(l->qwer(x, m), r->qwer(m+1, y));
    }
} *root;
int C[200005];
int p[200005], q[200005];
deque<int> qstk[400005];
const int offset = 200001;
main(){
    ios_base::sync_with_stdio(0); cin.tie(0);
    int n, k; cin >> n >> k;
    for (int i = 1; i <= n; i++){
        cin >> C[i];
    }
    int ans = 0;
    for (int x = 1; x <= 3; x++){
        for (int i = 1; i <= n; i++){
            if (C[i] == 1) C[i] = x;
            else if (C[i] == x) C[i] = 1;
        }
        // now 1 is the one with most common species
        map<int, vector<int>> pasc;
        pasc[0].push_back(0);
        for (int i = 1; i <= n; i++){
            p[i] = p[i-1] + (C[i] == 1 ? 1: (C[i] == 2 ? -1: 0));
            q[i] = q[i-1] + (C[i] == 1 ? 1: (C[i] == 2 ? 0: -1));
            pasc[p[i]].push_back(i);
        }
        root = new node(0, 400004);
        auto rptr = pasc.begin();
        for (auto [pv, pvl]: pasc){
            while (rptr != pasc.end() && rptr->first - pv <= k){
                for (auto idx: rptr->second){
                    while (!qstk[q[idx] + offset].empty() && qstk[q[idx] + offset].back() > idx){
                        qstk[q[idx] + offset].pop_back();
                    }
                    qstk[q[idx] + offset].push_back(idx);
                    root->upd(q[idx] + offset, qstk[q[idx] + offset][0]);
                }
                for (auto i : rptr->second){
                    int minl = root->qwer(max(0, q[i] - k + offset), min(400004, q[i] + offset));
                    ans = max(ans, i - minl);
                    //cout << minl + 1 << ' ' << i << '\n';
                }
                rptr++;
            }
    
            for (auto l : pvl){
                if (!qstk[q[l] + offset].empty() && qstk[q[l] + offset][0] == l){
                    qstk[q[l] + offset].pop_front();
                    if (!qstk[q[l] + offset].empty()) root->upd(q[l] + offset, qstk[q[l] + offset][0]);
                    else root->upd(q[l] + offset, INF);
                }
            }
        }
    }
    cout << ans << '\n';
}