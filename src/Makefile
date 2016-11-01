
CPPFLAGS = -std=c++11

SeedCup2016.exe:recolic_main.o processor.o analyser.o
	g++ $(CPPFLAGS) -o SeedCup2016.exe recolic_main.o processor.o analyser.o

recolic_main.o: recolic_main.cpp statements.hpp stdafx_.hpp
	g++ $(CPPFLAGS) -c -o recolic_main.o recolic_main.cpp

processor.o: processor.cpp statements.hpp stdafx_.hpp processor.hpp
	g++ $(CPPFLAGS) -c -o processor.o processor.cpp

analyser.o: analyser.cpp analyser.hpp statements.hpp stdafx_.hpp
	g++ $(CPPFLAGS) -c -o analyser.o analyser.cpp

clean:
	rm *.o
	rm SeedCup2016.exe
