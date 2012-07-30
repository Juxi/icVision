#ifndef _ASTAR_H_
#define _ASTAR_H_


#include <boost/graph/astar_search.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/random.hpp>
#include <boost/random.hpp>
#include <iostream>



template <class Graph, class CostType>
  class distance_heuristic : public boost::astar_heuristic<Graph, CostType>
  {
  public:
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
  distance_heuristic(Graph &g, Vertex goal)
	: m_graph(g), m_goal(goal) {}

    CostType operator()(Vertex u)
    {
	  Vertex &from(u);
	  Vertex &to(m_goal);
	  std::vector<double>::iterator it(m_graph[from].q.begin()), it_end(m_graph[from].q.end());
	  std::vector<double>::iterator it2(m_graph[to].q.begin());
	  CostType cost(0);
	  for (; it != it_end; ++it, ++it2)
		cost += (*it - *it2) * (*it - *it2);
	  return cost;
    }

  private:
    Graph &m_graph;
    Vertex m_goal;
  };


struct found_goal {}; // exception for termination

// visitor that terminates when we find the goal
template <class Vertex, class Edge>
  class astar_goal_visitor : public boost::default_astar_visitor
  {
  public:
  astar_goal_visitor(Vertex goal) : m_goal(goal) {}
	
	template <class Graph>
	  void examine_vertex(Vertex u, Graph& g) {
	  if(u == m_goal)
		throw found_goal();
	}

	template <class Graph>
	  void examine_edge(Edge e, Graph& g) {
	  std::cout << "e " << e << std::endl;
	}

  private:
	Vertex m_goal;
  };




#endif
