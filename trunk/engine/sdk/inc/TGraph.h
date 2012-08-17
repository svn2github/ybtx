#pragma once

#include <vector>
#include <list>
#include <map>
#include "CMathMallocObject.h"

using namespace std;


namespace sqr
{
	class CGraphPath;

	template<typename T>
	struct NBver : public CMathMallocObject
	{
		int v; // ¶¥µã u -> v
		T w;
	};

	template<typename T>
	class TGraph : public CMathMallocObject
	{
	public:
		typedef  MathList< NBver<T> >  NBvList;

		TGraph();
		~TGraph();

		void AddEdge(int start, int end, T weight);
		void Dijkstra(int start);
		void PrintPath(int source, int dest);
		void PrintAllPath(int source);
		CGraphPath* GetMinPath();

	private:
		int    m_heapSize;
		int    m_start;

		MathVector< NBvList >  m_graph;
		MathVector<T>          m_heap;
		MathVector<int>        m_parent;

		void Relax(int u, int v, T w);
		void MinHeapify(MathVector<int>& indexList, MathVector<T>& heap, int i);
		void SortHeap(MathVector<int>& indexList, MathVector<T>& heap);
		int PopHeap(MathVector<int>& indexList, MathVector<T>& heap);
	};
}

