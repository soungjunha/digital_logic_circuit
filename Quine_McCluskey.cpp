#include<iostream>
#include<string>
#include<fstream>
#include<cmath>
#include <vector>

using namespace std;

class Term
{
public:
    Term(bool is_minterm, string term) :is_minterm(is_minterm), term(term) {}
    ~Term();
    string get_term() { return term; }
    //bool operator==(const Term& other);

private:
    bool is_minterm;
    string term;

};

Term::~Term()
{
}

class Group
{
public:
    Group();
    ~Group();

private:
    string term;
    vector<int> minterms;
    bool only_dont_care;
};

Group::Group()
{
}

Group::~Group()
{
}

int main() {

    ifstream infile(".\\input_minterm.txt");
    string line;

    getline(infile, line);

    int bit = stoi(line);
    vector<Term> terms;
    vector<Group> groups;

    cout << bit << endl;
    if (infile.is_open()) {
        while (getline(infile, line)) {
            Term buffer(line[0] == 'm', line.substr(2));
            terms.push_back(buffer);
        }
        infile.close();
    }
    else {
        cerr << "Error: Failed to open file 'input_minterm.txt'" << endl;
    }

    for (int i = 0; i < terms.capacity(); i++)
    {
        for (int j = i + 1; j < terms.capacity(); j++)
        {

        }
    }

}
