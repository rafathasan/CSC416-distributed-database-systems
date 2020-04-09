/*
 * lsmdef.h
 *
 *  Created on: Apr 7, 2020
 *      Author: rafikhan
 */

#ifndef LSMTREE_H_
#define LSMTREE_H_


typedef unsigned int _key;
typedef unsigned int _value;
typedef unsigned int _num;
typedef unsigned int _weight;

typedef struct _lsmnode{
	_key key;
	_value value;
	_weight l_weight;
	_weight r_weight;
	_num index;
	bool deleted;
	_lsmnode *l;
	_lsmnode *r;
	_lsmnode *p;
	_lsmnode();
	signed int getWeight();
} lsmnode;



typedef struct _lsmtree{
	lsmnode *root;
	lsmnode **emptying_buffer;
	lsmnode **filling_buffer;
	bool *bloom_filter;
	_num emptying_buffer_pointer;
	_num filling_buffer_pointer;
	_num node_count;
	_lsmtree();
	_lsmtree(_num, _num, _num);
	bool _peek_bloom_filter(_key);
	void _update_bloom_filter(_key);
	void _write_to_file();
	void _emptying_buffer();
	void _update_priority(lsmnode*);
	bool _insert(_key, _value);
	int _delete(_key);
	bool _update(_key, _value);
	_value _find(_key);
} lsmtree;


struct map{
	unsigned int pos[255] = {0};
};

struct inode{
	_key blocks_key[255] = {0};
};

typedef struct _disk{
	char* FILE;
	_disk();
	void _merge_sort(lsmnode**,_num);
	void _write_buffer_to_disk(lsmnode**, _num);
	friend lsmtree;
} disk;
#endif /* LSMTREE_H_ */
