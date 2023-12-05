#pragma once

#include <atomic>
#include <cstddef>

template <typename T> 
class FreeQueue
{
public:
  FreeQueue(size_t capacity = 1000000)
  {
    _capacity = capacity;

    _queue = (Node*)new char[sizeof(Node) * _capacity];
    for(size_t i = 0; i < _capacity; ++i)
    {
      _queue[i].tail.store(i);
      _queue[i].head.store(-1);
    }

    _tail.store(0);
    _head.store(0);
  }

  ~FreeQueue()
  {
    for(size_t i = _head; i != _tail; ++i)
      (&_queue[i % _capacity].data)->~T();

    delete [] (char*)_queue;
  }
  
  size_t capacity() const { return _capacity; }
  
  size_t size() const
  {
    size_t head = _head.load();
    return _tail.load() - head;
  }
  
  bool push(const T& data)
  {
    Node* node;
    size_t tail = _tail.load();
    for(;;)
    {
      node = &_queue[tail % _capacity];
      if(node->tail.load() != tail)
        return false;
      if((_tail.compare_exchange_strong(tail, tail + 1)))
        break;
    }
    new (&node->data)T(data);
    node->head.store(tail);
    return true;
  }

  bool pop(T& result)
  {
    Node* node;
    size_t head = _head.load();
    for(;;)
    {
      node = &_queue[head % _capacity];
      if(node->head.load() != head)
        return false;
      if(_head.compare_exchange_strong(head, head + 1))
        break;
    }
    result = node->data;
    (&node->data)->~T();
    node->tail.store(head + _capacity);
    return true;
  }

private:
  struct Node
  {
    T data;
    std::atomic<size_t> tail;
    std::atomic<size_t> head;
  };

private:
  Node* _queue;
  size_t _capacity;
  std::atomic<size_t> _tail;
  std::atomic<size_t> _head;
};