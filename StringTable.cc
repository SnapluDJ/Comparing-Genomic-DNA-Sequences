// STRINGTABLE.CC
// A hash table mapping strings to their positions in the the pattern sequence
// You get to fill in the methods for this part.  Since we're passing around
// raw character strings, I've made the match length a parameter of the
// hash table so we don't have to keep figuring it out on each call.

#include <iostream>
#include <cmath>
#include <cstring>


#include "StringTable.h"

using namespace std;

/////////////////////////////
// HINT: the built-in C++ function strncmp 
// 	 (from the cstring library included above) may be used to 
// 	 compare two C-style strings of type (const char*) for equality.
// 	 format: strncmp(const char* s1, const char* s2, length) compares
// 	 up to length characters of s1 and s2, returning an int equal to
// 	 0 if they are identical and nonzero otherwise.
/////////////////////////////

//
// Create an empty table big enough to hold maxSize records.
//
StringTable::StringTable(int maxSize, int imatchLength): matchLength{imatchLength}
{
	tableSize = 2;
	recordNum = 0;

	hashTable = new Record*[tableSize];

	for(int i = 0; i < tableSize; i++){
		hashTable[i] = nullptr;
	}
}


//
// Insert a Record r into the table.  Return true if
// successful, false if the table is full.  You shouldn't ever
// get two insertions with the same key value, but you may
// simply return false if this happens.
//
bool StringTable::insert(Record *r)
{
	int hashKey;
	int hashVal1;
	int hashVal2;
	int s;

	// Check load factor
	while((float)recordNum/tableSize > 0.25)
		enlargeTable();

	hashKey = toHashKey(r->key);
	hashVal1 = baseHash(hashKey);
	hashVal2 = stepHash(hashKey);
	s = hashVal1 % tableSize;

	while (hashTable[s] != nullptr){
		if((hashKey == hashTable[s]->hashKey)
			&& (strncmp(hashTable[s]->key, r->key, matchLength) == 0))
			return false;
		s = ((s + hashVal2) % tableSize) % tableSize;
	}

	hashTable[s] = r;
	++recordNum;
	r->hashKey = hashKey;

	return true;
}


//
// Delete a Record r from the table.  Note that you'll have to
// find the record first unless you keep some extra info in
// the Record structure.
//
void StringTable::remove(Record *r)
{
	int hashKey;
	int hashVal1;
	int hashVal2;
	int s;

	hashKey = toHashKey(r->key);
	hashVal1 = baseHash(hashKey);
	hashVal2 = stepHash(hashKey);
	s = hashVal1 % tableSize;

	while (hashTable[s] != nullptr)
		if (this->find(r->key) != nullptr){
			Record *del = new Record("deleted",-128);
			hashTable[s] = del;
			--recordNum;
			break;
		}
}


//
// Find a record with a key matching the input.  Return the
// record if it exists, or nullptr if no matching record is found.
//
Record *StringTable::find(const char *key)
{
	int hashKey;
	int hashVal1;
	int hashVal2;
	int s;

	hashKey = toHashKey(key);
	hashVal1 = baseHash(hashKey);
	hashVal2 = stepHash(hashKey);
	s = hashVal1 % tableSize;

	if(hashTable[s] == nullptr)
		return nullptr;
	else
		while(true){
			if(hashKey == hashTable[s]->hashKey){
				if(strncmp(hashTable[s]->key, key, matchLength) !=0)
					s = ((s + hashVal2) % tableSize) % tableSize;
				else
					break;
			}
			else
				s = ((s + hashVal2) % tableSize) % tableSize;

		if(hashTable[s] == nullptr || s == hashVal1 % tableSize)
			return nullptr;
	}

	return hashTable[s];
}


//////////////////////////////////////////////////////////

// Convert a string key into an integer that serves as input to hash
// functions.  This mapping is based on the idea of a linear-congruential
// pesudorandom number generator, in which successive values r_i are 
// generated by computing
//    r_i = ( A * r_(i-1) + B ) mod M
// A is a large prime number, while B is a small increment thrown in
// so that we don't just compute successive powers of A mod M.
//
// We modify the above generator by perturbing each r_i, adding in
// the ith character of the string and its offset, to alter the
// pseudorandom sequence.
//
// NB: This fcn maps from space of arbitrary strings to ints, so
//     collisions are very possible and NOT resolved, i.e. two
//     distinct input strings may return the same value
//
int StringTable::toHashKey(const char *s) const
{
  int A = 1952786893;
  int B = 367257;
  int v = B;
  
  for (int j = 0; j < matchLength; j++)
    v = A * (v + int(s[j]) + j) + B;
  
  if (v < 0) v = -v;
  return v;

}


int StringTable::baseHash(int hashKey) const
{
	int index = -1;
	index = floor(tableSize*(hashKey*KNUTHVAL - floor(hashKey*KNUTHVAL)));
	return index;
}


int StringTable::stepHash(int hashKey) const
{
	int index = -1;
	index = floor(tableSize*(hashKey*((double)PI/4) - floor(hashKey*((double)PI/4))));
	if (index % 2 == 0)
		++index;
	return index;
}


void StringTable::enlargeTable()
{
	int hashVal1;
	int hashVal2;
	int s;

	tableSize *= 2;

	Record** tempTable = new Record* [tableSize];
	for(int i=0; i<tableSize; i++)
		tempTable[i] = nullptr;

	// Re-hashing
	for(int j=0; j<(tableSize/2); j++)
		if(hashTable[j] != nullptr){
			hashVal1 = baseHash(hashTable[j]->hashKey);
			hashVal2 = stepHash(hashTable[j]->hashKey);
			s = hashVal1 % tableSize;

			while (tempTable[s] != nullptr)
				s = ((s + hashVal2) % tableSize) % tableSize;

			tempTable[s] = hashTable[j];
		}

	hashTable = tempTable;
}
