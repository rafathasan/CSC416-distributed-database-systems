# CSC 416: Distributed Database Systems

### Project Topic: An LSM Tree based key-value store
LSM trees maintain key-value pairs and store data in two or more separate structures, each of which is optimized for its respective underlying storage medium. Data is synchronized between the two structures efficiently, in batches.

**Files**

 - lsmdef.h - contains necessary prototypes, type definition and data structures.
 - lsmdef.cpp - the lsm core implementation.
 - lsmtree.cpp - the interactive shell implementation.
 
 **Brief Explaination**
 
 A non-balancing AVL Tree is used for the LSM Tree with a priority queue and a bloom filter. Priority queue pop out the most infrequent node from the LSM Tree and it fills the filling buffer. When the filling buffer gets full, it sorts the node and writes to the disk.
 
Bloom Filter: delete, update and find check for bloom filter status. And the insert updates the bloom filter.

Priority queue(emptying buffer): It updates each node priority on update, find and insert operations.

Deleted Node: A Deleted node is marked as deleted but not removed from the tree. A deleted node is removed when it reached zero
priority.

### Description
A detailed study of advanced topics on relational database theory, query processing and optimization, recovery techniques, concurrency control, data fragmentation, replication, security and integrity of distributed database systems. Discussion on heterogeneous databases and paradigms (deductive, object oriented) used in distributed systems.

### Prerequisites
- CSC 306 + Lab : Algorithms
- CSC 401 + Lab : Database Management

### Expected Outcome(s)
- Analyze real-word data management scenarios and design appropriate data schemas.
- Formulate SQL queries to access a relational database system.
- Design and develop data management applications using relational database systems.
- Analyze and apply query optimization techniques for optimizing query.
- Analyze and apply data indexing techniques for speeding up query processing.
- Analyze and apply transaction processing techniques for executing transactions in relational database systems

Suggested Books:
- [Principles of Distributed Database Systems by M. Tamer Özsu and Patrick Valduriez](https://www.amazon.com/Principles-Distributed-Database-Systems-Tamer/dp/1441988335)
- [Distributed Database Management Systems: A Practical Approach by Saeed K. Rahimi and Frank S. Haug](https://www.amazon.com/Distributed-Database-Management-Systems-Practical/dp/047040745X)
- [Distributed Systems: Principles and Paradigms by Andrew S. Tanenbaum and Maarten Van Steen](https://www.amazon.com/Distributed-Systems/dp/1292025522/ref=dp_ob_title_bk)
