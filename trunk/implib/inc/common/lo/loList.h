#ifndef __locomm_LIST_H__
#define __locomm_LIST_H__

#include <common/lo/lodefine.h>

DEFINE_NAMESPACE(locom)

/** 结点值 */
template<class T>
class CloNodeT : public t_loPOSITION
{
public:
	CloNodeT():prev(NULL),next( NULL ){}
private:
	CloNodeT* prev;   /**< prev CloNodeT containing element */
	CloNodeT* next;   /**< next CloNodeT containing element */
	T element;      /**< element in Current node */

	template<class T> friend class CloListT;
};

/// 定义
#define Position_node_cast(pos)    ( (CloNodeT<T>*)pos )
#define Node_position_cast(node)   ( (POSITION_l)node )

/// 释放的对象的回调函数
typedef void (*PNodeFreeCallBack)(void* element);

/// 查询对象的回调函数［return=0,表示查询成功］
typedef int  (*PNodeQueryCallBack)(POSITION_l position , void* element , void* data);

/** */
template<class T>
class CloListT
{	
public:
	CloListT();
	virtual ~CloListT();
public:
	/**
	* Free a list of element.
	* Each element will be free with the method given as the second parameter.
	* @param li The element to work on.
	* @param free_func The method that is able to release one element of the list.
	*/
	void free ( PNodeFreeCallBack freeCB );

	/**
	* Get the size of a list of element.
	* @param li The element to work on.
	*/
	int size () const;
	/**
	 * @brief add an element in a list
	*/
	int push_back(T element) { return add(element); }
	/**
	* Add an element in a list.
	* @param li The element to work on.
	* @param element The pointer on the element to add.
	*/
	int add (T element , POSITION_l insert=NULL);
	POSITION_l add_position(T element , POSITION_l insert=NULL);
	/**
	* Remove an element from a list.
	* @param li The element to work on.
	* @param pos the index of the element to remove.
	* @return CloNodeT<T>->element.
	*/
	T remove (int pos);
	T remove (POSITION_l node);

	/** move the node to dest node front */
	int move_front(POSITION_l node , POSITION_l destnode);

	/** move the node to dest node back */
	int move_back(POSITION_l node , POSITION_l destnode);

	/**
	 * query a list of element
	*/
	POSITION_l query( PNodeQueryCallBack queryCB , void* object);
	/**
	 * @brief 获取点
	*/
	POSITION_l at(int pos);
	/**
	* Get an element from a list.
	* @param pos the index of the element to get.
	* @return CloNodeT<T>->element.
	*/
	T  get (int pos);
	T  get (POSITION_l node);

	T* getptr (int pos);
	T* getptr (POSITION_l node);
	/**
	 * swap two element from this list
	 * @param srcNode source element
	 * @param destNode dest element
	*/
	int   swap(POSITION_l srcNode,POSITION_l destNode);
	/**
	 * @brief 开始一个结点
	 *
	 * @param li list of VLLIST
	 * @return 结点指针
	 */
	POSITION_l first();
	/**
	 * @brief 最后一个结点
	 *
	 * @param li list of VLLIST
	 * @return 结点指针
	 */
	POSITION_l last();
	/**
	 * @brief 下一个结点
	 *
	 * @param node node of CloNodeT<T>
	 * @return 结点指针
	 */
	POSITION_l next(const POSITION_l  node);
	/**
	 * @brief 前一个结点
	 *
	 * @param node node of CloNodeT<T>
	*/
	POSITION_l prev(const POSITION_l node);
protected:
	int m_counts;         /**< Number of element in the list */
	CloNodeT<T>* m_first;   /**< Next node containing element  */
	CloNodeT<T>* m_tail;    /**< Next node containing element  */
};

template<class T>
CloListT<T>::CloListT()
{
	m_counts= 0;
	m_tail= m_first  = 0;
}

template<class T>
CloListT<T>::~CloListT()
{
}

template<class T>
void CloListT<T>::free( PNodeFreeCallBack freeCB )
{
	CloNodeT<T> *next = 0,*node = 0;
	next = Position_node_cast( first() );	
	while( next )
	{
		if( freeCB )
		{
			freeCB((void*)next->element);
		}
		node = next ;
		next = next->next;
		node->prev = 0;
		node->next = 0;
		DeleteM_lo(node);
	}
	m_counts = 0;
	m_tail = m_first   = 0;
}

template<class T>
POSITION_l CloListT<T>::query( PNodeQueryCallBack queryCB , void* object)
{
	POSITION_l pos = first() ;	
	while( pos )
	{
		if( 0 == queryCB( pos , get(pos) , object ) )
			return pos;
		pos = next(pos);
	}	
	return NULL;
}

template<class T>
int CloListT<T>::size() const
{
	return m_counts;
}

