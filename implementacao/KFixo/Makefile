INSTANCIA = rc_201.2.txt
build-graph:
	g++ -O3 -Wall -pedantic implementacao.cpp -o build-graph.bin
run-graph:
	./build-graph.bin < instancias/$(INSTANCIA) > graph.txt
build-dijkstra:
	g++ -O3 -Wall -pedantic dijkstra.cpp -o build-dijkstra.bin
run-dijkstra:
	./build-dijkstra.bin < graph.txt
clean:
	rm graph.txt build-dijkstra.bin build-graph.bin

