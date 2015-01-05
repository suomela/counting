#include <iostream>

using namespace std;

int main() {
	auto count = 0;
	string token;
	while(cin >> token) {
		count += 1;
		cout << token << " " << count << endl;
	}
}