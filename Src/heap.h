#ifndef HEAP_HEAD
#define HEAP_HEAD

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/*------------------------------- HEAP STRUCTURE ----------------------------*/
typedef struct {
  int *data; /*Dynamic array of element data */
  int *prio; /* Dynamic array of element priorities */
  int *index; /* Dynamic array of heap elements */
  int len; /* Number of allocated elements*/
  int size; /* Maximum heap capacity*/
} heap_t;


/*---------------------------- FUNCTION DECLARATIONS ------------------------*/
/*-------------------------------CREATE A HEAP ------------------------------*/
/* INPUT: Number of elements the heap will contain
   OUTPUT: Pointer to created heap
   OPERATION: This function creates a heap with n elements
*/
heap_t *create_heap (int n);

/*--------------------------------- MIN HEAP --------------------------------*/
/* INPUT: h :     Pointer to heap.
	  i,j,k : Three indexes of heap elements
   OUTPUT: min_heap : The element, among i,j,k with minimum priority in the heap
   OPERATION: This function finds the element, among i,j,k, with minimum priority
   in the heap. It does not return the minimum priority element in the heap,
   because that is at the top of the heap.
*/
int  min_heap (heap_t *h, int i, int j, int k);

/*-------------------------------- PUSH HEAP --------------------------------*/
/* INPUT: h : Pointer to heap
	  v : Element index
	  p : Element priority
   OUTPUT: NONE
   OPERATION: This function inserts an element in the heap and leaves the heap
   balanced.
*/
void push_heap (heap_t *h, int idx, int prio);

/*-------------------------------- POP HEAP ---------------------------------*/
/* INPUT: h : Pointer to heap
   OUTPUT: Element index at the top of the heap
   OPERATION: This function returns and deletes the top element in the heap
   and leaves the heap balanced
*/
int  pop_heap (heap_t *h);

#endif
