#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

typedef struct edge{
    int key;
    struct edge *next;
}edge;


typedef struct node{
    int key;
    struct node *next;
}node;


typedef struct graph{
    int nv; // number of vertices of the graph
    edge** adj; // array of pointers to adjacency lists
}graph;


typedef struct couple{
    int u;
    int v;
    struct couple *next;
}couple;


// Initializes an empty graph by returning a pointer to a graph with n empty adjacency lists.
graph* initEmptyGraph(int n){
    graph* G = (graph*)malloc(sizeof(graph));
    if(!G)
        printf("\033[0;31mERROR\033[0m: unable to allocate memory for the graph.\n");
    else{
        G->adj = (edge**)malloc(n * sizeof(edge*));
        if(G->adj == NULL){
            printf("\033[0;31mERROR\033[0m: unable to allocate memory for the adjacent list.\n");
            free(G);
            G = NULL;
        }
        else{
            G->nv = n;
            int i;
            for(i = 0; i < G->nv; ++i)
                G->adj[i] = NULL;
        }
    }
    return G;
}


// Checks wheter the graph is empty or not
int isEmpty(graph* G){
    return (G == NULL);
}


// Prints the graph
void printGraph(graph* G){
    edge* e;
    if(!isEmpty(G)){
        //printf("-----------------------------------------------------------\n");
        printf("number of vertices: %d\n", G->nv);
        int i, num_edge = 0;
        for(i = 0; i < G->nv; ++i){
            printf("\nG->adj");
            printf("[\033[0;33m%d\033[0m]", i);
            //printf(" -> ");
            e = G->adj[i];
            while(e != NULL){
                printf(" -> %d", e->key);
                num_edge = num_edge + 1;
                e = e->next;
            }
            printf("\n");
        }
        printf("\nnumber of edges: %d\n", num_edge);
        printf("-----------------------------------------------------------\n");
    }
    else
        printf("\nThere is no graph...\n");
}


// Adds an edge in the graph
int addEdge(graph* G, int u, int v){
    // Add code for controls
    edge* new_edge = malloc(sizeof(edge));
    if(new_edge == NULL){
        printf("\033[0;31mERROR\033[0m: Unable to allocate memory for a new edge.\n");
        return 0;
    }
    else{
        new_edge->key = v;
        new_edge->next = NULL;
        if(G->adj[u] == NULL){ // u vertex has no edges
            G->adj[u] = new_edge;
            printf("edge (%d, %d) has been added.\n", u, v);
            return 1;
        }
        else{
            edge* e = G->adj[u];
            while(e->next != NULL){
                if(e->key == new_edge->key){
                    printf("\033[0;31mERROR\033[0m: This is not a multigraph, cannot insert the same edge twice.\n");
                    return 0;
                }
                e = e->next;
            }
            e->next = new_edge;
            printf("edge (%d, %d) has been added.\n", u, v);
            return 1;
        }
    }
}



graph *randomize(graph *G) {
    int i, j;
    for(i = 0; i < G->nv; ++i){
        for(j = 0; j < G->nv; ++j){
            if(rand()%3 == 1){
                addEdge(G, i, j);
            }
        }
    }
    return G;
}


// Removes an edge
void removeEdge(graph* G, int u, int v){
    edge* prev, *e;
    e = G->adj[u];
    if(e->key == v)
        G->adj[u] = e->next;
    else{
        prev = e;
        while(prev->next->key != v)
            prev = prev->next;
        e = prev->next;
        prev->next = e->next;
    }
    free(e);
}



void insertHead(struct node** top, int x){
    node *tmp = malloc(sizeof(node));
    tmp->key = x;
    tmp->next = *top;
    *top = tmp;
}



void insertHead2(struct couple** cp, int x, int y){
    couple *tmp = malloc(sizeof(couple));
    tmp->u = x;
    tmp->v = y;
    tmp->next = *cp;
    *cp = tmp;
}



int searchG(graph* G, int u, int v){ // checks for every vertex v if there is an edge directed to u
    int res = 0;
    edge* e;
    for(e = G->adj[u]; e; e = e->next){
        if(e->key == v){
            res = 1;
            break;
        }
    }
    return res;
}



void printList(node** top){ // Prints the list containing the topological order
    if(*top != NULL){
        printf("%d, ", (*top)->key);
        printList(&(*top)->next);
    }
    //else printf("NULL\n");
}



