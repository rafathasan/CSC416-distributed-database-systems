/*
 * lsmdef.h
 *
 *  Created on: Apr 2, 2020
 *      Author: Rafat Hasan
 */

#ifndef LSMDEF_H_
#define LSMDEF_H_
#include <fstream>
#include <string.h>
#include <algorithm>

using namespace std;
#define FILE "aaaa"
namespace lsm{
	const unsigned int _page = 1<<18; //256 Kilobytes. best optimal size is 256 Kilobyte
	const unsigned int _block = 1<<12; //4 Kilobytes per block
	const unsigned int _tree = 1<<18; //256 Kilobytes per tree;
	typedef unsigned int _psize;
	typedef unsigned int _key;
	typedef unsigned int _value;
	typedef unsigned int _pid; //4 bytes page id

	unsigned int _unique = 25; //unique a-z char _pid
	unsigned int _ascii_start = 97;
	unsigned int _ascii[sizeof(_pid)] = {0}; // N byte permutation array
	unsigned int _ascii_ptr = 0;

	struct Node{
		_key key;
		_value value;
	};

	bool comp(Node &l, Node &r){
		return l.key < r.key;
	}

	struct Block{
		Node block[_block/sizeof(Node)];
		int count=0;
		Block *next=NULL;
	};

	class Page{
		// fields order are kept in the files same as below
		_pid pid;
		_psize next_idx;
		bool sorted;
		bool extended;
		bool empty;
		_psize count;
		Node data[_page/sizeof(Node)];
	public:
		Page(){init();}
		~Page(){}
		char* getid(){
			char *ptr = new char[4];

			memcpy(ptr, (char *)&pid, 4);
			return (char*)&pid;
		}
		void init(){
			_pid i;for(i=0;i < sizeof(pid);i++){
				if(_ascii[_ascii_ptr] >= _unique) {
					if(++_ascii_ptr > sizeof(pid)) _ascii_ptr = 0;
					_ascii[_ascii_ptr] = 0;
				}
				pid = (pid << 8) + _ascii_start + _ascii[i];
			} _ascii[_ascii_ptr]++;

			next_idx = 0;
			extended = sorted = false;
			empty = true;
			count = 0;
		}
		char* getbuffer(){
			_psize buffer_size =
					sizeof(pid)+
					sizeof(next_idx)+
					sizeof(sorted)+
					sizeof(extended)+
					sizeof(empty)+
					sizeof(count)+
					_page;
			char *buffer = new char[sizeof(Page)];
			int trav = 0;
			memcpy(buffer+trav, (char *)&pid, sizeof(pid));
			trav += sizeof(pid);
			memcpy(buffer+trav, (char *)&next_idx, sizeof(next_idx));
			trav += sizeof(next_idx);
			memcpy(buffer+trav, (char *)&sorted, sizeof(sorted));
			trav += sizeof(sorted);
			memcpy(buffer+trav, (char *)&extended, sizeof(extended));
			trav += sizeof(extended);
			memcpy(buffer+trav, (char *)&empty, sizeof(empty));
			trav += sizeof(empty);
			memcpy(buffer+trav, (char *)&count, sizeof(count));
			trav += sizeof(count);
			memcpy(buffer+trav, (char *)&data, sizeof(data));
			trav += sizeof(data);
			return buffer;
		}
		void setbuffer(char* buffer){
			int trav = 0;

			memcpy((char *)&this->pid, buffer+trav, sizeof(pid));
			trav += sizeof(pid);
			memcpy((char *)&next_idx, buffer+trav, sizeof(next_idx));
			trav += sizeof(next_idx);
			memcpy((char *)&sorted, buffer+trav, sizeof(sorted));
			trav += sizeof(sorted);
			memcpy((char *)&extended, buffer+trav, sizeof(extended));
			trav += sizeof(extended);
			memcpy((char *)&empty, buffer+trav, sizeof(empty));
			trav += sizeof(empty);
			memcpy((char *)&count, buffer+trav, sizeof(count));
			trav += sizeof(count);
			memcpy((char *)&data, buffer+trav, _page);
			trav += sizeof(data);
		}
		friend class DiskOperation;
	};

