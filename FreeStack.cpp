// #include <memory>
// #include <atomic>
// #include <stdio.h>
// #include <stdlib.h>

// template<typename T>
// class FreeStack
// {
//     private:
//         struct node;

//         struct counted_node_ptr
//         {
//             int external_count;
//             node* ptr;
//         };

//         struct node
//         {
//             std::shared_ptr<T> data;
//             std::atomic<int> internal_count;
//             counted_node_ptr next;
//             node(T const& data_):
//             data(std::make_shared<T>(data_)),
//             internal_count(0)
//             {}
//         };

//         std::atomic<counted_node_ptr> head;

//         void increase_head_count(counted_node_ptr& old_counter)
//         {
//             counted_node_ptr new_counter;
//             do
//             {
//                 new_counter=old_counter;
//                 ++new_counter.external_count;
//             }
//             while(!head.compare_exchange_strong(old_counter,new_counter,
//             std::memory_order_acquire,
//             std::memory_order_relaxed));
//             old_counter.external_count=new_counter.external_count;
//         }
//     public:

//         void push(T const& data)
//         {
//             counted_node_ptr new_node;
//             new_node.ptr=new node(data);
//             new_node.external_count=1;
//             new_node.ptr->next=head.load(std::memory_order_relaxed);
//             while(!head.compare_exchange_weak(new_node.ptr->next,new_node,
//                     std::memory_order_release,
//                     std::memory_order_relaxed));
//         }

//         std::shared_ptr<T> peek() {
//             counted_node_ptr old_head=
//             head.load(std::memory_order_relaxed);

//             return std::shared_ptr<T>();
//         }
        

//         std::shared_ptr<T> pop()
//         {
//             counted_node_ptr old_head=
//             head.load(std::memory_order_relaxed);
//             for(;;)
//             {
//                 increase_head_count(old_head);
//                 node* const ptr=old_head.ptr;

//                 if(!ptr)
//                 {
//                     return std::shared_ptr<T>();
//                 }

//                 if(head.compare_exchange_strong(old_head,ptr->next,
//                     std::memory_order_relaxed))
//                 {
//                     std::shared_ptr<T> res;
//                     res.swap(ptr->data);
//                     int const count_increase=old_head.external_count-2;

//                     if(ptr->internal_count.fetch_add(count_increase,
//                         std::memory_order_release)==-count_increase)
//                     {
//                         delete ptr;
//                     }
//                     return res;
//                 } 
//                 else if (ptr->internal_count.fetch_add(-1, std::memory_order_relaxed)==1)
//                 {
//                     ptr->internal_count.load(std::memory_order_acquire);
//                     delete ptr;
//                 }
//             }
//         }
// };


#include <atomic>
#include <memory>

template <typename T>
class FreeStack {
public:
    FreeStack() : head(nullptr) {}

    ~FreeStack() {
        while (pop()); // Pop all elements to prevent memory leak
    }

    void push(const T& data) {
        Node* new_node = new Node(data);
        new_node->next = head.load(std::memory_order_relaxed);
        while (!head.compare_exchange_strong(new_node->next, new_node));
    }

    std::shared_ptr<T> pop() {
        Node* old_head = head.load(std::memory_order_relaxed);
        while (old_head &&
               !head.compare_exchange_strong(old_head, old_head->next));
        return old_head ? old_head->data : std::shared_ptr<T>();
    }

    std::shared_ptr<T> peek() const {
        Node* top_node = head.load(std::memory_order_relaxed);
        return top_node ? top_node->data : std::shared_ptr<T>();
    }

private:
    struct Node {
        std::shared_ptr<T> data;
        Node* next;

        Node(const T& data) : data(std::make_shared<T>(data)), next(nullptr) {}
    };

    std::atomic<Node*> head;
};