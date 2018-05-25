#include <iostream>
#include <string>
#include <fstream>
#include <regex>

#include "graph.hpp"

#define WEIGHT_MAX 10000

void affiche(Graph<std::string,int>& g)
{
    std::string lien = " -- ";

    for (auto vi = g.vertex_iterator(); vi.has_next(); vi.next())
    {
        auto curr = vi.current();
        std::cout << "vertex [" << curr->get_value() << "]" << std::endl;

        for (auto ni = curr->neighbor_iterator(); ni.has_next(); ni.next())
        {
            auto neighbor_edge = ni.current();
            auto neighbor = neighbor_edge->get_destination(curr);
            std::cout << "  edge " << curr->get_value() << lien << neighbor->get_value() << " [" << neighbor_edge->get_weight() << "] " << std::endl;
        }
    }
}

Graph<std::string, int> graph_from_file(std::string filename)
{
    std::function<int(int)> max = [](int i) { return WEIGHT_MAX; };
    std::function<int(int)> min = [](int i) { return 0; };
    std::function<int(int)> priority = [](int i) { return i; };

    Graph<std::string, int> g(false, max, min, priority);

    std::ifstream file;
    std::set<std::string> set_firstnames;


    file.open(filename);
    if(file.is_open())
    {
        std::string line;

        while(std::getline(file, line))
        {
            std::regex re("(\\w+) -- (\\w+) \\[label=(\\d+)\\]");
            std::smatch match;

            if (std::regex_search(line, match, re) && match.size() > 1)
            {
                std::string first_name = match.str(1);
                Graph<std::string, int>::Vertex* vertex1 = g.get_vertex_data(first_name);

                if(vertex1 == nullptr)
                {
                    vertex1 = g.add_vertex(first_name);
                }

                std::string second_name = match.str(2);
                Graph<std::string, int>::Vertex* vertex2 = g.get_vertex_data(second_name);

                if(vertex2 == nullptr)
                {
                    vertex2 = g.add_vertex(second_name);
                }

                int weight = std::stoi(match.str(3));
                g.add_edge(weight, vertex1, vertex2);
            }
        }

        file.close();
    }
    else
    {
        std::cout << "file could not be opened" << std::endl;
    }


    return g;
}

void minimum_spanning_tree(Graph<std::string, int> g)
{
    for(auto i = g.min_spanning_tree_iterator(); i.has_next(); i.next())
    {
        i.current()->mark_red();
    }
    const std::string filename = "min_spanning_tree.dot";

    g.save_graph(filename);
    std::cout << "minimum spanning tree printed in " << filename << std::endl;
}

void shortest_path_from_all(Graph<std::string, int> g)
{
    Graph<std::string, int>::Vertex* end = g.vertex_iterator().current();

    for(auto itVertices = g.vertex_iterator(); itVertices.has_next(); itVertices.next())
    {
        auto start = itVertices.current();

        if(start->get_value() != end->get_value())
        {
            for(auto itSPI = g.shortest_path_iterator(start, end); itSPI.has_next(); itSPI.next())
            {
                itSPI.current()->mark_red();
            }
        }

        const std::string filename = "shortest_path_from_"+ start->get_value() +"_to_"+end->get_value()+".dot";

        g.save_graph(filename);

        std::cout << "path from " << start->get_value() << " to " << end->get_value() << " printed in " << filename << std::endl;

        for(auto itEdge = g.edge_iterator(); itEdge.has_next(); itEdge.next())
        {
            itEdge.current()->unmark_red();
        }
    }
}

void shortest_path_pivot(Graph<std::string, int> g)
{
    int sumWeight = WEIGHT_MAX;
    Graph<std::string, int>::Vertex* v;

    for(auto itVertex = g.vertex_iterator(); itVertex.has_next(); itVertex.next())
    {
        int sumEdgesCurrentVertex = 0;

        for(auto itSp = g.shortest_path_tree_iterator(itVertex.current()); itSp.has_next(); itSp.next())
        {
            sumEdgesCurrentVertex += itSp.current()->get_weight();
        }

        if(sumEdgesCurrentVertex < sumWeight)
        {
            v = itVertex.current();
            sumWeight = sumEdgesCurrentVertex;
        }
    }

    for(auto itSp = g.shortest_path_tree_iterator(v); itSp.has_next(); itSp.next())
    {
        itSp.current()->mark_red();
    }

    const std::string filename = "pivot.dot";

    g.save_graph(filename, "\tlabelloc=\"t\";labeljust=\"l\";label=\"totalweight="+std::to_string(sumWeight)+"\"");

    std::cout << "pivot printed in " << filename << std::endl;
}

int main()
{
    Graph<std::string, int> g3 = graph_from_file("test.dot");

    std::cout << std::endl << "---------------------- minimum spanning tree -----------------" << std::endl;

    minimum_spanning_tree(g3);

     std::cout << std::endl << "---------------------- shortest path -----------------" << std::endl;

    shortest_path_from_all(g3);

     std::cout << std::endl << "---------------------- shortest path pivot -----------------" << std::endl;

    shortest_path_pivot(g3);
}
