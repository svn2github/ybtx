#include "TGraph.h"
#include "CGraphPath.h"
#include <algorithm>


#define  NORMAL_CAPACITY  256
#define  MAX_DIST  65532 // 以后需要改为浮点

template<typename T>
TGraph<T>::TGraph()
{
}

template<typename T>
TGraph<T>::~TGraph()
{
}

template<typename T>
void TGraph<T>::AddEdge(int start, int end, T weight)
{
	NBver<T> ver;
	ver.v = end;
	ver.w = weight;
	if ( start >= static_cast<int>(m_graph.size()) )
		m_graph.push_back(NBvList());

	if ( static_cast<int>(m_graph.size()) - 1 < start )
		m_graph.resize(start);
	m_graph.at(start).push_back(ver);
}


template<typename T>
void TGraph<T>::Relax(int u, int v, T w)
{	
	if ( m_heap.at(v) > ( m_heap.at(u) + w ) )
	{
		m_heap.at(v) = m_heap.at(u) + w;
		m_parent.at(v) = u;
	}
}

void swap(int *x, int *y)
{
	int temp;
	temp = *x;
	*x = *y;
	*y = temp;
}

template<typename T>
void TGraph<T>::PrintPath(int source, int dest)
{
	int* path = new int[m_parent.size()];
	int j = 0;
	if ( m_parent.at(dest) == -1 )
		printf("no path from %d to %d exists!", source, dest);

	while ( dest != -1 )
	{
		path[j] = dest; // 记录路径中的顶点
		dest = m_parent[dest];
		++j;
	}

	for ( j=j-1; j>=1; --j )
		printf(" %d-->",path[j]);
	printf("%d:",path[j]);

	delete[] path;
}

// 打印出所有的从source到其他顶点的最短路径
template<typename T>
void TGraph<T>::PrintAllPath(int source)
{
	int i = 0;
	printf("the minimum paths figured out by dijkstra algorithm are follows:\n");
	for ( i=0; i<static_cast<int>(m_heap.size()); ++i )
	{
		PrintPath(source, i);
		cout<<m_heap.at(i)<<endl;
	}
}

// 获得起点所有最短路径中需要的终点最短路径
template<typename T>
CGraphPath* TGraph<T>::GetMinPath()
{
	CGraphPath* path = new CGraphPath();
	CGraphPath::GraphaPath& pathMap = path->GetPathMap();

	int i = 0;
	int dest = 0;
	int j = 0;
	pathMap.resize(m_heap.size());
	for ( i=0; i<static_cast<int>(m_heap.size()); ++i )
	{
		int* path = new int[m_parent.size()];
		dest = i;
		j = 0;
		if ( m_parent.at(dest) == -1 )
			printf("no path from %d to %d exists!", m_start, dest);

		while ( dest != -1 )
		{
			path[j] = dest; // 记录路径中的顶点
			dest = m_parent[dest];
			++j;
		}

		for ( j=j-1; j>=0; --j )
			pathMap.at(i).push_back(path[j]);

		delete[] path;
	}

	return path;
}


// 构建以i为根的子树成为最小堆
template<typename T>
void TGraph<T>::MinHeapify(MathVector<int>& indexList, MathVector<T>& heap, int i)
{
	int right = ( i + 1 ) << 1; // i节点的左右子节点
	int left = right - 1;
	int smallest = 0;

	if ( left < m_heapSize && heap.at(indexList.at(left)) < heap.at(indexList.at(i)) )
		smallest = left;
	else
		smallest = i;

	if ( right < m_heapSize && heap.at(indexList.at(right)) < heap.at(indexList.at(smallest)) )
		smallest = right;

	if ( smallest != i ) // 当前节点如果比子节点大，则子节点往上滚动
	{
		swap(&indexList.at(i), &indexList.at(smallest));
		MinHeapify(indexList, heap, smallest); // 以原来最新子节点作为根树递归
	}
}

template<typename T>
int TGraph<T>::PopHeap(MathVector<int>& indexList, MathVector<T>& heap)
{
	int min = 0;
	min = indexList.at(0);
	indexList.at(0) = indexList.back(); // indexList.at(m_heapSize-1);
	indexList.pop_back();
	--m_heapSize;
	MinHeapify(indexList, heap, 0);
	return min;
}

template<typename T>
void TGraph<T>::SortHeap(MathVector<int>& indexList, MathVector<T>& heap)
{
	int i = m_heapSize / 2;
	for ( ; i>=0; --i ) // 调整每一个非叶子结点为根的树
		MinHeapify(indexList, heap, i);
}

template<typename T>
void TGraph<T>::Dijkstra(int start)
{
	m_start = start;
	MathVector<int> UList; // 未寻出最短路径点
	int vertexNum = static_cast<int>(m_graph.size());
	int i = 0;
	for ( i=0; i<vertexNum; ++i )
	{
		UList.push_back(i);
		m_heap.push_back(MAX_DIST);
		m_parent.push_back(-1);
	}
	m_heap.at(start) = 0;
	m_heapSize = static_cast<int>(m_heap.size());

	SortHeap(UList, m_heap);

	int current = 0;
	while ( m_heapSize > 0 )
	{
		current = PopHeap(UList, m_heap);
		NBvList& neighbors = m_graph.at(current);
		typename NBvList::iterator it = neighbors.begin();
		for ( ; it != neighbors.end(); ++it )
			Relax(current, it->v, it->w);

		SortHeap(UList, m_heap);
	}
}

