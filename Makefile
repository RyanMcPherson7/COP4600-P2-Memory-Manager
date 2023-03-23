make: CommandLineTest.cpp
	c++ -std=c++17 -o test CommandLineTest.cpp -L ./MemoryManager -lMemoryManager
leaks: CommandLineTest.cpp
	c++ -std=c++17 -o test CommandLineTest.cpp -L ./MemoryManager -lMemoryManager -fsanitize=address -g