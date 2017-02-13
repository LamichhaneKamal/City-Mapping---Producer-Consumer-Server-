#include "dijkstra.h"

void add_vertex (graph_t *g, int i, char *n, int vt) {
  if (g->vertices_size < i + 1) {
    int size = g->vertices_size * 2 > i ? g->vertices_size * 2 : i + 4;
    g->vertices = realloc(g->vertices, size * sizeof (vertex_t *));
    for (int j = g->vertices_size; j < size; j++)
      g->vertices[j] = NULL;
    g->vertices_size = size;
  }
  if (!g->vertices[i]) {
    g->vertices[i] = calloc(1, sizeof (vertex_t));
    if(n[0] != '\0')
      strcpy(g->vertices[i]->name, n);

    g->vertices[i]->type = vt;
    g->vertices_len++;
  }
}

void add_edge (graph_t *g, int a, char *nv1, int vt1, int b, char *nv2, int vt2, int l, int sl, char *en, int dir) {
  add_vertex(g, a, nv1, vt1);
  add_vertex(g, b, nv2, vt2);

  /* For a unidirectional vertex, only add edge a,b */
  vertex_t *v = g->vertices[a];
  if (v->edges_len >= v->edges_size) {
    v->edges_size = v->edges_size ? v->edges_size * 2 : 4;
    v->edges = realloc(v->edges, v->edges_size * sizeof (edge_t *));
  }
  /* Allocate the edge and store its values */
  edge_t *e = calloc(1, sizeof (edge_t));
  e->vertex = b;
  e->length = l;
  e->speed_lim = sl;
  strcpy(e->name,en);
  v->edges[v->edges_len++] = e;

  /* For bidirectional edges also add path b,a using common properties */
  if (dir == EDGE_BIDIRECTIONAL) {
    v = g->vertices[b];
    if (v->edges_len >= v->edges_size) {
      v->edges_size = v->edges_size ? v->edges_size * 2 : 4;
      v->edges = realloc(v->edges, v->edges_size * sizeof (edge_t *));
    }
    /* Allocate the edge and store its values */
    e = calloc(1, sizeof (edge_t));
    e->vertex = a;
    e->length = l;
    e->speed_lim = sl;
    strcpy(e->name,en);
    v->edges[v->edges_len++] = e;
  }
}




void trip (graph_t *g, int a, int b) {
  int i, j;
  for (i = 0; i < g->vertices_len; i++){
    vertex_t *v = g->vertices[i];
    v->dist = INT_MAX;
    v->prev = 0;
    v->visited = 0;
  }
  vertex_t *v = g->vertices[a];
  v->dist = 0;
  heap_t *h = create_heap(g->vertices_len);
  push_heap(h, a, v->dist);
  while (h->len) {
    i = pop_heap(h);
    if (i == b)
      break;
    v = g->vertices[i];
    v->visited = 1;
    for (j = 0; j < v->edges_len; j++) {
      edge_t *e = v->edges[j];
      vertex_t *u = g->vertices[e->vertex];
      if (!u->visited && v->dist + e->length <= u->dist) {
        u->prev = i;
        u->dist = v->dist + e->length;
        push_heap(h, e->vertex, u->dist);
      }
    }
  }
}


edge_t *find_edge(graph_t *g, int v, char *n)
{
  if (!g->vertices[v])
    return NULL-1; /* There is no vertex with index v*/

  for (int i = 0; i < g->vertices[v]->edges_len; i++)
    {
      if(strcmp(g->vertices[v]->edges[i]->name, n) == 0)
        return g->vertices[v]->edges[i]; /* Return found edge */
    }
  return NULL; /* No edge with name n was found */
}

vertex_t *find_vertex(graph_t *g, char *n)
{
  for (int i = 0; i < g->vertices_len; i++)
    {
      if(strcmp(g->vertices[i]->name, n) == 0)
        return g->vertices[i]; /* Return found vertex*/
    }
  return NULL; /* No edge with name n was found */
}

void serialize (graph_t *g, FILE * fp) 
{
	vertex_t *v;
	edge_t *e;
    int j=0;
    int i = 0;
   // fprintf(fp, "%s\n", g);

  
    if (g->vertices_len == 0);
    {
    	fprintf(fp, "%d\n", MARKER);
    }
    //fprintf(fp, "Vertex_Name   Type(1-unidir,2-Bidir)\n");
    for (i = 0; i < g->vertices_len; i++) /* Run this for all the vertices to serialize the data from all the vertices*/
    {   	
    	if (g->vertices[i]->visited != 0) 
    	/*Check if the vertex is already visited, if not visited, run this to find all the connecting edges- edges-len 
    	gives the total number of edges connected to this particular vertex. */
    	{
    		for ( j=0; j < g->vertices[i]->edges_len; j++)
    		{
    			fprintf(fp, "%s     %d     %d     %d       %d\n",g->vertices[i]->name, g->vertices[i]->type, g->vertices[i]->edges_len, g->vertices[i]->edges_size,g->vertices_size);
 //fprintf(fp,"%d  %s  %d %d %s  %d %d %d  %s %d \n",g->vertices[i]->name );           
    		}
    	}
    	else {
    	//printf("All vertices are visited\n");
    	 }
    }
 }

 void deserialize (graph_t *g , FILE * fp)
 {
 	int ch=0;
    int lines=0;
    int a,vt1,b, vt2,l,sl;
	char *nv1;char *nv2;char *en; char * dir;
	int val;
 	while(!feof(fp))
 	{
 		ch = fgetc(fp);
 		if(ch == '\n')
 		{
 			lines++;
 		}
 	}

	if (!fscanf (fp, "%d", &val) || val == MARKER)
		return;
	printf("Total edges in the files %d\n", lines);
	/*Read next item from file, if no more items or next item is marker, then return*/
	int c;
    while ((c = fgetc(fp)) != EOF)
    {
    fscanf(fp, "%d,%s,%d,%d,%s,%d,%d,%d,%s,%s\n",&a, nv1, &vt1, &b, nv2, &vt2, &l, &sl, en, dir);
	printf("%d\n", a);
	add_edge (g, a,nv1,vt1, b,nv2,vt2,l,sl,en,dir);
     }
}
