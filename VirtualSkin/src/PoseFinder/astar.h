#ifndef _ASTAR_H_
#define _ASTAR_H_


#include <boost/graph/astar_search.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/random.hpp>
#include <boost/random.hpp>
#include <iostream>

#include "treemode.h"


template <class Graph, class CostType>
  class distance_heuristic : public boost::astar_heuristic<Graph, CostType>
  {
  public:
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
  distance_heuristic(Graph &g, Vertex goal, TreeMode distance_mode)
	: m_graph(g), m_goal(goal), d_distance_mode(distance_mode) {}

    std::vector<double> &choose_vector(Vertex &u, TreeMode distance_mode) {
      switch (distance_mode) {
      case CONFIGURATIONSPACE:
        return m_graph[u].q;
        break;
      case SCALEDCONFIGURATIONSPACE:
        return m_graph[u].qs;
        break;
      case WORKSPACE:
        return m_graph[u].x;
        break;
      }
    }
    
    CostType operator()(Vertex u)
    {
	  Vertex &from(u);
	  Vertex &to(m_goal);
	  //std::vector<double>::iterator it(m_graph[from].q.begin()), it_end(m_graph[from].q.end());
	  //std::vector<double>::iterator it2(m_graph[to].q.begin());
	  std::vector<double>::iterator it(m_graph[from].qs.begin()), it_end(m_graph[from].qs.end());
	  std::vector<double>::iterator it2(m_graph[to].qs.begin());
	  //std::vector<double>::iterator it(m_graph[from].x.begin()), it_end(m_graph[from].x.end());
	  //std::vector<double>::iterator it2(m_graph[to].x.begin());
	  CostType cost(0.0);
	  for (; it != it_end; ++it, ++it2)
		cost += (*it - *it2) * (*it - *it2);
	  return sqrt(cost);
    }

  private:
    Graph &m_graph;
    Vertex m_goal;
    TreeMode d_distance_mode;
  };
    

struct found_goal {}; // exception for termination

template <typename Edge>
class EdgeTester {
 public:
  virtual void operator()(Edge &edge) = 0;
};

// visitor that terminates when we find the goal
template <class Vertex, class Edge>
  class astar_goal_visitor : public boost::default_astar_visitor
  {
  public:
  astar_goal_visitor(Vertex goal, EdgeTester<Edge> &edge_tester) : m_goal(goal), d_edge_tester(edge_tester) {}
	
	template <class Graph>
	  void examine_vertex(Vertex u, Graph& g) {
	  //std::cout << "examining vertex " << u << " " << m_goal << std::endl;
	  if(u == m_goal)
		throw found_goal();
	}

	template <class Graph>
	  void examine_edge(Edge e, Graph& g) {
	  d_edge_tester(e);
	  //std::cout << "e " << e << std::endl;
	}

  private:
	Vertex m_goal;
	EdgeTester<Edge> &d_edge_tester;
  };




#endif
