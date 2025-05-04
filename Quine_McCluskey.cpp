#include <iostream>
#include <string>     // ���ڿ� ó��
#include <fstream>    // ���� �����
#include <cmath>      // ���� �Լ�
#include <vector>     // ���� �迭
#include <algorithm>  // ����, unique ��
#include <map>        // map �����̳�
#include <set>        // set �����̳�
#include <limits>     // ��ġ �Ѱ谪
#include <bitset>     // ��Ʈ��

using namespace std;

// -----------------------------
// Term Ŭ����
// - minterm �Ǵ� don't care(term) ������ ���� �� 2���� ���ڿ� ��ȯ
// -----------------------------
class Term {
public:
    // ������: is_minterm=true�� ���� minterm, false�� don't care
    Term(bool is_minterm, int term)
        : is_minterm(is_minterm), term(term), is_prime(true) {
    }
    ~Term() {}

    // term ���� ������ ��ȯ
    int get_term() const { return term; }
    // minterm ���� ��ȯ
    bool get_is_minterm() const { return is_minterm; }
    // bit ���̿� ���� 2���� ���ڿ� ��ȯ
    string get_bin_term(int bit) const {
        string result;
        // ���� ��Ʈ���� �˻��Ͽ� '1' �Ǵ� '0' �߰�
        for (int i = bit - 1; i >= 0; --i) {
            result += (term & (1 << i)) ? '1' : '0';
        }
        return result;
    }

    bool is_prime;  // prime implicant���� ǥ��

private:
    bool is_minterm;  // true: minterm, false: don't care
    int term;         // term ��
};

// -----------------------------
// Implement Ŭ����
// - Quine-McCluskey combine ������ ���� ���� implicant(����)
// - mask: '-' ��ġ ǥ�� ��Ʈ
// - terms: ���յ� Term ��ü��
// - only_dont_care: don't care�� ���� ����
// -----------------------------
class Implement {
public:
    // ���� term ������: mask=0, prime implicant �ʱ� ����
    Implement(int mask, Term term)
        : mask(mask), only_dont_care(!term.get_is_minterm()), is_prime(true) {
        terms.push_back(term);
    }
    // �� term ���� ������: mask�� ���յ� ��Ʈ ��ġ ǥ��
    Implement(int mask, Term term1, Term term2)
        : mask(mask), only_dont_care(!term1.get_is_minterm() && !term2.get_is_minterm()), is_prime(true) {
        terms.push_back(term1);
        terms.push_back(term2);
    }
    // ���� term ���� ������: vector�� ���� �� �׷� ����
    Implement(int mask, vector<Term> terms1, vector<Term> terms2)
        : mask(mask), only_dont_care(true), is_prime(true) {
        // terms1�� terms2 ��ġ��
        terms.insert(terms.end(), terms1.begin(), terms1.end());
        terms.insert(terms.end(), terms2.begin(), terms2.end());
        // �ϳ��� minterm�� ���ԵǸ� only_dont_care=false
        for (auto& t : terms) {
            if (t.get_is_minterm()) {
                only_dont_care = false;
                break;
            }
        }
    }
    ~Implement() {}

    // ���յ� Term ���� ��ȯ
    vector<Term> get_terms() const { return terms; }
    // mask ��ȯ
    int get_mask() const { return mask; }
    // ���� don't care�� �����ϴ��� ��ȯ
    bool get_only_dont_care() const { return only_dont_care; }

    bool is_prime;  // prime implicant ����

    // ���� implicant���� �� (mask�� ��ǥ term �� ��ġ)
    bool operator==(Implement& other) {
        return mask == other.mask
            && ((terms[0].get_term() & ~mask) == (other.terms[0].get_term() & ~other.mask));
    }

    // �� implicant�� combine ���� ���� �Ǵ�
    friend bool CanCombine(Implement& a, Implement& b);
    // �ߺ� prime implicant ���� �Լ�
    friend void removeRedundantTerms(vector<Implement>& prime_impls);

private:
    vector<Term> terms;    // ���յ� term��
    int mask;              // '-' ��ġ ��Ʈ����ũ
    bool only_dont_care;   // don't care�� ���� ����
};

