THESE ARE WITHOUT -1 FOR X LEMMA
ID
X
Y
X+Y
Explanation

1
48000
1
48001
Send whole permutation, eat the fruit after the lemon or none if lemon is last fruit

2
0
3999
3999
Eat all except lemon

3
1080
2
1082
Send indices of the last 90 fruits in order. If lemon appears before, use 90C2 to encode information. Otherwise eat the fruit after the lemon, or none at all if lemon is the last one.


5
144
9
153
Send over the last 12 indices in order. If the lemon is before the last 12 (3988 positions), convey the info using a subset of the last 12 fruits (only need to eat at most 9, make sure to eat at least 2). If the lemon is in the last 12, eat the fruit after the lemon or none at all if the lemon is last.


6
102
11
113
We will use the last 12 fruits to communicate the lemon. We will send over the auxiliary tree of the segment tree of the last 12 indices, ordered in ascending index value. That is, we send over an edge-weighted tree where each node represents a segment tree node and edge weights represent the difference in depth between the two nodes. Only include a node if it is the root or the lca between a pair of the indices we want to communicate. We send over the structure of the tree using the up-down sequence representing the euler tour, then send the edge weights of all edges except the ones connected to leaves. This uses 2*23 + 11*4 bits. With this auxiliary tree and our segment tree, given an index that is supposed to have been recorded in the aux tree, we can determine which node in the aux tree it corresponds to. If the lemon appears before the last 12 fruits, we can communicate the index using the last 12 fruits. Otherwise, we can eat every fruit except the lemon. We need to send the xor of these fruits in the bitstring. Total bitstring length = 102 (46 for structure, 44 for weights, 12 for xor).


7
24
126
150
alice:
- locate last 2*sqrt(n) fruits
- send xor of smallest sqrt(n) among them
- send xor of largest sqrt(n) among them
- to encode information, eat every fruit except one from each bucket (if lemon appears before these fruits)
- otherwise eat all except lemon
First bucket has 63, second bucket has 64

8
82
11
93
X: 26 for structure, 44 for weights, 12 for xor
Use solution 6. Now we need to send over the structure of the unlabelled full binary tree. This tree has 12 leaf nodes. We work our way bottom-up. Each step, we merge 2 connected components of leaf nodes by drawing their LCA. Encode this step using the rightmost leaf node in the left connected component. No leaf node can appear twice hence it becomes a permutation of leaf nodes 1 to 11 (the last element will never be used), which is 26 bits.


9
76
11
87
X: 64 for function identification, 12 for xor
Refer to solution 5. To compress sending 9 numbers, Alice and Bob can share a set of functions, which maps [1, 4000] to [1, 12] such that for x < y, f(x) <= f(y). When Alice finds out the last 12 indices, she locates one function from the shared set which maps each of the 12 indices to a distinct output. She sends the index of this function through the bitstring. Bob, using this function, can find the ranks of the eaten fruits, thus conveying the subset choice.
(A valid function can be thought of as splitting [1, 4000] into 12 segments using 11 cutting points)
A heuristic to generate a set of functions is described as follows:
dp(x, y) represents the number of functions needed to map [1, x] to [1, y]. The answer to the question is dp(4000, 12). My strategy is to simply let x/2 be a cutting point in most functions, then iterate over dp(x/2, 1) * dp(x/2, y-1) + dp(x/2, 2) * dp(x/2, y-2)... to simulate distributing a different number of segments to each side. Then to account for cases where x/2 cannot be a cutting point (all y numbers are on the left side or right side), add dp(x/2, y) twice.
dp(4000, 12) = 10515526012044431302 (64 bits)


10
72
11
83
16 for structure, 44 for weights, 12 for xor
Use solution 6. Now we need to send over the structure of the unlabelled full binary tree. This tree has 12 leaf nodes, hence 23 total nodes. The 11 internal nodes form a normal binary tree, of which there are C11, where C11 is the 11th Catalan number. The index for each of these trees can be obtained via DP.




11
75
11
86
63 bits for 4000C6 (2nd, 4th, 6th, 8th), 12 for xor
Use 63 bits to encode 4000C6, which determines the 2nd, 4th, 6th, 8th, 10th, 12th smallest numbers in the last 12 fruits. Now, eat fruits from the last 9 fruits to create a binary string that encodes the position of the lemon. The corresponding positions of the eaten fruits in the binary string can be recovered from the 2nd, 4th, 6th, 8th, 10th, 12th smallest numbers. 
If the lemon is in the last 12 fruits, eat the other 11 fruits and use the xor. 


12
24
87
112
Use solution 7 but instead of sending information via the index of fruit in each bucket, send information via the number of fruits eaten in each bucket, as well as an element between the two buckets. For the element between, choose the median of the last few indices. This is effectively equivalent to buckets of size 43, 1, 44. Can convey 44*2*45 possibilities (-3 to avoid eating all but one). 
For the bitstring, also send xor of the last few indices. For the case where lemon is in the last few, eat all except the lemon.





14
36
28
64
Use solution 12,  but instead of 2 buckets, use 3 buckets and 2 elements between those 3 buckets. Bucket sizes are 9, 9, 9, sending 10*10*10*2*2=4000 possibilities.


16
49
11
60
Solution 10, but 37 bits for structure + weight by straight up discretising 90230486346 tree structures with considering weights
12 for xor



