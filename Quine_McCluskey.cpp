#include <iostream>
#include <string>     // 문자열 처리
#include <fstream>    // 파일 입출력
#include <cmath>      // 수학 함수
#include <vector>     // 동적 배열
#include <algorithm>  // 정렬, unique 등
#include <map>        // map 컨테이너
#include <set>        // set 컨테이너
#include <limits>     // 수치 한계값
#include <bitset>     // 비트셋

using namespace std;

// -----------------------------
// Term 클래스
// - minterm 또는 don't care(term) 정보를 저장 및 2진수 문자열 반환
// -----------------------------
class Term {
public:
    // 생성자: is_minterm=true면 실제 minterm, false면 don't care
    Term(bool is_minterm, int term)
        : is_minterm(is_minterm), term(term), is_prime(true) {
    }
    ~Term() {}

    // term 값을 정수로 반환
    int get_term() const { return term; }
    // minterm 여부 반환
    bool get_is_minterm() const { return is_minterm; }
    // bit 길이에 맞춘 2진수 문자열 반환
    string get_bin_term(int bit) const {
        string result;
        // 상위 비트부터 검사하여 '1' 또는 '0' 추가
        for (int i = bit - 1; i >= 0; --i) {
            result += (term & (1 << i)) ? '1' : '0';
        }
        return result;
    }

    bool is_prime;  // prime implicant인지 표시

private:
    bool is_minterm;  // true: minterm, false: don't care
    int term;         // term 값
};

// -----------------------------
// Implement 클래스
// - Quine-McCluskey combine 과정을 거쳐 나온 implicant(집합)
// - mask: '-' 위치 표시 비트
// - terms: 결합된 Term 객체들
// - only_dont_care: don't care만 포함 여부
// -----------------------------
class Implement {
public:
    // 단일 term 생성자: mask=0, prime implicant 초기 형태
    Implement(int mask, Term term)
        : mask(mask), only_dont_care(!term.get_is_minterm()), is_prime(true) {
        terms.push_back(term);
    }
    // 두 term 결합 생성자: mask에 결합된 비트 위치 표시
    Implement(int mask, Term term1, Term term2)
        : mask(mask), only_dont_care(!term1.get_is_minterm() && !term2.get_is_minterm()), is_prime(true) {
        terms.push_back(term1);
        terms.push_back(term2);
    }
    // 다중 term 결합 생성자: vector로 받은 두 그룹 결합
    Implement(int mask, vector<Term> terms1, vector<Term> terms2)
        : mask(mask), only_dont_care(true), is_prime(true) {
        // terms1과 terms2 합치기
        terms.insert(terms.end(), terms1.begin(), terms1.end());
        terms.insert(terms.end(), terms2.begin(), terms2.end());
        // 하나라도 minterm이 포함되면 only_dont_care=false
        for (auto& t : terms) {
            if (t.get_is_minterm()) {
                only_dont_care = false;
                break;
            }
        }
    }
    ~Implement() {}

    // 결합된 Term 벡터 반환
    vector<Term> get_terms() const { return terms; }
    // mask 반환
    int get_mask() const { return mask; }
    // 오직 don't care만 포함하는지 반환
    bool get_only_dont_care() const { return only_dont_care; }

    bool is_prime;  // prime implicant 여부

    // 같은 implicant인지 비교 (mask와 대표 term 값 일치)
    bool operator==(Implement& other) {
        return mask == other.mask
            && ((terms[0].get_term() & ~mask) == (other.terms[0].get_term() & ~other.mask));
    }

    // 두 implicant를 combine 가능 여부 판단
    friend bool CanCombine(Implement& a, Implement& b);
    // 중복 prime implicant 제거 함수
    friend void removeRedundantTerms(vector<Implement>& prime_impls);

private:
    vector<Term> terms;    // 결합된 term들
    int mask;              // '-' 위치 비트마스크
    bool only_dont_care;   // don't care만 포함 여부
};

// -----------------------------
// CanCombine 함수
// - mask가 같고, 두 term의 차이가 정확히 한 비트인 경우에만 true
// -----------------------------
bool CanCombine(Implement& a, Implement& b) {
    if (a.mask != b.mask)
        return false;
    int val_a = a.terms[0].get_term();
    int val_b = b.terms[0].get_term();
    int diff = (val_a ^ val_b) & ~a.mask;  // '-' 비트 제외
    // diff가 0이 아니고, 한 비트만 set인지 확인
    return diff != 0 && ((diff & (diff - 1)) == 0);
}

