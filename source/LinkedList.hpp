#ifndef _LINKEDLIST_HPP_
#define _LINKEDLIST_HPP_
template <class Elem>
class LinkedList
{
private:
  template <class NodeElem>
  class ListNode
  {
  public:
    NodeElem* e;
    ListNode<NodeElem>* next;
    ListNode(Elem* e,ListNode<NodeElem>*next)
    {
      this->e=e;
      this->next=next;
    }
  };
  ListNode<Elem>* head;
  int size;
public:
  LinkedList();
  void push(Elem*);
  Elem* pop(void);
  void unshift(Elem*);
  Elem* shift(void);
  Elem* get(int);
  void add(int,Elem*);
  Elem* remove(int);
  int getSize();
  bool hasNext();
  void debug_print();
};

template<class Elem>
void LinkedList<Elem>::push(Elem* e)
{
  if(size==0)
  {
    head = new ListNode<Elem>(e,NULL);
  }
  else
  {
    ListNode<Elem>* endNode=head;
    while(endNode->next!=NULL)
      endNode=endNode->next;
    endNode->next = new ListNode<Elem>(e,NULL);
  }
  size++;
}

template<class Elem>
Elem* LinkedList<Elem>::pop()
{
  if(size==0)
  {
    printf("ERROR: TRIED TO POP AN EMPTY LIST!\n");
    return NULL;
  }
  Elem* returnBuff;
  ListNode<Elem>* prevNode=NULL;
  ListNode<Elem>* endNode=head;
  while(endNode->next!=NULL)
  {
    prevNode=endNode;
    endNode=endNode->next;
  }
  if(size==1)
    head = NULL;
  else
    prevNode->next = NULL;
  returnBuff = endNode->e;
  delete endNode;
  size--;
  return returnBuff;
}

template<class Elem>
void LinkedList<Elem>::unshift(Elem* e)
{
  head = new ListNode<Elem>(e,head);
  size++;
  
}

template<class Elem>
Elem* LinkedList<Elem>::shift(void)
{
  if(size == 0)
  {
    printf("LIST: ERROR: TRIED TO POP AN EMPTY LIST!\n");
    return NULL;
  }
  Elem* returnBuff = head->e;
  ListNode<Elem>* removeNode=head;
  head = head->next;
  delete removeNode;
  size--;
  return returnBuff;
}

template<class Elem>
Elem* LinkedList<Elem>::get(int index)
{
  if(index >= size)
    return NULL;
  int i;
  ListNode<Elem>* traverse = head;
  for(i=0;i<index;i++)
  {
    traverse = traverse->next;
  }
  return traverse->e;
}

template<class Elem>
void LinkedList<Elem>::add(int index,Elem* e)
{
  if(index > size)
    push(e);
  else if(index==0)
    unshift(e);
  //traverse into the right spot
  int i;
  ListNode<Elem>* prevNode = NULL;
  ListNode<Elem>* currentNode = head;
  for(i=0;i<index;i++)
  {
    prevNode = currentNode;
    currentNode = currentNode->next;
  }
  //do pointer magic
  prevNode->next = new ListNode<Elem>(e,currentNode);
  size++;
}

template<class Elem>
int LinkedList<Elem>::getSize()
{
  return size;
}

template<class Elem>
bool LinkedList<Elem>::hasNext()
{
  return size>0;
}

template<class Elem>
Elem* LinkedList<Elem>::remove(int index)
{
  if(index<0 || index>=size)
  {
    printf("LIST: ERROR: REMOVE INDEX OUT OF BOUNDS!\n");
    return NULL;
  }
  else if(index==0)
    return shift();
  else
  {
    int i;
    Elem * returnBuff;
    ListNode<Elem>* prevNode = NULL;
    ListNode<Elem>* currentNode = head;
    for(i=0;i<index;i++)
    {
      prevNode = currentNode;
      currentNode = currentNode->next;
    }
    //do pointer magic
    prevNode->next = currentNode->next;
    returnBuff = currentNode->e;
    delete currentNode;
    size--;
    return returnBuff;
  }
}

template<class Elem>
LinkedList<Elem>::LinkedList()
{
  this->size=0;
  this->head = NULL;
}

template<class Elem>
void LinkedList<Elem>::debug_print()
{
  
  printf("LIST: ELEM STATUS: %i ELEMS, (\n",getSize());
  
  ListNode<Elem> * currentNode=head;
  
  while(currentNode!=NULL)
  {
    printf("OK,\n");
    currentNode = currentNode->next;
  }
  printf("END)\n");
  
  return;
}
#endif
