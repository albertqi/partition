# CS 124: Number Partition 
## Albert Qi and Steve Dalla
### April 20, 2023

## Table of Contents
1. Introduction
2. Dynamic Programming Solution
3. Karmarkar-Karp Algorithm
4. 
5. Discussion of Experiments

## 1. Introduction

In this write-up, we will compare the residues outputted by the Karmarkar-Karp algorithm as well as a repeated random, a hill climbing, and a simulated annealing algorithm both with and without prepartitions. First, we will illustrate a dynamic programming algorithm that runs in pseudo-polynomial time. Then, we will explain how the Karmarkar-Karp algorithm can be implemented in $O(n\log n)$ steps and then compare the algorithm to a variety of randomized heuristic algorithms on random input sets. Finally, we will discuss our experiments in more depth, covering the intricacies of our algorithms and other intriguing details we discovered along the way.

## 2. Dynamic Programming Solution

Although the Number Partition problem is NP-complete, it can still be solved in pseudo-polynomial time. We will illustrate a dynamic programming algorithm that does so:

1. Let $dp[i][j]$ be $\text{TRUE}$ if and only if there exists a subset of the first $i$ elements of $A$ whose sum equals $j$. Then, we have the following recurrence relation:

$$dp[i][j]=
\begin{cases}
\text{TRUE}&\text{if }i=0\wedge j=0\\
\text{FALSE}&\text{if }i=0\wedge j\neq 0\\
dp[i-1][j]&\text{if }j < A[i]\\
dp[i-1][j]\vee dp[i-1][j-A[i]]&\text{otherwise}
\end{cases}$$

2. To actually find a partition, however, we need to keep track of some extra information. Let $s[i][j]$ be some subset of the first $i$ elements of $A$ whose sum equals $j$, if one exists. Then, when we update $dp[i][j]$, we also need to update $s[i][j]$ appropriately:

$$s[i][j]=
\begin{cases}
\lbrace\rbrace&\text{if }i=0\\
s[i-1][j]&\text{if }dp[i-1][j]=\text{TRUE}\\
s[i-1][j-A[i]]\cup\lbrace A[i]\rbrace&\text{if }dp[i-1][j-A[i]]=\text{TRUE}\\
\lbrace\rbrace&\text{otherwise}
\end{cases}$$

3. Loop from $i=0$ to $i=n$ and on each iteration, loop from $j=0$ to $j=\sum a_k$. On each of these inner iterations, calculate and update $dp[i][j]$ and $s[i][j]$ according to the recurrence relations.

4. Let $i=\lfloor(\sum a_k)/2\rfloor$. Then, while $dp[n][i]=\text{FALSE}$, set $i=i-1$.

5. Return $(s[n][i],A\setminus s[n][i])$.

Now, we will analyze the runtime of our algorithm:

1. Suppose that the sequence of terms in $A$ sum up to some number $b$. Then, creating an array $dp$ of size $O(n)\times O(b)$ takes $O(nb)$ time.

2. Because we are using memoization via $dp$, there are only $O(nb)$ subproblems that we need to solve. In each subproblem, we only perform a constant amount of work, meaning filling $dp$ takes $O(nb)$ time.

3. Creating an array $s$ of size $O(n)\times O(b)$ takes $O(nb)$ time.

4. Again, there are only $O(nb)$ subproblems that we need to solve. In each subproblem, updating $s$ only takes a constant amount of time, meaning filling $s$ takes $O(nb)$ time.

5. Each $a_k$ has $O(\log b)$ bits, and we have $O(n)$ of these numbers in total. Thus, adding all of the $a_k$'s takes $O(n\log b)$ time.

6. Dividing an $O(\log b)$-bit number by $2$ takes $O(\log b)$ time.

7. Finding the floor takes $O(1)$ time.

8. Our while loop runs for $O(b)$ iterations, and we perform a constant amount of work on each iteration. Thus, our while loop takes $O(b)$ time overall.

9. Finding the set difference takes $O(n)$ time.

Our overall runtime for the algorithm is just the sum of the runtimes for these steps. As a result, the runtime for the algorithm is $O(nb)+O(nb)+O(nb)+O(nb)+O(n\log b)+O(\log b)+O(1)+O(b)+O(n)=O(nb)$ by asymptotic theory.

## 3. Karmarkar-Karp Algorithm



## 4. 



## 5. Discussion of Experiments

