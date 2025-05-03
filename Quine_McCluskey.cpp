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
	bool get_is_minterm(){ return is_minterm; }
	string get_bin_term(int bit);

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

	bool operator==(Implement& other);
	vector<Term> get_terms() const { return terms; }
	int get_mask() const { return mask; }
	friend bool CanCombine( Implement& a, Implement& b);

	bool is_prime;

private:
	vector<Term> terms;
	int mask;
	bool only_dont_care;//don't care term 만으로 이루어졌는지 여부
	
};

Implement::Implement(int mask, Term term) :mask(mask), only_dont_care(0),is_prime(1)
{
	terms.push_back(term);
	if (!term.get_is_minterm())only_dont_care = 1;
}

Implement::Implement(int mask, Term term1, Term term2) :mask(mask), only_dont_care(0), is_prime(1)
{
	terms.push_back(term1);
	terms.push_back(term2);
	if (!term1.get_is_minterm()&& !term2.get_is_minterm())only_dont_care = 1;
}

Implement::~Implement()
{
}

bool Implement::operator==(Implement& other) {
	return (this->mask == other.mask) && ((this->terms[0].get_term() & ~this->mask) == (other.terms[0].get_term() & ~other.mask));
}

bool CanCombine(Implement& a, Implement& b) {
	if (a.mask != b.mask) return false;

	int val_a = a.terms[0].get_term();
	int val_b = b.terms[0].get_term();
	int diff = (val_a ^ val_b) & ~a.mask;

	return (diff != 0) && ((diff & (diff - 1)) == 0);
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

	for (int i = 0; ; i++)
	{
		for (int j = 0; j < implements[i].size(); j++)
		{
			for (int k = j; k < implements[i].size(); k++)
			{

			}
		}
	}

}
