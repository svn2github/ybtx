#pragma once
template<class T>
class Node{
	public:
		Node(T v)                  //Constructor with parameter
			:value(v),Next(NULL){}

		T GetValue()               //Get value of Node
		{
			return value;
		}

		Node& SetValue(T element)      //Set value of Node
		{
			value = element;
			return *this;
		}

		Node* GetNext()                //Get the next node of current
		{
			return Next;
		}

		Node& SetNext(Node<T>* p)         //Set the next node
		{
			Next = p;
			return *this;
		}

		Node& operator ++(){
			value = Next->value;
			Next = Next->Next;
			return *this;
		}
	private:
		T value;                        //Store the truth value of a node
		Node<T>* Next;                  //Point to the next Node

};

template<class T>
class TList{
	public:
		static size_t no_pos;
		//typedef Node<T>*	iterator;
		typedef Node<T>*	node_type;
	private:
		Node<T>*	m_pHead;
		Node<T>*	m_pTail;
		size_t	m_szSize;
	public:
		TList():m_pHead(NULL),m_pTail(NULL),m_szSize(0){
		}

		Node<T>* begin(){
			return m_pHead;
		}

		Node<T>* end(){
			return m_pTail;
		}

		bool empty(){
			return m_szSize==0;
		}

		size_t size()const{
			return m_szSize;
		}

		TList& push_back(T element){
			Node<T> * pNode = new Node<T>(element);

			if (m_szSize == 0)
				m_pTail=m_pHead=pNode;
			else{
				m_pTail->SetNext(pNode);
				m_pTail=pNode;
			}

			++m_szSize;
			return *this;
		}

		Node<T>* front()
		{
			Node<T>* pNode = m_pHead;
			return pNode; 
		}


		void pop_front(){
			if (!m_pHead)
				return;
				
			Node<T>* pNode = m_pHead;
			m_pHead = m_pHead->GetNext();
			--m_szSize;

			if (m_szSize == 0)
				m_pTail = m_pHead;

			delete pNode;
		}

		TList& insert(size_t pos, Node<T>* element);

		TList& insert(size_t pos, size_t num, Node<T>* element);

		TList& remove(size_t pos, Node<T>& element);

		TList& erase(size_t pos_start, size_t pos_end);

		Node<T>* list_end()const{
			return m_pTail;
		}

		Node<T>* list_start()const{
			return m_pHead;
		}

		size_t find(Node<T>* element){
			size_t index = 0;
			for (Node<T>* pNode = m_pHead; pNode != NULL; pNode=pNode->GetNext(), ++index)
			{
				if (pNode->GetValue() == element->GetValue())
					return index;
			}
			return no_pos;
		}



		void swap(TList<T>* pList){
			Node<T>* pHead = m_pHead;
			size_t szSize = m_szSize;
			Node<T>* pTail = m_pTail;

			m_pHead = pList->begin();
			m_szSize = pList->m_szSize;
			m_pTail = pList->end();

			pList->m_szSize = szSize;
			pList->m_pHead = pHead;
			pList->m_pTail = pTail;

		}

		void splice(TList<T>* pList){
			if (empty()){
				swap(pList);
				return;
			}

			if (pList->empty())
				return;

			m_pTail->SetNext(pList->begin());
			m_pTail = pList->end();
			m_szSize += pList->m_szSize;
			pList->m_pHead = pList->m_pTail = NULL;
			pList->m_szSize=0;
		}

		void clear(){
			while(!empty()){
				pop_front();
			}
		}

};

template <class T>
size_t TList<T>::no_pos = -1;

