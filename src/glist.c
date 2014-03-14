#include "ropc.h"

#define GLIST_TABLE_SIZE 0x8000

static uint32_t glist_hash(const char *comment) {
  uint32_t hash;
  const uint8_t *p = (const uint8_t*)comment;
  hash = 0x1337;

  while(*p) {
    hash = (hash << 8) | (hash >> 24);
    hash ^= *p;
    p++;
  }
  return hash % GLIST_TABLE_SIZE;
}

void glist_foreach(GLIST *glist, void(*callback)(GADGET*)) {
  int i;
  GADGET *g;

  for(i = 0; i < GLIST_TABLE_SIZE; i++) {
    g = glist->g_table[i];
    while(g != NULL) {
      callback(g);
      g = g->next;      
    }
  }
}

/* Free gadget list */
void glist_free(GLIST **glist) {
  GADGET *g, *tmp;
  int i;

  for(i = 0; i < GLIST_TABLE_SIZE; i++) {
    g = (*glist)->g_table[i];
    while(g != NULL) {
      tmp = g->next;
      free(g);
      g = tmp;      
    }
  }

  free((*glist)->g_table);
  free(*glist);
  *glist = NULL;
}

GLIST* glist_new(void) {
  GLIST *glist;

  glist = xcalloc(1, sizeof(GLIST));
  glist->g_table = xcalloc(GLIST_TABLE_SIZE, sizeof(GADGET*));
  
  return glist;
}

/* Add a gadget to the GADGETS list */
void glist_add(GLIST *glist, GADGET *g) {
  GADGET *new;
  uint32_t hash;

  new = xmalloc(sizeof(*new));

  memcpy(new, g, sizeof(GADGET));

  hash = glist_hash(new->comment);

  new->next = glist->g_table[hash];
  glist->g_table[hash] = new;
  glist->size++;
}

GADGET* glist_find(const GLIST *glist, int (*compare)(GADGET*, const void*), const void *user) {
  GADGET *g;
  int i;

  for(i = 0; i < GLIST_TABLE_SIZE; i++) {
    g = glist->g_table[i];
    while(g != NULL) {
      if(compare(g, user))
	return g;
      g = g->next;      
    }
  }
  return NULL;
}

int glist_exist(GLIST *glist, const char *comment) {
  GADGET *g;
  uint32_t hash;

  hash = glist_hash(comment);

  for(g = glist->g_table[hash]; g != NULL; g = g->next) {
    if(!strcmp(g->comment, comment))
      return 1;
  }
  return 0;
}

int glist_size(GLIST *glist) {
  return glist->size;
}