// -----------------------------
// removeRedundantTerms 함수
// - prime implicant 리스트에서 중복 제거
// -----------------------------
void removeRedundantTerms(vector<Implement>& prime_impls) {
    // 각 implicant의 terms를 정렬(비교 용이하게)
    for (auto& impl : prime_impls) {
        sort(impl.terms.begin(), impl.terms.end(), [](Term& a, Term& b) {
            return a.get_term() < b.get_term();
            });
    }
    // mask, term 순으로 전체 implicant 정렬
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
    // unique를 이용해 동일한 implicant 제거
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
// generateCoverChart 함수
// - prime implicant(P) vs minterm(M) 매핑 차트 생성
// - chart[p][m] = true 이면 P implicant가 m minterm을 커버
// - coverMap에 implicant 인덱스당 커버되는 minterm 인덱스 저장
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

    // 모든 implicant와 minterm 쌍 검사
    for (int i = 0; i < P; ++i) {
        int mask = primeImpls[i].get_mask();
        int val = primeImpls[i].get_terms()[0].get_term();
        for (int j = 0; j < M; ++j) {
            int m = minterms[j].get_term();
            // mask 제외 비트가 같으면 커버
            if ((m & ~mask) == (val & ~mask)) {
                chart[i][j] = true;
                coverMap[i].push_back(j);
            }
        }
    }
}

// -----------------------------
// findEssentialPIs 함수
// - 차트에서 꼭 필요한(essential) prime implicant 인덱스 반환
// - 각 minterm이 오직 하나의 implicant에만 커버되면 그 implicant는 essential
// -----------------------------
vector<int> findEssentialPIs(const vector<vector<bool>>& chart) {
    int P = chart.size();
    if (P == 0) return {};
    int M = chart[0].size();
    vector<bool> isEssential(P, false);

    // 각 컬럼(minterm)에 대해
    for (int j = 0; j < M; ++j) {
        int count = 0, last = -1;
        for (int i = 0; i < P; ++i) {
            if (chart[i][j]) {
                ++count;
                last = i;
            }
        }
        // 오직 하나의 implicant가 커버하면 essential 표시
        if (count == 1)
            isEssential[last] = true;
    }

    // essential 인덱스 리스트 반환
    vector<int> result;
    for (int i = 0; i < P; ++i)
        if (isEssential[i]) result.push_back(i);
    return result;
}

// -----------------------------
// removeEssentialCoverage 함수
// - essential implicant가 커버하는 minterm 제거(커버 차트에서 false로 설정)
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
            // 모든 implicant 행에서 해당 minterm(col) false 처리
            for (auto& row : chart) row[col] = false;
        }
    }
}

