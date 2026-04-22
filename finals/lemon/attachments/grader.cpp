#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include "lemon.h"

static double score_fraction(int subtask, int max_X, int max_Y) {
    if (subtask == 1) {
        if (max_Y > 2) return 0.0;
        if (max_X == 0) return 1.0;
        return std::min(288.0 / max_X, 1.0);
    } else if (subtask == 2) {
        if (max_Y > 9) return 0.0;
        if (max_X == 0) return 1.0;
        return std::min(30.0 / max_X, 1.0);
    } else {
        int sum = max_X + max_Y;
        if (sum == 0) return 1.0;
        return std::min(20.0 / sum, 1.0);
    }
}

int main() {
    int subtask;
    scanf("%d", &subtask);
    int T;
    scanf("%d", &T);

    int max_X = 0, max_Y = 0;
    bool all_correct = true;

    for (int tc = 0; tc < T; tc++) {
        int N, L;
        scanf("%d %d", &N, &L);

        std::vector<int> p(N + 1, 0);
        for (int i = 1; i <= N; i++) {
            scanf("%d", &p[i]);
        }

        // --- Alice phase ---
        std::string B = init(subtask, N, p);

        // Validate bitstring length
        if ((int)B.size() > 5000) {
            fprintf(stderr, "init() returned bitstring of length %d, exceeds limit 5000\n", (int)B.size());
            printf("0.0\n");
            return 0;
        }

        // Validate bitstring characters
        for (char c : B) {
            if (c != '0' && c != '1') {
                fprintf(stderr, "Invalid bitstring character '%c'\n", c);
                printf("0.0\n");
                return 0;
            }
        }

        int X = (int)B.size();
        if (X > max_X) max_X = X;

        // Feed fruits to Alice
        std::vector<int> uneaten;
        for (int i = 1; i <= N; i++) {
            int id = p[i];
            bool is_lemon = (id == L);

            bool eat = receive_fruit(id, is_lemon);

            // Cannot eat the lemon
            if (is_lemon && eat) {
                fprintf(stderr, "TC %d: receive_fruit(%d, true) returned true; attempted to eat lemon\n", tc, id);
                printf("0.0\n");
                return 0;
            }

            if (!eat) {
                uneaten.push_back(id);
            }
        }

        int Y = N - (int)uneaten.size();
        if (Y > max_Y) max_Y = Y;

        // Sort uneaten fruits
        std::sort(uneaten.begin(), uneaten.end());
        uneaten.insert(uneaten.begin(), 0);

        // --- Bob phase ---
        int ans = answer(subtask, N, B, uneaten);

        if (ans < 1 || ans > N) {
            fprintf(stderr, "TC %d: INVALID — answer() returned %d, not a valid fruit id (expected 1..%d)\n",
                    tc, ans, N);
            printf("0.0\n");
            return 0;
        }

        if (ans != L) {
            fprintf(stderr, "TC %d: WRONG — Bob answered %d, expected %d (X=%d, Y=%d)\n",
                    tc, ans, L, X, Y);
            all_correct = false;
        } else {
            fprintf(stderr, "TC %d: OK (X=%d, Y=%d)\n", tc, X, Y);
        }
    }

    fprintf(stderr, "Max X = %d, Max Y = %d\n", max_X, max_Y);

    if (!all_correct) {
        fprintf(stderr, "Subtask %d: WRONG ANSWER, score=0.0\n", subtask);
        printf("0.0\n");
    } else {
        double frac = score_fraction(subtask, max_X, max_Y);
        fprintf(stderr, "Subtask %d: X=%d, Y=%d, score=%.6f\n", subtask, max_X, max_Y, frac);
        printf("%.6f\n", frac);
    }

    return 0;
}
