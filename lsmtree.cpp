/*
 * lsmtree.cpp
 *
 *  Created on: Apr 4, 2020
 *      Author: rafikhan
 */

#include "lsm_def.h"
#include <iostream>
#include <time.h>
#include <assert.h>
#include <algorithm>

using namespace std;

bool int_comp(_key l, _key r){
	return l < r;
}

int main(){
	BufferOperation bo;
	srand(time(NULL));
	cout<<"size: "<<_max_node_per_tree<<endl;

	_key keys[_max_node_per_tree];
	for(int i=0;i<255;i++){
		keys[i] = (rand()%999)+1;
	}

	for(int i=0;i<_max_node_per_tree;i++){
		bo._insert(keys[i], 0);
	}

	sort(keys, keys + _max_node_per_tree, int_comp);

//	bo._view();
	bo._sort_tree_depth();

	for(int i=0;i<_max_node_per_tree;i++){
			assert(keys[i] == bo.getTree()[i].key );
	}


	return 0;
}


