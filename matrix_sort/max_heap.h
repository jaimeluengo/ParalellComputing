 typedef struct max_heap {
	int* heap;
	int heapSize;
 } max_heap_t;

/*Initialize Heap*/
max_heap_t* HeapInit(int size);
/*Insert an element into the heap */
void Insert(max_heap_t* heap_struct, int element);
int DeleteMin(max_heap_t* heap_struct);
