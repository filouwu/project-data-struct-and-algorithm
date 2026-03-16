#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>

#define ROTTE_MASSIME 5
#define NONVISITABILE INT_MAX
#define CACHE_CAPACITY 82  
#define CACHE_MISS INT_MIN

typedef struct Node {
    uint64_t key;
    int      value;
    struct Node *prev, *next;
} Node;

typedef struct {
    Node *head, *tail;
    int   size;
} DoublyLinkedList;

static Node* hashMap[CACHE_CAPACITY];
static DoublyLinkedList lru_list;

typedef struct {
    int costo;                   
    int rotte_aeree[ROTTE_MASSIME][2];    
    int rotte_aeree_cost[ROTTE_MASSIME];     
    int rotte_count;             
} Esagono;

typedef struct {
    int x,y;
    int distanza;
} Nodo;

typedef struct {
    Nodo* data;
    int size;
    int capacity;
} MinHeap;

Esagono *p_map = NULL;  
int  colonne = 0;   
int  righe = 0;    

int x_pari[6] = {+1,0,-1,-1,-1,0};
int y_pari[6] = {0,+1,+1,0,-1,-1};

int x_dispari[6]  = {+1,+1,0,-1,0,+1};
int y_dispari[6]  = {0,+1,+1,0,-1,-1};

static inline int hash_idx(uint64_t k){ return (int)(k % CACHE_CAPACITY); }

static Node* new_node(uint64_t k, int v){
    Node* n = malloc(sizeof(Node));
    if(!n) return NULL;
    n->key=k; n->value=v; n->prev=n->next=NULL;
    return n;
}
static void cache_init(void){
    memset(hashMap,0,sizeof(hashMap));
    lru_list.head=lru_list.tail=NULL;
    lru_list.size=0;
}
static void cache_clear(void){
    Node* cur=lru_list.head;
    while(cur){ Node* nxt=cur->next; free(cur); cur=nxt; }
    cache_init();
}
static void list_add_front(Node* n){
    n->prev=NULL; n->next=lru_list.head;
    if(lru_list.head) lru_list.head->prev=n;
    lru_list.head=n;
    if(!lru_list.tail) lru_list.tail=n;
    lru_list.size++;
}
static void list_remove(Node* n){
    if(n->prev) n->prev->next=n->next; else lru_list.head=n->next;
    if(n->next) n->next->prev=n->prev; else lru_list.tail=n->prev;
    lru_list.size--;
}
static void list_move_front(Node* n){
    if(lru_list.head==n) return;
    list_remove(n); list_add_front(n);
}
static void list_remove_lru(void){
    if(!lru_list.tail) return;
    Node* lru=lru_list.tail;
    int idx=hash_idx(lru->key);
    hashMap[idx]=NULL;
    list_remove(lru);
    free(lru);
}
static int cache_get(uint64_t k){
    int idx=hash_idx(k);
    Node* n=hashMap[idx];
    if(n && n->key==k){
        list_move_front(n);
        return n->value;
    }
    return CACHE_MISS;
}
static void cache_put(uint64_t k,int v){
    int idx=hash_idx(k);
    Node* n=hashMap[idx];
    if(n && n->key==k){ 
        n->value=v; list_move_front(n); return;
    }
    if(n){ list_remove(n); free(n); }
    if(lru_list.size==CACHE_CAPACITY) list_remove_lru();
    Node* nuovo=new_node(k,v);
    hashMap[idx]=nuovo;
    list_add_front(nuovo);
}

MinHeap* create_heap(int capacity) {
    MinHeap* heap = malloc(sizeof(MinHeap));
    if(heap == NULL) return NULL;

    heap->size = 0;
    heap->capacity = capacity;

    heap->data = malloc(sizeof(Nodo) * capacity);
    if(heap->data == NULL)return NULL;
    
    return heap;
}

void free_heap(MinHeap* heap) {
    free(heap->data);
    free(heap);
}

void push(MinHeap *heap, Nodo nodo)
{
    if (heap->size == heap->capacity) {
        int new_cap;

    if (heap->capacity != 0) {
    new_cap = heap->capacity * 2;
    } else {
    new_cap = 1;
    }
        Nodo *tmp = realloc(heap->data, new_cap * sizeof(Nodo));
        if (!tmp) return;
        heap->data     = tmp;
        heap->capacity = new_cap;
    }

    int i = heap->size++;
    heap->data[i] = nodo;

    while (i > 0) {
        int parent = (i - 1) / 2;
        if (heap->data[parent].distanza <= heap->data[i].distanza)
            break;               
        Nodo swap = heap->data[i];
        heap->data[i] = heap->data[parent];
        heap->data[parent] = swap;
        i = parent;
    }
}

