/* TODO: Replace vector with map in node outgoing for easier access to indices */

#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <algorithm>
#include <map>

using std::string;
using std::pair;
using std::vector;
using std::map;

// Use map for easy lexicographical ordering and fast member access ( Theta(log n) )
// usage of Rope class motivated by wanting O(1) substr operation on strings and also for space efficiency

class Rope{
public:
	static string * str;

	Rope(){ 
		startIndx = 0;
		m_length = str->length();
	}
	Rope(size_t start, size_t len=str->length()){
		startIndx = start;
		m_length=len;
	}
	void print() const{
		for (size_t i = 0; i < m_length; ++i)
			putchar(this->at(i));
	}
	char at(size_t const index) const{
		if (index >= m_length || index < 0)
			return 0;
		return str->at(index + startIndx);
	}
	Rope substr(size_t const index, size_t const len) const{
		return Rope(index+startIndx, len);
	}
	string cpp_string() const{
		string ret = "";
		for (size_t i = 0; i < m_length; ++i)
			ret += this->at(i);
		return ret;
	}
	int start(){
		return startIndx;
	}
	int length(){
		return m_length;
	}
	bool operator==(Rope & rhs){
		if (this->startIndx == rhs.startIndx && this->m_length == rhs.m_length)
			return true;
		return false;
	}
	std::ostream& operator<<(std::ostream& os){
		this->print();
		return os;
	}
private:
	size_t startIndx;
	size_t m_length;
};
string * Rope::str = nullptr;

class Node{
public:
	Node(){
		m_parentEdge = std::make_pair(nullptr,Rope(0,0));
		m_sl = nullptr;
	}
	map<char, pair<Node *, Rope>> m_outgoing; // Node, edge label
	Node * m_sl;
	std::pair<Node *, Rope> m_parentEdge;
};

