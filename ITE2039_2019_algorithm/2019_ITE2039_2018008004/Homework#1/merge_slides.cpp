#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <climits>
#define INF LONG_MAX
using namespace std;

typedef long long int lli;

void merge_slide(string str);
lli sum_array(lli *arr, int begin, int end);
void Extra_credit(lli *arr, vector<vector<lli> > &s, int i, int j);

int main() {
    
    string str;

    while (getline(cin,str) && str != "")
        merge_slide(str);

    return 0;
}

void merge_slide(string str) {

    int n = 1, l, i, j, k;
    lli *arr, val, sum;
    istringstream iss(str);

    for (i = 0; i < str.size(); ++i)
        n += str[i] == ' ';
    
    arr = new lli[n + 1];
    for (i = 1; i <= n; ++i)
        iss >> arr[i];
    
    vector<lli> temp(n + 1, 0);
    vector<vector<lli> > dp(n + 1, temp);
    vector<vector<lli> > s(n + 1, temp);

    if (n == 1) {
        cout << dp[1][n] << endl;
        return;
    }
 
    for (l = 2; l <= n; ++l) {
        for (i = 1; i <= n - l + 1; ++i) {
            j = i + l - 1;
            dp[i][j] = INF;
            sum = sum_array(arr, i, j);
            for (k = i; k <= j - 1; ++k) {
                val = dp[i][k] + dp[k + 1][j] + sum;
                if (val < dp[i][j]) {
                    dp[i][j] = val;
                    s[i][j] = k;
                }
            }
        }
    }
            
    cout << dp[1][n] << ' ';
    Extra_credit(arr, s, 1, n);
    cout << endl;
    delete(arr);
}

lli sum_array(lli *arr, int begin, int end) {

    int i;
    lli sum = 0;
    for (i = begin; i <= end; ++i) 
        sum += arr[i];
    return sum;
}

void Extra_credit(lli *arr, vector<vector<lli> > &s, int i, int j) {
    if (i == j)
        cout << arr[i];
    else {
        cout << '(';
        Extra_credit(arr, s, i, s[i][j]);
        cout << ',';
        Extra_credit(arr, s, s[i][j] + 1, j);
        cout << ')';
    }
}
        
