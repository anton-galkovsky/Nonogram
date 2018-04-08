#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <set>

using namespace std;
//Структура конкретизаций. Содержит конкретизируемую букву (letter) и её позицию в слове (number)
struct Conc {
	int number;
	char letter;

	Conc() {}

	Conc(int n, char l) {
		number = n;
		letter = l;
	}

	bool operator<(const Conc conc) const {
		if (number != conc.number)
			return number < conc.number;
		return letter < conc.letter;
	}
};

struct Node;
//Структура рёбер. Содержит указатель на вершину (*node) и соответствующую конкретизацию (conc)
struct Rib {
	Conc conc;
	Node *node;

	Rib() {}

	Rib(Conc c, Node *n) {
		conc = c;
		node = n;
	}

	bool operator<(const Rib rib) const {
		return conc < rib.conc;
	}
};
//Структура узлов. Содержит набор слов, соответствующих узлу (words), 
//набор исходящих из узла и ведущих в другие узлы рёбер (ribs) и набор петель узла (loops)
struct Node {
	set<string> words;
	set<Rib> ribs;
	set<Conc> loops;

	Node() {}

	Node(string str) {
		words.insert(str);
		for (unsigned i = 0; i < str.size(); i++)
			loops.insert(Conc(i, str[i]));
	}
};
//Функция, возвращающая набор петель узла со словом str
set<Conc> loop(string str) {
	set<Conc> concs;
	for (unsigned i = 0; i < str.size(); i++)
		concs.insert(Conc(i, str[i]));
	return concs;
}

void print(set<string> ss) {
	for (set<string>::iterator it = ss.begin(); it != ss.end(); it++) 
		cout << "  " << *it << endl;
}
//Процедура, печатающая самый жёсткий запрос, соответствующий данному узлу
void print(Node *node) {
	set<Conc> loop = node->loops;
	for (unsigned i = 0; i < node->words.begin()->size(); i++)
		loop.insert(Conc(i, '?'));
	for (set<Conc>::iterator it = node->loops.begin(); it != node->loops.end(); it++)
		loop.erase(Conc(it->number, '?'));
	for (set<Conc>::iterator it = loop.begin(); it != loop.end(); it++)
		cout << it->letter;
	cout << endl;
}

