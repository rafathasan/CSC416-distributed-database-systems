/*
 * lsm.h
 *
 *  Created on: Apr 5, 2020
 *      Author: rafikhan
 */

#ifndef LSM_H_
#define LSM_H_

typedef unsigned int _key;
typedef unsigned int _value;
typedef unsigned int _pid; //4 bytes page id
typedef unsigned int _psize;

const _key EMPTY_KEY = -1;
const int _page_size = 1 << 18; //256 Kilobytes. best optimal size is 256 Kilobyte
const int _block_size = 1 << 11; //4 Kilobytes per block

struct Node {
	_key key;
	_value value;
	Node(){
		key = EMPTY_KEY;
		value = EMPTY_KEY;
	}
};

const _psize _max_node_per_block = _block_size / sizeof(Node); //256
const _psize _max_node_per_page = _page_size/ sizeof(Node);
const _psize _max_block_per_page = _page_size/_max_node_per_block;
const _psize _max_node_per_tree = 0xFF; //255 4095 65***

struct Block {
	_psize depth;
	_psize size;
	Node block[_max_node_per_tree];
	Block(){
		depth = 0;
		size = 0;
	}
};

struct Page {
	_pid pid;
	bool sorted;
	bool extended;
	bool empty;
	_psize block_size;
	Block data[_max_node_per_page];
public:
	Page();
	char* getid();
	void init();
	char* getbuffer();
	void setbuffer(char*);
};



#endif /* LSM_H_ */