template<class T>
int CloListT<T>::add (T el , POSITION_l position)
{
	int i = m_counts;
	if( !add_position(el , position) )
	{
		return -1;
	}
	return i;
}

template<class T>
POSITION_l CloListT<T>::add_position(T el , POSITION_l position)
{
	CloNodeT<T> *newnode = 0;
	CloNodeT<T> *insert= Position_node_cast(position);
	if (m_counts == 0)
	{
		newnode = new CloNodeT<T>();		
		if (newnode == NULL)
		{
			return NULL;
		}
		newnode->element = el;
		newnode->next = 0;
		newnode->prev = 0;
		m_tail = m_first = newnode;
	}
	else if( !insert )
	{
		CloNodeT<T> *node = Position_node_cast( last() );
		if ( !node )
		{
			return NULL;
		}
		newnode = new CloNodeT<T>();
		if ( !newnode )
		{
			return NULL;
		}
		newnode->element = el;
		newnode->next = NULL;
		newnode->prev = node;
		node->next = newnode;			
		m_tail = newnode;
	}
	else 
	{
		CloNodeT<T> *prev = 0;
		newnode = new CloNodeT<T>();
		if ( !newnode ) 
		{
			return NULL;
		}
		newnode->element = el;
		newnode->next    = 0;
		newnode->prev    = 0;
		//插入insert前
		prev  = insert->prev ;
		newnode->next = insert;
		newnode->prev = prev;
		if( prev )
		{
			prev->next= newnode;
		}
		insert->prev   = newnode ;
		if( insert == m_first )
		{
			m_first = newnode;
		}
	}
	m_counts++;
	return Node_position_cast(newnode);
}

template<class T>
POSITION_l CloListT<T>::at(int pos)
{
	int i = 0;
	CloNodeT<T> *node = 0;	
	if (pos < 0 || pos >= m_counts)
		return NULL;

	node = m_first;
	while (pos > i){
		i++;
		node = (CloNodeT<T> *) node->next;
	}
	return node;
}

template<class T>
T CloListT<T>::get(int pos)
{
	return get( at(pos) );
}

template<class T>
T CloListT<T>::get (POSITION_l position)
{
	if ( !position ) return NULL;
	return Position_node_cast(position)->element;
}

template<class T>
T* CloListT<T>::getptr (int pos)
{
	if ( !position ) return NULL;
	return &Position_node_cast(position)->element;
}

template<class T>
T* CloListT<T>::getptr (POSITION_l node)
{
	if ( !position ) return NULL;
	return &Position_node_cast(position)->element;
}

template<class T>
int CloListT<T>::swap(POSITION_l srcPos,POSITION_l destPos)
{
	T tmp_element;
	CloNodeT<T> *src_node=0,*dest_node=0;
	if ( !srcPos || !destPos || srcPos == destPos )
		return -1;	
	//
	src_node = Position_node_cast(srcPos);
	dest_node= Position_node_cast(destPos);	
	tmp_element       = src_node->element;
	src_node->element = dest_node->element;
	dest_node->element= tmp_element;
	//
	return 0;
}

template<class T>
T CloListT<T>::remove (int pos)
{
	CloNodeT<T> *node=0,*prev=0,*next=0;T ele;
	int i = 0;
	if (pos < 0 || pos >= m_counts)
		return NULL;
	node = m_first;
	while ( i < pos && node ){
		i++;
		node = (CloNodeT<T> *) node->next;
	}
	prev = node->prev;
	next = node->next;
	ele  = node->element;
	if( prev )
		prev->next = next;
	if( next )
		next->prev = prev;
	//
	if ( node == m_first )
		m_first = next;
	if ( node == m_tail )
		m_tail = prev;
	//
	DeleteM_lo(node);
	m_counts--;
	return (T)ele;
}

template<class T>
T CloListT<T>::remove (POSITION_l position)
{
	CloNodeT<T> *prev=0,*next=0; void *ele=0;
	CloNodeT<T> *node = Position_node_cast(position);
	if ( node == NULL ) return NULL;
	prev = node->prev;
	next = node->next;
	ele  = node->element;
	if( prev )
		prev->next = next;
	if( next )
		next->prev = prev;
	//
	if ( node == m_first )
		m_first = next;
	if ( node == m_tail )
		m_tail = prev;
	//
	DeleteM_lo(node);
	m_counts--;
	return (T)ele;
}

template<class T>
int CloListT<T>::move_front(POSITION_l position , POSITION_l destposition)
{
	CloNodeT<T> *node = Position_node_cast(position);
	CloNodeT<T> *destnode = Position_node_cast(destposition);
	if( NULL == node )
		return -1;

	if( position == destposition )
		return 0;

	if( NULL == destnode ){
		destnode = Position_node_cast( first() );
		if( NULL == destnode )
			return -1;
	}
	// disconnect node
	CloNodeT<T> *prev = node->prev;
	CloNodeT<T> *next = node->next;
	node->prev = node->next = NULL;
	if( prev )
		prev->next = next;
	if( next )
		next->prev = prev;

	if ( node == m_first )
		m_first = next;

	if ( node == m_tail )
		m_tail = prev;

	prev = destnode->prev;

	if( prev )
	{
		prev->next = node;
		node->prev = prev;
	}

	node->next = destnode;
	destnode->prev = node;

	if( destnode == m_first )
		m_first = node;

	return 0;
}

