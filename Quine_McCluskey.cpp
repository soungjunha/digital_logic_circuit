#include<iostream>
#include<string>
#include<fstream>
#include<cmath>
#include <vector>

using namespace std;

class Term
{
public:
	Term(bool is_minterm, int term) :is_minterm(is_minterm), term(term),is_prime(true) {}
	~Term();
	int get_term() { return term; }
	string get_bin_term(int bit);
	//bool operator==(const Term& other);

	bool is_prime;

private:
	bool is_minterm; //minterm여부
	int term;
};

Term::~Term()
{
}

string Term::get_bin_term(int bit) {
	string result;
	for (int i = bit-1; i >= 0; --i) {
		result += (term & (1 << i)) ? '1' : '0';
	}
	return result;
}

class Implement
{
public:
	Implement(int mask, Term term);
	Implement(int mask, Term term1, Term term2);
	~Implement();

private:
	vector<Term> terms;
	int mask;
	bool only_dont_care;//don't care term 만으로 이루어졌는지 여부
};

Implement::Implement(int mask, Term term) :mask(mask)
{
	terms.push_back(term);
}

Implement::Implement(int mask, Term term1, Term term2) :mask(mask)
{
	terms.push_back(term1);
	terms.push_back(term2);
}

Implement::~Implement()
{
}

int main() {
	ifstream infile(".\\input_minterm.txt");
	string line;

	getline(infile, line);

	int bit = stoi(line);
	vector<Term> terms;
	vector<vector<Implement>> implements(1);
	vector<Implement> prime_implements;

	cout << bit << endl;
	if (infile.is_open()) {
		while (getline(infile, line)) {
			Term buffer(line[0] == 'm', stoi(line.substr(2), nullptr, 2));
			terms.push_back(buffer);
			//cout << buffer.get_term() << endl;
		}
		infile.close();
	}
	else {
		cerr << "Error: Failed to open file 'input_minterm.txt'" << endl;
	}

	for (int i = 0; i < terms.size() - 1; i++)
	{
		for (int j = i + 1; j < terms.size(); j++)
		{
			int diff = terms[i].get_term() ^ terms[j].get_term();
			if (!(diff & (diff - 1))) {
				Implement buffer(diff, terms[i], terms[j]);
				implements[0].push_back(buffer);
				terms[i].is_prime = terms[j].is_prime = false;
			}
		}
		if (terms[i].is_prime) {
			Implement buffer(0, terms[i]);
			prime_implements.push_back(buffer);
		}
	}

}
