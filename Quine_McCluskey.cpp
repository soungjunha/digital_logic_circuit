#include<iostream>
#include<string>
#include<fstream>
#include<cmath>
#include <vector>
#include <algorithm>

using namespace std;

class Term
{
public:
	Term(bool is_minterm, int term) :is_minterm(is_minterm), term(term), is_prime(true) {}
	~Term();
	int get_term() const { return term; }
	bool get_is_minterm() { return is_minterm; }
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
	for (int i = bit - 1; i >= 0; --i) {
		result += (term & (1 << i)) ? '1' : '0';
	}
	return result;
}

class Implement
{
public:
	Implement(int mask, Term term);
	Implement(int mask, Term term1, Term term2);
	Implement(int mask, vector<Term> terms1, vector<Term> terms2);
	~Implement();

	bool operator==(Implement& other);
	vector<Term> get_terms() const { return terms; }
	int get_mask() const { return mask; }
	bool get_only_dont_care()const { return only_dont_care; }

	friend bool CanCombine(Implement& a, Implement& b);
	friend void removeRedundantTerms(vector<Implement>& prime_implements);

	bool is_prime;

private:
	vector<Term> terms;
	int mask;//-표시가 있는 위치의 비트를 1로 마스킹
	bool only_dont_care;//don't care term 만으로 이루어졌는지 여부

};

Implement::Implement(int mask, Term term) :mask(mask), only_dont_care(0), is_prime(1)
{
	terms.push_back(term);
	if (!term.get_is_minterm())only_dont_care = 1;
}

Implement::Implement(int mask, Term term1, Term term2) :mask(mask), only_dont_care(0), is_prime(1)
{
	terms.push_back(term1);
	terms.push_back(term2);
	if (!term1.get_is_minterm() && !term2.get_is_minterm())only_dont_care = 1;
}

Implement::Implement(int mask, vector<Term> terms1, vector<Term> terms2) :mask(mask), only_dont_care(1), is_prime(1)
{
	terms.insert(terms.end(), terms1.begin(), terms1.end());
	terms.insert(terms.end(), terms2.begin(), terms2.end());
	for (int i = 0; i < terms.size(); i++)
	{
		if (terms[i].get_is_minterm()) {
			only_dont_care = 0;
			break;
		}
	}

}


Implement::~Implement()
{
}

bool Implement::operator==(Implement& other) {
	//implement가 같은지 획인
	return (this->mask == other.mask) && ((this->terms[0].get_term() & ~this->mask) == (other.terms[0].get_term() & ~other.mask));
}

bool CanCombine(Implement& a, Implement& b) {
	if (a.mask != b.mask) return false;

	int val_a = a.terms[0].get_term();
	int val_b = b.terms[0].get_term();
	int diff = (val_a ^ val_b) & ~a.mask;

	return (diff != 0) && ((diff & (diff - 1)) == 0);
}

void removeRedundantTerms(vector<Implement>& prime_implements) {

	for (int i = 0; i < prime_implements.size(); i++)
	{
		sort(prime_implements[i].terms.begin(), prime_implements[i].terms.end(), [](Term& a, Term& b) {return a.get_term() < b.get_term(); });
	}
	std::sort(prime_implements.begin(), prime_implements.end(),
		[](const Implement& a, const Implement& b) {
			if (a.get_mask() != b.get_mask())
				return a.get_mask() < b.get_mask();

			// mask 동일할 때, terms[0] 비교 → 이후 차례로 비교
			const auto& ta = a.get_terms();
			const auto& tb = b.get_terms();
			size_t n = std::min(ta.size(), tb.size());
			for (size_t i = 0; i < n; ++i) {
				if (ta[i].get_term() != tb[i].get_term())
					return ta[i].get_term() < tb[i].get_term();
			}
			return ta.size() < tb.size();
		}
	);

	// 3) std::unique + erase 로 완전 동일 항목 제거
	auto new_end = std::unique(prime_implements.begin(), prime_implements.end(),
		[](const Implement& a, const Implement& b) {
			if (a.get_mask() != b.get_mask()) return false;
			const auto& ta = a.get_terms();
			const auto& tb = b.get_terms();
			if (ta.size() != tb.size()) return false;
			for (size_t i = 0; i < ta.size(); ++i) {
				if (ta[i].get_term() != tb[i].get_term())
					return false;
			}
			return true;
		}
	);
	prime_implements.erase(new_end, prime_implements.end());

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
	bool flag = false;
	int i = 0;
	do {
		flag = false;
		vector<Implement> new_group;
		vector<bool> was_combined(implements[i].size(), false);

		for (int j = 0; j < implements[i].size(); j++) {
			for (int k = j + 1; k < implements[i].size(); k++) {
				if (CanCombine(implements[i][j], implements[i][k])) {
					int n = 0;
					while (implements[i][j].get_terms()[n].get_term() == implements[i][k].get_terms()[n].get_term())
					{
						n++;
					}
					int diff = implements[i][j].get_terms()[0].get_term() ^ implements[i][k].get_terms()[n].get_term();
					int new_mask = implements[i][j].get_mask() | diff;

					Implement combined(new_mask, implements[i][j].get_terms(), implements[i][k].get_terms());

					new_group.push_back(combined);

					implements[i][j].is_prime = false;
					implements[i][k].is_prime = false;
					was_combined[j] = true;
					was_combined[k] = true;
					flag = true;
				}
			}
		}

		for (int j = 0; j < implements[i].size(); j++) {
			if (implements[i][j].is_prime) {
				prime_implements.push_back(implements[i][j]);
			}
		}

		if (!new_group.empty()) {
			implements.push_back(new_group);
		}

		i++;
	} while (flag);

	removeRedundantTerms(prime_implements);

	for (int i = prime_implements.size() - 1; i >= 0; --i) {
		if (prime_implements[i].get_only_dont_care()) prime_implements.erase(prime_implements.begin() + i);
	}



}
