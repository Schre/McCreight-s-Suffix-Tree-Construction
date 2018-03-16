#include <iostream>
#include "SuffixTree.h"
int main(int argc, char * argv[]){

	string s = "mississippi";
	for (int i = 0; i < 1000000; ++i)
		s += 'a';

	SuffixTree * t = new SuffixTree(&s);
	return 1;
}