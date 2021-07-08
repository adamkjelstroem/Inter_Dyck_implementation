echo "compiling the files"
# echo "compiling utils.cpp"
# g++ -std=c++11 -I ./include -c src/utils/utils.cpp -o ./build/utils.o &&
# echo "compiling DSU.cpp" &&
# g++ -std=c++11 -I ./include -c src/DSU/DSU.cpp -o ./build/dsu.o &&
# echo "compiling graph.cpp" &&
# g++ -std=c++11 -I ./include -c src/graph/graph.cpp -o ./build/graph.o &&
# echo "compiling fdll.cpp" &&
# g++ -std=c++11 -I ./include -c src/utils/fdll.cpp -o ./build/fdll.o &&
# echo "compiling Ngraph.cpp" &&
# g++ -std=c++11 -I ./include -c src/graph/Ngraph.cpp -o ./build/Ngraph.o &&
# echo "compiling main.cpp" &&
# g++ -std=c++11 -ggdb -pg -I ./include src/main.cpp ./build/graph.o ./build/utils.o ./build/dsu.o ./build/Ngraph.o ./build/fdll.o -o ./build/main.out

mkdir -p build;
g++ -std=c++11 -I ./include src/main.cpp src/graph/Ngraph.cpp src/utils/fdll.cpp src/graph/graph.cpp src/DSU/DSU.cpp src/utils/utils.cpp -o ./build/main.out

exit $? 