// -----------------------------
// CanCombine �Լ�
// - mask�� ����, �� term�� ���̰� ��Ȯ�� �� ��Ʈ�� ��쿡�� true
// -----------------------------
bool CanCombine(Implement& a, Implement& b) {
    if (a.mask != b.mask)
        return false;
    int val_a = a.terms[0].get_term();
    int val_b = b.terms[0].get_term();
    int diff = (val_a ^ val_b) & ~a.mask;  // '-' ��Ʈ ����
    // diff�� 0�� �ƴϰ�, �� ��Ʈ�� set���� Ȯ��
    return diff != 0 && ((diff & (diff - 1)) == 0);
}

// -----------------------------
// removeRedundantTerms �Լ�
// - prime implicant ����Ʈ���� �ߺ� ����
// -----------------------------
void removeRedundantTerms(vector<Implement>& prime_impls) {
    // �� implicant�� terms�� ����(�� �����ϰ�)
    for (auto& impl : prime_impls) {
        sort(impl.terms.begin(), impl.terms.end(), [](Term& a, Term& b) {
            return a.get_term() < b.get_term();
            });
    }
    // mask, term ������ ��ü implicant ����
    sort(prime_impls.begin(), prime_impls.end(), [](const Implement& a, const Implement& b) {
        if (a.mask != b.mask)
            return a.mask < b.mask;
        auto ta = a.terms;
        auto tb = b.terms;
        size_t n = min(ta.size(), tb.size());
        for (size_t i = 0; i < n; ++i) {
            if (ta[i].get_term() != tb[i].get_term())
                return ta[i].get_term() < tb[i].get_term();
        }
        return ta.size() < tb.size();
        });
    // unique�� �̿��� ������ implicant ����
    auto new_end = unique(prime_impls.begin(), prime_impls.end(), [](const Implement& a, const Implement& b) {
        if (a.mask != b.mask) return false;
        if (a.terms.size() != b.terms.size()) return false;
        for (size_t i = 0; i < a.terms.size(); ++i) {
            if (a.terms[i].get_term() != b.terms[i].get_term())
                return false;
        }
        return true;
        });
    prime_impls.erase(new_end, prime_impls.end());
}

// -----------------------------
// generateCoverChart �Լ�
// - prime implicant(P) vs minterm(M) ���� ��Ʈ ����
// - chart[p][m] = true �̸� P implicant�� m minterm�� Ŀ��
// - coverMap�� implicant �ε����� Ŀ���Ǵ� minterm �ε��� ����
// -----------------------------
void generateCoverChart(
    const vector<Implement>& primeImpls,
    const vector<Term>& minterms,
    vector<vector<bool>>& chart,
    map<int, vector<int>>& coverMap)
{
    int P = primeImpls.size();
    int M = minterms.size();
    chart.assign(P, vector<bool>(M, false));
    coverMap.clear();

    // ��� implicant�� minterm �� �˻�
    for (int i = 0; i < P; ++i) {
        int mask = primeImpls[i].get_mask();
        int val = primeImpls[i].get_terms()[0].get_term();
        for (int j = 0; j < M; ++j) {
            int m = minterms[j].get_term();
            // mask ���� ��Ʈ�� ������ Ŀ��
            if ((m & ~mask) == (val & ~mask)) {
                chart[i][j] = true;
                coverMap[i].push_back(j);
            }
        }
    }
}

// -----------------------------
// findEssentialPIs �Լ�
// - ��Ʈ���� �� �ʿ���(essential) prime implicant �ε��� ��ȯ
// - �� minterm�� ���� �ϳ��� implicant���� Ŀ���Ǹ� �� implicant�� essential
// -----------------------------
vector<int> findEssentialPIs(const vector<vector<bool>>& chart) {
    int P = chart.size();
    if (P == 0) return {};
    int M = chart[0].size();
    vector<bool> isEssential(P, false);

    // �� �÷�(minterm)�� ����
    for (int j = 0; j < M; ++j) {
        int count = 0, last = -1;
        for (int i = 0; i < P; ++i) {
            if (chart[i][j]) {
                ++count;
                last = i;
            }
        }
        // ���� �ϳ��� implicant�� Ŀ���ϸ� essential ǥ��
        if (count == 1)
            isEssential[last] = true;
    }

    // essential �ε��� ����Ʈ ��ȯ
    vector<int> result;
    for (int i = 0; i < P; ++i)
        if (isEssential[i]) result.push_back(i);
    return result;
}

