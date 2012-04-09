#include "lisp.h"

#define BINS 64

unsigned int getHashNumber(st_table_t *self,char * s)
{
	unsigned int b = (s[0] + 31) % self->num_bins;
	return  b;
}

st_table_t  *HashTable_init()
{
	st_table_t *table = (st_table_t *)malloc(sizeof(st_table_t));
	table->num_bins = BINS;
	table->num_entries = 0;
	table->bins = (st_table_entry_t **)malloc(sizeof(st_table_t) * BINS);
	table->getHash = getHashNumber;
	return table;
}

void HashTable_free(st_table_t *self)
{

}
