/*main.cpp*/

//
// Program to input Nodes (positions), Buildings and Footways
// from an Open Street Map file.
// 
// Prof. Joe Hummel
// Northwestern University
// CS 211: Winter 2023
// 

#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include <stack>

#include "building.h"
#include "buildings.h"
#include "footway.h"
#include "footways.h"
#include "dist.h"
#include "graph.h"
#include "node.h"
#include "nodes.h"
#include "osm.h"
#include "tinyxml2.h"
#include "util.h"

using namespace std;
using namespace tinyxml2;

long long nearestFootway(pair<double, double> Bcoord, Nodes& nodes, Footways& footways)
{
  Footway startF = footways.MapFootways[0];     // closest footway id
  long long N = 0;                              // closest node id of footway
  double INF = numeric_limits<double>::max();
  double min = INF;
  for (Footway footway : footways.MapFootways)
  {
    for (long long nodeid : footway.NodeIDs)
    {
      double nodeLat, nodeLon;
      bool entrance;
      bool node = nodes.find(nodeid, nodeLat, nodeLon, entrance);
      double test = distBetween2Points(Bcoord.first, Bcoord.second, nodeLat, nodeLon);
      if (test < min)
      {
        min = test;
        N = nodeid;
        startF = footway;
      }
    }
  }
  cout << "  Closest footway ID " << startF.ID << ", node ID " << N << ", distance "
    << min << endl;
  return N;
}

void navigate(Buildings& buildings, Nodes& nodes, Footways& footways, graph& G)
{
  // get start building
  cout << "Enter start building name (partial or complete)> " << endl;
  string name;
  getline(cin, name);

  // find start building and output info
  Building startB = Building(0, "", "");
  for (Building& B: buildings.MapBuildings)
  {
    if (B.Name.find(name) != string::npos) // contains name:
    {
      startB = B;
      break;
    }
  }

  if (startB.ID == 0) // start building not found
  {
    cout << "**Start building not found" << endl;
    return;
  }
  else  // start building found
  {
    cout << "  Name: " << startB.Name << endl;
    pair<double, double> start_coord = startB.getLocation(nodes);
    cout << "  Approximate location: (" << start_coord.first << ", " << 
      start_coord.second << ")" << endl;
    long long startN = nearestFootway(start_coord, nodes, footways);


    // get destination building
    cout << "Enter destination building name (partial or complete)> " << endl;
    getline(cin, name);

    // find destination building and output info
    Building endB = Building(0, "", "");
    for (Building& B: buildings.MapBuildings)
    {
      if (B.Name.find(name) != string::npos) // contains name:
      {
        endB = B;
        break;
      }
    }

    if (endB.ID == 0) // destination building not found
    {
      cout << "**Destination building not found" << endl;
      return;
    }
    else  // destination building found
    {
      cout << "  Name: " << endB.Name << endl;
      pair<double, double> end_coord = endB.getLocation(nodes);
      cout << "  Approximate location: (" << end_coord.first << ", " << 
        end_coord.second << ")" << endl;
      long long endN = nearestFootway(end_coord, nodes, footways);


      vector <long long> visited;
      map<long long, double> distances;
      map <long long, long long> predecessors;
      visited = Dijkstra(G, startN, distances, predecessors);
      
      cout << "Shortest weighted path:" << endl;

      if (distances[endN] == INF)
      {
        cout << "**Sorry, destination unreachable" << endl;
      }
      else
      {
        cout << "  # nodes visited: " << visited.size() << endl;
        cout << "  Distance: " << distances[endN] << " miles" << endl;

        stack<long long> c;
        long long curr = endN;
        
        while(true)
        {
          if (curr == 0) break;
          c.push(curr);
          curr = predecessors[curr];
        }
        cout << "  Path: " << c.top();
        c.pop();
        while (!c.empty())
        {
          cout << "->" << c.top();
          c.pop();
        }
        cout << endl;
      }
    }
  }


}

//
// main
//
int main()
{
  cout << setprecision(12);
  XMLDocument xmldoc;
  Nodes nodes;
  Buildings buildings;
  Footways footways;
  graph G;
  
  cout << "** NU open street map **" << endl;

  string filename;

  cout << endl;
  cout << "Enter map filename> " << endl;
  getline(cin, filename);

  //
  // 1. load XML-based map file 
  //
  if (!osmLoadMapFile(filename, xmldoc))
  {
    // failed, error message already output
    return 0;
  }
  
  //
  // 2. read the nodes, which are the various known positions on the map:
  //
  nodes.readMapNodes(xmldoc);

  //
  // NOTE: let's sort so we can use binary search when we need 
  // to lookup nodes.
  //
  nodes.sortByID();

  //
  // 3. read the university buildings:
  //
  buildings.readMapBuildings(xmldoc);

  //
  // 4. read the footways, which are the walking paths:
  //
  footways.readMapFootways(xmldoc);

  // build graph
  buildGraph(nodes, footways, G);

  //
  // 5. stats
  //
  cout << "# of nodes: " << nodes.getNumMapNodes() << endl;
  cout << "# of buildings: " << buildings.getNumMapBuildings() << endl;
  cout << "# of footways: " << footways.getNumMapFootways() << endl;
  cout << "# of graph vertices: " << G.NumVertices() << endl;
  cout << "# of graph edges: " << G.NumEdges() << endl;

  //
  // now let the user for search for 1 or more buildings:
  //
  while (true)
  {
    string name;

    cout << endl;
    cout << "Enter building name, * to list, @ to navigate, or $ to end> " << endl;

    getline(cin, name);

    if (name == "$") {
      break;
    }
    else if (name == "*") {
      buildings.print();
    }
    else if (name == "@")
    {
      navigate(buildings, nodes, footways, G);
    }
    else if (name == "!") {
      sanity(G);
    }
    else {
      buildings.findAndPrint(name, nodes, footways);
    }

  }//while

  //
  // done:
  //
  cout << endl;
  cout << "** Done  **" << endl;
  cout << "# of calls to getID(): " << Node::getCallsToGetID() << endl;
  cout << "# of Nodes created: " << Node::getCreated() << endl;
  cout << "# of Nodes copied: " << Node::getCopied() << endl;
  cout << endl;

  return 0;
}
