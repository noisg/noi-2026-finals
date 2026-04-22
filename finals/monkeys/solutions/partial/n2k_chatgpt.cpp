#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, k;
    cin >> n >> k;

    vector<long long> p(n);
    for (int i = 0; i < n; i++) cin >> p[i];

    string d;
    cin >> d;

    set<pair<int,int>> friends;

    for (int day = 0; day <= k; day++) {

        vector<pair<long long,int>> pos(n);

        for (int i = 0; i < n; i++) {
            long long x = (d[i] == 'L') ? p[i] - day : p[i] + day;
            pos[i] = {x, i};
        }

        sort(pos.begin(), pos.end());

        for (int i = 0; i < n; ) {
            int j = i;
            vector<int> group;

            while (j < n && pos[j].first == pos[i].first) {
                group.push_back(pos[j].second);
                j++;
            }

            for (int a = 0; a < group.size(); a++)
                for (int b = a+1; b < group.size(); b++) {
                    int u = group[a], v = group[b];
                    if (u > v) swap(u,v);
                    friends.insert({u,v});
                }

            i = j;
        }
    }

    cout << friends.size() << "\n";
}