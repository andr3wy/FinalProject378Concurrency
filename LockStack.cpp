#include <exception>
#include <stack>
#include <mutex>
#include <stdlib.h>
#include <memory>
struct empty_stack: std::exception
{
    const char* what() const throw() {
        return NULL;
    }
    ~empty_stack() {

    }
};
template<typename T>
class LockStack
{
    private:
        std::stack<T> data;
        mutable std::mutex m;
    public:
    LockStack(){}
    LockStack(const LockStack& other)
    {
        std::lock_guard<std::mutex> lock(other.m);
        data=other.data;
    }
    virtual ~LockStack() {

    }
    LockStack& operator=(const LockStack&) = delete;
    void push(T new_value)
    {
        std::lock_guard<std::mutex> lock(m);
        data.push(std::move(new_value));
    }

    std::shared_ptr<T> peek()
    {
        std::lock_guard<std::mutex> lock(m);
        if(data.empty()) 
            return std::shared_ptr<T>();
        std::shared_ptr<T> const res(
        std::make_shared<T>(data.top()));
        return res;
    }

    std::shared_ptr<T> pop()
    {
        std::lock_guard<std::mutex> lock(m);
        if(data.empty()) 
            return std::shared_ptr<T>();
        std::shared_ptr<T> const res(
        std::make_shared<T>(std::move(data.top())));
        data.pop();
        return res;
    }
    void pop(T& value)
    {
        std::lock_guard<std::mutex> lock(m);
        if(data.empty()) 
            return;
        value=std::move(data.top());
        data.pop();
    }
    bool empty() const
    {
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }
};