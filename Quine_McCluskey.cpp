#include<iostream>
#include<string>
#include<fstream>
#include<cmath>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <limits>
#include <bitset>

using namespace std;

class Term
{
public:
	Term(bool is_minterm, int term) :is_minterm(is_minterm), term(term), is_prime(true) {}
	~Term();
	int get_term() const { return term; }
	bool get_is_minterm() { return is_minterm; }
	string get_bin_term(int bit)const;

	bool is_prime;

private:
	bool is_minterm; //minterm여부
	int term;
};

Term::~Term()
{
}

string Term::get_bin_term(int bit) const {
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

void generateCoverChart(const vector<Implement>& primeImpls, const vector<Term>& minterms, vector<vector<bool>>& chart, map<int, vector<int>>& coverMap)
{
	int P = primeImpls.size();
	int M = minterms.size();
	chart.assign(P, std::vector<bool>(M, false));
	coverMap.clear();

	for (int i = 0; i < P; ++i) {
		int mask = primeImpls[i].get_mask();
		int value = primeImpls[i].get_terms()[0].get_term();
		for (int j = 0; j < M; ++j) {
			int m = minterms[j].get_term();
			if ((m & ~mask) == (value & ~mask)) {
				chart[i][j] = true;
				coverMap[i].push_back(j);
			}
		}
	}
}

vector<int> findEssentialPIs(const vector<vector<bool>>& chart) {
	int P = chart.size();
	if (P == 0) return {};
	int M = chart[0].size();

	vector<bool> isEssential(P, false);

	for (int j = 0; j < M; ++j) {
		int count = 0, lastRow = -1;
		for (int i = 0; i < P; ++i) {
			if (chart[i][j]) {
				++count;
				lastRow = i;
			}
		}
		if (count == 1) {
			isEssential[lastRow] = true;
		}
	}

	vector<int> essentialIdx;
	for (int i = 0; i < P; ++i) {
		if (isEssential[i]) essentialIdx.push_back(i);
	}
	return essentialIdx;
}

void removeEssentialCoverage(vector<vector<bool>>& chart, const map<int, vector<int>>& coverMap, const vector<int>& essentialIdx)
{
	for (int pi : essentialIdx) {
		auto it = coverMap.find(pi);
		if (it == coverMap.end()) continue;
		for (int col : it->second) {
			for (auto& row : chart) {
				row[col] = false;
			}
		}
	}
}

vector<vector<int>> buildPetrick(const vector<vector<bool>>& chart) {
	vector<vector<int>> sums;
	for (int j = 0; j < (int)chart[0].size(); ++j) {
		vector<int> termCover;
		for (int i = 0; i < (int)chart.size(); ++i)
			if (chart[i][j]) termCover.push_back(i);
		if (!termCover.empty()) sums.push_back(termCover);
	}
	if (sums.empty()) return {};

	vector<set<int>> products;
	for (int pi : sums[0])
		products.push_back({ pi });

	for (int k = 1; k < (int)sums.size(); ++k) {
		vector<set<int>> next;
		for (const auto& prod : products)
			for (int pi : sums[k]) {
				auto np = prod;
				np.insert(pi);
				next.push_back(np);
			}
		vector<set<int>> filtered;
		for (const auto& s : next) {
			bool absorbed = false;
			for (const auto& t : next)
				if (s != t && includes(s.begin(), s.end(),
					t.begin(), t.end())) {
					absorbed = true;
					break;
				}
			if (!absorbed) filtered.push_back(s);
		}
		products = move(filtered);
	}

	vector<vector<int>> combos;
	for (const auto& s : products)
		combos.emplace_back(s.begin(), s.end());
	return combos;
}

vector<int> selectMinCombination(const vector<vector<int>>& combos) {
	size_t bestSize = numeric_limits<size_t>::max();
	for (const auto& c : combos)
		bestSize = min(bestSize, c.size());
	for (const auto& c : combos)
		if (c.size() == bestSize) return c;
	return {};
}

int popcount_manual(int x, int bits) {
	int cnt = 0;
	for (int b = 0; b < bits; ++b) {
		cnt += (x >> b) & 1;
	}
	return cnt;
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

	for (int i = terms.size() - 1; i >= 0; --i) {
		if (!terms[i].get_is_minterm()) terms.erase(terms.begin() + i);
	}

	vector<vector<bool>> chart;
	map<int, vector<int>> coverMap;
	generateCoverChart(prime_implements, terms, chart, coverMap);

	vector<int> essentialIdx = findEssentialPIs(chart);

	cout << "Essential Prime Implicants indices: ";
	for (int i = 0; i < essentialIdx.size(); ++i) {
		int piIndex = essentialIdx[i];
		cout << prime_implements[piIndex].get_terms()[0].get_bin_term(bit) << " ";
	}
	cout << endl;

	removeEssentialCoverage(chart, coverMap, essentialIdx);

	auto combos = buildPetrick(chart);
	auto petSelect = selectMinCombination(combos);

	// 4) 최종 PI 집합
	set<int> finalSet(essentialIdx.begin(), essentialIdx.end());
	finalSet.insert(petSelect.begin(), petSelect.end());

	// 5) 결과 출력
	ofstream fout("result.txt");
	int totalCost = 0;
	vector<int> finalList(finalSet.begin(), finalSet.end());
	vector<bool> invertor_flag(bit);

	// 전통적인 for문: int i = 0; 조건; i++ 형태
	for (int i = 0; i < static_cast<int>(finalList.size()); ++i) {
		int idx = finalList[i];                       // i번째 요소 얻기
		const auto& impl = prime_implements[idx];     // 해당 prime implicant 참조

		int mask = impl.get_mask();
		int value = impl.get_terms()[0].get_term();

		string pat;
		for (int b = bit - 1; b >= 0; --b) {
			if (mask & (1 << b)) {
				pat.push_back('_');
			}
			else {
				if (value & (1 << b))
					pat.push_back('1');
				else {
					pat.push_back('0');
					if (!invertor_flag[bit - b - 1]) {
						invertor_flag[bit - b - 1] = true;
						totalCost += 2;
					}
				}
			}
		}
		fout << pat << "\n";

		int dashCount = bitset<64>(mask).count();
		totalCost += (2 * (bit - dashCount) + 2);
	}
	totalCost += (2 * finalSet.size() + 2);
	fout << "Cost (# of transistors): " << totalCost << "\n";
	fout.close();

}
