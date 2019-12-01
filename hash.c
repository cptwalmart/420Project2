// hash.c

#include "hash.h"
#include <stdlib.h>
#include <string.h>

// ripped from Java stdlib
uint32_t _hash(const char *key) {
  uint32_t result = 1;
  while(*key) {
    result = 31 * result + *key;
    key++;
  }
  return result % HASH_BUCKETS;
}

void hashtable_init(struct hashtable *h) {
  h->buckets = calloc(HASH_BUCKETS, sizeof(struct hashbucket *));
}

void list_append(struct paper_list **l, const char *paper_id) {
  if(!*l) {
    *l = calloc(1, sizeof(struct paper_list));
  }
  struct paper_list *p = *l;
  struct paper_list *start = p;
  if(p->last) {
    if(!strcmp(p->last->id, paper_id)) return;
    p->last->next = calloc(1, sizeof(struct paper_list));
    p = p->last->next;
  }
  p->id = strdup(paper_id);
  start->last = p;
}

// normal linked list stuff, really
void hashtable_append(struct hashtable *h, const char *key, const char *paper_id) {
  uint32_t bucket_idx = _hash(key);
  if(h->buckets[bucket_idx]) {
    struct hashbucket *bucket, *last;
    for(bucket = h->buckets[bucket_idx]; bucket; bucket = bucket->next) {
      if(!strcmp(bucket->key, key)) {
 	list_append(&bucket->value, paper_id);
        return;
      }
      last = bucket;
    }
    last->next = calloc(1, sizeof(struct hashbucket));
    last->next->key = strdup(key);
    list_append(&last->next->value, paper_id);
  } else {
    h->buckets[bucket_idx] = calloc(1, sizeof(struct hashbucket));
    h->buckets[bucket_idx]->key = strdup(key);
    list_append(&h->buckets[bucket_idx]->value, paper_id);
  }
}

struct paper_list *hashtable_get(struct hashtable *h, const char *key) {
  uint32_t bucket_idx = _hash(key);
  if(h->buckets[bucket_idx]) {
    struct hashbucket *bucket;
    for(bucket = h->buckets[bucket_idx]; bucket; bucket = bucket->next) {
      if(!strcmp(bucket->key, key)) {
        return bucket->value;
      }
    }
  }
  return (void *) 0;
}

void hashtable_free(struct hashtable *h) {
  int i;
  for(i = 0; i < HASH_BUCKETS; i++) {
    if(h->buckets[i]) {
      struct hashbucket *bucket = h->buckets[i];
      while(bucket) {
        struct hashbucket *last = bucket;
        bucket = bucket->next;
        free(last->key);
        free(last->value); // if your values are complex types this may need to be modified
        free(last);
      }
    }
  }
  free(h->buckets);
}