Nodo pop(MinHeap* heap) {
    Nodo h = heap->data[0];
    heap->data[0] = heap->data[--heap->size];

    int i = 0;
    while (1) {
        int min = i;
        int left = 2 * i + 1, right = 2 * i + 2;

        if (left < heap->size && heap->data[left].distanza < heap->data[min].distanza)
            min = left;
        if (right < heap->size && heap->data[right].distanza < heap->data[min].distanza)
            min = right;
        if (min == i) break;

        Nodo tmp = heap->data[i];
        heap->data[i] = heap->data[min];
        heap->data[min] = tmp;

        i = min;
    }
    return h;
}

int verifica_bordi(int x, int y){
    return (x>=0 && x<colonne && y>=0 && y<righe);
}

void verifica_vicini(int x, int y){
    int *p_x, *p_y;
    if (y % 2 == 0){
        p_x = x_pari;
        p_y = y_pari;
    }
    else{
        p_x = x_dispari;
        p_y = y_dispari;
    }

    //printf("vicini(%d,%d):\n",x,y);
    for (int i=0; i<6; i++) {
        int vicini_x = x+p_x[i];
        int vicini_y = y+p_y[i];
        if (verifica_bordi(vicini_x, vicini_y)) {
           // printf("(%d, %d)\n", vicini_x, vicini_y);
        }
    }
}

Esagono *posizione_esagono(int x, int y){
    if (p_map==NULL || !verifica_bordi(x,y))
        return NULL;
    return &p_map[(y*colonne)+x];
}

void free_grid(void){
    free(p_map);
    p_map = NULL;
    righe = colonne = 0;
}

void init(int c, int r)
{
    if (p_map)
        free_grid();

    colonne = c;
    righe   = r;

    size_t n = (size_t)colonne * righe;
    p_map = calloc(n, sizeof(Esagono));
    if (!p_map)
        return;

    for (size_t i = 0; i < n; ++i) {
        p_map[i].costo = 1;
    }   

    cache_clear();         
}


int calcolo_idx(int x, int y) {
    return y * colonne + x;
}

static uint64_t cantor(uint64_t a, uint64_t b) {
    uint64_t s = (a+b);
    return (s*(s + 1))/2 + b;   
}

static uint64_t calcolo_key(uint32_t xp, uint32_t yp,
                                       uint32_t xd, uint32_t yd)
{
    uint64_t k1 = cantor(xp, yp);
    uint64_t k2 = cantor(xd, yd);
    return cantor(k1, k2);
}

int travel_cost(int xp, int yp, int xd, int yd)
{
    uint64_t key = calcolo_key((uint32_t)xp, (uint32_t)yp,
                               (uint32_t)xd, (uint32_t)yd);

    int cached = cache_get(key);
    if (cached != CACHE_MISS)          
        return cached;

    if (!verifica_bordi(xp, yp) || !verifica_bordi(xd, yd))
        return -1;
    if (xp == xd && yp == yd) {
        cache_put(key, 0);
        return 0;
    }

    const int n = colonne * righe;
    int  *dist = malloc(n * sizeof(int));
    char *vis  = calloc(n, sizeof(char));
   
    for (int i = 0; i < n; ++i) dist[i] = NONVISITABILE;

    MinHeap *heap = create_heap(n);

    int sidx = yp * colonne + xp;
    dist[sidx] = 0;
    push(heap, (Nodo){xp, yp, 0});

    while (heap->size) {
        Nodo node = pop(heap);
        int idx   = node.y * colonne + node.x;
        if (vis[idx]) continue;
        vis[idx] = 1;

        if (node.x == xd && node.y == yd) break;

        Esagono *e = &p_map[idx];
        if (e->costo == 0) continue;            

        int *px, *py;
        if (node.y % 2 == 0) { px = x_pari;    py = y_pari;    }
        else                 { px = x_dispari; py = y_dispari; }

        for (int i = 0; i < 6; ++i) {
            int nx = node.x + px[i];
            int ny = node.y + py[i];
            if (!verifica_bordi(nx, ny)) continue;

            int vidx = ny * colonne + nx;
            int nuovo = dist[idx] + e->costo;
            if (nuovo < dist[vidx]) {
                dist[vidx] = nuovo;
                push(heap, (Nodo){nx, ny, nuovo});
            }
        }
        for (int i = 0; i < e->rotte_count; ++i) {
            int nx = e->rotte_aeree[i][0];
            int ny = e->rotte_aeree[i][1];
            int costo_rotta = e->rotte_aeree_cost[i];
            if (!verifica_bordi(nx, ny)) continue;

            int vidx = ny * colonne + nx;
            int nuovo = dist[idx] + costo_rotta;
            if (nuovo < dist[vidx]) {
                dist[vidx] = nuovo;
                push(heap, (Nodo){nx, ny, nuovo});
            }
        }
    }

    int risultato = dist[yd * colonne + xd];
    if (risultato == NONVISITABILE) risultato = -1;

    free(dist);
    free(vis);
    free_heap(heap);

    if (risultato >= 0) cache_put(key, risultato);
    return risultato;
}