// -----------------------------
// removeEssentialCoverage �Լ�
// - essential implicant�� Ŀ���ϴ� minterm ����(Ŀ�� ��Ʈ���� false�� ����)
// -----------------------------
void removeEssentialCoverage(
    vector<vector<bool>>& chart,
    const map<int, vector<int>>& coverMap,
    const vector<int>& essentialIdx)
{
    for (int pi : essentialIdx) {
        auto it = coverMap.find(pi);
        if (it == coverMap.end()) continue;
        for (int col : it->second) {
            // ��� implicant �࿡�� �ش� minterm(col) false ó��
            for (auto& row : chart) row[col] = false;
        }
    }
}

// -----------------------------
// buildPetrick �Լ�
// - ���� ��Ŀ��(minterms) �÷��� ���� Petrick ������� ���� ����
// - sums: �� ��Ŀ�� minterm�� Ŀ���ϴ� implicant �ε��� ����Ʈ
// - products: ������ implicant ������ ����(����) ���� �� ��� ��Ģ���� ����
// -----------------------------
vector<vector<int>> buildPetrick(const vector<vector<bool>>& chart) {
    vector<vector<int>> sums;
    int P = chart.size();
    int M = chart[0].size();

    // �� ��Ŀ�� �÷��� ���� ����
    for (int j = 0; j < M; ++j) {
        vector<int> coverers;
        for (int i = 0; i < P; ++i)
            if (chart[i][j]) coverers.push_back(i);
        if (!coverers.empty()) sums.push_back(coverers);
    }
    if (sums.empty()) return {};

    // ù ��° sum�� �ʱ� products��
    vector<set<int>> products;
    for (int pi : sums[0]) products.push_back({ pi });

    // ������ sums�� ���� Ȯ��
    for (int k = 1; k < sums.size(); ++k) {
        vector<set<int>> next;
        for (auto& prod : products) {
            for (int pi : sums[k]) {
                auto np = prod;
                np.insert(pi);
                next.push_back(np);
            }
        }
        // ��� ��Ģ ����: �ٸ� ������ �����ϴ� ��� ����
        vector<set<int>> filtered;
        for (auto& s : next) {
            bool absorbed = false;
            for (auto& t : next) {
                if (s != t && includes(s.begin(), s.end(), t.begin(), t.end())) {
                    absorbed = true;
                    break;
                }
            }
            if (!absorbed) filtered.push_back(s);
        }
        products = move(filtered);
    }

    // ����� vector<vector<int>>�� ��ȯ
    vector<vector<int>> combos;
    for (auto& s : products) combos.emplace_back(s.begin(), s.end());
    return combos;
}

// -----------------------------
// selectMinCombination �Լ�
// - Petrick ���� �� �ּ� ũ��(implicant ����) ���� ����
// -----------------------------
vector<int> selectMinCombination(const vector<vector<int>>& combos) {
    size_t best = numeric_limits<size_t>::max();
    for (auto& c : combos)
        best = min(best, c.size());
    for (auto& c : combos)
        if (c.size() == best) return c;
    return {};
}

