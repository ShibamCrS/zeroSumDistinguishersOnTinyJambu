#include<iostream>
#include<cstdio>
#include<bitset>
#include<vector>
#include<set>
#include<map>
#include<cmath>
#include<fstream>
#include<chrono>
#include<string>
#include<algorithm>
#include"gurobi_c++.h" 

using namespace std;
#define THREAD 8
#define STATE 384
#define PUBVAR 128
#define KEYVAR 256
typedef bitset<STATE> Monomial;
typedef bitset<PUBVAR> Monomial_pub;
int depth = 0;
const int MAX = 200000000; // the maximum value of PoolSearchMode, P625

void PRINT_MONOMIAL(bitset<STATE> mono)
{
    string v ="";
    for (int i=0; i < STATE; i++){
        if(mono.test(i)){
            if (i < PUBVAR)
                v = v + "v" + to_string(i);  
            else
                v = v + "k" + to_string(i-PUBVAR);  
            if(v.size() > 1)
                v = v +"&";
            }
        }
        if (v[v.size() - 1] == '&')
            v.pop_back();
        if (v.size() == 0)
        	v = v+"1";
    cout << v << endl;
}

void PRINT_VEC(vector<bitset<STATE> > &term)
{
    auto b = term.begin();
    auto e = term.end();   
    vector<string> polynomial;
    for (auto it = b; it != e; it++ )
    {
        //cout << *it << endl;
        string v ="";
        for (int i=0; i < STATE; i++)
        {
            if((*it).test(i))
            {
                //cout << i << " " <<"128 + " << i-STATE << endl;
                if (i < PUBVAR)
                	v = v + "v" + to_string(i);  
               	else
               		v = v + "k" + to_string(i-PUBVAR);  
                if(v.size() > 1)
                    v = v +"&";
            }
            
        }
        if (v[v.size() - 1] == '&')
                        v.pop_back();
        if (v.size() == 0)
        	v = v+"1";
        polynomial.push_back(v);  
    }
    cout << endl;
    int count = 0;
    for (auto it = polynomial.begin(); it != polynomial.end(); it++)
    {
        cout <<(*it)<<"+" ;
        count++;   
    }
    cout << " : " << count <<endl;
}
void PRINT_VEC_K(vector<bitset<KEYVAR> > &term)
{
    auto b = term.begin();
    auto e = term.end();   
    vector<string> polynomial;
    for (auto it = b; it != e; it++ )
    {
        //cout << *it << endl;
        string v ="";
        for (int i=0; i < KEYVAR; i++)
        {
            if((*it).test(i))
            {
                //cout << i << " " <<"128 + " << i-STATE << endl;
                v = v + to_string(i);  
                if(v.size() > 1)
                    v = v +"&";
            }
            
        }
        if (v[v.size() - 1] == '&')
                        v.pop_back();
        polynomial.push_back(v);  
    }
    cout << endl;
    for (auto it = polynomial.begin(); it != polynomial.end(); it++)
    {
        cout << (*it) <<" ^ ";   
    }
    cout << endl;
}



string getCurrentSystemTime()
{
    auto tt = chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm* ptm = localtime(&tt);
    char date[60] = { 0 };
    sprintf(date, "%d-%02d-%02d-%02d:%02d:%02d", (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
                                        (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
    return string(date);
}

struct cmp288
{
    bool operator()( const bitset<288> & a, const bitset<288> & b ) const
    {
        for ( int i = 0; i < 288; i++ )
            if ( a[i] < b[i] ) return true;
            else if ( a[i] > b[i] ) return false;
        return false; // equal
    }
};

struct cmp
{
    bool operator()( const bitset<STATE> & a, const bitset<STATE> & b ) const
    {
        for ( int i = 0; i < STATE; i++ )
            if ( a[i] < b[i] ) return true;
            else if ( a[i] > b[i] ) return false;
        return false; // equal
    }
};

struct cmp285
{
    bool operator()( const bitset<285> & a, const bitset<285> & b ) const
    {
        for ( int i = 0; i < 285; i++ )
            if ( a[i] < b[i] ) return true;
            else if ( a[i] > b[i] ) return false;
        return false; // equal
    }
};