void toggle_air_route(int x1, int y1, int x2, int y2) {
    if (!verifica_bordi(x1, y1) || !verifica_bordi(x2, y2)) {
        printf("KO\n");
        return;
    }

    Esagono *src = &p_map[calcolo_idx(x1, y1)];
  
    for (int i = 0; i < src->rotte_count; i++){
        if (src->rotte_aeree[i][0] == x2 && src->rotte_aeree[i][1] == y2){
            for (int j = i; j<src->rotte_count - 1; j++) {
                src->rotte_aeree[j][0] = src->rotte_aeree[j+1][0];
                src->rotte_aeree[j][1] = src->rotte_aeree[j+1][1];
                src->rotte_aeree_cost[j] = src->rotte_aeree_cost[j+1];
            }
            src->rotte_count--;
            printf("OK\n");
            return;
        }
    }

    if (src->rotte_count >= ROTTE_MASSIME) {
        printf("KO\n");
        return;
    }

    int somma = src->costo;
    for (int i = 0; i < src->rotte_count; i++)
        somma += src->rotte_aeree_cost[i];

    int nuovo_costo = (somma/(src->rotte_count+1));

    src->rotte_aeree[src->rotte_count][0] = x2;
    src->rotte_aeree[src->rotte_count][1] = y2;
    src->rotte_aeree_cost[src->rotte_count] = nuovo_costo;
    src->rotte_count++;

    
    printf("OK\n");
    cache_clear();
}

void change_cost(int x, int y, int v, int raggio) {
    if (!verifica_bordi(x, y) || raggio <= 0 || v < -10 || v > 10) {
        printf("KO\n");
        return;
    }

    int n = colonne * righe;
    int *visitato = calloc(n, sizeof(int));
    if (!visitato) return;

    Nodo *queue = malloc(n * sizeof(Nodo));
    if (!queue) { free(visitato); return; }

    int front = 0, back = 0;
    queue[back++] = (Nodo){x, y, 0};
    visitato[calcolo_idx(x, y)] = 1;  

    while (front < back) {
        Nodo curr = queue[front++];
        Esagono *e = &p_map[calcolo_idx(curr.x, curr.y)]; 

        int num = v * (raggio - curr.distanza);
        int delta;
        if (num >= 0) delta = num / raggio;
        else { delta = num / raggio; if (num % raggio) delta -= 1; }

        if (delta != 0) {
            int nuovo = e->costo + delta;
            if (nuovo < 0) nuovo = 0;
            if (nuovo > 100) nuovo = 100;
            e->costo = nuovo;

            for (int i = 0; i < e->rotte_count; i++) {
                int rc = e->rotte_aeree_cost[i] + delta;
                if (rc < 0) rc = 0;
                if (rc > 100) rc = 100;
                e->rotte_aeree_cost[i] = rc;
            }
        }

        if (curr.distanza >= raggio) continue;

        int *px, *py;
        if (curr.y % 2 == 0) { px = x_pari; py = y_pari; }
        else                 { px = x_dispari; py = y_dispari; }

        for (int i = 0; i < 6; i++) {
            int nx = curr.x + px[i];
            int ny = curr.y + py[i];
            if (!verifica_bordi(nx, ny)) continue;

            int idn = calcolo_idx(nx, ny);
            if (visitato[idn]) continue;

            visitato[idn] = 1;
            queue[back++] = (Nodo){nx, ny, curr.distanza + 1};
        }
    }

    free(queue);
    free(visitato);
    printf("OK\n");
    cache_clear();
}


int main(){
    char comando[32];
        cache_init();          
    while (scanf("%s", comando) != EOF) {
        if (strcmp(comando, "init") == 0) {
            int c, r;
            if (scanf("%d %d", &c, &r) != 2) return 1;
            init(c, r);
            printf("OK\n");
        } else if (strcmp(comando, "change_cost") == 0) {
            int x, y, v, raggio;
            if (scanf("%d %d %d %d", &x, &y, &v, &raggio) != 4) return 1;
            change_cost(x, y, v, raggio);
        } else if (strcmp(comando, "toggle_air_route") == 0) {
            int x1, y1, x2, y2;
            if (scanf("%d %d %d %d", &x1, &y1, &x2, &y2) != 4) return 1;
            toggle_air_route(x1, y1, x2, y2);
        } else if (strcmp(comando, "travel_cost") == 0) {
            int xp, yp, xd, yd;
            if (scanf("%d %d %d %d", &xp, &yp, &xd, &yd) != 4) return 1;
            int risultato = travel_cost(xp, yp, xd, yd);
            printf("%d\n", risultato);
        } else {
            printf("KO\n");
            while (getchar() != '\n'); 
        }
    }
    free_grid(); 
        cache_clear();

    return 0;
}