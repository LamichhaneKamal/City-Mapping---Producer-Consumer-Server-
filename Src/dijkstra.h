#ifndef DIJKSTRA_HEAD
#define DIJKSTRA_HEAD
#define MARKER -1

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "heap.h"

enum vertex_t {Intersection, POI, POI_at_intersection};

typedef struct {
  int vertex; /* Destiny (v). Edge is a member of origin (u) vertex, thus no need to specify origin. */
  int length; /* Distance from u to v */
  int speed_lim; /* Speed limit */
  char name[60];
  unsigned int events; /* Event flags */
} edge_t;

typedef struct {
  edge_t **edges; /* Pointer to an array of edges leaving vertex */
  int edges_len;  /* Number of edges leaving vertex */
  int edges_size;
  int dist;       /* Distance path from origin vertex */
  int prev;       /* Index of previous vertex on path */
  int visited;    /* Not zero if vertex was visited */
  enum vertex_t type; /* Type of vertex */
  char name[60]; /* Name of vertex */
} vertex_t;

typedef struct {
  vertex_t **vertices;
  int vertices_len;  /* Number of vertices in graph */
  int vertices_size; /* Graph capacity in number of vertices */
} graph_t;

/*--------------------------------- ADD VERTEX  --------------------------------*/
/* INPUTS: g  : Pointer to graph.
           i  : Integer index of the vertex
           n  : C string with the name of the vertex
           vt : Enum type of the vertex {INTERSECTION, PIO, POI_at_intersection}
   OPERATION: This function adds a new vertex to the specified graph. It is intended
   to be used by add_edge, but it can be called independently.
*/
void add_vertex (graph_t *g, int i, char *n, int vt);

/*--------------------------------- ADD EDGE ----------------------------------*/
/* INPUTS: g   : Pointer to graph.
           a   : Integer index of the starting vertex (where the edge starts).
           nv1 : Name of the starting vertex.
           vt1 : Vertex type of the starting vertex (see add_vertex).
           b   : Integer index of the ending vertex.
           nv2 : Name of the ending vertex.
           vt2 : Vertex type of the ending vertex.
           vt  : Enum type of the vertex {INTERSECTION, PIO, POI_at_intersection}
           l   : Integer edge length.
           sl  : Integer edge speed limit.
           en  : C string edge name.
           dir : int edge direction. Use EDGE_BIDIRECTIONAL or EDGE_UNIDIRECTIONAL.
   OPERATION: This function adds a new edge to the specified graph. Use it to
   populate the map. If a null string is specified for nv1 or nv2 the current
   name is unchanged; this feature allows the saving of vertex names only once
   when storing the map into a file.
*/
void add_edge (graph_t *g, int a, char *nv1, int vt1, int b, char *nv2, int vt2, int l, int sl, char *en, int dir);

/*------------------------------------ TRIP  ---------------------------------*/
/* INPUTS: g : Pointer to graph.
           a : Integer index of the vertex where the trip starts
           b : Integer index of the vertex where the trip ends
   OPERATION: This function evaluates the Dijkstra algorithm on graph g. The
   graph is left in a state that allows future reading of the shortest path. The
   path can be recovered in reverse order, i.e.: from b to a and the distance is
   stored in b->distance.
*/
void trip (graph_t *g, int a, int b);

/*----------------------------------- FIND EDGE  ------------------------------*/
/* INPUTS: g : Pointer to graph.
           v : Integer index of the vertex to which the edge belongs to.
   OUTPUT: A pointer to edge. NULL if no edge is found.
   OPERATION: Returns the pointer to the edge with specified name for vertex
   index v in graph g. If no edge is found the function returns NULL.
*/
edge_t *find_edge(graph_t *g, int v, char *n);

/*--------------------------------- FIND VERTEX -------------------------------*/
/* INPUTS: g : Pointer to graph.
           n : String with vertex name
   OUTPUT: A pointer to vertex with name n.
   OPERATION: Returns a pointer to the vertex with name n in graph g. Names must
   be unique, the function will search and return on first match. If no vertex is
   found the function returns NULL.
*/
vertex_t *find_vertex(graph_t *g, char *n);


/*--------------------------------- SERIALIZE -------------------------------*/

/* INPUTS: g : Pointer to graph.
           fp : pointer to the file to store the map
   OUTPUT: Stores the full map structure in the text file.
   OPERATION: Starts with MARKER (#define MARKER -1) and 
   store each vertex and the edge connecting to the other 
   vertex. Visits all the vertex and make the VISITED flag 
   non-zero if it is visited. It stops once all the vertexs are visited*/

void serialize (graph_t *g, FILE * fp) ;

/*--------------------------------- DESERIALIZE -------------------------------*/
/* INPUTS: g : Pointer to graph.
           fp : pointer to the file to retrieve the map
   OUTPUT: Retrieves the full map structure in the text file.
   OPERATION: Searches the MARKER (#define MARKER -1) and retrieve each 
   vertex and the edge connecting to the other vertex until the end of 
   the file. */
void deserialize (graph_t *g , FILE * fp);

/*--------------------------------- END-------------------------------*/


#define EDGE_BIDIRECTIONAL  1
#define EDGE_UNIDIRECTIONAL 2

#define EV_NO_EVENTS     0x00000000
#define EV_ROAD_CLOSED   0x00000001
#define EV_ACCIDENT      0x00000002
#define EV_CONSTRUCTION  0x00000004
#define EV_ROAD_JAMMED   0x00000008
#define EV_SPEED_REDUCED 0x00000010

#endif
