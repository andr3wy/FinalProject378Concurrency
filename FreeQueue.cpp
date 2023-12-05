
// // // #include <atomic>
// // // #include <cstddef>
// // // #include <type_traits>


// // // namespace lockfree {
// // // namespace mpmc {
// // // /*************************** TYPES ****************************/

// // // template <typename T, size_t size> class Queue {

// // //     /********************** PUBLIC METHODS ************************/
// // //   public:
// // //     Queue();

// // //     /**
// // //      * @brief Adds an element into the queue.
// // //      * @param[in] element
// // //      * @retval Operation success
// // //      */
// // //     bool Push(const T &element);

// // //     /**
// // //      * @brief Removes an element from the queue.
// // //      * @param[out] element
// // //      * @retval Operation success
// // //      */
// // //     bool Pop(T &element);

// // //     bool Peek();

// // //     /*********************** PRIVATE TYPES ************************/
// // //   private:
// // //     struct Slot {
// // //         T val;
// // //         std::atomic_size_t pop_count;
// // //         std::atomic_size_t push_count;

// // //         Slot() : pop_count(0U), push_count(0U) {}
// // //     };

// // //     /********************** PRIVATE MEMBERS ***********************/
// // //   private:
// // //     Slot _data[size]; /**< Data array */
// // // };

// // // } /* namespace mpmc */
// // // } /* namespace lockfree */

// // // /************************** INCLUDE ***************************/

// // // /* Include the implementation */

// // // #endif /* LOCKFREE_MPMC_QUEUE_HPP */

// // // namespace lockfree {
// // // namespace mpmc {
// // // /********************** PUBLIC METHODS ************************/

// // // template <typename T, size_t size>
// // // Queue<T, size>::Queue() : _r_count(0U), _w_count(0U) {}

// // // template <typename T, size_t size> bool Queue<T, size>::Push(const T &element) {
// // //     size_t w_count = _w_count.load(std::memory_order_relaxed);

// // //     while (true) {
// // //         const size_t index = w_count % size;

// // //         const size_t push_count =
// // //             _data[index].push_count.load(std::memory_order_acquire);
// // //         const size_t pop_count =
// // //             _data[index].pop_count.load(std::memory_order_relaxed);

// // //         if (push_count > pop_count) {
// // //             return false;
// // //         }

// // //         const size_t revolution_count = w_count / size;
// // //         const bool our_turn = revolution_count == push_count;

// // //         if (our_turn) {
// // //             /* Try to acquire the slot by bumping the monotonic write counter */
// // //             if (_w_count.compare_exchange_weak(w_count, w_count + 1U,
// // //                                                std::memory_order_relaxed)) {
// // //                 _data[index].val = element;
// // //                 _data[index].push_count.store(push_count + 1U,
// // //                                               std::memory_order_release);
// // //                 return true;
// // //             }
// // //         } else {
// // //             w_count = _w_count.load(std::memory_order_relaxed);
// // //         }
// // //     }
// // // }

// // // template <typename T, size_t size> bool Queue<T, size>::Pop(T &element) {
// // //     size_t r_count = _r_count.load(std::memory_order_relaxed);

// // //     while (true) {
// // //         const size_t index = r_count % size;

// // //         const size_t pop_count =
// // //             _data[index].pop_count.load(std::memory_order_acquire);
// // //         const size_t push_count =
// // //             _data[index].push_count.load(std::memory_order_relaxed);

// // //         if (pop_count == push_count) {
// // //             return false;
// // //         }

// // //         const size_t revolution_count = r_count / size;
// // //         const bool our_turn = revolution_count == pop_count;

// // //         if (our_turn) {
// // //             /* Try to acquire the slot by bumping the monotonic read counter. */
// // //             if (_r_count.compare_exchange_weak(r_count, r_count + 1U,
// // //                                                std::memory_order_relaxed)) {
// // //                 element = _data[index].val;
// // //                 _data[index].pop_count.store(pop_count + 1U,
// // //                                              std::memory_order_release);
// // //                 return true;
// // //             }
// // //         } else {
// // //             r_count = _r_count.load(std::memory_order_relaxed);
// // //         }
// // //     }
// // // }

// // // template <typename T, size_t size> bool Queue<T, size>::Peek() {
// // //     size_t r_count = _r_count.load(std::memory_order_relaxed);

// // //     return false;
// // // }

// // // } /* namespace mpmc */
// // // } /* namespace lockfree */


