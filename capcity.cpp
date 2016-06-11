#include <vector>
#include <iostream>

using namespace std;

// Global Variables
vector< vector<int> > *cityGraph, *reversedCityGraph;
bool* isVisited;
int* sccGroup;
int* TraversedNodes;
int* numOutgoingRoads;
int topStack = -1;

/* pre : reference to int, currNode, of current node
 * peri: Marks currNode as traversed and perform dfsOrigGraph on all chidNode based
 *       off cityGraph. Once all child nodes have been traversed with dfsOrigGraph,
 *       push currNode to end of vector of TraversedNodes. This algorithm ensures
 *       that all potential central nodes are at front of TraversedNodes while all
 *       "beginning" nodes are at end of vector and will be 
 * post: applies recursion to perform "DFS" on graph from currNode
 */
void dfsOrigGraph(int &currNode) {
    isVisited[currNode] = true;
    for (int childNode = 0; childNode < (*cityGraph)[currNode].size(); ++childNode) {
        if (!isVisited[(*cityGraph)[currNode][childNode]]) {
            dfsOrigGraph((*cityGraph)[currNode][childNode]);
        }
    }
    // DFS on entire graph
    // All end nodes will be at bottom of stack to ensure no false cylcic groups
    TraversedNodes[++topStack] = currNode;
}

/* pre : reference to int, currNode and currIndex, of current node and current
 *       cyclic group, respective
 * peri: Marks currNode as traversed and perform dfsRevGraph on all chidNode based
 *       off cityGraph. Once all child nodes have been traversed with dfsRevGraph.
 *       For all child nodes of currNodes, mark it as part of that cyclic group.
 *       Know this is true because the initial dfsOrigGraph marked all nodes
 *       connected from A->B.
 * post: applies recursion to perform "DFS" on reversed graph from currNodes and
 *       find all cyclic groups
 */
void dfsRevGraph(int &currNode, int *currIndex) {
    // Reversed DFS, mark as "visited"
    isVisited[currNode] = false;
    // Find strongly connected graphs from potential "center point"
    sccGroup[currNode] = *currIndex;
    for (int childNode = 0; childNode < (*reversedCityGraph)[currNode].size(); ++childNode) {
        if (isVisited[(*reversedCityGraph)[currNode][childNode]]) {
            dfsRevGraph((*reversedCityGraph)[currNode][childNode], currIndex);
        }
    }
}

/* pre : none
 * peri: deletes memory of each variabled pointing to memory allocated on heap
 * post: clears all memory allocated on heap
 */
void clearMemory() {
    delete cityGraph, reversedCityGraph;
    delete [] numOutgoingRoads, isVisited, sccGroup, TraversedNodes;
}

/* pre   : none
 * post  : runs the main method to solve CAPCITY of SPOJ
 * return: 1 if input not within respective range; otherwise, 0 if found number 
 *         of capital cities
 */
int main() {
    std::cout.sync_with_stdio(false);
    int numRoads, numCities, currNode;
    
    // Input for number of vectors and edges
    cin >> numCities >> numRoads;
    if ((numCities < 1 || numCities > 100000) || (numRoads < 1 || numRoads > 200000)) {
        cerr << "Error: N=[1:1000000] and M=[1:200000]";
        return 1;
    }

    // Initialize variables
    cityGraph = new vector< vector<int> >(numCities + 1);
    reversedCityGraph = new vector< vector<int> >(numCities + 1);
    TraversedNodes = new int[numCities];
    sccGroup = new int[numCities + 1];
    isVisited = new bool[numCities + 1];

    // Clear all vector and visited array to default
    // Make sure no nodes marked as visited and all vectors have any edges
    for (currNode = 0; currNode <= numCities; ++currNode) {
        isVisited[currNode] = false;
        (*cityGraph)[currNode].clear();
        (*reversedCityGraph)[currNode].clear();
    }

    // Input for unidirectional edges
    int cityA, cityB;
    for (currNode = 0; currNode < numRoads; ++currNode) {
        cin >> cityA >> cityB;
        // Create normal graph from A->B
        (*cityGraph)[cityA].push_back(cityB);
        // Create reversed graph from B->A
        (*reversedCityGraph)[cityB].push_back(cityA);
    }

    // Perform Kosajura's algorithm
    for (currNode = 1; currNode <= numCities; ++currNode) {
        if (!isVisited[currNode]) {
            dfsOrigGraph(currNode);
        }
    }

    // Find all cyclic groups
    int currIndex = 0;
    for (currNode = topStack; currNode >= 0; --currNode) {
        if (isVisited[TraversedNodes[currNode]]) {
            dfsRevGraph(TraversedNodes[currNode], &currIndex);
            ++currIndex;
        }
    }

    // Create counter see how many outgoing roads each cyclic graph has
    numOutgoingRoads = new int[currIndex];
    for (currNode = 0; currNode < currIndex; ++currNode) {
        numOutgoingRoads[currNode] = 0;
    }

    // Find number of outgoing arrows from each cyclic group
    for (currNode = 1; currNode <= numCities; ++currNode) {
        for (int edgedVector = 0; edgedVector != (*cityGraph)[currNode].size(); ++edgedVector) {
            // If current cyclic group has outgoing arrow to another cyclic graph 
            if (sccGroup[currNode] != sccGroup[(*cityGraph)[currNode][edgedVector]]) {
                ++numOutgoingRoads[sccGroup[currNode]];
            }
        }
    }

    // Find group that has no outgoing arrows
    // If num == 0, means it is target city
    // If mult. 0, return 0 because multiple cyclic groups with no outgoing arrows
    int sccGroupOfTarget = -1;
    for (currNode = 0; currNode < currIndex; ++currNode) {
        if (numOutgoingRoads[currNode] == 0) {
            // If already found another group that has no outgoing arrows
            if (sccGroupOfTarget != -1) {
                cout << "0\n";
                clearMemory();
                return 0;
            }
            sccGroupOfTarget = currNode;
        }
    }

    // Find all vectors in cyclic group
    vector<int> targetCities;
    for (currNode = 1; currNode <= numCities; ++currNode) {
        if (sccGroup[currNode] == sccGroupOfTarget) {
            targetCities.push_back(currNode);
        }
    }

    // Print out the capital cities
    cout << targetCities.size() << "\n";
    for (currNode = 0; currNode < targetCities.size(); ++currNode) {
        cout << targetCities[currNode] << " ";
    }
    
    // CLear all memory allocated on heap
    clearMemory();
    return 0;
}
