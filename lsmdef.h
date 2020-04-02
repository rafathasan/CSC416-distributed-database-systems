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

using namespace std;

namespace lsm{
	const unsigned int _fixed = 1 << 18; //256 bytes. best optimal size is 256 Kilobyte
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

	class FileIO{
		fstream fs;
	public:
		virtual void getmeta();
		virtual void write();
		virtual void read();
	};

	class Page{

		// fields order are kept in the files same as below
		_pid pid;
		_psize next;
		bool sorted;
		bool extended;
		bool empty;
		_psize count;
		Node data[_fixed/sizeof(Node)];
	public:
		Page(){init();}
		~Page(){}
		void init(){
			_pid i;for(i=0;i < sizeof(pid);i++){
				if(_ascii[_ascii_ptr] >= _unique) {
					if(++_ascii_ptr > sizeof(pid)) _ascii_ptr = 0;
					_ascii[_ascii_ptr] = 0;
				}
				pid = (pid << 8) + _ascii_start + _ascii[i];
			} _ascii[_ascii_ptr]++;

			next = 0;
			extended = sorted = false;
			empty = true;
			count = 0;
		}
		void getpid(){
			cout<<"pid: "<<this->pid<<endl;
		}
		char* getbuffer(){
			_psize buffer_size =
					sizeof(pid)+
					sizeof(next)+
					sizeof(sorted)+
					sizeof(extended)+
					sizeof(empty)+
					sizeof(count)+
					_fixed;
			char *buffer = new char[buffer_size];
			strcat(buffer,(char *)&pid);
			strcat(buffer,(char *)&next);
			strcat(buffer,(char *)&sorted);
			strcat(buffer,(char *)&extended);
			strcat(buffer,(char *)&empty);
			strcat(buffer,(char *)&count);
			strcat(buffer,(char *)&data);
			return buffer;
		}
		void setbuffer(char* buffer){
			int trav = 0;

			cout<<"old:"<<pid<<endl;
			memcpy((char *)&this->pid, buffer+trav, sizeof(pid));
			cout<<"new:"<<pid<<endl;
			trav += sizeof(pid);
			memcpy((char *)&next, buffer+trav, sizeof(next));
			trav += sizeof(next);
			memcpy((char *)&sorted, buffer+trav, sizeof(sorted));
			trav += sizeof(sorted);
			memcpy((char *)&extended, buffer+trav, sizeof(extended));
			trav += sizeof(extended);
			memcpy((char *)&empty, buffer+trav, sizeof(empty));
			trav += sizeof(empty);
			memcpy((char *)&count, buffer+trav, sizeof(count));
			trav += sizeof(count);
			memcpy((char *)&data, buffer+trav, _fixed);
			trav += sizeof(data);
		}
		friend class FileEngine;
	};

	// Only defined for one file handling
	class FileEngine{
		Page page;
	public:
		FileEngine(){
		}
		void getmeta();
	};

	void FileEngine::getmeta(){
		cout<<"pid\t"<<page.pid<<endl;
		cout<<"next\t"<<page.next<<endl;
		cout<<"sorted\t"<<page.sorted<<endl;
		cout<<"extend\t"<<page.extended<<endl;
		cout<<"empty\t"<<page.empty<<endl;
		cout<<"count\t"<<page.count<<endl;
		cout<<"data\t"<<sizeof(page.data)<<" byte"<<endl;
	}

}// namespace lsm





#endif /* LSMDEF_H_ */