	// Only defined for one file handling
	class DiskOperation{
		Page *page;
		fstream fs;
	public:
		DiskOperation(){
			page = new Page();
		}
		void getmeta(){
			cout<<"pid\t"<<page->pid<<endl;
			cout<<"next\t"<<page->next_idx<<endl;
			cout<<"sorted\t"<<page->sorted<<endl;
			cout<<"extend\t"<<page->extended<<endl;
			cout<<"empty\t"<<page->empty<<endl;
			cout<<"count\t"<<page->count<<endl;
			cout<<"data\t"<<sizeof(page->data)<<" byte"<<endl;
		}
		void write_file(){
			fs.open(FILE, ios::out|ios::binary|ios::ate);
			fs.write(page->getbuffer(), sizeof(Page));
			fs.close();
		}
		void read_file(){
			char *buffer = new char[sizeof(Page)];
			fs.open(FILE, ios::in|ios::binary);
			fs.read(buffer, sizeof(Page));
			fs.close();
			page->setbuffer(buffer);
		}

		void _fill_random(){
			for(int i=0;i<_page/sizeof(Node);i++){
				page->data[i].key =  i+1;
				page->data[i].value = (rand()%999)+1;
				page->next_idx++;
				page->count++;
			}
			if(page->empty) page->empty = false;
			page->sorted = false;
		}

		void _insert(_key key, _value value){// no bound or extended page handling
			page->data[page->next_idx].key = key;
			page->data[page->next_idx].value = value;
			page->next_idx++;
			page->count++;
			if(page->empty) page->empty = false;
			page->sorted = false;
		}
		void _insert(Node *buffer, int count){// no bound or extended page handling
			for(int i=0;i>count;i++){
				page->data[page->next_idx].key = buffer->key;
				page->data[page->next_idx].value = buffer->value;
				page->next_idx++;
				page->count++;
			}
		}

		void _heap_sort(Node *buffer, int count){
			sort(buffer, buffer+count, comp);
		}
		bool _delete(_key key){
			for(_key i=0;i<_page/sizeof(Node);i++){
				if(page->data[i].key == key){
					page->data[i].key = -1;
					page->data[i].value = -1;
					page->count--;
					return true;
				}
			}
			return false;
		}

		bool _update(_key key, _value value){
			for(_key i=0;i<_page/sizeof(Node);i++){
				if(page->data[i].key == key){
					page->data[i].key = key;
					page->data[i].value = value;
					return true;
				}
			}
			return false;
		}

		_value _find(_key key){
			if(!page->sorted) _defrag_sort();
			_key res = _logn_search(key, 0, _page/sizeof(Node));
			if(res) return page->data[res].value;
			return 0;
		}

		_key _logn_search(_key key, _key l, _key r){
		    if (r >= l) {
		        int mid = l + (r - l) / 2;
		        if (page->data[mid].key == key) return mid;
		        if (page->data[mid].key < key) return _logn_search(key, l, mid - 1);
		        return _logn_search(key, mid + 1, r);
		    }
		    return 0;
		}

		void _defrag_sort(){
			if(page->sorted) return;
			sort(page->data, page->data + _page/sizeof(Node), comp);
			if(page->next_idx > page->count) page->next_idx = page->count;
			page->sorted = true;
		}

		void _view(){
			for(int i=0;i<_page/sizeof(Node);i++){
				cout<<page->data[i].key<<"\t"<<page->data[i].value<<endl;
			}
		}
	};

	class BufferOperation{
		Block *root;
		int block_count;
		bool bloom[_tree] = {0}; // Not efficient size for bloom filter, bool is 1 byte. optimal is 1 bit per record
	public:
		BufferOperation(){
			Block *root = new Block();
		}
		bool _write_trigger(){
			return block_count*sizeof(Block) >= _tree;
		}
		void _insert(_key key, _value value){
			Block *trav = root;
			while(trav->count < (_block/sizeof(Node)) ){
				trav = trav->next;
				if(trav == NULL) trav = new Block();
			}
			for(int i=0;i< (_block/sizeof(Node));i++ ){
				if(trav->block[i].key == (_key)-1){
					trav->block[i].key = key;
					trav->block[i].value = value;
					trav->count++;
				}
			}
		}
	};

}// namespace lsm

#endif /* LSMDEF_H_ */
