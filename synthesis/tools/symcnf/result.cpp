#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

using keyvec = vector<string>;

static keyvec load_map(string filename) {
	keyvec vec {};
	ifstream stream(filename, std::ios::in);

	string name = "";
	unsigned int id = 0;
	vec.push_back("");
	while (stream >> name >> id) {
		if (id != vec.size()) {
			cerr << "invalid ID in map file" << endl;
			exit(1);
		}
		vec.push_back(name);
	}
	return vec;
}

static void translate_result(string filename, keyvec &vec) {
	ifstream stream(filename, std::ios::in);
	
	string line;
	while(getline(stream, line)) {
		if (line.size() < 1) continue;
		if (line[0] != 'v') continue;
		
		istringstream tokens(line.substr(1));
		string tok;
		while (tokens >> tok) {
			bool value = true;
			if (tok[0] == '-') {
				tok = tok.substr(1);
				value = false;
			}
			unsigned long id = stoul(tok);
			if (id > 0) {
				if (id >= vec.size()) {
					cerr << "invalid ID in result file" << endl;
					exit(1);
				}
				if (value) {
					cout << vec[id] << "\n";
				}
			}
		}
	}
}

int main(int argc, char **argv) {
	if (argc < 3) {
		cout << "Usage: " << argv[0] << " [result file] [translation map]" << std::endl;
		exit(0);
	}
	string res_filename = argv[1];
	string map_filename = argv[2];
	
	auto vec = load_map(map_filename);
	translate_result(res_filename, vec);
}