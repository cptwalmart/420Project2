// hash.c

#include "hash.h"
#include <stdio.h>
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
  /* printf("Unable to find bucket\n"); */
  /* struct hashbucket *bucket; */
  /* bucket->value = -1; */
  /* return */ 
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

uint32_t _read_int(const char *s) {
  uint32_t i = 0;
  i |= ((uint8_t) *s++) << 0;
  i |= ((uint8_t) *s++) << 8;
  i |= ((uint8_t) *s++) << 16;
  i |= ((uint8_t) *s++) << 24;
  return i;
}

void _write_int(char *s, uint32_t i) {
  *s++ = i & 0xFF; i >>= 8;
  *s++ = i & 0xFF; i >>= 8;
  *s++ = i & 0xFF; i >>= 8;
  *s++ = i & 0xFF;
}

uint32_t string_serialized_length(const char *s) {
  return strlen(s) + 4;
}

void serialize_string(const char *s, char *out) {
  uint32_t string_length = strlen(s);
  _write_int(out, string_length);
  strcpy(out + 4, s);
}

uint32_t paper_list_serialized_length(struct paper_list *l) {
  uint32_t overall_length = 4;
  while(l) {
    overall_length += string_serialized_length(l->id);
    l = l->next;
  }
  return overall_length;
}

void serialize_paper_list(struct paper_list *l, char *out) {
  uint32_t items = 0;
  char *bufp = out + 4;
  while(l) {
    serialize_string(l->id, bufp);    
    bufp += string_serialized_length(l->id);
    items++;
    l = l->next;
  }
  _write_int(out, items);
}

uint32_t bucket_serialized_length(struct hashbucket *b) {
  uint32_t overall_length = 4;
  while(b) {
    overall_length += string_serialized_length(b->key);
    overall_length += paper_list_serialized_length(b->value);
    b = b->next;
  }
  return overall_length;
}

void serialize_bucket(struct hashbucket *b, char *out) {
  uint32_t items = 0;
  char *bufp = out + 4;
  while(b) {
    serialize_string(b->key, bufp);
    bufp += string_serialized_length(b->key);
    serialize_paper_list(b->value, bufp);
    bufp += paper_list_serialized_length(b->value);
    items++;
    b = b->next;
  }
  _write_int(out, items);
}

uint32_t hashtable_serialized_length(struct hashtable *h) {
  int i;
  uint32_t overall_length = 0;
  for(i = 0; i < HASH_BUCKETS; i++) {
    overall_length += bucket_serialized_length(h->buckets[i]);
  }
  return overall_length;
}

void serialize_hashtable(struct hashtable *h, char *out) {
  int i;
  char *bufp = out;
  for(i = 0; i < HASH_BUCKETS; i++) {
    serialize_bucket(h->buckets[i], bufp);
    bufp += bucket_serialized_length(h->buckets[i]);
  }
}

uint32_t deserialize_string(const char *serial, char **out) {
  uint32_t string_length = _read_int(serial);
  *out = calloc(string_length, 1);
  strncpy(*out, serial + 4, string_length);
  return string_length + 4;
}

void deserialize_hashtable(const char *serial, struct hashtable *target) {
  int i;
  for(i = 0; i < HASH_BUCKETS; i++) {
    int items = _read_int(serial);
    serial += 4;
    int j;
    for(j = 0; j < items; j++) {
      char *key;
      serial += deserialize_string(serial, &key);
      uint32_t items_2 = _read_int(serial);
      serial += 4;
      int k;
      for(k = 0; k < items_2; k++) {
	char *id;
	serial += deserialize_string(serial, &id);
	hashtable_append(target, key, id);
	free(id);
      }
      free(key);
    }
  }
}

