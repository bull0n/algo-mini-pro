#pragma once

#include <vector>
#include <set>
#include <stack>

template<class VDATA, class EDATA>
class Graph
{

public:
	class Edge;

    class Vertex
    {
	private:
		VDATA value;
		std::vector<Edge*> edges;

	public:
        class NeighborEdgeIterator
        {
		private:
            unsigned int pos;
			std::vector<Edge*>* np;

		public:
			NeighborEdgeIterator() { pos = 0; }
			bool has_next() { return pos < np->size(); }
			void next() { pos ++; }
			Edge* current() { return has_next() ? (*np)[pos] : 0; }
			void set_np(std::vector<Edge*>* np) { this->np = np; }
		};

        NeighborEdgeIterator neighbor_iterator()
        {
			NeighborEdgeIterator ni;
			ni.set_np(&edges);
			return ni;
		}

		Vertex(VDATA value) { this->value = value; }
		int get_degree() { return edges.size(); }
		VDATA& get_value() { return value; }
		void add_neighbor(Edge* neighbor) { edges.push_back(neighbor); }
		Edge* get_edge(int i) { return edges[i]; }
	};

    class Edge
    {
	private:
		EDATA weight;
		Vertex* source;
		Vertex* destination;
		bool directed;
	public:

		Edge(EDATA weight, Vertex* source, Vertex* destination, bool directed = false)
		{
			this->weight = weight;
			this->source = source;
			this->destination = destination;
			this->directed = directed;
		}

		EDATA get_weight() { return weight; }
		Vertex* get_source() { return source; }
		Vertex* get_destination() { return destination; }

        Vertex* get_destination(Vertex* source)
        {
			if (source == this->source)
            {
				return destination;
            }
			if (!directed && source == this->destination)
            {
                return this->source;
            }

			return 0;
		}
	};

private:
	std::vector<Vertex*> vertices;
	std::vector<Edge*> edges;
	bool directed;

public:

    class VertexIterator
    {
	private:
        unsigned int pos;
		std::vector<Vertex*>* vp;

	public:
		VertexIterator() { pos = 0; }
		bool has_next() { return pos < vp->size(); }
		void next() { pos ++; }
		Vertex* current() { return has_next() ? (*vp)[pos] : 0; }
		void set_vp(std::vector<Vertex*>* vp) { this->vp = vp; }
	};

	VertexIterator vertex_iterator()
	{
		VertexIterator vi;
		vi.set_vp(&vertices);
		return vi;
	}

    class EdgeIterator
    {
	private:
		int pos;
		std::vector<Edge*>* ep;

	public:
		EdgeIterator() { pos = 0; this->ep = ep; }
		bool has_next() { return pos < ep->size(); }
		void next() { pos ++; }
		Edge* current() { return has_next() ?(*ep)[pos] : 0; }
		void set_ep(std::vector<Edge*>* ep) { this->ep = ep; }
	};

	VertexIterator edge_iterator()
	{
		EdgeIterator ei;
		ei.set_ep(&edges);
		return ei;
	}

    class DepthFirstIterator
    {
	private:
        struct StackElement
        {
			Vertex* vertex;
			int pos;
		};

		std::vector<Vertex*>* vp;
		std::set<Vertex*> visited;
		std::stack<StackElement> stack;
		Vertex* following;

	public:
		DepthFirstIterator(Vertex* begin)
		{
			following = begin;
			StackElement se;
			se.vertex = begin;
			se.pos = 0;
			stack.push(se);
		}

		void set_vp(std::vector<Vertex*>* vp) { this->vp = vp; }
        bool has_next() { return following != 0; }

        void next()
        {
            if (following == 0)
            {
                return;
            }

			following = 0;

            if (stack.size() == 0)
            {
                return;
            }

			StackElement se = stack.top();
			stack.pop();

            while (following == 0 && se.pos < se.vertex->get_degree())
            {
				Edge* edge = se.vertex->get_edge(se.pos);
				Vertex* next = edge->get_destination(se.vertex);
                if (visited.count(next) == 0)
                {
					following = next;
				}
				se.pos ++;
			}

			if (se.pos < se.vertex->get_degree())
            {
				stack.push(se);
            }

            if (following)
            {
				se.vertex = following;
				se.pos = 0;
				stack.push(se);
			}
		}
		Vertex* current()
		{
			if (following)
            {
				visited.insert(following);
            }

			return following;
		}
		
	};
	DepthFirstIterator depth_first_iterator(Vertex* begin)
	{
		DepthFirstIterator dfi(begin);
		dfi.set_vp(&vertices);

		return dfi;
	}

	Graph(bool dir)
	{
		directed = dir;
    }

	int num_vertices() { return vertices.size(); }
	bool is_directed() { return directed; }

	Vertex* add_vertex(VDATA data)
	{
		Vertex* ret = new Vertex(data);
		vertices.push_back(ret);
		return ret;
	}

	Edge* add_edge(EDATA weight, Vertex* source, Vertex* destination)
	{
        if (source && destination)
        {
			Edge* ret = new Edge(weight, source, destination, is_directed());
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

	Vertex* get_vertex(int i) { return vertices[i]; }

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
};