class Voc {

private:
	//Набор словарей (каждый содержит слова с одинаковым числом букв), в каждом из которых хранятся указатели на узлы 
	vector<vector<Node*>> nodes;
	//Проверка, содержится ли в i-узле m-словаря набор слов words 
	bool contains(set<string> words, int m, int i) {
		bool b = true;
		for (set<string>::iterator it = words.begin(); it != words.end(); it++)
			if (nodes[m][i]->words.find(*it) == nodes[m][i]->words.end()) {
				b = false;
				break;
			}
		return b;
	}
	//Функция, возвращающая nullptr или указатель на вершину m-словаря, содержащую только слова words, если такая есть
	Node *exist(set<string> words, int m) {
		for (unsigned i = 0; i < nodes[m].size(); i++)
			if (nodes[m][i]->words.size() == words.size()) 
				if (contains(words, m, i))
					return nodes[m][i];
		return nullptr;
	}
	//Функция, возвращающая nullptr или указатель на вершину, содержащую только слова words и слово str, если такая есть
	Node *exist(set<string> words, string str) {
		int m = str.size();
		for (unsigned i = 0; i < nodes[m].size(); i++)
			if (nodes[m][i]->words.size() == words.size() + 1 && nodes[m][i]->words.find(str) != nodes[m][i]->words.end()) 
				if (contains(words, m, i))
					return nodes[m][i];
		return nullptr;
	}
	//Функция, возвращающая nullptr или указатель на вершину, содержащую только слово str, если такая есть
	Node *exist(string str) {
		int m = str.size();
		for (unsigned i = 0; i < nodes[m].size(); i++)
			if (nodes[m][i]->words.size() == 1 && nodes[m][i]->words.find(str) != nodes[m][i]->words.end())
				return nodes[m][i];
		return nullptr;
	}
	//Процедура, удаляющая слово str из узлов. Обход осуществляется с помощью вектора vNode
	void check(vector<Node*> vNode, string str) {
		int m = str.size();
		while (vNode.size() > 0) {
			int n = vNode.size() - 1;

			set<string> words = vNode[n]->words;
			words.erase(str);
			Node *node = exist(words, str.size());

			if (node != nullptr) {
				for (int i = 0; i < n; i++) {
					set<Rib> ribs = vNode[i]->ribs;
					for (set<Rib>::iterator it = ribs.begin(); it != ribs.end(); it++)
						if (it->node == vNode[n]) {
							vNode[i]->ribs.erase(*it);
							vNode[i]->ribs.insert(Rib(it->conc, node));
						}
				}
				for (unsigned i = 0; i < nodes[m].size(); i++)
					if (nodes[m][i] == vNode[n]) {
						delete vNode[n];
						nodes[m].erase(nodes[m].begin() + i);
						break;
					}
			} else {
				vNode[n]->words.erase(str);
				set<string> words = vNode[n]->words;
				for (unsigned i = 0; i < words.begin()->size(); i++) {
					bool b = true;
					for (set<string>::iterator it = words.begin(); it != words.end(); it++)
						if ((*it)[i] != (*words.begin())[i]) {
							b = false;
							break;
						}
					if (b)
						vNode[n]->loops.insert(Conc(i, (*words.begin())[i]));
				}
			}
			vNode.pop_back();
		}
	}
	//Функция, добавляющая слово str в узлы (рекурсивно), создающая новые узлы (если нужно) и возвращающая указатель на промежуточный корень
	Node *add(Node *node, string str) {
		set<Rib> brown, green = node->ribs; //brown/green - рёбра, идущие в узлы, в который нужно будет/нет довавить str
		set<Conc> blue, yellow = node->loops, violet = loop(str); //blue - пересечение множества петель узла с loop(str),
		//yellow - разность множества петель узла и blue, violet - разность loop(str) и blue		
		set<Conc> help1;
		for (set<Conc>::iterator it = violet.begin(); it != violet.end(); it++) 
			if (yellow.find(*it) != yellow.end()) {
				help1.insert(*it);
				yellow.erase(*it);
				blue.insert(*it);
			}
		set<Rib> help2;
		for (set<Rib>::iterator it = green.begin(); it != green.end(); it++)
			if (violet.find(it->conc) != violet.end()) {
				help1.insert(it->conc);
				help2.insert(*it);
				brown.insert(*it);
			}
		for (set<Conc>::iterator it = help1.begin(); it != help1.end(); it++)
			violet.erase(*it);
		for (set<Rib>::iterator it = help2.begin(); it != help2.end(); it++)
			green.erase(*it);

		Node *_node;
		if (yellow.size() == 0)
			_node = node;
		else
			_node = new Node();

		_node->words = node->words;
		_node->words.insert(str);
		_node->loops = blue;

		_node->ribs.insert(green.begin(), green.end());
		if (yellow.size() != 0)
			for (set<Conc>::iterator it = yellow.begin(); it != yellow.end(); it++)  
				_node->ribs.insert(Rib(*it, node));

		Node *strNode = exist(str);
		if (strNode == nullptr) {
			strNode = new Node(str);
			nodes[str.size()].push_back(strNode);
		}
		for (set<Conc>::iterator it = violet.begin(); it != violet.end(); it++)
			_node->ribs.insert(Rib(*it, strNode));

		for (set<Rib>::iterator it = brown.begin(); it != brown.end(); it++) 
			if (it->node->words.find(str) == it->node->words.end()) {
				Node *__node = exist(it->node->words, str);
				_node->ribs.erase(Rib(it->conc, nullptr));
				if (__node != nullptr) 
					_node->ribs.insert(Rib(it->conc, __node));
				else 
					_node->ribs.insert(Rib(it->conc, add(it->node, str)));
			}

		if (exist(_node->words, str.size()) == nullptr)
			nodes[str.size()].push_back(_node);
		return _node;
	}
	//Сортировка в порядке убывания
	void sort(vector<Node*> vNode) {
		for (unsigned i = 0; i < vNode.size() - 1; i++)
			if (vNode[i]->words.size() < vNode[i + 1]->words.size()) {
				Node *node = vNode[i];
				vNode[i] = vNode[i + 1];
				vNode[i + 1] = node;
				i = 0;
			}
	}

public:
	//Создание 31 словаря (слова, длиннее 30 букв почти не встречаются)
	Voc() {
		for (int i = 0; i <= 30; i++)
			nodes.push_back(vector<Node*>());
	}

