from functools import lru_cache




@lru_cache(None)
def dp(n, budget):
    """
    Number of valid rooted trees with:
    - n nodes
    - remaining path budget = budget
    - edge weights >= 1
    - nodes have 0, 1, or 2 children
    - left/right distinguishable
    - for 1 child: left vs right distinct
    """
    if budget < 0:
        return 0
    if n == 1:
        return 1  # leaf
    if budget <= 0:
        return 0
    total = 0

    # Case 1: exactly one child (left or right = 2 choices)
    for w in range(1, budget+1):
        total += 2 * dp(n - 1, budget - w)

    # Case 2: exactly two children (ordered)
    for s1 in range(1, n-1):
        s2 = n - 1 - s1
        for w1 in range(1, budget+1):
            for w2 in range(1, budget+1):
                total += dp(s1, budget - w1) * dp(s2, budget - w2)

    return total


def main():
    leaves = 9
    subtree_nodes = 8  # root already counted
    total = 0
    MAX_SUM = 8 # root-to-leaf path sum must be < 11
    # root edge weight can be >= 0
    for w0 in range(0, MAX_SUM+1): #w0 is the depth of the root
        remaining_budget = (MAX_SUM) - w0  # must stay < 8
        if remaining_budget >= 0:
            total += dp(subtree_nodes, remaining_budget)

    print("Number of valid trees:", total)


if __name__ == "__main__":
    main()