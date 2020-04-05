/*
 * lsm_impl.cpp
 *
 *  Created on: Apr 4, 2020
 *      Author: rafikhan
 */

#include <fstream>
#include <string.h>
#include <algorithm>
#include <math.h>
#include "lsm_def.h"
#include <iostream>

using namespace std;

const char* filename = "aaaa";

// Page Implementation
Page::Page(){
	init();
}
char* Page::getid() {
	char *ptr = new char[4];

	memcpy(ptr, (char *) &pid, 4);
	return (char*) &pid;
}
void Page::init() {
	pid = 1633771873;
	next_idx = 0;
	extended = sorted = false;
	empty = true;
	count = 0;
}
char* Page::getbuffer() {
	char *buffer = new char[sizeof(Page)];
	int trav = 0;
	memcpy(buffer + trav, (char *) &pid, sizeof(pid));
	trav += sizeof(pid);
	memcpy(buffer + trav, (char *) &next_idx, sizeof(next_idx));
	trav += sizeof(next_idx);
	memcpy(buffer + trav, (char *) &sorted, sizeof(sorted));
	trav += sizeof(sorted);
	memcpy(buffer + trav, (char *) &extended, sizeof(extended));
	trav += sizeof(extended);
	memcpy(buffer + trav, (char *) &empty, sizeof(empty));
	trav += sizeof(empty);
	memcpy(buffer + trav, (char *) &count, sizeof(count));
	trav += sizeof(count);
	memcpy(buffer + trav, (char *) &data, sizeof(data));
	trav += sizeof(data);
	return buffer;
}
void Page::setbuffer(char* buffer) {
	int trav = 0;

	memcpy((char *) &this->pid, buffer + trav, sizeof(pid));
	trav += sizeof(pid);
	memcpy((char *) &next_idx, buffer + trav, sizeof(next_idx));
	trav += sizeof(next_idx);
	memcpy((char *) &sorted, buffer + trav, sizeof(sorted));
	trav += sizeof(sorted);
	memcpy((char *) &extended, buffer + trav, sizeof(extended));
	trav += sizeof(extended);
	memcpy((char *) &empty, buffer + trav, sizeof(empty));
	trav += sizeof(empty);
	memcpy((char *) &count, buffer + trav, sizeof(count));
	trav += sizeof(count);
	memcpy((char *) &data, buffer + trav, _page_size);
	trav += sizeof(data);
}

bool comp(Node &l, Node &r) {
	return l.key < r.key;
}

void Block::_sort(){
	sort(block, block + _max_node_per_block, comp);
}

// DiskOperation Implementation
DiskOperation::DiskOperation(){
	page = new Page();
}
void DiskOperation::getmeta() {
	cout << "pid\t" << page->pid << endl;
	cout << "next\t" << page->next_idx << endl;
	cout << "sorted\t" << page->sorted << endl;
	cout << "extend\t" << page->extended << endl;
	cout << "empty\t" << page->empty << endl;
	cout << "count\t" << page->count << endl;
	cout << "data\t" << sizeof(page->data) << " byte" << endl;
}
void DiskOperation::write_file() {
	fs.open(filename , ios::out | ios::binary | ios::ate);
	fs.write(page->getbuffer(), sizeof(Page));
	fs.close();
}
void DiskOperation::read_file() {
	char *buffer = new char[sizeof(Page)];
	fs.open(filename , ios::in | ios::binary);
	fs.read(buffer, sizeof(Page));
	fs.close();
	page->setbuffer(buffer);
}

void DiskOperation::_fill_random() {
	for (_psize i = 0; i < _max_node_per_page; i++) {
		page->data[i].key = i + 1;
		page->data[i].value = (rand() % 999) + 1;
		page->next_idx++;
		page->count++;
	}
	if (page->empty)
		page->empty = false;
	page->sorted = false;
}

void DiskOperation::_insert(_key key, _value value) { // no bound or extended page handling
	page->data[page->next_idx].key = key;
	page->data[page->next_idx].value = value;
	page->next_idx++;
	page->count++;
	if (page->empty)
		page->empty = false;
	page->sorted = false;
}
void DiskOperation::_insert(Node *buffer, _psize count) { // no bound or extended page handling
	for (_psize i = 0; i > count; i++) {
		page->data[page->next_idx].key = buffer->key;
		page->data[page->next_idx].value = buffer->value;
		page->next_idx++;
		page->count++;
	}
}