	void add(string str) {
		int m = str.size();
		if (nodes[m].size() == 0) {
			nodes[m].push_back(new Node(str));
		} else {
			if (nodes[m][0]->words.find(str) != nodes[m][0]->words.end())
				cout << "Already have it" << endl;
			else {
				Node *node = add(nodes[m][0], str);
				if (nodes[m][0] != node) {
					nodes[m].insert(nodes[m].begin(), node);
					nodes[m].pop_back();
				}
			}
		}
	}

	void del(string str) {
		int m = str.size();
		if (nodes[m].size() > 0 && nodes[m][0]->words.find(str) != nodes[m][0]->words.end()) {

			vector<Node*> vNode;
			vNode.push_back(nodes[m][0]);

			while (true) {
				if (vNode.size() == 0)
					break;
				int m = str.size();
				set<Conc> strLoop = loop(str);
				for (set<Rib>::iterator it = vNode[0]->ribs.begin(); it != vNode[0]->ribs.end(); it++)
					if (it->node->words.size() == 1 && it->node->words.find(str) == it->node->words.begin()) {
						vNode[0]->ribs.erase(*it);
						it = vNode[0]->ribs.begin();
					}

				for (set<Rib>::iterator it = vNode[0]->ribs.begin(); it != vNode[0]->ribs.end(); it++)
					if (strLoop.find(it->conc) != strLoop.end())
						vNode.push_back(it->node);

				vNode.erase(vNode.begin());
			}

			for (unsigned i = 0; i < nodes[m].size(); i++)
				if (nodes[m][i]->words.find(str) != nodes[m][i]->words.end())
					if (nodes[m][i]->words.size() == 1) {
						delete nodes[m][i];
						nodes[m].erase(nodes[m].begin() + i);
						i--;
					} else
						vNode.push_back(nodes[m][i]);
			if (vNode.size() > 0)
				sort(vNode);

			check(vNode, str);
		} 
		else
			cout << "Not exist" << endl;
	}

	void find(string str) {
		int m = str.size();
		if (nodes[m].size() > 0) {
			Node *node = nodes[m][0];
			for (int i = 0; i < m; i++) {
				if (str[i] != '?') {
					Conc conc = Conc(i, str[i]);
					set<Conc>::iterator ik = node->loops.find(conc);
					if (ik == node->loops.end()) {
						set<Rib>::iterator ir = node->ribs.find(Rib(conc, nullptr));
						if (ir != node->ribs.end())
							node = ir->node;
						else {
							cout << "Not exist" << endl;
							break;
						}
					}
				}
				if (i == m - 1) {
					cout << "----find :" << endl;
					print(node->words);
					cout << "----------" << endl;
				}
			}
		} else
			cout << "Vocabulary is empty" << endl;
	}
	//Печать словарей
	void pr() {
		for (int m = 1; m <= 30; m++) 
			for (unsigned i = 0; i < nodes[m].size(); i++)
				print(nodes[m][i]);
	}
};

int main() {

	Voc voc;
	string str;

	cout << "////////////////Instruction/////////////////" << endl;
	cout << "Keyboard commands:" << endl;
	cout << "  a - to add word" << endl;
	cout << "  d - to delete word" << endl;
	cout << "  f - to find word" << endl;
	cout << "  p - to print vocabulary" << endl;
	cout << "  e - to exit" << endl;
	cout << "All input words must be from 1 to 30 letters" << endl;
	cout << "////////////////////////////////////////////" << endl;

	while (true) {
		cout << "Press letter: ";
		cin >> str;
		if (str == "e")
			break;
		if (str == "a") {
			cout << "Enter the word to add" << endl;
			cin >> str;
			voc.add(str);
		}
		else if (str == "d") {
			cout << "Enter the word to delete" << endl;
			cin >> str;
			voc.del(str);
		}
		else if (str == "f") {
			cout << "Enter the word (with question marks) to find" << endl;
			cin >> str;
			voc.find(str);
		}
		else if (str == "p") {
			cout << "Current nodes (designated as the most tight requests):" << endl;
			voc.pr();
		}
	}
	return EXIT_SUCCESS;
}