template<class T>
int CloListT<T>::move_back(POSITION_l node , POSITION_l destnode)
{
	CloNodeT<T> *node = Position_node_cast(position);
	CloNodeT<T> *destnode = Position_node_cast(destposition);
	if( NULL == node )
		return -1;

	if( position == destposition )
		return 0;

	if( NULL == destnode ){
		destnode = Position_node_cast( last() );
		if( NULL == destnode )
			return -1;
	}
	// disconnect node
	CloNodeT<T> *prev = node->prev;
	CloNodeT<T> *next = node->next;
	node->prev = node->next = NULL;
	if( prev )
		prev->next = next;
	if( next )
		next->prev = prev;

	if ( node == m_first )
		m_first = next;

	if ( node == m_tail )
		m_tail = prev;

	next = destnode->next;
	if( next )
	{
		next->prev = node;
		node->next = next;
	}

	node->prev = destnode;
	destnode->next = node;

	if( destnode == m_tail )
		m_tail = node;

	return 0;
}

template<class T>
POSITION_l CloListT<T>::first()
{
	if( m_counts <= 0  )  return NULL;
	return Node_position_cast(m_first);
}

template<class T>
POSITION_l CloListT<T>::last()
{
	if( m_counts <= 0  )  return NULL;
	return Node_position_cast(m_tail);
}

template<class T>
POSITION_l CloListT<T>::next(const POSITION_l node)
{
	return node?Node_position_cast( (Position_node_cast(node)->next) ):NULL;
}

template<class T>
POSITION_l CloListT<T>::prev(const POSITION_l node)
{
	return node?Node_position_cast( (Position_node_cast(node)->prev) ):NULL;	
}

//--------------------------------------------------------------------------------//
/** 简单的单项链表 */
template<typename T>
class CloLinkNodeT
{
public:
	CloLinkNodeT()
	{
		next = NULL;
	}
	T data;
	CloLinkNodeT<T>* next;
};

// 回调函数定义
typedef int  (*funLinkNodeCallback)(void* data , void* userdata);

// 回调释放
typedef void (*funLinkNodeFree)(void* data);

/** 消毁单项链表 */
template<typename T>
void glodestroy_linknodeT(CloLinkNodeT<T>* plink , funLinkNodeFree pfree)
{
	if( !plink )
		return ;

	CloLinkNodeT<T>* pnext = plink->next;
	while( pnext )
	{
		CloLinkNodeT<T>* ptemp = pnext;
		pnext = pnext->next;
		if( ptemp )
		{
			if( pfree )
			{
				pfree( (void*)(&ptemp->data) );
			}
			delete ptemp; ptemp = NULL;
		}
	}
	plink->next = NULL;
}

/** 删除一个结点  */
template<typename T>
int gloremove_linknodeT( CloLinkNodeT<T>* &plink , void* data , funLinkNodeCallback pcall , funLinkNodeFree pfree )
{
	if( !plink || !pcall )
		return -1;

	CloLinkNodeT<T>* pnext = plink;
	CloLinkNodeT<T>* pprev = plink;
	CloLinkNodeT<T>* pcurrent = NULL;

	while( pnext )
	{
		pcurrent = pnext;
		pnext    = pcurrent->next;

		if( 0 == pcall( (void*)(&pcurrent->data) , data ) )
		{
			pprev->next = pnext;
			if( plink == pcurrent )
			{//当删除第一个结点
				plink = pnext;
			}
			if( pfree )
			{
				pfree( (void)(&pcurrent->data) );
			}
			delete pcurrent;
			return 0;
		}
		else
		{
			pprev = pcurrent;
		}
	}
	return 0;
}


template<typename T>
class CloLinkT
{
public:
	CloLinkT()
	{
		_iCounts = 0;
		_link = _last = 0;
	}

	~CloLinkT()
	{
		destroy();
	}

	int counts(void) const
	{
		return _iCounts;
	}

	void destroy(funLinkNodeFree pfree = 0)
	{		
		if( !_link )
			return ;

		glodestroy_linknodeT<T>( _link , pfree );

		delete _link; _link = 0;
		_last = 0;
		_iCounts = 0;
	}

	/** 根结点 */
	CloLinkNodeT<T>* _link;

	/** 最后项 */
	CloLinkNodeT<T>* _last;

	int _iCounts;
};

END_NAMESPACE(locom)

#endif /* __locomm_LIST_H__ */