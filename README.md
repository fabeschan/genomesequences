##Finding Maximal Matches in Genome Sequences

####ECE358 End of Semester Exercise

*Bonus: Finding Maximal Matches in O(n) Time (Done) *

*Bonus: Edit Distance Aligner in O(n) Space (Done)*


###Finding Maximal Matches (A)

Description:

To find maximal matches on the two strings A and B of size >= k, we first hash the string A by associating each consecutive sub-strings of length k with the position of the sub-string (denoted by beginning and end points).

Using this hash, we use consecutive sub-strings of length k in B as key to determine if matching sub-strings exist and where these sub-strings are in A. The position of each of the matching sub-string in A and the position of the sub-string in B are used together to create match objects. The end point of B is used as a key to store the list of match objects in a second hash. (ie the hash returns the list of all matches that have B ending in position n, where n is the key passed in.)

These matches are extended using the following rules:

1. If there are matches in B that ends in n, look at the list of matches in B that ends in n-1.
	- a. 	If any of these match have sub-string A also ending 1 earlier than the current sub-string A and the two intervals overlap. 
		Then combine them. Delete the original matches.
	- b. 	Delete any remaining matches that only have the two intervals overlapping.
	- c. 	Add the combined string into the appropriate place in the hash (by its end position)

2. If there is no match, just move on.

After this is done with all consecutive sub-strings of length k in B, the remaining entries in the second hash are the Maximal Matches.

Implementation:

The hash table we used were provided by C++'s unordered map. A match class was defined with the fields a1, a2, b1, b2. Where a1, a2 are the beginning and end points of sub-string in A, and b1, b2 are the beginning and end points of sub-string in B.

Complexity:

- O(m+n-k) Time: We assume that k is big enough so there is on average of one match entry per key in the first hash table. This assumption also implies that there will be only one entry per key in the second hash table. (There is only one match to start with. Any combining will result in both a deletion and insertion of an entry. So the total is never more than one). Thus checking and extending the matches is constant time (only need to access the hash entries). This constant work need to be done a number of time directly proportional to the size of (n or m) - k. Where m and n are the number of characters in the string A and B.

- O(m+n-k) Space: The first table need to store m-k entries and the second table need to store n-k entries. 


_______________________________________________________________________________________

###Sub-selecting a set of Anchors (B)

Description:

Using the list of maximal matches, we first sort them by their lengths. (defined by a2 - a1 + 1 of the match object). By greedily taking the longest consistent matches first, we create a list of anchors. The list of anchors are stored in a self balancing binary search tree using a1 as the value of the node (AVL implementation provided by professor Francois Pitt as lecture material in CSC190 during 2011 winter semester) 

Implementation:

The list of maximal matches were sorted by their length using C++'s sort function for lists. A number of sub-functions were made to check the consistency of any two interval by checking if they overlap and checking if they crisscross. The list of maximal matches were then examined one by one, and compared with the existing anchors using the following algorithm.

1. If the new interval checks out to be inconsistent at any step, reject it and start over with a new interval.
2. If the new interval is consistent with the root of the tree. Check its a1. If it is smaller than the root's a1, repeat for left subtree. Else repeat for right subtree.
3. When there are no more children and no inconsistency has been detected, add that interval to the avl tree and balance.

After all the anchors have been chosen, convert the avl tree to a list.

Complexity:

- O(n log n) Time: Checking inconsistency and adding new interval is log n. Need to do it for n matches. n is the number of maximal matches.
- O(n) Space: Only need to store the tree. Needs one node for each match.


_______________________________________________________________________________________

###Edit Distance Aligner (C)

Description and Implementation:

Using the O(n) space edit distance algorithm discussed in lecture. Using the recursion relationships:
	m[i%2][j] = min( if match (m[(i-1)%2][j-1]) else m[(i-1)%2][j-1]+1, m[(i-1)%2][j]+1, m[i%2][j-1]+1 ) for forward calculations
and	m[i%2][j] = min( if match (m[(i+1)%2][j+1]) else m[(i+1)%2][j+1]+1, m[(i+1)%2][j]+1, m[i%2][j+1]+1 ) for backward calculations

The alignment algorithm is called recursively, with the base cases 1) either string being empty or 2) one character aligned with multiple characters.
- In case 1), dashes are added to the appropriate string while edit distance is incremented
- In case 2), edit distance is incremented for point mutations and deletion/insertion. Dashes are added 

If neither of the base cases applies:

1. find middle column
2. for each of the element in the column, calculate the forward edit distance and the reverse edit distance. 
3. identify which element have the lowest sum, include that element in the resulting string.
4. call recursively using that element as the dividing point. align top left half and bottom right half.

Complexity:

- O(nm) Time: Recursion Relation T(x) = 2(T(x/2) + O(1) => O(x). Where x is nm and n, m are the length of the two strings. 
- O(n+m) Space: Only need two columns to calculate the edit distance. Need one column to store the elements that are identified as deletion/insertion/mutation.
_______________________________________________________________________________________

###Putting it all together (D)

Description and Implementation:

After reading in the strings from the .mfa files, call the function from part A to return a list of maximal matches. Calling the part B function on this list of maximal matches, we obtain a list of anchors. This list of anchors are then sorted by their starting position of one of the strings. The anchors are then looped through, using the function from part C on the sub-strings between the anchors to find the edit distance and the edited string. The edit distances are then added together and the edited strings were combined with the anchors. 

Complexity:
- O(nm) Time: If only one small anchor was found, the time complexity of the Edit Distance Aligner dominates. 
- O(n+m) Space: no sub-function of the alignment need more than linear space. So the entire thing need only linear space. 
