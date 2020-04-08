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


typedef struct __disk__{

} disk;


typedef struct _lsmtree{
	lsmnode *root;
	lsmnode **emptying_buffer;
	lsmnode **filling_buffer;
	bool *bloom_filter;
	_num emptying_buffer_pointer;
	_num filling_buffer_pointer;
	_num node_count;
	_lsmtree();
	bool _peek_bloom_filter(_key);
	void _update_bloom_filter(_key);
	void _delete_node(lsmnode*);
	void _write_to_file();
	void _emptying_buffer();
	void _update_priority(lsmnode*);
	bool _insert(_key, _value);
	bool _delete(_key);
	bool _update(_key, _value);
	_value _find(_key);
} lsmtree;
#endif /* LSMTREE_H_ */
