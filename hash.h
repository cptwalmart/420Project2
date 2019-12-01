// hash.h

#ifndef HASH_H
#define HASH_H

#include <stdint.h>

#define HASH_BUCKETS 65536

struct paper_list {
  struct paper_list *next;
  struct paper_list *last;
  char *id;
};

struct hashbucket {
  struct hashbucket *next;
  char *key;
  struct paper_list *value;
};

struct hashtable {
  struct hashbucket **buckets;
};

void hashtable_init(struct hashtable *h);
void hashtable_append(struct hashtable *h, const char *key, const char *paper_id);
struct paper_list *hashtable_get(struct hashtable *h, const char *key);
void hashtable_free(struct hashtable *h);

#endif
