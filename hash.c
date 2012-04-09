#include "lisp.h"

#define BINS 64

st_table  *HashTable_init()
{
	st_table *table = (st_table *)malloc(sizeof(st_table));
	table->num_bins = BINS;
	table->num_entries = 0;	
	table->bins = (st_table_entry **)malloc(sizeof(st_table) * BINS);
	table->getHash = getHashNumber;
	return table;
}

unsigned int getHashNumber(st_table *self,char * s)
{
	unsigned int b = (s[0] + 31) % self->num_bins;
	return  b;
}

void HashTable_free(st_table *self)
{

}
