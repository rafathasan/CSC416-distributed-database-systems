/*
 * lsmdata.cpp
 *
 *  Created on: Apr 9, 2020
 *      Author: rafikhan
 */

#include <fstream>
#include <random>
#include <iostream>
#include <stdlib.h>
using namespace std;

int main(int argc, char* argv[]){
	if(argc < 2) std::cout<<"lsmdata [dataset]2^[INT] [random]2^[INT]\n";
	long unsigned int count = 1 << 22;
	if(argc == 2) count = 1 << atoi(argv[1]);
	unsigned int range = 1 << 18;
	if(argc == 3) range = 1 << atoi(argv[2]);

	unsigned int value_range = 1 << 20;
	char* FILE = "lsmdata";
	fstream fs;
	fs.open(FILE, ios::out|ios::binary| ios::ate);
	unsigned int key;
	unsigned int value;
	for(long unsigned int i=0;i<count;i++){
		key = (rand()%range)+1;
		value = (rand()%value_range)+1;
		fs.write((char*) &key, 4);
		fs.write((char*) &value, 4);
	}
	fs.close();

	return 0;
}