class SuffixTree{
public:
	SuffixTree(string * s){
		Rope::str = s; // initialize rope class static member str
		//Rope::setStr("s");
		// Initialize root and add the first string
		Rope t;
		//std::cout << t.substr(0, 10);
		getchar();
		root = new Node();
		auto p = std::make_pair(root, Rope());
		//root->m_outgoing.push_back(p); // add root pointing to itself
		root->m_parentEdge = p;
		/*for (int i = 0; i < s.length(); ++i)
			FindPath(root, s.substr(i, s.length()-i));
		*/
		std::cout << "Press any key to begin...";
		getchar();
		//DoNaiveConstruction(s);
		LinearTimeConstruction();
		/*last = FindPath(root, "ana");
		while (last){
			for (auto pair : last->m_outgoing)
				std::cout << pair.second << ' ';
			std::cout << '\n';
			if (last == root)
				break;
			last = last->m_parentEdge.first;
		}
		std::cout << "OUTPUT (Naive Construction)\n";
		
		std::cout << "\nOUTPUT (McCreight's Construction)\n";*/


		//outputTree(root, "");
		std::cout << "done\n";
	}
private:
	// START MAY NOT BE NULL! Unless you like your programs to crash, otherwise by all means...
	// For string s, use s.substr(begin, length) if only a particular substring of s's path is to be found
	void LinearTimeConstruction()
	{
		root->m_sl = root;
		Node * v = nullptr;
		Node * currentLeaf = nullptr;
		Node * u = root; // u internal node is initialized to be the root
		int alphaLen = 0;
		// add first string...
		Rope r(0,Rope::str->length());
		currentLeaf = FindPath(root, r)->m_parentEdge.first;

		for (int i = 1;i < Rope::str->length();++i){
			//if (i % 1000 == 0)
			//	std::cout << (float)i*100/s.length() << "%" << '\n';
			Node * u_prime = u->m_parentEdge.first;
			// CASE IA
			if (u->m_sl != nullptr && u != root){
				//std::cout << "CASE IA " << '\n';//<< s.substr(i+alphaLen,s.length()-i-alphaLen) << '\n';
				currentLeaf = HandleIA(u, alphaLen, i);
			}
			// CASE IB
			else if (u->m_sl != nullptr && u == root){
				//std::cout << "CASE IB " << '\n';//s.substr(i+alphaLen,s.length()-i-alphaLen) << '\n';
				currentLeaf = HandleIB(u, alphaLen, i);
			}
			// CASE IIA
			else if (u->m_sl == nullptr && u_prime != root){
				//std::cout << "CASE IIA " << '\n';// s.substr(i+alphaLen,s.length()-i-alphaLen) <<  "Alpha : " << alpha << '\n';
				currentLeaf = HandleIIA(u, alphaLen, i);
			}
			// CASE IIB
			else if (u->m_sl == nullptr && u_prime == root){
				//std::cout << "CASE IIB " << '\n';//s.substr(i+alphaLen,s.length()-i-alphaLen) << '\n';
				currentLeaf = HandleIIB(u, i);
			}
			if (currentLeaf == nullptr){
				//outputTree(root, "");
				exit(1);
				return;
			}
			// set u to internal node
			u = currentLeaf->m_parentEdge.first;
			// find the length of alpha (if current leaf's parent is root, the alpha is 0)
			alphaLen = (currentLeaf->m_parentEdge.first != root) ?
			Rope::str->length() - (i+1) - currentLeaf->m_parentEdge.second.length()
			: 0;
			if (currentLeaf->m_parentEdge.second.at(0) == '$')
				++alphaLen;
			//outputTree(root, "");

		}
	}
	Node * HandleIA(Node * u, int alphaLen, int i){
		// 2.) Take SL(u) to v
		Node * v = u->m_sl;
		// 3.) Add suffix
		Rope r(i+alphaLen, Rope::str->length()-(i+alphaLen));
		return FindPath(v, r);

	}
	Node * HandleIB(Node * u, int alphaLen, int i){
		// 2.) Take SL(u) => root => v
		Node * v = u->m_sl;
		// 3.) Add Suffix
		Rope r(i+alphaLen, Rope::str->length()-(i+alphaLen));
		return FindPath(v, r);
	}
	Node * HandleIIA(Node * u, int alphaLen, int i){
		// 2.) Go to u', and also record beta <-- edge label from u' to u
		Node * u_prime = u->m_parentEdge.first;
		Rope beta = u->m_parentEdge.second;
		// 3.) Take Suffix Link
		Node * v_prime = u_prime->m_sl;
		// 4.) Go to v by tracing beta (end of alpha)
		Node * v = NodeHop(v_prime, beta);
		// 5.) Set sl(u) to v
		u->m_sl = v;
		// 6.) Find path
		Rope r(i+alphaLen, Rope::str->length()-(i+alphaLen));
		return FindPath(v, r);

	}
	Node * HandleIIB(Node * u,int i){
		// 2.) Go to u', and also record beta <-- edge label from u' to u and beta'
		Rope beta_prime(u->m_parentEdge.second);
		beta_prime = beta_prime.substr(1, beta_prime.length() - 1);
		//std::cout << beta_prime << '\n';
		Node * u_prime = u->m_parentEdge.first;
		// 3.) Take Suffix Link
		Node * v_prime = root;
		// 4.) Go to v by tracing beta'
		Node * v = NodeHop(root, beta_prime);
		// 5.) Set sl(u) to v
		u->m_sl = v;
		// 6.) Find path
		Rope r(i+beta_prime.length(), Rope::str->length()-(i+beta_prime.length()));
		return FindPath(v, r);

	}
	Node * NodeHop(Node * node, Rope s){
		int curIndex = 0;
		int offset = 0;
		while (curIndex < s.length()){
				if (node->m_outgoing.find(s.at(curIndex)) != node->m_outgoing.end()){ // hop to node if there is an edge starting with s.at(0)

					auto edge = node->m_outgoing[s.at(curIndex)];
					if (curIndex + edge.second.length() <= s.length()){
						node = edge.first;
						curIndex += edge.second.length();
					}
					else{
						// create a new node to split string
						Node * newNode = SplitEdge(node, edge, s, offset, curIndex);
						return newNode;
					}
				}
				else{
					std::cout << "Fatal: In Node Hop\n";
					exit(-1);
				}
				++offset;
		}
		return node;
	}
	void DoNaiveConstruction(string &s){
		root->m_sl = root;
		Node * lastLeaf = nullptr;
		for (int i = 0; i < s.length(); ++i){
			Rope r(i, Rope::str->length()-i);
			FindPath(root, r);
		}
	}
	Node * FindPath(Node * start, Rope s){ // Function creates or traces path
		// Keep matching until cannot match characters, then diverge to create new suffix if need be
		Node * it = nullptr; // it and indx will indicate which node it diverges and which index in that node
		Node * ret = nullptr;
		int lenMatch = 0;
		int offset = 0;
		Rope outString;

			if (start->m_outgoing.find(s.at(0)) != start->m_outgoing.end()){
				// found node to take
				it = start->m_outgoing[s.at(0)].first;
				outString = start->m_outgoing[s.at(0)].second;
			}

		if (it == nullptr){ // no out node to take.. make one and return it
			if (s.length() != 0){
				// Create node for label s
				Node * temp = new Node();
				// Set node's parent pointer to start
				temp->m_parentEdge = std::make_pair(start,s);
				auto p = std::make_pair(temp, s);
				// scan to correct lexicographical ordering
				start->m_outgoing[p.second.at(0)] = p; // add new node and edge with label s
				ret = new Node();
				// create backpointer to parent
				ret->m_parentEdge = p;
				p.first->m_outgoing['$'] = (std::make_pair(ret, Rope()));
				ret = temp;
			}
			else{
				ret = new Node();
				// establish backpointer to parent
				ret->m_parentEdge = std::make_pair(start,Rope());
				start->m_outgoing['$'] = std::make_pair(ret, Rope());
				
			}

		}
		else{ // find longest common prefix in this outnode with string s
			while (s.at(lenMatch) == outString.at(lenMatch) && lenMatch < (s.length())){lenMatch++;} // exhaust as many characters as possible
				if(lenMatch < outString.length() && lenMatch < (s.length())){ // middle of outstring and not all characters exhausted
					// Split outString
					Rope firstSlice = outString.substr(0, lenMatch); // matching slice
					Rope secondSlice = outString.substr(lenMatch, outString.length() - lenMatch);
					//std::cout << firstSlice << ' ' << secondSlice << '\n';

					auto oldPair = start->m_outgoing[s.at(0)]; // save old pair, but set old pair's label to secondSlice
					oldPair.second = secondSlice;
					Node * temp = new Node();
					// establish a backpointer to the parent
					temp->m_parentEdge = std::make_pair(start,firstSlice); 
					start->m_outgoing[firstSlice.at(0)].first = temp; 
					start->m_outgoing[firstSlice.at(0)].second = firstSlice;
					// set new node's parent to temp
					oldPair.first->m_parentEdge = std::make_pair(temp, secondSlice);
					temp->m_outgoing[secondSlice.at(0)] = (oldPair);
					ret = FindPath(start->m_outgoing[s.at(0)].first, s.substr(lenMatch, s.length()-lenMatch));
				}
				else if (lenMatch == outString.length() && lenMatch < (s.length())){ // end of outstring and more characters to match
					ret = FindPath(start->m_outgoing[s.at(0)].first, s.substr(lenMatch, s.length()-lenMatch));
				}
				else if (lenMatch < outString.length() && lenMatch == (s.length())){
					// Split outString
					Rope firstSlice = outString.substr(0, lenMatch); // matching slice
					Rope secondSlice = outString.substr(lenMatch, outString.length() - lenMatch);

					auto oldPair = start->m_outgoing[firstSlice.at(0)]; // save old pair, but set old pair's label to secondSlice
					oldPair.second = secondSlice;

					Node * newNode = new Node();
					Node * endNode = new Node();

					endNode->m_parentEdge = std::make_pair(newNode, Rope());
					newNode->m_parentEdge = std::make_pair(start, firstSlice);
					oldPair.first->m_parentEdge = std::make_pair(newNode, secondSlice);

					newNode->m_outgoing['$'] = (std::make_pair(endNode, Rope()));
					start->m_outgoing[s.at(0)].first = newNode; 
					start->m_outgoing[s.at(0)].second = firstSlice;
					newNode->m_outgoing[secondSlice.at(0)] = (oldPair);
					ret = endNode;
				}
				else {
					if (it->m_outgoing.find('$') == it->m_outgoing.end()){
						Node * newNode = new Node();
						it->m_outgoing['$'] = std::make_pair(newNode,Rope());
						newNode->m_parentEdge = std::make_pair(it, Rope());
					}
					ret = it->m_outgoing['s'].first;
				}
		}
		return ret;
	}

