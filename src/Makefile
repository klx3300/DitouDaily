
main:recolic_main.o processor.o analyser.o
	g++ -o main recolic_main.o processor.o analyser.o

recolic_main.o: recolic_main.cpp statements.hpp stdafx_.hpp
	g++ -c -o recolic_main.o recolic_main.cpp

processor.o: processor_debug.cpp processor.hpp statements.hpp stdafx_.hpp
	g++ -c -o processor.o processor_debug.cpp

analyser.o: analyser.cpp analyser.hpp statements.hpp stdafx_.hpp
	g++ -c -o analyser.o analyser.cpp

clean:
	rm *.o
	rm main
