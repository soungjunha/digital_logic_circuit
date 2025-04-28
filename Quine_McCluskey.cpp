#include<iostream>
#include<string>
#include<fstream>
#include<cmath>
#include <vector>

using namespace std;

class Term
{
public:
	Term();
	~Term();

private:
	bool is_minterm;
	string term;

};

Term::Term()
{
}

Term::~Term()
{
}

int main() {

    ifstream input(".\\input_minterm.txt");
    string line;

    getline(input, line);

    int bit = stoi(line);
    vector<Term> terms;

    cout << bit << endl;
    if (input.is_open()) {
        while (getline(input, line)) {
            cout << line << endl;
        }
        input.close();
    }
    else {
        cerr << "Error: Failed to open file 'input_minterm.txt'" << endl;
    }

}
