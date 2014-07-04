#include <iostream>
#include <fstream>
#include <list>
#include <limits>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

class match {
    public:
        match(int, int, int, int);

        int ai;
        int aj;
        int bi;
        int bj;
};

match::match(int gai, int gaj, int gbi, int gbj){
    ai = gai;
    aj = gaj;
    bi = gbi;
    bj = gbj;
}

typedef std::unordered_map<string, list<match> > umap;

bool match_merge_check(match first, match second){
    // if true, merge.
    // first = before, second = current
    if (first.aj + 1 == second.aj)
        if (first.ai <= second.ai && second.ai <= first.aj){
            return true;
        }
    return false;
}

bool match_del_check(match first, match second){
    // first = new
    // if true, should delete second
    if (first.aj + 1 != second.aj)
        if (first.ai <= second.ai && second.ai <= first.aj){
            return true;
        }
    return false;
}

struct NODE {
	int Info;
	int Flag;
	struct  NODE *Left_Child;
	struct  NODE *Right_Child;
    match *matchp;
};

struct NODE *Binary_Tree (int , struct NODE *, int *, match *);
void Output(struct NODE *, int );
struct  NODE *Balance_Right_Heavy(struct NODE *, int *);
struct NODE *Balance_Left_Heavy(struct NODE *, int *);
struct NODE *DELETE(struct NODE *, struct NODE *, int *);
struct NODE *Delete_Element(struct NODE *, int , int *);

void hash_the_string(string a, int k, umap &table){
    // hash all substrings of k consecutive chars into table by starting index
    int i = 0;
    while (i < (signed) a.length() - k + 1){
        if (table.count(a.substr(i, k)) == 0)
            table[a.substr(i, k)] = list<match>();
			
        table[a.substr(i, k)].push_back(match(i, i + k - 1, 0, 0));
        i++;
    }
}

list<match> find_kmer(string b, int k, umap &table){
    //

    int i = 0;
    std::unordered_map<int, list<match> > mapint;
    while (i < (signed) b.length() - k + 1){
//        cout << b.substr(i, k) << "\n";
        if (table.count(b.substr(i, k)) == 1){
//            cout << "count\n";
            // copy list from table and put into mapint
            mapint[i + k - 1] = table[b.substr(i, k)];
            for (list<match>::iterator iter = mapint[i + k - 1].begin(); iter != mapint[i + k - 1].end(); iter++){
                (*iter).bi = i;
                (*iter).bj = i + k - 1;
            }

            if (i == 0){
                i++;
                continue;
            }

            list<match> tmpinv; //tmp there so we don't modify the list as we loop through it
            for (list<match>::iterator iter = mapint[i + k - 1].begin(); iter != mapint[i + k - 1].end(); iter++){
                for (list<match>::iterator iter2 = mapint[i + k - 2].begin(); iter2 != mapint[i + k - 2].end(); iter2++){
                    if (match_merge_check(*iter2, *iter)){
//                        cout << "mergecheck\n";
                        match invl((*iter2).ai, (*iter).aj, (*iter2).bi, i + k - 1);
                        tmpinv.push_back(invl);
                    }

                }
            }

            // loop through tmpinv and delete any intervals that need to be deleted
            for (list<match>::iterator iter = tmpinv.begin(); iter != tmpinv.end(); iter++){
                for (list<match>::iterator iter2 = mapint[i + k - 1].begin(); iter2 != mapint[i + k - 1].end(); iter2++){
                    if (match_del_check(*iter, *iter2)){
                        mapint[i + k - 1].erase(iter2++);
                        iter2--;
                    }

                }

                for (list<match>::iterator iter2 = mapint[i + k - 2].begin(); iter2 != mapint[i + k - 2].end(); iter2++){
                    if (match_del_check(*iter, *iter2)){
                        mapint[i + k - 2].erase(iter2++);
                        iter2--;
                    }

                }
            }

            mapint[i + k - 1].splice(mapint[i + k - 1].end(), tmpinv); // stick the two lists together
        }
        i++;
    }

    list<match> result;
    for (std::unordered_map<int, list<match> >::iterator iter = mapint.begin(); iter != mapint.end(); iter++){
        result.splice(result.end(), iter->second);
    }
    return result;
}

bool compareLength(match a, match b){
    return (a.aj - a.ai + 1) > (b.bj - b.bi + 1);
}

int check_conflict(match a, match b){
    if (a.aj < b.ai && a.bj < b.bi) return 0; // check if both substrings from a is left of b
    if (a.ai > b.aj && a.bi > b.bj) return 0; // check if both substrings from a is right of b
    return 1; // else conflicts
}

int try_anchor(match a, struct NODE *anchors){
    // Try to see if a is consistant with the current list of anchors
    // Return 0 if not consistant
    // Return 1 if we should add a to the anchors

    if (anchors == NULL)         return 1;      // a is not found in anchor, no conflict

    match root = *(anchors->matchp);            // root of subtree
    if (check_conflict(a, root)) return 0;      // a conflict is found

    if (a.aj < root.ai) return try_anchor(a, anchors->Left_Child);
    else return try_anchor(a, anchors->Right_Child);
}