void printList1(couple** cp){ // Prints the list containing directed edges
    if(*cp != NULL){
        printf(" -> (%d,%d)", (*cp)->u, (*cp)->v);
        printList1(&(*cp)->next);
    }
    //else printf("NULL\n");
}



int DFS_Visit1(graph* G, int i, int* aux, node** top, couple** cp){
    edge* e;
    int res = 1;
    aux[i] = 1;
    for(e = G->adj[i]; res && e; e = e->next){ // for each node in adjacency list
        if(aux[e->key] == 0){
            if(searchG(G, e->key, i)){
                insertHead2(cp, e->key, i); // inserting the couple of vertices of the edge into a list
                
                removeEdge(G, e->key, i);
                removeEdge(G, i, e->key);
            }
            else
                res = DFS_Visit1(G, e->key, aux, top, cp);
        }
        else if(aux[e->key] == 1) {
            printf("\033[0;31mERROR\033[0m: There is a cycle because of the directed edge (%d,%d)\n", i, e->key);
            return 0;
        }
    }
    aux[i] = 2;
    insertHead(top, i);
    return res;
}



int topologicalOrder(graph* G, node** top, couple** cp){
    int i, res = 1, *aux = calloc(G->nv, sizeof(int));
    if(!aux)
        printf("ERRORE DI ALLOCAZIONE\n");
    else{
        for(i = 0; res == 1 && i < G->nv; ++i){
            if(aux[i] == 0)
                res = DFS_Visit1(G, i, aux, top, cp);
        }
        printf("\n");
        free(aux);
    }
    return res;
}



void replaceEdges(graph* G, couple** cp, node** top){
    couple** cp_tmp = cp;
    node* top_tmp = *top;
    while(*cp_tmp != NULL){
        while(top_tmp != NULL){
            if(top_tmp->key == (*cp_tmp)->u){
                addEdge(G, (*cp_tmp)->u, (*cp_tmp)->v); // Adds node v to the list of u
                break;
            }
            else if(top_tmp->key == (*cp_tmp)->v){
                addEdge(G, (*cp_tmp)->v, (*cp_tmp)->u); // Adds node u to the list of v
                break;
            }
            top_tmp = top_tmp->next;
        }
        top_tmp = *top;
        *cp_tmp = (*cp)->next;
    }
}



void mixedToDAG(graph* G){ // Interface
    couple* cp = NULL;
    node* top = NULL;
    int res;
    res = topologicalOrder(G, &top, &cp);
    if(res) {
        printf("\n\033[0;33mTopological order\033[0m:\n");
        printList(&top); // gli mandiamo la copia dell'indirizzo della variabile puntatore top
        printf("\n");
        printf("\n\033[0;33mEdges to re-add to the graph\033[0m:\n");
        printList1(&cp);
        printf("\n");
        replaceEdges(G, &cp, &top);
        printGraph(G);
    }
}



void *removeAlledges(edge *top) {
    if(top != NULL) {
        removeAlledges(top->next);
        top->next = NULL;
        free(top);
    }
    return NULL;
}



void *deleteGraph(graph *G) {
    if(G != NULL && G->nv > 0) {
        G->adj[G->nv-1] = removeAlledges(G->adj[G->nv-1]);
        G->nv--;
        G = deleteGraph(G);
    }
    else if(G->nv == 0) {
        free(G->adj);
        G->adj = NULL;
        free(G);
    }
    return NULL;
}



graph *manualEdgeInsert(graph* G){
    char choice;
    int i = 0;
    int max_edges = (G->nv)*((G->nv) - 1);
    system("clear");
    printf("\033[0;33mGraph\033[0m (%d vertices)\n", G->nv);
    do{
        printf("Insert edge? (%d potential edges remaining) [y/n]: ", max_edges - i);
        scanf(" %c", &choice);
        if(choice == 'y'){
            int u, v;
            do{
                printf("Insert vertex \033[0;33mu\033[0m: ");
                scanf("%d", &u);
                if(u >= 0 && u < G->nv){
                    do{
                        printf("Insert vertex \033[0;33mv\033[0m: ");
                        scanf("%d", &v);
                        if(v >= 0 && v < G->nv){
                            i += addEdge(G, u, v);
                        }
                        else if(v > (G->nv) - 1)
                            printf("\033[0;31mERROR\033[0m: Invalid value for vertex v. The graph has only %d vertices. Please insert a valid input.\n", G->nv);
                        else if(v < 0){
                            printf("\033[0;31mERROR\033[0m: Invalid value for vertex v. First node is 0. Please insert a valid input.\n");
                        }
                    }while(v < 0 && v > G->nv);
                }
                else
                    printf("033[0;31mERROR\033[0m: Invalid value for vertex u. The graph has only %d vertices. Please insert a valid input.\n", G->nv);
                break;
            }while((u >= 0 && u < G->nv));
            
        }
        
    }while(choice == 'y' && i < max_edges);
    return G;
}



