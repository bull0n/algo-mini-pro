#pragma once

#include <vector>
#include <set>
#include <queue>
#include <stack>
#include <string>
#include <iostream>
#include <fstream>
#include <functional>


template<class VDATA, class EDATA>
class Graph
{

public:
    template<class T>
    class Iterator
    {

    public:
        virtual bool has_next() = 0;
        virtual void next() = 0;
        virtual T current() = 0;
    };

    template<class ELEMP>
    class ArrayIterator : public Iterator<ELEMP>
    {

    private:
        unsigned pos;
        std::vector<ELEMP>* array;

    public:
        ArrayIterator(std::vector<ELEMP>* p) { pos = 0; this->array = p; }
        bool has_next() { return pos < array->size(); }
        void next() { pos ++; }
        ELEMP current() { return has_next() ? (*array)[pos] : 0; }
    };

    template<class ELEMP>
    class QueueIterator : public Iterator<ELEMP>
    {

    protected:
        struct Token
        {
            unsigned priority;
            ELEMP elem;
            Token(unsigned p, ELEMP e) {priority = p; elem = e;}
            bool operator<(const Token &o) const { return priority > o.priority; }
        };
        std::priority_queue<Token> queue;

    public:
        bool has_next() { return queue.size() > 0; }
        void next() { queue.pop(); }
        ELEMP current() { return has_next() ? queue.top().elem : 0; }
        void push(unsigned prio, ELEMP elem) { queue.push(Token(prio, elem)); }
    };

    class Edge;

    class Vertex {
    private:
        VDATA value;
        unsigned index;
        std::vector<Edge*> edges;
        unsigned mark;

    public:
        Vertex(VDATA value, unsigned index) { this->value = value; this->index = index; }
        unsigned get_degree() { return edges.size(); }
        unsigned get_index() { return index; }
        VDATA& get_value() { return value; }
        void set_value(VDATA v) { value = v; }
        void add_neighbor(Edge* neighbor) { edges.push_back(neighbor); }
        Edge* get_edge(unsigned i) { return edges[i]; }
        ArrayIterator<Edge*> neighbor_iterator() { return ArrayIterator<Edge*>(&edges); }

        Edge* find_edge(Vertex* destination)
        {
            for (unsigned int i=0; i<edges.size(); i++)
            {
                Vertex* dest = edges[i]->get_destination(this);
                if (dest == destination)
                {
                    return edges[i];
                }
            }
            return 0;
        }
    };

    class Edge
    {

    private:
        EDATA weight;
        Vertex* source;
        Vertex* destination;
        bool directed;
        unsigned int index;
        bool red;

    public:
        Edge(EDATA weight, Vertex* source, Vertex* destination, unsigned index, bool directed = false)
        {
            this->weight = weight;
            this->source = source;
            this->destination = destination;
            this->directed = directed;
            this->index = index;
            this->red = false;
        }

        EDATA get_weight() { return weight; }
        void set_weight(EDATA w) { weight = w; }
        unsigned get_index() { return index; }
        Vertex* get_source() { return source; }
        Vertex* get_destination() { return destination; }

        Vertex* get_destination(Vertex* source)
        {
            if (source == this->source)
                return destination;
            if (!directed && source == this->destination)
                return this->source;
            return 0;
        }

        void mark_red()
        {
            this->red = true;
        }

        void unmark_red()
        {
            this->red = false;
        }

        bool is_red()
        {
            return this->red;
        }
    };

    void save_graph(std::string filename, std::string label = "")
    {
        std::ofstream file(filename);

        file << "graph { " << std::endl;
        file << label << std::endl;

        for(unsigned int i = 0; i < this->edges.size(); i++)
        {
            file << "\t" << this->edges[i]->get_source()->get_value()
                 << " -- " << this->edges[i]->get_destination()->get_value()
                 << " [label=" << this->edges[i]->get_weight();

            if(this->edges[i]->is_red())
            {
                file << ", penwidth=3, color=\"red\"";
            }

            file << "]" << std::endl;
        }

        file << "}" << std::endl;

        file.close();
    }

