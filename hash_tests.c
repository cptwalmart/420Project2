#include "hash.h"
#include <stdio.h>
#include <stdlib.h>

void hashtable_print_contents(struct hashtable *h) {
  uint32_t bucket_idx;
  fprintf(stderr, "Hashtable at %p:\n", h);
  for(bucket_idx = 0; bucket_idx < HASH_BUCKETS; bucket_idx++) {
    if(h->buckets[bucket_idx]) {
      fprintf(stderr, "  Bucket %d:\n", bucket_idx);
      struct hashbucket *p = h->buckets[bucket_idx];
      while(p) {
	fprintf(stderr, "    Key '%s':\n", p->key);
	struct paper_list *l = p->value;
	while(l) {
	  fprintf(stderr, "      Paper '%s'\n", l->id);
	  l = l->next;
	}
	p = p->next;
      }
    }
  }
}

int main(void) {
  struct hashtable h;
  fprintf(stderr, "Initializing hashtable\n");
  hashtable_init(&h);
  hashtable_print_contents(&h);
  hashtable_append(&h, "Key 1", "Paper 1");
  hashtable_append(&h, "Key 1", "Paper 1");
  hashtable_append(&h, "Key 1", "Paper 2");
  hashtable_append(&h, "Key 2", "Paper 3");
  uint32_t target_hash = _hash("Key 1");
  char candidate[10];
  int i;
  for(i = 0; i < HASH_BUCKETS * 10; i++) {
    sprintf(candidate, "%x", i);
    if(_hash(candidate) == target_hash) break;
  }
  hashtable_append(&h, candidate, "Paper 4");
  hashtable_print_contents(&h);

  uint32_t length = hashtable_serialized_length(&h);
  char *serialized = calloc(length, 1);
  serialize_hashtable(&h, serialized);
  FILE *f = fopen("/tmp/serial", "w");
  fwrite(serialized, length, 1, f);
  fclose(f);

  struct hashtable h2;
  hashtable_init(&h2);
  deserialize_hashtable(serialized, &h2);
  hashtable_print_contents(&h2);
}
