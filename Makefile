
all: amount_indexed_test account_test calc
	@./calc

calc.cpp : calc-dist.cpp
	cp -n calc-dist.cpp calc.cpp

amount_indexed.obj: amount_indexed.cpp amount_indexed.hpp
	g++ --std=c++23 -c amount_indexed.cpp -o amount_indexed.obj

amount_indexed_test.obj: amount_indexed.hpp amount_indexed_test.cpp
	g++ --std=c++23 -c amount_indexed_test.cpp -o amount_indexed_test.obj

amount_indexed_test: amount_indexed.obj amount_indexed_test.cpp
	g++ --std=c++23 amount_indexed.obj amount_indexed_test.cpp -o amount_indexed_test
	./amount_indexed_test

account.obj: amount_indexed.hpp account.cpp account.hpp
	g++ --std=c++23 -c account.cpp -o account.obj

account_test.obj: account.hpp account_test.cpp
	g++ --std=c++23 -c account_test.cpp -o account_test.obj

account_test: amount_indexed.obj account.obj account_test.cpp
	g++ --std=c++23 amount_indexed.obj account.obj account_test.cpp -o account_test
	./account_test

calc.obj: amount_indexed.hpp account.hpp calc.cpp
	g++ --std=c++23 -c calc.cpp -o calc.obj

calc: amount_indexed.obj account.obj calc.obj
	g++ --std=c++23 amount_indexed.obj account.obj calc.obj -o calc

clean:
	rm -f *.obj *test calc
