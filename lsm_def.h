/*
 * lsm_def.h
 *
 *  Created on: Apr 4, 2020
 *      Author: rafikhan
 */

#ifndef LSM_DEF_H_
#define LSM_DEF_H_

#include <fstream>

typedef unsigned int _key;
typedef unsigned int _value;
typedef unsigned int _pid; //4 bytes page id
typedef unsigned int _psize;

const _key EMPTY_KEY = -1;
const int _page_size = 1 << 18; //256 Kilobytes. best optimal size is 256 Kilobyte
const int _block_size = 1 << 11; //4 Kilobytes per block

struct Node {
	_key key = EMPTY_KEY;
	_value value;
};



struct Page {
	// fields order are kept in the files same as below
	_pid pid;
	_psize next_idx;
	bool sorted;
	bool extended;
	bool empty;
	_psize count;
	Node data[_max_node_per_page];
public:
	Page();
	char* getid();
	void init();
	char* getbuffer();
	void setbuffer(char*);

	friend class DiskOperation;
};

struct DiskOperation {
	Page *page;
	std::fstream fs;
public:
	DiskOperation();
	void getmeta();
	void write_file();
	void read_file();
	void _fill_random();
	void _insert(_key key, _value value);
	void _insert(Node *buffer, _psize count);
	void _heap_sort(Node *buffer, _psize count);
	bool _delete(_key key);
	bool _update(_key key, _value value);
	_value _find(_key key);
	_key _logn_search(_key key, _key l, _key r);
	void _defrag_sort();
	void _view();
};



struct Block {
	Node block[_max_node_per_tree];
	_psize depth;
	_psize size = 0;
	bool isEmpty(){
		return size;
	}
	bool isFull(){
		return size == _max_node_per_block;
	}
	void _sort();
};

class BufferOperation {
	Block buffer;
	bool isBufferEmpty;
	Node *tree;
	_key tree_root;
	_psize tree_size;
public:
	BufferOperation();
	Node*& getTree();
	void _insert(_key, _value);
	void _update();
	void _find();
	void _delete();
	void _sort_tree_depth();
	void _view();
	void _flush();
	_key _logn_insert(_key key, _key l, _key r);
};

class LSMOperation {
	DiskOperation disk;
	BufferOperation buffer;
	bool b_fillter[_max_node_per_tree];
public:
	LSMOperation();
	void _init();
	void _insert();
	void _delete();
	void _update();
	void _find();
	void _load();
	void _stat();
};



#endif /* LSM_DEF_H_ */