    Vertex* get_vertex_data(VDATA data)
    {
        for(unsigned int i = 0; i < this->vertices.size(); i++)
        {
            if(vertices[i]->get_value() == data)
            {
                return vertices[i];
            }
        }

        return nullptr;
    }

private:

    std::function<int(EDATA&)> maxWeight;
    std::function<int(EDATA&)> minWeight;
    std::function<int(EDATA&)> priorityWeight;

    class DepthFirstIterator : public QueueIterator<Vertex*>
    {

    private:
        std::set<unsigned> visited;
        unsigned counter;

        void visit(Vertex* v)
        {
            unsigned index = v->get_index();
            if (visited.count(index) > 0)
            {
                return;
            }
            this->push(counter ++, v);
            visited.insert(index);

            for (auto ne = v->neighbor_iterator(); ne.has_next(); ne.next())
            {
                Edge* edge = ne.current();
                Vertex* neighbor = edge->get_destination(v);
                visit(neighbor);
            }
        }

    public:
        DepthFirstIterator(Vertex* start) { counter = 0; visit(start); }
    };

    class BreadthFirstIterator : public QueueIterator<Vertex*>
    {

    private:
        std::set<unsigned> visited;

    public:
        BreadthFirstIterator(Vertex* start) {
            this->push(0, start);
            visited.insert(start->get_index());
        }

        void next()
        {
            if (this->queue.size() == 0)
            {
                return;
            }

            auto token = this->queue.top();
            Vertex* v = token.elem;
            this->queue.pop();

            unsigned depth = token.priority + 1;

            for (auto ne = v->neighbor_iterator(); ne.has_next(); ne.next())
            {
                Edge* edge = ne.current();
                Vertex* neighbor = edge->get_destination(v);
                unsigned index = neighbor->get_index();
                if (visited.count(index) == 0)
                {
                    this->push(depth, neighbor);
                    visited.insert(index);
                }

            }
        }
    };

    class EdgeListIterator : public ArrayIterator<Edge*>
    {

    private:
        std::vector<Edge*> sarray;

    public:
        EdgeListIterator() : ArrayIterator<Edge*>(&sarray){ }

        EdgeListIterator(const EdgeListIterator& src) : ArrayIterator<Edge*>(&sarray)
        {
            sarray = src.sarray;
        }
        void push(Edge* edge) {sarray.push_back(edge);}
    };

    class SpanningTreeIteratorBuilder
    {

    private:
        QueueIterator<Edge*> queue;
        std::set<unsigned> missing;
        EdgeListIterator iter;
        Graph *graph;

        void visit_vertex(Vertex *v)
        {
            for (auto ne = v->neighbor_iterator(); ne.has_next(); ne.next())
            {
                Edge* e = ne.current();
                Vertex* neighbor = e->get_destination(v);

                if (missing.count(neighbor->get_index()))
                {
                    queue.push((unsigned) e->get_weight(), e);
                }
            }
            missing.erase(v->get_index());
        }

        Vertex* get_next_vertex()
        {
            Vertex *v = 0;
            Edge* e = queue.current();
            queue.next();
            unsigned s = e->get_source()->get_index();
            unsigned d = e->get_destination()->get_index();

            if (missing.count(s))
            {
                v = e->get_source();
                iter.push(e);
            }

            if (missing.count(d))
            {
                v = e->get_destination();
                iter.push(e);
            }

            return v;
        }

    public:
        SpanningTreeIteratorBuilder(Graph* g, Vertex* start)
        {
            this->graph = g;

            for (auto vi = g->vertex_iterator(); vi.has_next(); vi.next())
            {
                Vertex* v = vi.current();
                missing.insert(v->get_index());
            }
            visit_vertex(start);
        }

