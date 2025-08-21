BUILDDIR:=.build

prep:
	mkdir -p $(BUILDDIR)

fibonacci: prep
	gcc additional/fibonacci/fib.c -o $(BUILDDIR)/fib

fibonacci_recursive: prep
	gcc additional/fibonacci/recursive_fib.c -o $(BUILDDIR)/fib_recursive

test_fibonacci: fibonacci
	bash tests/additional/fibonacci/test.sh tests/additional/fibonacci/fib.txt $(BUILDDIR)/fib

test_fibonacci_recursive: fibonacci_recursive
	bash tests/additional/fibonacci/test.sh tests/additional/fibonacci/fib.txt $(BUILDDIR)/fib_recursive

clean:
	rm -rf $(BUILDDIR)
