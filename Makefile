
all: merge
merge: merge-lab4.c
	gcc merge-lab4.c -o normal_sort
	./normal_sort

thread: merge-lab4.c
	gcc -pthread merge-lab4.c -o thread_sort
	./thread_sort
