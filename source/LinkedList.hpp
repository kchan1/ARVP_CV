#ifndef _LINKEDLIST_HPP_
#define _LINKEDLIST_HPP_
template <class Elem>
class LinkedList
{
private:
  template <class Elem>
  class ListNode
  {
    Elem* e;
    ListNode<Elem>* next = NULL;
    ListNode(Elem* e,ListNode<Elem>*next)
    {
      this->e=e;
      this->next=next;
    }
  };
  ListNode<Elem>* head;
  int size;
public:
  void push(Elem*);
  Elem* pop(void);
  void unshift(Elem*);
  Elem* shift(void);
  Elem* get(int);
  void set(int,Elem*);
}
#endif
