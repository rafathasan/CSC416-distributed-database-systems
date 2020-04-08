#include "lsmdef.h"
#include <iostream>
#include <sstream>
#include <string.h>
using namespace std;

const _key __EMPTY_KEY__ = -1;
const _value __EMPTY_VALUE__ = -1;

void lsm_shell(lsmtree*);
void _warning(char*);
void _message(char*);
void _message(_value);

int main(){
	lsmtree *tree = new lsmtree;
	lsm_shell(tree);
	return 0;
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
		else if( !word.compare("l") ) cout<<"load"<<endl;
		else if( !word.compare("s") ) cout<<"status"<<endl;
		else if( !word.compare("") );
		else cout<<"others"<<endl;
	}
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