graph *createManualG(int n){
    graph* G = malloc(sizeof(graph));
    int i;
    if(G == NULL)
        printf("033[0;31mERROR\033[0m: It is not possible to allocate memory.\n");
    else{
        G->adj = (edge **) calloc(n, sizeof(edge *));
        if(G->adj == NULL){
            printf("033[0;31mERROR\033[0m: It is not possible to allocate memory.\n");
            free(G);
            G = NULL;
        }
        else
            G->nv = n;
    }
    
    G = manualEdgeInsert(G);
    return G;
}


void Intro_Es1(){
    do{
        printf("\t\t\t\t##############  \033[0;33mGRUPPO 5\033[0m  #################\n\t\t\t\t#####");
        printf("\tN86/1968  Scotti Vincenzo     #####\n\t\t\t\t#####\tN86/2121  Salzano Federico    ");
        printf("#####\n\t\t\t\t#####\tN86/1860  Ruggiero Gianmarco  #####\n");
        printf("\t\t\t\t###########################################\n\n");
        printf("\033[0;33mThis program transforms a mixed graph into a directed acyclic graph (DAG).\033[0m\n[press enter to continue] ");
    }while(getchar() != '\n');
    system("clear");
}





void Menu_Es1(){
    system("clear");
    int choice, n;
    graph *G = NULL;
    do{
        //system("clear");
        printf("\033[0;33mHow would you like to proceed?\033[0m\n");
        printf("1. Generate graph [Random]\n2. Generate graph [Manual]\n3. Execute the program\n4. Print Graph\n5. Terminate program\n");
        printf("Insert a value: ");
        scanf("%d", &choice);
        switch(choice){
            case 1: {
                if(!isEmpty(G)){
                    deleteGraph(G);
                    G = NULL;
                    system("clear");
                    do{
                        printf("Insert |V|: ");
                        scanf("%d", &n);
                        if(n < 1)
                            printf("\033[0;31mERROR\033[0m: the number of vertices of the graph has to be more than 0.\n");
                    }while(n < 1);
                    G = initEmptyGraph(n);
                    G = randomize(G);
                }
                else {
                    system("clear");
                    do{
                        printf("Insert |V|: ");
                        scanf("%d", &n);
                        if(n < 1)
                            printf("\033[0;31mERROR\033[0m: the number of vertices of the graph has to be more than 0.\n");
                    }while(n < 1);
                    G = initEmptyGraph(n);
                    G = randomize(G);
                }
                break;
            }
            case 2: {
                if(!isEmpty(G)){
                    deleteGraph(G);
                    G = NULL;
                    system("clear");
                    do{
                        printf("Insert |V|: ");
                        scanf("%d", &n);
                        if(n < 1)
                            printf("\033[0;31mERROR\033[0m: the number of vertices of the graph has to be more than 0.\n");
                    }while(n < 1);
                    G = initEmptyGraph(n);
                    printf("\n");
                    G = createManualG(n);
                }
                else {
                    system("clear");
                    do{
                        printf("Insert |V|: ");
                        scanf("%d", &n);
                        if(n < 1)
                            printf("\033[0;31mERROR\033[0m: the number of vertices of the graph has to be more than 0.\n");
                    }while(n < 1);
                    G = initEmptyGraph(n);
                    printf("\n");
                    G = createManualG(n);
                }
                break;
            }
            case 3: {
                if(G != NULL)
                    mixedToDAG(G);
                else{
                    system("clear");
                    printf("\033[0;31mERROR\033[0m: No graph allocated. Generate one first.\n");
                }
                break;
            }
            case 4: {
                if(G != NULL)
                    printGraph(G);
                else{
                    system("clear");
                    printf("\033[0;31mERROR\033[0m: No graph allocated. Generate one first.\n");
                }
                break;
            }
        }
  	 }while(choice >=1 && choice <= 4);
}
