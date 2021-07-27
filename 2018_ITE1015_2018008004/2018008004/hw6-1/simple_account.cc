#include <iostream>
#include "simple_account.h"
using namespace std;

void account::balance(){
cout << "Balance of user "<< user<<": "<<money<<endl;
}


void master::D(int user,int mon){
if(count<=user) cout << "Account does not exist" <<endl;

else{

if(ptr[user].checkD(mon))
	cout << "Failure: Deposit to user "<< user << " "<<mon<<endl;
else{
	ptr[user].add(mon);
	cout << "Success: Deposit to user "<< user << " "<<mon<<endl;
}	
ptr[user].balance();
}
}

void master::W(int user, int mon){
if(count<=user) cout << "Account does not exist" <<endl;

else{

if(ptr[user].checkW(mon))
        cout << "Failure: Withdraw to user "<< user << " "<<mon<<endl;
else{
        ptr[user].add(-mon);
        cout << "Success: Withdraw to user "<< user << " "<<mon<<endl;
}
ptr[user].balance();
}
}

void master::T(int from,int to,int mon){
if(count<=from||count<=to) cout << "Account does not exist" <<endl;

else{

if( (!((ptr+from)->checkW(mon))) && (!((ptr+to)->checkD(mon))) ){
cout << "Success: Transfer from user " << (ptr+from)->getuser() << " to user " << (ptr+to)->getuser() <<" " << mon << endl;
(ptr+from)->add(-mon);
(ptr+to)->add(mon);
}
else
cout << "Failure: Transfer from user " << (ptr+from)->getuser() << " to user " << (ptr+to)->getuser() <<" " << mon << endl;

ptr[from>to?to:from].balance();
ptr[from>to?from:to].balance();
}
}

void master::N(){
account a;
ptr[count] = a;
ptr[count].setaccount(count);
cout<<"Account for user "<< count <<" registered"<< endl;
ptr[count].balance();
count++;
}

void master::status(){
if(count>9) cout << "full" <<endl;
else N();
}

/*if(ch=='N'){
if(count>9) cout << "full" <<endl;
else N();
return true;
}
else if(ch=='Q') return false;

else{
if(count==0) {cout<< "Account does not exist"<<endl;
return false;}
else
return true;}*/


