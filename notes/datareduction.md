## Degree-0-Rule
Delete any vertex v with d(v) = 0.

## Degree-1-Rule
For any vertex v with d(v) = 1, take its neighbour into the MVC.

## Triangle-Rule
For any vertex u with N(u) = {v, w} and {v,w} in E take v and w into the MVC.

## Degree-Higher-k-Rule
For any vertex x with d(x) > k, take it into the MVC.

## 2-Fold-Rule
For any vertex u with N(u) = {v,w} create a new vertex u' that merges u,v and w. Connect u' to any vertex in N²(u).
*N²(v) is defined as the neighbourhood with distance 2 from the vertex v*

## Dominate-Rule
For any 2 vertices u,v, if u dominates v take u into the MVC
*A vertex u is said to dominate v if N(v) ⊆ N(u) and {u,v} in E*

## Degree-3-Rule
For any 2 vertices u,v with N(u) = N(v) and d(u) = d(v) = 3 we look at 2 cases:
    - If there exists at least one edge amoung N(u) put N(u) in MVC
	- Otherwise remove N[u] and N[v] from G and create a new vertex w connected to N²(u)
	- Now if w is put in the MVC, put N(u) in there instead
	- Otherwise if w is not in the MVC, put u and v in it 
