/*util.h*/

// includes the buildGraph function and ! helper function from Part 1
// used to make code cleaner

#include <string>
#include <vector>
#include <limits>

#include "graph.h"
#include "nodes.h"
#include "footways.h"

using namespace std;

// buildGraph
// builds the graph of the Northwestern University campus
void buildGraph(Nodes& nodes, Footways& footways, graph& G);

// sanity
// sanity check to make sure graph is working
void sanity(graph& G);

// dijkstra
// employs disjkstra's algorithem to find shortest weighted
// path between a start building and destination building
vector<long long> Dijkstra(graph&G, long long startV, map<long long,double>& distances,
							map<long long, long long>& predecessors);

const double INF = numeric_limits <double>::max();