        EdgeListIterator get()
        {
            while (queue.has_next())
            {
                Vertex* v = get_next_vertex();
                if (v)
                {
                    visit_vertex(v);
                }

                if (!queue.has_next() && missing.size())
                {
                    unsigned vi = *missing.begin();
                    visit_vertex(this->graph->get_vertex(vi));
                }
            }

            return iter;
        }
    };


    class ShortestPathTreeIteratorBuilder
    {

    private:
        struct SPToken { Vertex* src; Vertex* dst; EDATA total; };
        QueueIterator<unsigned> queue;
        std::vector<SPToken> array;
        Vertex* start;
        std::function<int(EDATA&)> minWeight;
        std::function<int(EDATA&)> maxWeight;
        std::function<int(EDATA&)> priorityWeight;

        void update_token(Vertex* src, Vertex* dst, EDATA total)
        {
            int index = dst->get_index();
            array[index] = {src, dst, total};
            unsigned prio = this->priorityWeight(total);
            queue.push(prio, index);
        }

    public:
        ShortestPathTreeIteratorBuilder(Graph* g, Vertex* start, std::function<int(EDATA&)> minWeight, std::function<int(EDATA&)> maxWeight, std::function<int(EDATA&)> priorityWeight)
        {
            this->start = start;
            this->minWeight = minWeight;
            this->maxWeight = maxWeight;
            this->priorityWeight = priorityWeight;

            EDATA weight = g->edge_iterator().current()->get_weight();

            int max = this->maxWeight(weight);
            int min = this->minWeight(weight);

            for (auto vi = g->vertex_iterator(); vi.has_next(); vi.next())
            {
                array.push_back({0, 0, 0});
                Vertex* v = vi.current();
                EDATA weight = (v == start) ? min : max;
                update_token(v, v, weight);
            }
        }


        EdgeListIterator get(Vertex* end)
        {
            while (queue.has_next())
            {
                unsigned vindex = queue.current();
                queue.next();
                Vertex* v = array[vindex].dst;
                EDATA vtotal = array[vindex].total;

                for (auto ne = v->neighbor_iterator(); ne.has_next(); ne.next())
                {
                    Edge* e = ne.current();
                    Vertex* neighbor = e->get_destination(v);
                    unsigned nindex = neighbor->get_index();
                    EDATA newtotal = vtotal + e->get_weight();
                    if (newtotal < array[nindex].total)
                        update_token(v, neighbor, newtotal);
                }
            }

            EdgeListIterator iter;
            if (end)
            {
                Vertex* dst = end;

                while (dst != this->start) {
                    unsigned index = dst->get_index();
                    Vertex* src = array[index].src;
                    if (src != dst)
                    {
                        Edge* edge = src->find_edge(dst);
                        iter.push(edge);
                    }
                    else
                    {
                        break;
                    }
                    dst = src;
                }
            }
            else
            {
                for (unsigned int i=0; i<array.size(); i++)
                {
                    Vertex* src = array[i].src;
                    Vertex* dst = array[i].dst;

                    if (src != dst)
                    {
                        Edge* edge = src->find_edge(dst);
                        iter.push(edge);
                    }
                }
            }
            return iter;
        }
    };


    // global graph attributes
    std::vector<Vertex*> vertices;
    std::vector<Edge*> edges;
    bool directed;

public:

    ArrayIterator<Vertex*> vertex_iterator() { return ArrayIterator<Vertex*>(&vertices); }
    ArrayIterator<Edge*> edge_iterator() { return ArrayIterator<Edge*>(&edges); }

    DepthFirstIterator depth_first_iterator(Vertex* start) { return DepthFirstIterator(start); }
    BreadthFirstIterator breadth_first_iterator(Vertex* start) { return BreadthFirstIterator(start); }