void dfsAddMatchesToList(struct NODE *Anchors, list<match> &aclist){
	// add each elem in the tree to aclist, at the sametime delete the tree
	if (Anchors == NULL)         return;

    match root = *(Anchors->matchp);            // root of subtree
    aclist.push_back(root); // add to list

    dfsAddMatchesToList(Anchors->Left_Child, aclist);
    dfsAddMatchesToList(Anchors->Right_Child, aclist);
	free(Anchors);
}

list<match> subSelAnchors(list<match> array, string a, string b){
    struct NODE *Anchors = NULL;
    int H;

    array.sort(compareLength); // sort the list of matches by length
/*	cout << "\n -- Part Ba ----- \n\n";
	list<match> &anchors = array;
	for (list<match>::iterator iter = anchors.begin(); iter != anchors.end(); iter++){
        cout << iter->ai << " " << iter->aj << " " << iter->bi << " " << iter->bj << "\n";
        cout << "A: " << a.substr(iter->ai, iter->aj - iter->ai + 1) << " | B: " << b.substr(iter->bi, iter->bj - iter->bi + 1) << "\n\n";
    }*/

    for (list<match>::iterator iter = array.begin(); iter != array.end(); iter++){ // starting from the second element
        if (iter == array.begin())
			Anchors = Binary_Tree(array.begin()->ai, NULL, &H, &*iter); // initalize the Binary Tree with the first element
		else if (try_anchor(*iter, Anchors)){
            Binary_Tree(iter->ai, Anchors, &H, &*iter);
        }
    }
	
	list<match> aclist;
	dfsAddMatchesToList(Anchors, aclist); // go over the tree, add each elem to aclist (and del the tree as well!)
	return aclist;
}

using std::vector;

void fwdEdit(int a1, int a2, int b1, int b2, string &s1, string &s2, vector<vector<int> > &m){
    //a1, a2 begin and end of the first string s1
    //b1, b2 begin and end of the second string s2
    //m

    int i, j;
    m[a1%2][b1] = 0; // use mod to keep the size of m down to 2*n
    for ( j = b1+1; j <= b2; j++)
            m[a1%2][j] = m[a1%2][j-1]+1;

    for(i=a1+1; i <= a2; i++)     //Determine which column to use
    {
        m[i%2][b1] = m[(i-1)%2][b1] + 1;

        for(j=b1+1; j <= b2; j++)  //Fill up that column
        {
            int diag = m[(i-1)%2][j-1];
            if( s1[i-1] != s2[j-1] ) ++diag;

            m[i%2][j] = min( diag, min( m[(i-1)%2][j]+1, m[i%2][j-1]+1 ) );
        }
    }
}

void revEdit(int a1, int a2, int b1, int b2, string &s1, string &s2, vector<vector<int> > &m)
// LCS on (s1[a1..a2)) and reverse(s2[b1..b2))
{   int i, j;


    m[a2%2][b2] = 0; // boundary conditions
    for(j=b2-1; j >= b1; j--)
        m[a2%2][j] = m[a2%2][j+1]+1;

    for(i=a2-1; i >= a1; i--){

        m[i%2][b2] = m[(i+1)%2][b2] + 1;

        for(j=b2-1; j >= b1; j--){

            int diag = m[(i+1)%2][j+1];

            if( s1[i] != s2[j] ) diag += 1;

            m[i%2][j] = min( diag,min( m[(i+1)%2][j]+1, m[i%2][j+1]+1 ) );
        }
    }
}//revDPA

