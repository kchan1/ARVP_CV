#include "LinkedList.hpp"
#include <iostream>

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
    return NULL;
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
    return NULL;
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
bool LinkedList<Elem>::hasNext()
{
  return size>0;
}

template<class Elem>
Elem* LinkedList<Elem>::remove(int index)
{
  if(index<0 || index>=size)
    return NULL;
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