// // /*
// // ENQUEUE(x)
// //     q ← new record
// //     q^.value ← x
// //     q^.next ← NULL
// //     repeat
// //         p ← tail
// //         succ ← COMPARE&SWAP(p^.next, NULL, q)
// //         if succ ≠ TRUE
// //             COMPARE&SWAP(tail, p, p^.next)
// //     until succ = TRUE
// //     COMPARE&SWAP(tail,p,q)
// // end

// // DEQUEUE()
// //     repeat
// //         p ← head
// //         if p^.next = NULL
// //             error queue empty
// //     until COMPARE&SWAP(head, p, p^.next)
// //     return p^.next^.value
// // end



// // */

// // #include <atomic>
// // #include <memory>

// // template <typename T>
// // class FreeQueue {
// // public:
// //     FreeQueue() {
// //         Node* dummy = new Node(); // Dummy node
// //         head.store(dummy, std::memory_order_relaxed);
// //         tail.store(dummy, std::memory_order_relaxed);
// //     }

// //     ~FreeQueue() {
// //         while (pop()); // Pop all elements to prevent memory leak
// //         delete head.load();
// //     }

// // /*

// // ENQUEUE(x)
// //     q ← new record
// //     q^.value ← x
// //     q^.next ← NULL
// //     repeat
// //         p ← tail
// //         succ ← COMPARE&SWAP(p^.next, NULL, q)
// //         if succ ≠ TRUE
// //             COMPARE&SWAP(tail, p, p^.next)
// //     until succ = TRUE
// //     COMPARE&SWAP(tail,p,q)
// // end

// // */
// //     void push(const T& data) {
// //         Node* q = new Node(data);
// //         while (true) {
// //             Node* p = tail.load(std::memory_order_relaxed);
// //             bool succ = p->next.compare_exchange_weak(NULL, q, std::memory_order_release, std::memory_order_relaxed);
// //             if (!succ) {
// //                 tail.compare_exchange_weak(p, p->next);
// //                 // old_tail->next.store(new_node, std::memory_order_release);
// //                 break;
// //             } else {
// //                 break;
// //             }
// //         }
// //         tail.compare_exchange_weak(p, q, std::memory_order_release, std::memory_order_relaxed)
// //     }

// // /*
// // DEQUEUE()
// //     repeat
// //         p ← head
// //         if p^.next = NULL
// //             error queue empty
// //     until COMPARE&SWAP(head, p, p^.next)
// //     return p^.next^.value
// // end

// // */
// //     std::shared_ptr<T> pop() {

// //         Node *p = head.load(std::memory_order_relaxed);
// //         if (p->next == NULL) {
// //             return std::shared_ptr<T>();
// //         }

// //         while ()


// //         Node* old_head = head.load(std::memory_order_relaxed);
// //         while (old_head != tail.load(std::memory_order_acquire)) {
// //             if (head.compare_exchange_weak(old_head, old_head->next.load(),
// //                                            std::memory_order_release,
// //                                            std::memory_order_relaxed)) {
// //                 return old_head->data;
// //             }
// //         }
// //         return std::shared_ptr<T>();
// //     }

// //     std::shared_ptr<T> peek() const {
// //         Node* front_node = head.load(std::memory_order_relaxed)->next.load(std::memory_order_relaxed);
// //         return front_node ? front_node->data : std::shared_ptr<T>();
// //     }

// // private:
// //     struct Node {
// //         std::shared_ptr<T> data;
// //         std::atomic<Node*> next;

// //         Node() : next(nullptr) {}
// //         Node(const T& data) : data(std::make_shared<T>(data)), next(nullptr) {}
// //     };

// //     std::atomic<Node*> head;
// //     std::atomic<Node*> tail;
// // };



// // // #include <atomic>
// // // #include <memory>

// // // template <typename T>
// // // class FreeQueue {
// // // private:
// // //     struct Node {
// // //         std::shared_ptr<T> data;
// // //         std::atomic<Node*> next;
// // //         std::atomic<unsigned> version;

// // //         Node() : next(nullptr), version(0) {}
// // //     };

// // //     std::atomic<Node*> head;
// // //     std::atomic<Node*> tail;

// // // public:
// // //     FreeQueue() : head(new Node), tail(head.load()) {}

// // //     ~FreeQueue() {
// // //         // while (Node* const old_head = head.load()) {
// // //         //     head.store(old_head->next);
// // //         //     delete old_head;
// // //         // }
// // //     }

