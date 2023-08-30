/*util.cpp*/

// includes the buildGraph function and ! helper function from Part 1
// used to make code cleaner

#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <functional>
#include <utility>
#include <queue>

#include "util.h"
#include "dist.h"
#include "graph.h"
#include "nodes.h"
#include "footways.h"

using namespace std;

// buildGraph
// builds the graph of the Northwestern University campus
void buildGraph(Nodes& nodes, Footways& footways, graph& G)
{
  for (auto& pair: nodes)
  {
    G.addVertex(pair.first);
  }

  for (Footway& footway : footways.MapFootways)
  {
    int num_nodes = footway.NodeIDs.size();
    for (int i = 0; i < num_nodes - 1; i++)
    {
      // define variables needed to get retrieve distance
      double c_lat, c_lon, n_lat, n_lon;
      bool n;
      bool curr_node = nodes.find(footway.NodeIDs[i], c_lat, c_lon, n);
      bool next_node = nodes.find(footway.NodeIDs[i+1], n_lat, n_lon, n);

      double distance = distBetween2Points(c_lat, c_lon, n_lat, n_lon);
      G.addEdge(footway.NodeIDs[i], footway.NodeIDs[i+1], distance);
    }

    for (int i = num_nodes - 1; i > 0; i--)
    {
      // define variables needed to get retrieve distance
      double c_lat, c_lon, n_lat, n_lon;
      bool n;
      bool curr_node = nodes.find(footway.NodeIDs[i], c_lat, c_lon, n);
      bool next_node = nodes.find(footway.NodeIDs[i-1], n_lat, n_lon, n);

      double distance = distBetween2Points(c_lat, c_lon, n_lat, n_lon);
      G.addEdge(footway.NodeIDs[i], footway.NodeIDs[i-1], distance);
    }
  }
}

// sanity
// sanity check to make sure graph is working
void sanity(graph& G)
{
	cout << "Graph check of Footway ID 986532630" << endl;
	// for (long long id : footway.nodeIDs)
	double weight;
	if (G.getWeight(9119071425, 533996671, weight))
	cout << "  Edge:  (9119071425, 533996671, " << weight << ")" << endl;

	if (G.getWeight(533996671, 9119071425, weight))
	cout << "  Edge:  (533996671, 9119071425, " << weight << ")" << endl;

	if (G.getWeight(533996671, 533996672, weight))
	cout << "  Edge:  (533996671, 533996672, " << weight << ")" << endl;

	if (G.getWeight(533996672, 533996671, weight))
	cout << "  Edge:  (533996672, 533996671, " << weight << ")" << endl;

	if (G.getWeight(533996672, 2240260064, weight))
	cout << "  Edge:  (533996672, 2240260064, " << weight << ")" << endl;

	if (G.getWeight(2240260064, 533996672, weight))
	cout << "  Edge:  (2240260064, 533996672, " << weight << ")" << endl;

}

struct orderer {
  bool operator()(const pair<long long, double>& p1, const pair<long long, double>& p2) const
    {
      if (p1.second > p2.second)
          return true;
      else if (p1.second < p2.second)
          return false;
      else { // if two values are the same :
          return true;
      }
    }
};

// dijkstra
// employs disjkstra's algorithem to find shortest weighted
// path between a start building and destination building

// return void, have map called visited
vector<long long> Dijkstra(graph&G, long long startV, map<long long,double>& distances,
                            map<long long, long long>& predecessors)
{
  vector<long long> visited;     // returns this
  priority_queue<pair<long long, double>, vector<pair<long long, double>>, orderer> unvisitedQ;

  // initialize distances and predecessor array
  for (long long currentV : G.getVertices())
  {
    distances[currentV] = INF;
    predecessors[currentV] = 0;
    unvisitedQ.push(make_pair(currentV, distances[currentV]));
  }
  distances[startV] = 0.0;
  unvisitedQ.push(make_pair(startV,distances[startV]));

  // goes through entire unvisited Queue
  while (!unvisitedQ.empty())
  {
    long long currentV = unvisitedQ.top().first;
    unvisitedQ.pop();

    if (distances[currentV] == INF)
      break;
    else if (std::find(visited.begin(), visited.end(), currentV) != visited.end())
      continue;
    else
      visited.push_back(currentV); // add to visited
    
    set<long long> neighbors = G.neighbors(currentV);
    for (long long adjV : neighbors) {
      double edgeWeight;
      G.getWeight(currentV, adjV, edgeWeight);
      double alternativePathDistance = distances[currentV] + edgeWeight;
      
      // replace distance because smaller
      if(alternativePathDistance < distances[adjV]) 
      {
        distances[adjV] = alternativePathDistance;
        unvisitedQ.push(make_pair(adjV, alternativePathDistance));
        predecessors[adjV] = currentV;
      }
    }
  }
  return visited;
}