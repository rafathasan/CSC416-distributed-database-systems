#include "lsmdef.h"
#include <iostream>
using namespace std;

int main(){
	lsmtree tree;
	srand(time(0));
	for(int i=0;i<299999;i++){
		tree._insert(rand()%9999,1);
	}
	tree._delete(997);
	std::cout<<tree._find(2)<<endl;
	cout<<"node count: "<<tree.node_count<<endl;
	cout<<"filling counter: "<<tree.filling_buffer_pointer<<endl;
	cout<<"emptying counter: "<<tree.emptying_buffer_pointer<<endl;
	return 0;
}