// -----------------------------
// buildPetrick 함수
// - 남은 미커버(minterms) 컬럼에 대해 Petrick 방법으로 조합 생성
// - sums: 각 미커버 minterm을 커버하는 implicant 인덱스 리스트
// - products: 가능한 implicant 조합의 집합(집합) 생성 및 흡수 법칙으로 정리
// -----------------------------
vector<vector<int>> buildPetrick(const vector<vector<bool>>& chart) {
    vector<vector<int>> sums;
    int P = chart.size();
    int M = chart[0].size();

    // 각 미커버 컬럼에 대해 수집
    for (int j = 0; j < M; ++j) {
        vector<int> coverers;
        for (int i = 0; i < P; ++i)
            if (chart[i][j]) coverers.push_back(i);
        if (!coverers.empty()) sums.push_back(coverers);
    }
    if (sums.empty()) return {};

    // 첫 번째 sum을 초기 products로
    vector<set<int>> products;
    for (int pi : sums[0]) products.push_back({ pi });

    // 나머지 sums와 곱셈 확장
    for (int k = 1; k < sums.size(); ++k) {
        vector<set<int>> next;
        for (auto& prod : products) {
            for (int pi : sums[k]) {
                auto np = prod;
                np.insert(pi);
                next.push_back(np);
            }
        }
        // 흡수 법칙 적용: 다른 집합을 포함하는 경우 제거
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

    // 결과를 vector<vector<int>>로 변환
    vector<vector<int>> combos;
    for (auto& s : products) combos.emplace_back(s.begin(), s.end());
    return combos;
}

// -----------------------------
// selectMinCombination 함수
// - Petrick 조합 중 최소 크기(implicant 개수) 조합 선택
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
// main 함수
// 1) 입력 파일 읽어 bit 수와 minterm/don't care 목록 파싱
// 2) 1차 implicant 그룹 생성 및 prime implicant 찾기
// 3) 반복 결합(combine)하여 prime implicant 수집
// 4) 중복 제거 및 don't care 전용 제거
// 5) cover chart 생성, essential 계산, Petrick 방법 적용
// 6) 결과(result.txt)에 패턴 및 트랜지스터 비용 출력
// -----------------------------
int main() {
    // 입력 파일 오픈 및 첫 줄(bit 수) 읽기
    ifstream infile("input_minterm.txt");
    string line;
    getline(infile, line);
    int bit = stoi(line);

    vector<Term> terms;
    vector<vector<Implement>> groups(1);
    vector<Implement> prime_impls;

    // 나머지 줄: 'm 0101' 또는 'd 0110' 형태로 term 추가
    while (getline(infile, line)) {
        bool is_m = (line[0] == 'm');
        int val = stoi(line.substr(2), nullptr, 2);
        terms.emplace_back(is_m, val);
    }
    infile.close();

    // 초기 combine: 한 비트 차이인 pair를 groups[0]에 추가
    for (int i = 0; i < terms.size(); ++i) {
        for (int j = i + 1; j < terms.size(); ++j) {
            int diff = terms[i].get_term() ^ terms[j].get_term();
            // diff가 한 비트만 다르면 결합
            if (!(diff & (diff - 1))) {
                groups[0].emplace_back(diff, terms[i], terms[j]);
                terms[i].is_prime = terms[j].is_prime = false;
            }
        }
        // combine되지 않고 is_prime 남으면 prime_impls에 추가
        if (terms[i].is_prime)
            prime_impls.emplace_back(0, terms[i]);
    }

    // 반복 결합: 새 그룹이 없을 때까지
    bool changed;
    int idx = 0;
    do {
        changed = false;
        vector<Implement> next_group;
        auto& current = groups[idx];

        // 각 pair에 대해 combine 가능한지 검사
        for (int i = 0; i < current.size(); ++i) {
            for (int j = i + 1; j < current.size(); ++j) {
                if (CanCombine(current[i], current[j])) {
                    // 결합 비트 찾고 새로운 mask 계산
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
        // 현 그룹의 prime implicant 추가
        for (auto& impl : current)
            if (impl.is_prime)
                prime_impls.push_back(impl);

        if (!next_group.empty()) groups.push_back(next_group);
        idx++;
    } while (changed);

    // 중복 prime implicant 제거
    removeRedundantTerms(prime_impls);
    // don't care 전용 implicant 제거
    for (int i = prime_impls.size() - 1; i >= 0; --i)
        if (prime_impls[i].get_only_dont_care())
            prime_impls.erase(prime_impls.begin() + i);

    // 실제 minterm 리스트로 필터링
    for (int i = terms.size() - 1; i >= 0; --i)
        if (!terms[i].get_is_minterm())
            terms.erase(terms.begin() + i);

    // cover chart 생성
    vector<vector<bool>> chart;
    map<int, vector<int>> coverMap;
    generateCoverChart(prime_impls, terms, chart, coverMap);

    // essential prime implicant 찾기
    vector<int> essential = findEssentialPIs(chart);
    cout << "Essential Prime Implicants indices: ";
    for (int pi : essential)
        cout << prime_impls[pi].get_terms()[0].get_bin_term(bit) << " ";
    cout << endl;

    // essential 커버 제거
    removeEssentialCoverage(chart, coverMap, essential);

    // Petrick 방법으로 추가 implicant 선택
    auto combos = buildPetrick(chart);
    auto petrickSel = selectMinCombination(combos);

    // 최종 implicant 인덱스 집합
    set<int> finalSet(essential.begin(), essential.end());
    finalSet.insert(petrickSel.begin(), petrickSel.end());

    // 결과 파일에 패턴 및 트랜지스터 비용 계산
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
                    totalCost += 2; // 인버터 비용
                }
            }
        }
        fout << pattern << "\n";
        int dashCount = bitset<64>(mask).count();
        if ((bit - dashCount) > 1) totalCost += (2 * (bit - dashCount) + 2); // AND 게이트 비용
    }
    if(finalSet.size()>1) totalCost += (2 * finalSet.size() + 2); // 최종 OR 게이트 비용
    fout << "Cost (# of transistors): " << totalCost << "\n";
    fout.close();

    return 0;
}
