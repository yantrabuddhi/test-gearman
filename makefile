all:
		g++ -o client1 test-client.cpp -lgearman
		g++ -o worker1 test-worker.cpp -lgearman
