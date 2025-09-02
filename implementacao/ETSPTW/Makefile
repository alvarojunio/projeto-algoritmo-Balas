INSTANCIA ?= ../../instancia_teste.txt

all: build-graph run-graph build-dijkstra run-dijkstra

build-graph:
	g++ -O3 -Wall -pedantic ETSPTW.cpp -o build-graph.bin

run-graph:
	./build-graph.bin < instancias/G/$(INSTANCIA) > graph.txt

build-dijkstra:
	g++ -O3 -Wall -pedantic dijkstra.cpp -o build-dijkstra.bin

run-dijkstra:
	./build-dijkstra.bin < graph.txt

clean:
	rm graph.txt build-dijkstra.bin build-graph.bin