void align(int a1,int a2, int b1,int b2, string &r1, string &r2, string &s1, string &s2, int &L)
// align s1[a1..a2) with s2[b1..b2)
// r1 is the processed string of first string
// r2 is the processed string of second string
{
    int mid, i;
    if( a2 <= a1 ){ // s1 is empty string

        for(i=b1; i < b2; i++){
            r1 += "-";
            r2 += s2.substr(i,1);
            L++;
        }
    }

    else if( b2 <= b1 ){ // s2 is empty string
        for(i=a1; i < a2; i++){
            r1 += s1.substr(i,1);
            r2 += "-";
            L++;
        }
    }
    else if( a2-1 == a1 ) // s1 is one character and s2 is not empty
    {

        string ch =  s1.substr(a1,1);
        int memo = b1;
        for(i=b1+1; i < b2; i++){
            if(s2.substr(i,1) == ch) memo=i;
        }
        for(i=b1; i < b2; i++)
        {
            if(i == memo){
                r1 += ch;
                if(s2.substr(i,1) != ch) L++;
            }
            else{
                r1 += "-";
                L++;
            }

            r2 += s2.substr(i, 1);
        }
    } // a b [l=2] mid=1, a b c [l=3] mid=1, a b c d [l=4] mid=2

    else // a2>a1+1, s1 has at least 2 chars,  divide s1 and conquer
    {

        int mid = floor((a1+a2)/2);

        vector<vector<int> > fwd; //check size

        fwd.resize(2);

        for (int k = 0; k < 2; ++k){
            vector<int> tmp (b2, std::numeric_limits<int>::max());
            fwd[k] = tmp;
        }

        vector<vector<int> > rev; //check size

        rev.resize(2);

        for (int k = 0; k < 2; ++k){
            vector<int> tmp (b2, 0);
            rev[k] = tmp;
        }

        fwdEdit(a1, mid, b1, b2, s1, s2, fwd);
        revEdit(mid, a2, b1, b2, s1, s2, rev);

        int s2mid=b1;
        int best=std::numeric_limits<int>::max(); //set best to infinity

        for(i=b1; i<=b2; i++) // look for cheapest split of s2
        {
            int sum = fwd[mid%2][i]+rev[mid%2][i];
            if( sum < best ) {
                best=sum;
                s2mid=i;
            }
        }
        align(a1,mid, b1,s2mid, r1, r2, s1, s2, L);
        align(mid,a2, s2mid,b2, r1, r2, s1, s2, L);
    }

 }
 
 bool compare_ai(match a, match b){
	 return a.ai < b.ai;
 }

int main(int argc, char *argv[]){
    string aa, bb, a, b, s1, s2;// = "ACTGACT", b = "ACGCTG";
	
	ifstream fa, fb;
	fa.open(argv[1], ifstream::in);
	fb.open(argv[2], ifstream::in);
	int k = atoi(argv[3]);
	
	getline(fa, s1);
	while(getline(fa, aa)){
		a += string(aa);
	}
	
	getline(fb, s2);
	while(getline(fb, bb)){
		b += string(bb);
	}
	
	list<match> anchors;
	int totalWeight = 0;
	if (k > 0){
		//cout << "\n -- Part A ----- \n\n";
		umap table;
		hash_the_string(a, k, table);
		list<match> matches = find_kmer(b, k, table);
		//for (list<match>::iterator iter = matches.begin(); iter != matches.end(); iter++){
		 //   cout << iter->ai << " " << iter->aj << " " << iter->bi << " " << iter->bj << "\n";
		 //   cout << "A: " << a.substr(iter->ai, iter->aj - iter->ai + 1) << " | B: " << b.substr(iter->bi, iter->bj - iter->bi + 1) << "\n\n";
		//}
		
		
		anchors = subSelAnchors(matches, a, b);
		
		//cout << "\n -- Part B ----- \n\n";
		for (list<match>::iterator iter = anchors.begin(); iter != anchors.end(); iter++){
			totalWeight += iter->aj - iter->ai + 1;
			//cout << iter->ai << " " << iter->aj << " " << iter->bi << " " << iter->bj << "\n";
			//cout << "A: " << a.substr(iter->ai, iter->aj - iter->ai + 1) << " | B: " << b.substr(iter->bi, iter->bj - iter->bi + 1) << "\n\n";
		}
	}

	//cout << "\n -- Part C ----- \n\n";

	string resA, resB, tmpA, tmpB;
	int editDistance = 0, tmpD;
	bool condition = true;
	for (list<match>::iterator iter = anchors.begin(); condition; iter++){
		match *prev;
		string tmpA, tmpB;
		tmpD = 0;
		
		if (anchors.begin() == anchors.end()) {
			align(0, (int) a.length(), 0, (int) b.length(), tmpA, tmpB, a, b, tmpD);
			condition = false;
		}
		
		else if (iter == anchors.begin()){
			align(0, iter->ai, 0, iter->bi, tmpA, tmpB, a, b, tmpD);
		}
		else if (iter == anchors.end()){
			//cout << prev->ai << " " << prev->aj << " " << prev->bi << " " << prev->bj << "\n";
			align(prev->aj + 1, (int) a.length(), prev->bj + 1, (int) b.length(), tmpA, tmpB, a, b, tmpD);
			condition = false;
			
		}
		else {
			align(prev->aj + 1, iter->ai + 1, prev->bj + 1, iter->bi + 1, tmpA, tmpB, a, b, tmpD);
		}
		
		//cout << iter->ai << " " << iter->aj << " " << iter->bi << " " << iter->bj << "\n";
		
		//cout << tmpD << "\n";
		editDistance += tmpD;
		resA += tmpA;
		resB += tmpB;
		prev = &*iter;
		if (iter != anchors.end()){
			resA += a.substr(iter->ai, iter->aj - iter->ai + 1);
			resB += b.substr(iter->bi, iter->bj - iter->bi + 1);
		}	
	}
	
	
	cout << s1 << "\n";
	cout << resA << "\n";
	cout << s2 << "\n";
	cout << resB << "\n";
	cout << "=" << editDistance << " " << totalWeight << "\n";
	
	fa.close();
	fb.close();
    return 0;
}

