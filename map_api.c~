#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "map_api.h"
#include "dijkstra.h"

void print_path (graph_t *g, int i) {
  int n, j;
  vertex_t *v, *u;
  v = g->vertices[i];
  if (v->dist == INT_MAX) {
    printf("no path\n");
    return;
  }
  for (n = 1, u = v; u->dist; u = g->vertices[u->prev], n++)
    ;/* Get the number of vertices in n */

    int *path = malloc(n);
  path[n - 1] = i;
  printf("Distance is %d points.\n ", v->dist); /* Prints the distance*/

  for (j = 0, u = v; u->dist; u = g->vertices[u->prev], j++)
  {
    path[n - j - 2] = u->prev;
  }

  for(j = 0; j < n; j++) /* Prints the trajectory */
  printf("%s--> ", g->vertices[path[j]]->name);
  printf("\n");
}

int main (int argc, char* argv[]) {
  char origin = (argv[1]);
  char destination = (argv[2]);

  graph_t *g = calloc(1, sizeof (graph_t));

/*  printf("Do you want to retrieve a map from a file? \nY-to retrieve from a file\nN- to use the heap.\n");
  char que[2]; FILE * fp;
  scanf("%s",que);
  if(que[0]=='Y' || que[0]=='y')
  {
  //Deserialization test
  printf("You are deserializing the map from a text file Map.txt \n");
  fp = fopen("map.txt", "r");
  //Uncomment the following line to deserialize the graph
  deserialize(g, fp);
  printf("Done! extracting the map\n");
  }
  else
  {
  fp = fopen("Map.txt", "w");
  }
  if (fp == NULL)
  {
    printf("Could not open file \n");
    return 0;
  } */
  //add_edge(graph, starting_vertex, name_start_vertex, vertex_type_strating, name_end_vert, type, Length, Speed_limit, direction)
  add_edge(g, 1, "DC",  1, 0, "RCH", 1, 5, 50, "Street 1",  EDGE_UNIDIRECTIONAL);
  add_edge(g, 1, "",    1, 2, "DPL", 1, 1, 50, "Street 2",  EDGE_UNIDIRECTIONAL);
  add_edge(g, 1, "",    1, 6, "PAC", 1, 1, 50, "Street 3",  EDGE_UNIDIRECTIONAL);
  add_edge(g, 2, "",    1, 6, "",    1, 4, 50, "Street 4",  EDGE_UNIDIRECTIONAL);
  add_edge(g, 3, "SLC", 1, 2, "",    1, 1, 50, "Street 5",  EDGE_UNIDIRECTIONAL);
  add_edge(g, 3, "",    1, 7, "SCH", 1, 3, 50, "Street 6",  EDGE_UNIDIRECTIONAL);
  add_edge(g, 3, "",    1, 5, "E5",  1, 7, 50, "Street 7",  EDGE_UNIDIRECTIONAL);
  add_edge(g, 5, "",    1, 3, "",    1, 4, 50, "Street 8",  EDGE_UNIDIRECTIONAL);
  add_edge(g, 6, "",    1, 5, "",    1, 3, 50, "Street 9",  EDGE_UNIDIRECTIONAL);
  add_edge(g, 4, "NP",  1, 4, "",    1, 0, 50, "Street 10", EDGE_UNIDIRECTIONAL);





  char  Starting_point[60],Destination[60];
 /* printf("Enter your Starting Point\n");
  printf("****************************\n");
  scanf("%s", &Starting_point[0]);
  printf("Enter your Destination\n");
  printf("****************************\n");
  scanf("%s", &Destination[0]); */
  #define DC 1 
  #define DPL 2
  #define E5 5
  #define SLC 3
  #define SCH 7
  #define NP 4
  #define PAC 6
  
  //TODO: Map the string with the integer and show the trip according to user input
  trip(g, 1, 7);
  //printf("****************************\n");
  print_path(g, 7);
 // printf("****************************\n");
//Test cases for the API
 // printf("Test cases for the API\n");
 // printf("****************************\n");
  for(int i = 0; i < g->vertices_len; i++)
  //  printf("Edge: %d, Name: %s\n", i, g->vertices[i]->name);

//Find Edge test
 // printf("****************************\n");
 // printf("Test case for non-available edge\n");
  //edge_t *my_edge;
  //my_edge = find_edge(g, 1, "Street 11");
 // if(my_edge != NULL)
  //  printf("Edge found. \n");
 // else
  //  printf("Edge not found. \n");

//Find vertex test
 // printf("****************************\n");
 // printf("Test case for non-available vertex\n");
  //vertex_t *my_vertex;
  //my_vertex = find_vertex(g, "BLA BLA");
  //if(my_edge != NULL)
  //  printf("Vertex found. \n");
  //else
   // printf("Vertex not found. \n");
  //printf("****************************\n");
//serialization test
  //serialize(g, fp);
 // printf("Graph serialized to a file\n");
 // printf("****************************\n");
 // printf("*****************************************************************************\n");



  return 0;
}
