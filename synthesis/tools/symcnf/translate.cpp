#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

using namespace std;

using keymap = unordered_map<string, unsigned int>;

static keymap load_map(string filename) {
	auto map = unordered_map<string, unsigned int>();
	ifstream stream(filename, std::ios::in);

	string name = "";
	unsigned int id = 0;
	while (stream >> name >> id) {
		map[name] = id;
	}
	return map;
}

static int count_clauses(string filename) {
	auto clauses = 0;

	ifstream stream(filename, std::ios::in);
	
	string line;
	while(getline(stream, line)) {
		if (line[0] != '#') clauses++;
	}
	return clauses;
}

static void translate_cnf(string filename, keymap &map) {
	ifstream stream(filename, std::ios::in);
	
	string line;
	while(getline(stream, line)) {
		if (line[0] == '#') continue;
		
		istringstream tokens(line);
		string tok;
		while (tokens >> tok) {
			if (tok[0] == '-') {
				tok = tok.substr(1);
				cout << '-';
			}
			cout << map[tok] << " ";
		}
		cout << "0\n";
	}
}

int main(int argc, char **argv) {
	if (argc < 3) {
		cout << "Usage: " << argv[0] << " [symbolic cnf] [translation map]" << std::endl;
		exit(0);
	}
	string cnf_filename = argv[1];
	string map_filename = argv[2];
	
	auto map = load_map(map_filename);
	auto clauses = count_clauses(cnf_filename);
	cout << "p cnf " << map.size() << " " << clauses << "\n";
	translate_cnf(cnf_filename, map);
}