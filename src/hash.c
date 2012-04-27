#include "lisp.h"

unsigned int getHashNumber(char * s, size_t len)
{
	uint32_t i = 0;
	unsigned int b = 0;
	for(i=0;i<len;++i) {
		b += s[i];
	}
	return (b + 31) % BINS;
}

hash_table_t  *HashTable_init()
{
	hash_table_t *table = (hash_table_t *)malloc(sizeof(hash_table_t));
	table->num_bins = BINS;
	table->num_entries = 0;
	table->bins = (hash_entry_t **)calloc(BINS,sizeof(hash_entry_t));
	return table;
}

void HashTable_free(hash_table_t *self)
{

	int i = 0;
	for(i=0;i<BINS;++i) {
		if(self->bins[i] != NULL){
			free(self->bins[i]->key);
			if(self->bins[i]->type == entryValue){
				free(self->bins[i]->v);
			}else{
				Command_free(self->bins[i]->list);
			}
			free(self->bins[i]);
		}
	}
	free(self->bins);
	free(self);
}

void HashTable_insert(hash_table_t *self, char *key, size_t len,entryType flag,void *p)
{
	unsigned int hash_number = getHashNumber(key, len);
	if(self->bins[hash_number] != NULL) {
		hash_entry_t *point = self->bins[hash_number];
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
			point->next = (hash_entry_t *)malloc(sizeof(hash_entry_t));
			if(flag == entryValue) {
				point->next->hash = hash_number;
				String_Copy(point->next->key,key,len);
				point->next->v = (value_t *)p;
				point->next->type = entryValue;
			}else{
				point->next->hash = hash_number;
				String_Copy(point->next->key,key,len);
				point->next->list = (command_t *)p;
				point->next->type = entryFunction;
			}
		}
	}else{
		self->bins[hash_number] = (hash_entry_t *)malloc(sizeof(hash_entry_t));
		if(flag == entryValue) {
			self->bins[hash_number]->hash = hash_number;
			String_Copy(self->bins[hash_number]->key,key,len);
			self->bins[hash_number]->v = (value_t *)p;
			self->bins[hash_number]->type = entryValue;
		}else{
			self->bins[hash_number]->hash = hash_number;
			String_Copy(self->bins[hash_number]->key,key,len);
			self->bins[hash_number]->list = (command_t *)p;
			self->bins[hash_number]->type = entryFunction;
		}
	}
  end:
	return;
}

void HashTable_insert_Function(hash_table_t *self,char *key, size_t len, command_t *list)
{
	HashTable_insert(self,key,len,entryFunction,list);
}
void HashTable_insert_Value(hash_table_t *self,char *key, size_t len, value_t *v)
{
	HashTable_insert(self,key,len,entryValue,v);
}


void *HashTable_lookup(hash_table_t *self, char *key, size_t len,entryType flag)
{
	unsigned int hash_number = getHashNumber(key, len);
	hash_entry_t *stet = self->bins[hash_number];
	while(stet != NULL) {
		if(strcmp(stet->key,key) == 0) {
			break;
		}
			stet = stet->next;
	}
	if(stet == NULL) {
		return HashTable_lookup(self->next,key,len,flag);
	}else {
		if(flag == entryFunction){
			return stet->list;
		}
		return stet->v;
	}
}

command_t *HashTable_lookup_Function(hash_table_t *self,char *key, size_t len)
{
	return (command_t *)HashTable_lookup(self,key,len,entryFunction);
}

value_t *HashTable_lookup_Value(hash_table_t *self,char *key, size_t len)
{
	return (value_t *)HashTable_lookup(self,key,len,entryValue);
}


hash_table_t *HashTable_createLocal(hash_table_t *self)
{
	hash_table_t *st = HashTable_init();
	st->next = self;
	return st;
}

hash_table_t *HashTable_freeLocal(hash_table_t *self)
{
	if(self->next != NULL){
		hash_table_t *st = self->next;
		HashTable_free(self);
		return st;
	}else{
		return self;
	}
}
