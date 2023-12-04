run:
	g++ -o main main.cpp main.hpp FreeStack.cpp LockStack.cpp FreeQueue.cpp -mcx16 -latomic -lpthread -g -std=c++17 && ./main