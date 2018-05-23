#include <iostream>
#include <string>
#include <fstream>
#include <regex>

#include "graph3.hpp"

void rempli(Graph<int,int>& g)
{
    std::vector<Graph<int,int>::Vertex*> vt;
    std::vector<Graph<int,int>::Edge*> et;

    for (int i=0; i<6; i++)
        vt.push_back(g.add_vertex(i));

    int src[] = {0, 0, 1, 2, 3, 4, -1};
    int dst[] = {1, 2, 3, 4, 5, 5};
    for (int i=0; src[i] >= 0; i++) {
        g.add_edge(i+1, vt[src[i]], vt[dst[i]]);
    }
}

void affiche(Graph<std::string,int>& g)
{
    std::string lien = g.is_directed() ? " -> " : " -- ";
    for (auto vi = g.vertex_iterator(); vi.has_next(); vi.next()) {
        auto curr = vi.current();
        std::cout << "vertex [" << curr->get_value() << "]" << std::endl;
        for (auto ni = curr->neighbor_iterator(); ni.has_next(); ni.next()) {
            auto neighbor_edge = ni.current();
            auto neighbor = neighbor_edge->get_destination(curr);
            std::cout << "  edge " << curr->get_value() << lien << neighbor->get_value() << " [" << neighbor_edge->get_weight() << "] " << std::endl;
        }
    }
}

void visite_profondeur(Graph<std::string,int>& g)
{
    auto v = g.get_vertex(0);

    for (auto i = g.depth_first_iterator(v); i.has_next(); i.next()) {
        std::cout << "visit " << i.current()->get_value() << std::endl;
    }
}

void visite_largeur(Graph<std::string,int>& g)
{
    auto v = g.get_vertex(0);
    for (auto i = g.breadth_first_iterator(v); i.has_next(); i.next()) {
        std::cout << "visit " << i.current()->get_value() << std::endl;
    }
}

void tous_tests(std::string name, Graph<std::string,int>& g)
{
    std::cout << name << " **********\n";
    affiche(g);
    std::cout << "prof " << name << " **********\n";
    visite_profondeur(g);
    std::cout << "larg " << name << " **********\n";
    visite_largeur(g);
}


Graph<std::string, int> graph_from_file(std::string filename)
{
    std::function<int(int)> max = [](int i) { return 10000; };
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

    g.save_graph("min_spanning_tree.dot");
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

        g.save_graph("shortest_path_from_"+ start->get_value() +"_to_"+end->get_value()+".dot");

        for(auto itEdge = g.edge_iterator(); itEdge.has_next(); itEdge.next())
        {
            itEdge.current()->unmark_red();
        }
    }
}

int main()
{
    Graph<std::string, int> g3 = graph_from_file("test.dot");
    affiche(g3);

    minimum_spanning_tree(g3);

    shortest_path_from_all(g3);

}
