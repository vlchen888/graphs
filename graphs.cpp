//#include <bits/stdc++.h>

#include <iostream>
#include <vector>
#include <queue>    //std::priority_queue

using namespace std;

typedef vector<int> vi; 
typedef vector< vi > vvi; 
typedef pair<int, int> ii; 

typedef long long ll;

#define sz(a) int((a).size()) 

#define pb push_back

#define all(c) (c).begin(),(c).end()

#define tr(c, i) \
    for(typeof(c.begin()) i = c.begin();\
            i != c.end();\
            ++i)

#define present(c,x) ((c).find(x) != (c).end())

#define cpresent(c,x) (find(all(c),x) != (c).end())

#define min(x, y) (x < y ? x : y)

#define max(x, y) (x > y ? x : y)

template <class T>
T **APSP(T **edges, int N);

int max_flow(int **edges, int N, int s, int t);

int find_path(int **residual, int N, int s, int t);

void testAPSP();
void testMaxFlow();

int main() {
    testMaxFlow();
    return 0;
}

void testAPSP() {
    int N = 3;
    double test[3][3] = {{0, 1.1, 100}, {1000, 0, 3}, {2, 10000, 0}};
   
    double **edges = new double*[N];
    for(int i = 0; i < N; i++) {
        edges[i] = new double[N];
        for(int j = 0; j < N; j++) {
            edges[i][j] = test[i][j];
        }
    }
    double **sol = APSP(edges, N);
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            cout << sol[i][j] << " ";
        }
        cout << endl;
    }
    // clean memory
    for(int i = 0; i < N; i++) {
        delete[] edges[i];
        delete[] sol[i];
    }
    delete[] edges;
    delete[] sol;
}

void testMaxFlow() {
    int N = 4;
    int s = 0;
    int t = 1;
    int n = 10;

    int wt = (1 << n);

    int test[4][4] = {{0, wt-1, wt, 0}, {1, 0, 1, wt}, {0, 1, 0, wt-1}, {0, 0, 1, 0}};

    int ** edges = new int*[N];
    for(int i = 0; i < N; i++) {
        edges[i] = new int[N];
        for(int j = 0; j < N; j++) {
            edges[i][j] = test[i][j];
        }
    }

    int maxflow = max_flow(edges, N, 0, 3);
    cout << maxflow << endl;
    // clean memory
    for(int i = 0; i < N; i++) {
        delete[] edges[i];
    }
    delete[] edges;
}


/**
 * @brief Given an adjacency matrix, returns a matrix of
 * all shortest path distances.
 *
 * @param edges Edge adjacency matrix
 * @param N     Number of vertices
 */
template <class T>
T **APSP(T **edges, int N) {
    // Implemented Floyd-Warshall
    T **dists = new T*[N];
    for(int i = 0; i < N; i++) {
        dists[i] = new T[N];
    }
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            dists[i][j] = edges[i][j];
        }
    }

    
    for(int k = 0; k < N; k++) {
        for(int i = 0; i < N; i++) {
            for(int j = 0; j < N; j++) {
                if(dists[i][k] + dists[k][j] < dists[i][j]) {
                    dists[i][j] = dists[i][k] + dists[k][j];
                }
            }
        }
    }
    return dists;
}

/**
 * @brief Given a flow network, returns the max flow.
 *
 * @param edges Flow capacity adjacency matrix
 * @param N     Number of vertices
 * @param s     Source
 * @param t     Sink
 */
int max_flow(int **edges, int N, int s, int t) {
    int result = 0;
    int **residual = new int*[N];
    for(int i = 0; i < N; i++) {
        residual[i] = new int[N];
        for(int j = 0; j < N; j++) {
            residual[i][j] = edges[i][j];
        }
    }
    while(true) {
        int path_capacity = find_path(residual, N, s, t);
        //cout << "Aug path capacity: " << path_capacity << endl;
        if(path_capacity == 0) {
            break;
        }
        result += path_capacity;
    }

    // clean up memory
    for(int i = 0; i < N; i++) {
        delete[] residual[i];
    }
    delete[] residual;

    return result;
}

class FlowNode
{
public:
    int vertex;
    int priority;
    int from; // previous vertex on path
    FlowNode(int v, int p, int f) {
        vertex = v;
        priority = p;
        from = f;
    }
};

class FlowCompare
{
public:
    bool operator() (FlowNode a, FlowNode b) {
        return a.priority < b.priority;
    }
};

/**
 * @brief Given a residual network, finds an augmenting path,
 * returns the capcity, and updates the residual network.
 *
 * Finds the maximum capacity augmenting path.
 * @param residual  Adjacency matrix of residual network
 * @param N         Number of vertices
 * @param s     Source
 * @param t     Sink
 */
int find_path(int **residual, int N, int s, int t) {
    priority_queue<FlowNode, vector<FlowNode>, FlowCompare> pq;
    pq.push(FlowNode(s, (1 << 15) - 1, -1));
    int path_cap = 0;

    // initialize visited array
    bool *visited = new bool[N];
    for(int i = 0; i < N; i++) {
        visited[i] = false;
    }

    // initialize from array
    int *from = new int[N];
    for(int i = 0; i < N; i++) {
        from[i] = -1;
    }

    while(!pq.empty()) {
        FlowNode aux = pq.top();
        pq.pop();
        int where = aux.vertex;
        int cost = aux.priority;
        if(visited[where]) {
            continue;
        }
        visited[where] = true;
        from[where] = aux.from;

        if(where == t) {
            path_cap = cost;
            break;
        }
        for(int next = 0; next < N; next++) {
            if(residual[where][next] > 0) {
                int new_cost = min(cost, residual[where][next]);
                pq.push(FlowNode(next, new_cost, where));
            }
        }
    }
    // Update residual network
    int where = t;
    while(from[where] > -1) {
        int prev = from[where];
        residual[prev][where] -= path_cap;
        residual[where][prev] += path_cap;
        where = prev;
    }
    delete[] visited;
    delete[] from;
    return path_cap;
}




