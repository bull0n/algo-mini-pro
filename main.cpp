#include <iostream>
#include <string>
#include <fstream>
#include <regex>

#include "graph3.hpp"

void rempli(Graph<int,int>& g)
{
    std::vector<Graph<int,int>::Vertex*> vt;
    std::vector<Graph<int,int>::Edge*> et;

    for (int i=10; i<=50; i+=10)
    {
        vt.push_back(g.add_vertex(i));
    }

    int src[] = {0, 0, 1, 1, 2, 2, 3, -1};
    int dst[] = {1, 2, 2, 3, 3, 4, 4};

    for (int i=0; src[i] >= 0; i++)
    {
        g.add_edge(i+1, vt[src[i]], vt[dst[i]]);
    }
}

void affiche(Graph<int,int>& g)
{
    std::string lien = g.is_directed() ? " -> " : " -- ";

    for (Graph<int,int>::VertexIterator vi = g.vertex_iterator(); vi.has_next(); vi.next())
    {
        Graph<int,int>::Vertex* curr = vi.current();
        std::cout << "vertex [" << curr->get_value() << "]" << std::endl;

        for (Graph<int,int>::Vertex::NeighborEdgeIterator ni = curr->neighbor_iterator(); ni.has_next(); ni.next())
        {
            Graph<int,int>::Edge* neighbor_edge = ni.current();
            Graph<int,int>::Vertex* neighbor = neighbor_edge->get_destination(curr);
            std::cout << "  edge " << curr->get_value() << lien << neighbor->get_value() << " [" << neighbor_edge->get_weight() << "] " << std::endl;
        }
    }
}

void visite_profondeur(Graph<int,int>& g)
{
    Graph<int,int>::Vertex* v = g.get_vertex(0);

    for (Graph<int,int>::DepthFirstIterator dfi = g.depth_first_iterator(v); dfi.has_next(); dfi.next())
    {
        std::cout << "visit " << dfi.current()->get_value() << std::endl;
    }
}

Graph<std::string, int> graph_from_file(std::string filename)
{
    Graph<std::string,int> g(false);

    std::ifstream file;
    std::set<std::string> set_firstnames;

    try
    {
        file.open(filename);
        std::string line;

        const int NUM_EMPTY_LINES = 2;

        for(int i = 0; i < NUM_EMPTY_LINES; i++)
        {
            std::getline(file, line);
        }

        while(std::getline(file, line))
        {
            std::regex re("(\\w+) -- (\\w+) \\[label=(\\d+)\\]");
            std::smatch match;
            std::string result;

            if (std::regex_search(line, match, re) && match.size() > 1)
            {
                result = match.str(1);
                std::cout << result << std::endl;
                result = match.str(2);
                std::cout << result << std::endl;
                result = match.str(3);
                std::cout << result << std::endl;
            }
            else
            {
                result = std::string("");
            }

        }

        file.close();
    }
    catch (std::ios_base::failure& fail)
    {
        std::cout << fail.what() << std::endl;
    }

    return g;
}

int main()
{
//    Graph<int,int> g1(true);
//    rempli(g1);
//    std::cout << "G1 **********\n";
//    affiche(g1);
//    std::cout << "dpi G1 **********\n";
//    visite_profondeur(g1);

//    Graph<int,int> g2(false);
//    rempli(g2);
//    std::cout << "G2 **********\n";
//    affiche(g2);
//    std::cout << "dpi G2 **********\n";
//    visite_profondeur(g2);

    graph_from_file("test.dot");
}