void DiskOperation::_heap_sort(Node *buffer, _psize count) {
	sort(buffer, buffer + count, comp);
}
bool DiskOperation::_delete(_key key) {
	for (_psize i = 0; i < _max_node_per_page; i++) {
		if (page->data[i].key == key) {
			page->data[i].key = EMPTY_KEY;
			page->data[i].value = 0;
			page->count--;
			return true;
		}
	}
	return false;
}

bool DiskOperation::_update(_key key, _value value) {
	for (_key i = 0; i < _max_node_per_page; i++) {
		if (page->data[i].key == key) {
			page->data[i].key = key;
			page->data[i].value = value;
			return true;
		}
	}
	return false;
}

_value DiskOperation::_find(_key key) {
	if (!page->sorted)
		_defrag_sort();
	_key res = DiskOperation::_logn_search(key, 0, _max_node_per_page);
	if (res)
		return page->data[res].value;
	return 0;
}

_key DiskOperation::_logn_search(_key key, _key l, _key r) {
	if (r >= l) {
		int mid = l + (r - l) / 2;
		if (page->data[mid].key == key)
			return mid;
		if (page->data[mid].key < key)
			return _logn_search(key, l, mid - 1);
		return _logn_search(key, mid + 1, r);
	}
	return 0;
}

void DiskOperation::_defrag_sort() {
	if (page->sorted)
		return;
	sort(page->data, page->data + _max_node_per_page, comp);
	if (page->next_idx > page->count)
		page->next_idx = page->count;
	page->sorted = true;
}

void DiskOperation::_view() {
	for (_psize i = 0; i < _max_node_per_page; i++) {
		cout << page->data[i].key << "\t" << page->data[i].value << endl;
	}
}

// BufferOperation Implementation
BufferOperation::BufferOperation() {
	isBufferEmpty = true;
	tree_size = 0;
	tree = new Node[_max_node_per_tree];
	tree_root = (_max_node_per_tree/2)+1;
}

Node*& BufferOperation::getTree(){
	return tree;
}

int getLNode(int l){
	return (l*2)+1;
}

int getRNode(int l){
	return (l*2)+2;
}

int getPNode(int l){
	return l/2;
}

_key BufferOperation::_logn_insert(_key key, _key l, _key r) {
	if (r >= l) {
		_psize mid = l + (r - l) / 2;
		if(mid > _max_node_per_tree || mid < 0) return EMPTY_KEY;
		if (tree[mid].key == EMPTY_KEY) return mid;
		if (tree[mid].key < key) return _logn_insert(key, l, mid - 1);
		return _logn_insert(key, mid + 1, r);
	}

	return EMPTY_KEY;
}

void BufferOperation::_sort_tree_depth(){
	sort(tree, tree + _max_node_per_tree, comp);
	_psize empty_size = _max_node_per_tree - tree_size;
	Node *t_tree = new Node[tree_size];

	for(_psize i=0;i<tree_size;i++){
		t_tree[i].key = tree[i].key;
		t_tree[i].value = tree[i].value;
	}
	delete tree;
	tree = new Node[_max_node_per_tree];
	for(_psize i=empty_size/2;i< (empty_size/2)+tree_size;i++){
			tree[i].key = t_tree[i - (empty_size/2)].key;
			tree[i].value = t_tree[i - (empty_size/2)].value;
	}
}

void BufferOperation::_insert(_key key, _value value){
	_key find = BufferOperation::_logn_insert(key, 0, _max_node_per_tree-1);
	if(find != EMPTY_KEY){
		tree[find].key = key;
		tree[find].value = value;
		tree_size++;
	}else if(tree_size == (_max_node_per_tree-1) ){
		_sort_tree_depth();
		if(tree[0].key == EMPTY_KEY){
			tree[0].key = key;
			tree[0].value = value;
		}else{
			tree[_max_node_per_tree-1].key = key;
			tree[_max_node_per_tree-1].value = value;
		}
		tree_size++;
	}else if(tree_size >= _max_node_per_tree ){
		_flush();
	}
	else{
		_sort_tree_depth();
		_insert( key, value);
	}
}



void BufferOperation::_delete() {
}

void BufferOperation::_flush(){// incomplete function
	_sort_tree_depth();
	for(int i=0;i<tree_size;i++){
		buffer.block[i].key = tree[i].key;
		buffer.block[i].value = tree[i].value;
		buffer.size++;
	}
	delete tree;
	tree = new Node[_max_node_per_tree];
	tree_size = 0;
	isBufferEmpty = false;
}

void BufferOperation::_view(){
	for(_psize i=0;i<_max_node_per_tree;i++){
			cout<<i<<" -> "<<tree[i].key<< " "<<endl;
	}
}

LSMOperation::LSMOperation(){
}

void LSMOperation::_init(){
	return;
}




