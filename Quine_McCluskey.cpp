#include<iostream>
#include<string>
#include<fstream>
#include<cmath>
#include <vector>

using namespace std;

class Term
{
public:
    Term(bool is_minterm, int term) :is_minterm(is_minterm), term(term) {}
    ~Term();
    int get_term() { return term; }
    //bool operator==(const Term& other);

private:
    bool is_minterm; //minterm¿©ºÎ
    int term;

};

Term::~Term()
{
}

class Implement
{
public:
    Implement(int mask, Term term1, Term term2);
    ~Implement();

private:
    vector<Term> terms;
    int mask;
    bool only_dont_care;
};

Implement::Implement(int mask=0, Term term1, Term term2):mask(mask)
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
    vector<vector<Implement>> implements;
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
    
    for (int i = 0; i < terms.size(); i++)
    {
        bool is_prime=true;
        for (int j = i + 1; j < terms.size(); j++)
        {
            int diff = terms[i].get_term() ^ terms[j].get_term();
            if (!(diff & (diff - 1))) {
                Implement buffer(diff, terms[i], terms[j]);
                implements[0].push_back(buffer);
                if (is_prime) is_prime = false;
            }
        }
        if (is_prime) {
            prime_implements.push_back(terms[i]);
        }
    }

}
