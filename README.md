# P* - Parallel A* (Aditya Kannan + Gabriel Lee)

### Summary
We are going to implement a parallel implementation of A* and analyze the performance of the algorithm on both systems. We plan to implement our algorithm on both GPU and multi-core CPU platforms, for which we will use CUDA and OpenMP respectively.

### Background
A* is a single-source shortest path search algorithm used widely in AI and Robotics for graph search problems. It finds the fastest path in a (un)weighted (un)directed graph between two nodes and can therefore be useful for graph search and motion planning. With the right heuristic, A* has been proved to be a complete and optimal search algorithm.

A* is a variant of Dijkstra's algorithm that involves using a heuristic to reduce the number of nodes searched by the algorithm. Heuristics are generally hand-selected and must satisfy some properties for optimality. On general graphs, heuristics must be consistent (i.e. two adjacent nodes and the goal node must satisfy the triangle inequality).

The key aspect of this problem is to find an algorithm that can add and expand candidate nodes in the priority queue in parallel. A large bottleneck in general graph search algorithms is that they have a large set of nodes on the search frontier that are waiting to be explored. Being able to schedule candidate nodes effiently would be important to this project.

### The Challenge
Implementating A* in parallel will rely on an efficient implementation of a Priority Queue. A*, like Dijkstra's algorithm, uses a priority queue to select the "best" next node to search. In this way, it reduces the number of nodes to search and makes the search process more efficient in general. 

Repeatedly adding and choosing the best element in a priority queue is generally implemented as a sequential task due to its dependencies. This is because when the child nodes of the best node is added to the priority queue, they could potentially change the values in or the order of the queue. We will need to find a way to allocate nodes to threads such that A*'s completeness and optimality properties still hold. These two aspects (dealing with dependencies and scheduling) are the two aspects that make this problem difficult.

### Resources
We will start by implementing a vanilla, serial implementation of A* and use that to compare with parallel versions of A*. We have found some approaches people have published previously to tackle this problem [1] and we hope to compare our implementation performance with theirs.

### Goals and Deliverables
Our 100% goal is to apply parallel A* to search applications, and measure speedup over a sequential A* implementation.

Our 125% goal is to expand our algorithmic domain to implement parallel Rapidly Random Trees (RRT), a widely used algorithm in Robotics [2].

Our 75% goal is to have complete working version of parallel A*, although it might not be up to similar performance as the paper in [1]. For this goal, we would be able to implement it in either GPU or multi-core CPU, but perhaps not both.

For demos, we could potentially visualize the A* search algorithm on various problems by showing the nodes that were expanded in the order that they were by different threads. We would be able to color code the nodes by the thread that expanded the node.

### Platform Choice
We will be using both GPU and multi-core CPU platforms for our project. We would like to use the Gates GHC machines as well as the PSC machines for larger runs on the cluster. These would be useful as we have used them in class for CUDA and OpenMP programming in class homeworks previously.

### Schedule

**Week** | **Goal**
--- | --- |
3/27 - 4/2 | Implement Serial A* and Parallel A* as in [1]
4/3 - 4/9 | Design CUDA version of Parallel A*
**4/11** | **Intermediate Checkpoint (Complete Milestone Report)**
4/10 - 4/16 | Create tangible search problem and apply A* to it
4/17 - 4/23 | Investigate optimizations and apply them to search problem
4/24 - 4/29 | Tabulate results and write up Final Report
**4/24 - 4/29** | **Final Report Due**
5/1 - 5/5 | Prepare for Poster Session
**5/5** | **Poster Session**

### Milestone

For the milestone, we've implemented a sequential A* with data structures reimplemented manually with the intention of easily porting them to CUDA code (_See Seq branch_). In particular, we've reimplemented unbounded arrays and heaps, all using sequential memory. We are currently in the process of creating a fast, shared hashmap implementation specific to parallel A* (for deduplication). This data structure is proving to be quite tricky, as it takes advantage of certain correctness attributes of A* in order to maximize parallelism (as described in [1]).

With respect to the original proposal, we are slightly behind but not terribly so. While we haven't written the CUDA version for parallel A*, we have moved optimizations up and have been focusing on that. As mentioned before, we are working on the hashmap data structure. Alternatively, we could use a hashmap with mutex locks per change, but we felt that it was worth prioritizing such an optimization as it would be a major bottleneck.

For the poster session, we plan to have a demo of the search, first sequentially then in parallel on a series of search problems, tentatively being path finding and puzzle solving.

As of right now, we've spent a significant amount of time trying to fully understand the hashmap data structure in the paper. While not a concern yet, it is a bottleneck in our progress currently.

### References

[1] https://yichaozhou.com/publication/1501massive/

[2] https://content.iospress.com/articles/integrated-computer-aided-engineering/ica190616