// -----------------------------
// main �Լ�
// 1) �Է� ���� �о� bit ���� minterm/don't care ��� �Ľ�
// 2) 1�� implicant �׷� ���� �� prime implicant ã��
// 3) �ݺ� ����(combine)�Ͽ� prime implicant ����
// 4) �ߺ� ���� �� don't care ���� ����
// 5) cover chart ����, essential ���, Petrick ��� ����
// 6) ���(result.txt)�� ���� �� Ʈ�������� ��� ���
// -----------------------------
int main() {
    // �Է� ���� ���� �� ù ��(bit ��) �б�
    ifstream infile("input_minterm.txt");
    string line;
    getline(infile, line);
    int bit = stoi(line);

    vector<Term> terms;
    vector<vector<Implement>> groups(1);
    vector<Implement> prime_impls;

    // ������ ��: 'm 0101' �Ǵ� 'd 0110' ���·� term �߰�
    while (getline(infile, line)) {
        bool is_m = (line[0] == 'm');
        int val = stoi(line.substr(2), nullptr, 2);
        terms.emplace_back(is_m, val);
    }
    infile.close();

    // �ʱ� combine: �� ��Ʈ ������ pair�� groups[0]�� �߰�
    for (int i = 0; i < terms.size(); ++i) {
        for (int j = i + 1; j < terms.size(); ++j) {
            int diff = terms[i].get_term() ^ terms[j].get_term();
            // diff�� �� ��Ʈ�� �ٸ��� ����
            if (!(diff & (diff - 1))) {
                groups[0].emplace_back(diff, terms[i], terms[j]);
                terms[i].is_prime = terms[j].is_prime = false;
            }
        }
        // combine���� �ʰ� is_prime ������ prime_impls�� �߰�
        if (terms[i].is_prime)
            prime_impls.emplace_back(0, terms[i]);
    }

    // �ݺ� ����: �� �׷��� ���� ������
    bool changed;
    int idx = 0;
    do {
        changed = false;
        vector<Implement> next_group;
        auto& current = groups[idx];

        // �� pair�� ���� combine �������� �˻�
        for (int i = 0; i < current.size(); ++i) {
            for (int j = i + 1; j < current.size(); ++j) {
                if (CanCombine(current[i], current[j])) {
                    // ���� ��Ʈ ã�� ���ο� mask ���
                    vector<Term> t1 = current[i].get_terms();
                    vector<Term> t2 = current[j].get_terms();
                    int diff = 0;
                    for (int k = 0; k < t1.size(); ++k) {
                        int x = t1[k].get_term();
                        int y = t2[k].get_term();
                        if (x != y) diff = x ^ y;
                    }
                    int new_mask = current[i].get_mask() | diff;
                    next_group.emplace_back(new_mask, t1, t2);

                    current[i].is_prime = current[j].is_prime = false;
                    changed = true;
                }
            }
        }
        // �� �׷��� prime implicant �߰�
        for (auto& impl : current)
            if (impl.is_prime)
                prime_impls.push_back(impl);

        if (!next_group.empty()) groups.push_back(next_group);
        idx++;
    } while (changed);

    // �ߺ� prime implicant ����
    removeRedundantTerms(prime_impls);
    // don't care ���� implicant ����
    for (int i = prime_impls.size() - 1; i >= 0; --i)
        if (prime_impls[i].get_only_dont_care())
            prime_impls.erase(prime_impls.begin() + i);

    // ���� minterm ����Ʈ�� ���͸�
    for (int i = terms.size() - 1; i >= 0; --i)
        if (!terms[i].get_is_minterm())
            terms.erase(terms.begin() + i);

    // cover chart ����
    vector<vector<bool>> chart;
    map<int, vector<int>> coverMap;
    generateCoverChart(prime_impls, terms, chart, coverMap);

    // essential prime implicant ã��
    vector<int> essential = findEssentialPIs(chart);
    cout << "Essential Prime Implicants indices: ";
    for (int pi : essential)
        cout << prime_impls[pi].get_terms()[0].get_bin_term(bit) << " ";
    cout << endl;

    // essential Ŀ�� ����
    removeEssentialCoverage(chart, coverMap, essential);

    // Petrick ������� �߰� implicant ����
    auto combos = buildPetrick(chart);
    auto petrickSel = selectMinCombination(combos);

    // ���� implicant �ε��� ����
    set<int> finalSet(essential.begin(), essential.end());
    finalSet.insert(petrickSel.begin(), petrickSel.end());

    // ��� ���Ͽ� ���� �� Ʈ�������� ��� ���
    ofstream fout("result.txt");
    int totalCost = 0;
    vector<bool> inv_flag(bit, false);
    for (int idx : finalSet) {
        int mask = prime_impls[idx].get_mask();
        int val = prime_impls[idx].get_terms()[0].get_term();
        string pattern;
        for (int b = bit - 1; b >= 0; --b) {
            if (mask & (1 << b)) {
                pattern.push_back('_');
            }
            else if (val & (1 << b)) {
                pattern.push_back('1');
            }
            else {
                pattern.push_back('0');
                if (!inv_flag[bit - b - 1]) {
                    inv_flag[bit - b - 1] = true;
                    totalCost += 2; // �ι��� ���
                }
            }
        }
        fout << pattern << "\n";
        int dashCount = bitset<64>(mask).count();
        if ((bit - dashCount) > 1) totalCost += (2 * (bit - dashCount) + 2); // AND ����Ʈ ���
    }
    if(finalSet.size()>1) totalCost += (2 * finalSet.size() + 2); // ���� OR ����Ʈ ���
    fout << "Cost (# of transistors): " << totalCost << "\n";
    fout.close();

    return 0;
}
