GXX = g++ -std=gnu++98 -ggdb3 -Wall -Werror -pedantic -o $@
.PHONY: clean
test_replacer: replacer.h testReplacer.cpp
	$(GXX) testReplacer.cpp

test_class: cache.h stghold.h cmdCenter.h replacer.h test_class.cpp cmdCenter.cpp stghold.cpp cache.cpp
	$(GXX) test_class.cpp cmdCenter.cpp stghold.cpp cache.cpp
clean:
	rm *~ 