	Node * SplitEdge(Node * node, pair<Node*, Rope> edge, Rope & label, int edgeIndx, int startIndx){
		Node * newNode = new Node();
		Rope firstSlice = label.substr(startIndx, label.length()-startIndx); // remaining string			
		Rope secondSlice = edge.second.substr(firstSlice.length(), edge.second.length()-firstSlice.length());
		//std::cout << firstSlice << "<-->" << secondSlice << '\n';
		// now insert the new node
		node->m_outgoing[firstSlice.at(0)].first = newNode;
		node->m_outgoing[firstSlice.at(0)].second = firstSlice;
		newNode->m_outgoing[secondSlice.at(0)] = (std::make_pair(edge.first, secondSlice));
		// set parent pointers
		newNode->m_parentEdge = std::make_pair(node, firstSlice);
		edge.first->m_parentEdge = std::make_pair(newNode, secondSlice);
		return newNode;
	}
	// to be completed
	/*int getLexicographicalIndex(Node * start, Rope s){
		int insrtIndx = 0;
		for (insrtIndx = 0; insrtIndx < start->m_outgoing.size(); ++insrtIndx){
			auto cmp = start->m_outgoing[insrtIndx].second;
			int i = 0;
			// find first that rope s is less than
			if (cmp.at(0) == '$')
				continue;
			while (i < s.length() && i < cmp.length()){
				if(s.at(i) < cmp.at(i)) // s goes here
					return insrtIndx;
				else if (s.at(i) > cmp.at(i))
					break;
				else
					++i; // try next character
			}
			if (s.at(i) == cmp.at(i) && s.length() < cmp.length())
				return insrtIndx; // if all characters match but s len < cmp len
		}
		return insrtIndx;
	}*/
	void outputTree(Node *& start, string s){
		bool printed=false;
		for (auto c : start->m_outgoing){
			auto edge = c.second;
			if (start->m_outgoing.find('$') != start->m_outgoing.end() && !printed){ // has edge to leaf, so print
				std::cout << s << '\n';
				printed=true;
			}
			outputTree(edge.first, s+edge.second.cpp_string());
		}
	}

	Node * root;
};