// // //     void push(T new_value) {
// // //         std::shared_ptr<T> new_data(std::make_shared<T>(new_value));
// // //         Node* const new_node = new Node();
// // //         new_node->data.swap(new_data);
// // //         Node* old_tail = tail.load();

// // //         while (true) {
// // //             Node* old_next = old_tail->next.load(); // Make old_next non-const
// // //             if (old_next == nullptr) {
// // //                 if (old_tail->next.compare_exchange_weak(old_next, new_node)) {
// // //                     break;
// // //                 }
// // //             } else {
// // //                 tail.compare_exchange_weak(old_tail, old_next);
// // //             }
// // //         }
// // //         tail.compare_exchange_weak(old_tail, new_node);
// // //     }

// // //     std::shared_ptr<T> pop() {
// // //         Node* old_head = head.load();
// // //         while (true) {
// // //             Node* next = old_head->next.load();
// // //             if (next == nullptr) {
// // //                 return std::shared_ptr<T>(); // Queue is empty
// // //             }
// // //             if (head.compare_exchange_weak(old_head, next)) {
// // //                 std::shared_ptr<T> res;
// // //                 res.swap(next->data);
// // //                 delete old_head;
// // //                 return res;
// // //             }
// // //         }
// // //     }

// // //     std::shared_ptr<T> peek() const {
// // //         Node* const front = head.load()->next.load();
// // //         return front ? front->data : std::shared_ptr<T>();
// // //     }

// // //     bool empty() const {
// // //         return head.load() == tail.load();
// // //     }
// // // };
// #pragma once

// #include <atomic>
// #include <cstddef>

// template <typename T> 
// class FreeQueue
// {
// public:
//   explicit FreeQueue(size_t capacity = 1000000)
//   {
//     _capacityMask = capacity - 1;
//     for(size_t i = 1; i <= sizeof(void*) * 4; i <<= 1)
//       _capacityMask |= _capacityMask >> i;
//     _capacity = _capacityMask + 1;

//     _queue = (Node*)new char[sizeof(Node) * _capacity];
//     for(size_t i = 0; i < _capacity; ++i)
//     {
//       _queue[i].tail.store(i, std::memory_order_relaxed);
//       _queue[i].head.store(-1, std::memory_order_relaxed);
//     }

//     _tail.store(0, std::memory_order_relaxed);
//     _head.store(0, std::memory_order_relaxed);
//   }

//   ~FreeQueue()
//   {
//     for(size_t i = _head; i != _tail; ++i)
//       (&_queue[i & _capacityMask].data)->~T();

//     delete [] (char*)_queue;
//   }
  
//   size_t capacity() const {return _capacity;}
  
//   size_t size() const
//   {
//     size_t head = _head.load(std::memory_order_acquire);
//     return _tail.load(std::memory_order_relaxed) - head;
//   }
  
//   bool push(const T& data)
//   {
//     Node* node;
//     size_t tail = _tail.load(std::memory_order_relaxed);
//     for(;;)
//     {
//       node = &_queue[tail & _capacityMask];
//       if(node->tail.load(std::memory_order_relaxed) != tail)
//         return false;
//       if((_tail.compare_exchange_weak(tail, tail + 1, std::memory_order_relaxed)))
//         break;
//     }
//     new (&node->data)T(data);
//     node->head.store(tail, std::memory_order_release);
//     return true;
//   }

//   bool pop(T& result)
//   {
//     Node* node;
//     size_t head = _head.load(std::memory_order_relaxed);
//     for(;;)
//     {
//       node = &_queue[head & _capacityMask];
//       if(node->head.load(std::memory_order_relaxed) != head)
//         return false;
//       if(_head.compare_exchange_weak(head, head + 1, std::memory_order_relaxed))
//         break;
//     }
//     result = node->data;
//     (&node->data)->~T();
//     node->tail.store(head + _capacity, std::memory_order_release);
//     return true;
//   }

// private:
//   struct Node
//   {
//     T data;
//     std::atomic<size_t> tail;
//     std::atomic<size_t> head;
//   };

// private:
//   size_t _capacityMask;
//   Node* _queue;
//   size_t _capacity;
//   char cacheLinePad1[64];
//   std::atomic<size_t> _tail;
//   char cacheLinePad2[64];
//   std::atomic<size_t> _head;
//   char cacheLinePad3[64];
// };


#pragma once

#include <atomic>
#include <cstddef>

template <typename T> 
class FreeQueue
{
public:
  explicit FreeQueue(size_t capacity = 1000000)
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