/*
 * lsmdef.cpp
 *
 *  Created on: Apr 7, 2020
 *      Author: rafikhan
 */

#include <stddef.h>
#include <iostream>
#include "lsmdef.h"
using namespace std;

const _num __BUFFER_SIZE__ = 1024;
const _num __TREE_SIZE__ = 255;
const _num __BLOOM_FILTER_SIZE__ = 128;
const _key __EMPTY_KEY__ = -1;
const _value __EMPTY_VALUE__ = -1;
const _num __EMPTY_INDEX__ = -1;
const _num __HASH_SIZE__ = 2;

lsmnode::_lsmnode(){
	key = __EMPTY_KEY__;
	value = __EMPTY_VALUE__;
	l_weight = 0;
	r_weight = 0;
	index = __EMPTY_INDEX__;
	l = NULL;
	r = NULL;
	p = NULL;
	deleted = false;
}

signed int lsmnode::getWeight(){
	return l_weight - r_weight;
}

lsmtree::_lsmtree(){
	root = NULL;
	emptying_buffer = new lsmnode*[__TREE_SIZE__];
	filling_buffer = new lsmnode*[__BUFFER_SIZE__];
	bloom_filter = new bool[__BLOOM_FILTER_SIZE__];
	emptying_buffer_pointer = 0;
	filling_buffer_pointer = 0;
	node_count = 0;
	int i;while(i<__BLOOM_FILTER_SIZE__){bloom_filter[i++] = false;}
}

_key hash_func1(_key key){
	return (key % __BLOOM_FILTER_SIZE__);
}
_key hash_func2(_key key){
	return ((key + (__BLOOM_FILTER_SIZE__/2)) % __BLOOM_FILTER_SIZE__);
}

bool _lsmtree::_peek_bloom_filter(_key key) {
	return 	bloom_filter[hash_func1(key)] && bloom_filter[hash_func2(key)];
}

void _lsmtree::_update_bloom_filter(_key key) {
	bloom_filter[hash_func1(key)] = true;
	bloom_filter[hash_func2(key)] = true;
}

void lsmtree::_emptying_buffer(){
	_num idx = emptying_buffer_pointer;
	while(true){
		if(emptying_buffer[idx] != NULL){
			filling_buffer[filling_buffer_pointer++] = emptying_buffer[idx];
			node_count--;
			if(filling_buffer_pointer == (__BUFFER_SIZE__-1) ) _write_to_file();
			return;
		}
		idx++;
	}
}

void lsmtree::_write_to_file(){
	cout<<"buffer full need to write"<<endl;
}

void lsmtree::_update_priority(lsmnode* node){
	if(node->index != __EMPTY_INDEX__) emptying_buffer[node->index] = NULL;
	emptying_buffer[emptying_buffer_pointer] = node;node->index = emptying_buffer_pointer;
	emptying_buffer_pointer = ++emptying_buffer_pointer % (__TREE_SIZE__+1);
	if(node_count >= (__TREE_SIZE__ -1)) _emptying_buffer();
}
bool lsmtree::_insert(_key key, _value value){
	lsmnode *trav = this->root;
	lsmnode *p_trav = NULL;

	if(root == NULL){
		root = new lsmnode;
		root->key = key;
		root->value = value;
		_update_priority(root);
		node_count++;
		_update_bloom_filter(key);
		return true;
	}
	while(true){
		if(trav->key == __EMPTY_KEY__ || trav->key == key){
			if(trav->key == key) return false;
			trav->key = key;
			trav->value = value;
			trav->p = p_trav;
			_update_priority(trav);
			node_count++;
			_update_bloom_filter(key);
			return true;
		}else if(trav->key > key){
			trav->l_weight++;
			p_trav = trav;
			if(trav->l == NULL) trav->l = new lsmnode;
			trav = trav->l;
		}else{
			trav->r_weight++;
			p_trav = trav;
			if(trav->r == NULL) trav->r = new lsmnode;
			trav = trav->r;
		}
	}
}


bool lsmtree::_delete(_key key){
	if(!_peek_bloom_filter(key)) return false;
	lsmnode *trav = this->root;
	while(true){
		if(trav == NULL) return false;
		if(trav->key == key){
			trav->deleted = false;
			emptying_buffer[trav->index] = NULL;
			node_count--;
			return true;
		}else if(trav->key > key){
			trav = trav->l;
		}else{
			trav = trav->r;
		}
	}
}

bool lsmtree::_update(_key key, _value value){
	if(!_peek_bloom_filter(key)) return false;
	lsmnode *trav = this->root;
	while(true){
		if(trav == NULL) return false;
		if(trav->key == key){
			trav->value = value;
			trav->deleted = false;
			_update_priority(trav);
			return true;
		}else if(trav->key > key){
			trav = trav->l;
		}else{
			trav = trav->r;
		}
	}
}

_value lsmtree::_find(_key key){
	if(!_peek_bloom_filter(key)) return __EMPTY_VALUE__;
	lsmnode *trav = this->root;
	while(true){
		if(trav == NULL ) return __EMPTY_VALUE__;
		if(trav->key == key){
			if(trav->deleted == true) return __EMPTY_VALUE__;
			_update_priority(trav);
			return trav->value;
		}else if(trav->key > key){
			trav = trav->l;
		}else{
			trav = trav->r;
		}
	}
}
