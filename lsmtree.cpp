#include "lsmdef.h"
#include <iostream>
#include <sstream>
#include <string.h>
#include <fstream>
#include <chrono>
using namespace std;
using namespace chrono;

const _key __EMPTY_KEY__ = -1;
const _value __EMPTY_VALUE__ = -1;

void lsm_shell(lsmtree*);
void _warning(char*);
void _message(char*);
void _message(_value);
void _man();
lsmtree* _parse_argv(int, char* []);
int main(int argc, char* argv[]){


	lsmtree *tree = _parse_argv(argc, argv);
//	if(tree = NULL) {cout<<"bad arguments!"<<endl; return 0;}
//	if( argc == 4)cout<<"Buffer size: "
//	<< argv[1]<< " Tree size: "
//			<< argv[2] <<
//			" Bloom Filter Size: "
//			<< argv[3] << endl;
	lsm_shell(tree);

	return 0;
}

std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::duration<long int, std::ratio<1, 1000000000> > > start;
std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::duration<long int, std::ratio<1, 1000000000> > > stop;
std::chrono::duration<long int, std::ratio<1, 1000000> > diff;

void time_start(){
	start = high_resolution_clock::now();
}
void time_stop(){
	stop = high_resolution_clock::now();
	diff = duration_cast<microseconds>(stop - start);
}
void time_print(){
	cout<<diff.count()<<"ms"<<endl;
}

lsmtree* _parse_argv(int argc, char* argv[]){
	if(argc == 1) return new lsmtree();
	stringstream ss;

	_num buffer_size = 0;
	_num tree_size = 0;
	_num bloom_size = 0;

	ss.str(argv[1]);

	ss >> buffer_size;

	ss.clear();
	ss.str(argv[2]);

	ss >> tree_size;

	ss.clear();
	ss.str(argv[3]);

	ss >> bloom_size;

	if(buffer_size == 0 || tree_size == 0 || bloom_size == 0){
		return new lsmtree;
	}
	return new lsmtree(buffer_size, tree_size, bloom_size);
}

void lsm_shell(lsmtree *tree){
	string line;

	stringstream ss;
	while(true){
		string word;
		getline(cin, line);
		ss.clear();
		ss.str(line);
		ss >> word;

		time_start();
		if( !word.compare("p") ) {;
			_key key;
			_value value;
			if(ss >> key && ss >> value){
				if(tree->_insert(key, value));
				else _message("duplicate key");
			}else _warning("puts parse error");
		}
		else if( !word.compare("g") ) {
			_key key;
			if(ss >> key){
				_value val = tree->_find(key);
				if(val == __EMPTY_VALUE__) _message("");
				else _message(val);
			}else _warning("gets parse error");
		}
		else if( !word.compare("r") ) {
			_key key1, key2;
			if(ss >> key1 && ss>> key2){
//				if(val == __EMPTY_VALUE__) _message("");
//				else _message(val);
			}else _warning("range parse error");
		}
		else if( !word.compare("d") ) {
			_key key;
			if(ss >> key){
				int k = tree->_delete(key);
				if(k == -1) _message("key not exist");
				else if(k == 0) _message("key not found");
				else _message("");
			}else _warning("delete parse error");
		}
		else if( !word.compare("l") ) {
			string FILE;
			if(ss >> FILE){
				fstream fs;
				fs.open(FILE, ios::in| ios::binary | ios::ate );
				if(fs){
					_key key;
					_value val;
					cout<<"file name: "<<FILE<<", file size: "<< fs.tellg()<< " bytes" << endl;
					fs.seekg(0);
					while(!fs.eof()){
						fs.read((char*)&key, 4);
						fs.read((char*)&val, 4);
						tree->_insert(key, val);
					}
					cout<< "data inserted into tree"<<endl;
				}else _warning("can't open the file");
				fs.close();
			}else _warning("load parse error");
		}
		else if( !word.compare("s") ){
			cout<<"Logical Pairs: "<<tree->node_count<<endl;
		}
		else if( !word.compare("") );
		else _man();
		time_stop();
		time_print();
	}
}

void _man(){
	char *FILE = "manual";
	ifstream ifs;
	ifs.open(FILE, ios::in|ios::binary);
	if(!ifs.is_open()){
		cout<<"Manual file is missing ! FILE: "<<FILE<<endl;
		return;
	};
	char ch;
	while(ifs.get(ch)){
		cout<<ch;
	}
	ifs.close();
}

void _warning(char *msg){
	cout<<msg<<endl;
}
void _message(char *msg){
	cout<<msg<<endl;
}
void _message(_value value){
	cout<<value<<endl;
}
