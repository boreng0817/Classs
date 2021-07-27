#include <iostream>
#include <sstream>
#include <string>

#define MAX(X,Y) (X) > (Y) ? (X) : (Y)
#define PRINT -5
using namespace std;

string LIS(string str);

int main() {

    string str;

    while(getline(cin, str) && str != "") {
        
        istringstream iss(str);
        iss >> str;
        cout << LIS(str) << endl;
    }

}

string LIS(string str) {

    string ret = "";
    int *dp = new int[str.size()];
    int i, j, max;

    for (i = 0; i < str.size(); ++i)
        dp[i] = 1;

    for (i = 1; i < str.size(); ++i) 
        for (j = 0; j < i; ++j)
            if (str[i] > str[j])
                dp[i] = MAX(dp[i], dp[j] + 1);
    
    for (i = 0, max = -1; i < str.size(); ++i)
        max = MAX(max, dp[i]);

    for (i = str.size() - 1; i >= 0; --i)
        if (dp[i] == max) {
            dp[i] = PRINT;
            max--;
        }

    for (i = 0; i < str.size(); ++i) 
        if (dp[i] == PRINT)
            ret.append(str.substr(i,1));

    delete(dp);
    return ret;
}

