class account{
private:
	int user;
	int money;
public:
	account(){}
	void setaccount(int n1){user = n1, money=0;}
	bool checkD(int num) {return num+money > 1000000;}
	bool checkW(int num) {return money-num < 0;}
	void balance();
	void add(int mon){money += mon;}
	int getuser(){return user;}
	int getmoney(){return money;}
};

class master{
private:
int count;
account* ptr;
public:
	master(){ptr = new account[10], count = 0;}
	void N();
	void D(int,int);
	void W(int,int);
	void T(int,int,int);
	void status();
};
