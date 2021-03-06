#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include "omp.h"

using namespace std;

struct tri_matrix
{
    int num_strings;
    vector <vector <double> > content;
    double a(int i)
    {
        return content[i][0];
    }
    double b(int i)
    {
        return content[i][2];
    }
    double c(int i)
    {
        return content[i][1];
    }

};

void fill_mat(tri_matrix * a);
void print_matrix(tri_matrix * a);
void print_vector_d(vector <double> & a);
void fill_vector(vector<double> &a, int size);
void init_matrix(tri_matrix * a, int size);

void reduction(tri_matrix *a, vector<double> & f, int where_to);
void reduct_matrix(tri_matrix *a, vector<double> &f, tri_matrix* b, vector<double> &g);
vector<double> answer;
vector<bool> answer_oc;
int main() {
    cout << "Hello, World!" << endl;
    time_t time_b;
    tri_matrix *matrix = new tri_matrix;
    vector<double> f;
    fill_mat(matrix);
    //print_matrix(matrix);
    fill_vector(f, matrix->num_strings);
    answer = vector<double>(matrix->num_strings, 0.0);
    answer_oc = vector<bool>(matrix->num_strings, false);
    time(&time_b);
    reduction(matrix, f, 0);
    time_t time_e;
    time(&time_e);
    cout << "\nTime Elapsed:" << difftime(time_b, time_e) << "\n";
    //print_vector_d(answer);
    delete matrix;
    return 0;
}

void fill_mat(tri_matrix * a)
{
    cout << "Enter number of strings:";
    cin >> a->num_strings;
    cout << "Enter matrix:" << endl;
    for(int i = 0; i<a->num_strings; i++)
    {
        vector<double> tmp(3, 0.0);
        if(i == 0) {
           // cin >> tmp[1] >> tmp[2];
            tmp[1] = rand();
            tmp[2] = rand();
            tmp[0] = 0.0;
        }
        else if(i == a->num_strings-1) {
           // cin >> tmp[0] >> tmp[1];
            tmp[0] = rand();
            tmp[1] = rand();
            tmp[2] = 0.0;
        }
        else
        {
            tmp[0] = rand();
            tmp[1] = rand();
            tmp[2] = rand();
            //cin >> tmp[0] >> tmp[1] >> tmp[2];
        }
        a->content.push_back(tmp);
    }
}

void print_matrix(tri_matrix * a)
{

    cout << "Matrix is:" << endl;
    string identation = "";
    for(int i = 0; i<a->num_strings; i++)
    {
        cout << identation;
        print_vector_d(a->content[i]);
        identation += "  ";
    }
}

void print_vector_d(vector <double> & a)
{
    int size = a.size();
    for(int i = 0; i < size; i++)
        cout << a[i] << " ";
    cout << endl;
}

void fill_vector(vector<double> &a, int size)
{
    double tmp;
    cout << "Enter RHS vector:";
    for(int i = 0; i < size; i++) {
        //cin >> tmp;
        a.push_back(rand());
    }
}

void reduction(tri_matrix *a, vector<double> &f, int where_to)
{
    tri_matrix *b = new tri_matrix;
    vector<double> g(a->num_strings/2);
    reduct_matrix(a, f, b, g);
    if (b->num_strings != 1) {
        reduction(b, g, where_to + 1);
    }
    else
    {
        int j = -1 + (1 << (where_to + 1));
        answer[j] = g[0]/b->c(0);
        answer_oc[j] = true;
    }
 //   #pragma omp parallel for 
    for(int i = 0; i < a->num_strings; i++)
    {
        int step = (1<<where_to);
        int j = -1+(i+1)*step;
        if(i == 0) {
            answer[j] = (f[i] - a->b(i) * answer[j + step]) / a->c(i);
            answer_oc[j] = true;
        }
        else if(i == a->num_strings - 1)
        {
            answer[j] = (f[i] - a->a(i) * answer[j - step]) / a->c(i);
            answer_oc[j] = true;
        }
        else if(!answer_oc[j])
        {
            answer[j] = (f[i] - a->a(i) * answer[j-step] - a->b(i) * answer[j+step]) / a->c(i);
            answer_oc[j] = true;
        }
    }
    delete b;
}
void reduct_matrix(tri_matrix *a, vector<double> &f, tri_matrix* b, vector<double> &g)
{
    init_matrix(b, a->num_strings/2);

    #pragma omp parallel for
    for(int i = 1; i < a->num_strings - 1; i+=2)
    {
        double alf = -(a->a(i))/(a->c(i-1));
        double beta = -(a->b(i))/(a->c(i+1));
        double aa = alf*a->a(i-1);
    	double bb = alf*a->b(i-1) + a->c(i) + beta*a->a(i+1);
        double cc = beta*a->b(i+1); 
        double dd = alf*f[i-1]+f[i]+beta*f[i+1];  
	    b->content[(i-1)/2].push_back(aa);
        b->content[(i-1)/2].push_back(bb);
        b->content[(i-1)/2].push_back(cc);
        g[(i-1)/2] = dd;
    }
 
}

void init_matrix(tri_matrix * a, int size)
{
    a->num_strings = size;
    for(int i = 0; i < size; i++)
    {
        a->content.push_back(vector<double>());
    }
}
