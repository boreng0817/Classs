#include <iostream>
#include <string>
#define NUM_OF_CHAT 10
using namespace std;

class ReplyAdmin{
private:
	string* chats;
	int count;
public:
	ReplyAdmin() {chats = new string[NUM_OF_CHAT],count = 0;}
	~ReplyAdmin() {delete[] chats, chats = NULL;}

	int getChatCount(){return count;}
	bool addChat(string chat);
	bool removeChat(int index);
	bool removeChat(int* indices,int len);
	bool removeChat(int start,int end);
	void print();
};
