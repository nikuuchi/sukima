#include "lisp.h"

#define BINS 64

typedef enum lookupType {
	lookupFunction, lookupValue
} lookupType;

unsigned int getHashNumber(char * s, size_t len)
{
	int i = 0;
	unsigned int b = 0;
	for(i=0;i<len;++i) {
		b += s[i];
	}
	return (b + 31) % BINS;
}

st_table_t  *HashTable_init()
{
	st_table_t *table = (st_table_t *)malloc(sizeof(st_table_t));
	table->num_bins = BINS;
	table->num_entries = 0;
	table->bins = (st_table_entry_t **)calloc(BINS,sizeof(st_table_entry_t));
	return table;
}

void HashTable_free(st_table_t *self)
{
/*
	int i = 0;
	for(i=0;i<BINS;++i) {
		if(self->bins[i] != NULL){
			free(self->bins[i]);
		}
	}
*/
	free(self->bins);
	free(self);
}

void HashTable_insert(st_table_t *self, char *key, size_t len,lookupType flag,void *p)
{
	unsigned int hash_number = getHashNumber(key, len);
	if(self->bins[hash_number] != NULL) {
		st_table_entry_t *point = self->bins[hash_number];
		while(point->next != NULL) {
			if(strcmp(point->key,key) == 0) {
				point->v = (value_t *)p;
				goto end;
			}
			point = point->next;
		}
		if(strcmp(point->key,key) == 0) {
			point->v = (value_t *)p;
		}else{
			point->next = (st_table_entry_t *)malloc(sizeof(st_table_entry_t));
			if(flag == lookupValue) {
				point->next->hash = hash_number;
				point->next->key  = key;
				point->next->v = (value_t *)p;
			}else{
				point->next->hash = hash_number;
				point->next->key  = key;
				point->next->list = (list_run_t *)p;
			}
		}
	}else{
		self->bins[hash_number] = (st_table_entry_t *)malloc(sizeof(st_table_entry_t));
		if(flag == lookupValue) {
			self->bins[hash_number]->hash = hash_number;
			self->bins[hash_number]->key  = key;
			self->bins[hash_number]->v = (value_t *)p;
		}else{
			self->bins[hash_number]->hash = hash_number;
			self->bins[hash_number]->key  = key;
			self->bins[hash_number]->list = (list_run_t *)p;
		}
	}
  end:
	return;
}

void HashTable_insert_Function(st_table_t *self,char *key, size_t len, list_run_t *list)
{
	HashTable_insert(self,key,len,lookupFunction,list);
}
void HashTable_insert_Value(st_table_t *self,char *key, size_t len, value_t *v)
{
	HashTable_insert(self,key,len,lookupValue,v);
}


void *HashTable_lookup(st_table_t *self, char *key, size_t len,lookupType flag)
{
	unsigned int hash_number = getHashNumber(key, len);
	st_table_entry_t *stet = self->bins[hash_number];
	while(stet != NULL) {
		if(strcmp(stet->key,key) == 0) {
			break;
		}
			stet = stet->next;
	}
	if(stet == NULL){
		return HashTable_lookup(self->next,key,len,flag);
	}else{
		if(flag == lookupFunction){
			return stet->list;
		}
		return stet->v;
	}
}

list_run_t *HashTable_lookup_Function(st_table_t *self,char *key, size_t len)
{
	return (list_run_t *)HashTable_lookup(self,key,len,lookupFunction);
}

value_t *HashTable_lookup_Value(st_table_t *self,char *key, size_t len)
{
	return (value_t *)HashTable_lookup(self,key,len,lookupValue);
}


st_table_t *HashTable_createLocal(st_table_t *self)
{
	st_table_t *st = HashTable_init();
	st->next = self;
	return st;
}

st_table_t *HashTable_freeLocal(st_table_t *self)
{
	if(self->next != NULL){
		st_table_t *st = self->next;
		HashTable_free(self);
		return st;
	}else{
		return self;
	}
}
