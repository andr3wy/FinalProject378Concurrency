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
        new_node->next = head.load();
        while (!head.compare_exchange_strong(new_node->next, new_node));
    }

    std::shared_ptr<T> pop() {
        Node* old_head = head.load();
        while (old_head &&
               !head.compare_exchange_strong(old_head, old_head->next));
        return old_head ? old_head->data : std::shared_ptr<T>();
    }

    std::shared_ptr<T> peek() const {
        Node* top_node = head.load();
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