    Graph(bool dir, std::function<int(EDATA&)> max, std::function<int(EDATA&)> min, std::function<unsigned int(EDATA&)> priority)
    {
        directed = dir;
        this->maxWeight = max;
        this->minWeight = min;
        this->priorityWeight = priority;
    }

    ~Graph()
    {
        for (unsigned int i=0; i<vertices.size(); i++)
        {
            delete vertices[i];
        }

        for (unsigned int i=0; i<edges.size(); i++)
        {
            delete edges[i];
        }
    }

    Graph(const Graph<VDATA,EDATA>& o)
    {
        directed = o.directed;

        for (unsigned int i=0; i<o.vertices.size(); i++)
        {
            add_vertex(o.vertices[i]->get_value());
        }
        for (unsigned int i=0; i<o.edges.size(); i++)
        {
            unsigned isrc = o.edges[i]->get_source()->get_index();
            unsigned idst = o.edges[i]->get_destination()->get_index();
            add_edge(o.edges[i]->get_weight(), vertices[isrc], vertices[idst]);
        }

        this->maxWeight = o.maxWeight;
        this->minWeight = o.minWeight;
        this->priorityWeight = o.priorityWeight;
    }

    unsigned num_vertices() { return vertices.size(); }
    Vertex* get_vertex(unsigned i) { return vertices[i]; }
    unsigned num_edges() { return vertices.size(); }
    Edge* get_edge(unsigned i) { return edges[i]; }
    bool is_directed() { return directed; }

    Vertex* add_vertex(VDATA data)
    {
        Vertex* ret = new Vertex(data, vertices.size());
        vertices.push_back(ret);
        return ret;
    }

    Edge* add_edge(EDATA weight, Vertex* source, Vertex* destination)
    {
        if (source && destination)
        {
            Edge* ret = new Edge(weight, source, destination, edges.size(), is_directed());
            edges.push_back(ret);
            source->add_neighbor(ret);

            if (!directed)
            {
                destination->add_neighbor(ret);
            }

            return ret;
        }
        return 0;
    }

    Graph subgraph(ArrayIterator<Edge*>* iterp, bool keep_vertices = false)
    {
        Graph subgraph(is_directed());
        std::map<unsigned,unsigned> vertexmap;

        if (keep_vertices)
        {
            for (int i=0; i<vertices.size(); i++)
            {
                Vertex* v1 = vertices[i];
                Vertex* v2 = subgraph.add_vertex(v1->get_value());
                vertexmap[v1->get_index()] = v2->get_index();
            }
        }

        while (iterp->has_next())
        {
            Edge* edge = iterp->current();
            unsigned src = edge->get_source()->get_index();
            unsigned dst = edge->get_destination()->get_index();

            if (!keep_vertices)
            {
                if (vertexmap.find(src) == vertexmap.end())
                {
                    Vertex* v = subgraph.add_vertex(edge->get_source()->get_value());
                    vertexmap[src] = v->get_index();
                }

                if (vertexmap.find(dst) == vertexmap.end())
                {
                    Vertex* v = subgraph.add_vertex(edge->get_destination()->get_value());
                    vertexmap[dst] = v->get_index();
                }
            }

            Vertex *ret_src = subgraph.vertices[vertexmap[src]];
            Vertex *ret_dst = subgraph.vertices[vertexmap[dst]];
            subgraph.add_edge(edge->get_weight(), ret_src, ret_dst);
            iterp->next();
        }
        return subgraph;
    }

    EdgeListIterator min_spanning_tree_iterator(Vertex* start = 0)
    {
        return SpanningTreeIteratorBuilder(this, start ? start : vertices[0]).get();
    }

    EdgeListIterator shortest_path_tree_iterator(Vertex* start)
    {
        return shortest_path_iterator(start, 0);
    }

    EdgeListIterator shortest_path_iterator(Vertex* start, Vertex* end)
    {
        return ShortestPathTreeIteratorBuilder(this, start, minWeight, maxWeight, priorityWeight).get(end);
    }

};
