#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
using namespace std;

string* split(const string& str, const string& delim) {
    string* string_list = new string[10];

    int idx = 0;
    char * token = strtok(const_cast<char*>(str.c_str()), delim.c_str());
    while (token != NULL) {
        string_list[idx] = token;
        token = strtok(NULL, delim.c_str());
        ++idx;
    }

    return string_list;
}

struct Item {
 string name;
 string age;
 string id;
 string subjects[11];
 struct Item* link;
};

struct Item* create_item() {
Item *p = new Item;
p->link = NULL;
return p;
}

struct Item* find_end_of_list(struct Item* linked_list_first_item) {
Item* p = linked_list_first_item;
while(p->link != NULL) p = p->link;

return p;
}

void insert_item(struct Item *prev_item, struct Item *item) {
prev_item->link = item;
}

int main(void){
ifstream fin;
fin.open("input.txt");
string *pstr , str;

Item* head = create_item();
Item* temp = head;

while(getline(fin,str)){
	pstr = split(str,":");
	temp->name = pstr[0];
	temp->age = pstr[1];
	temp->id = pstr[2];
	pstr = split(pstr[3],",");

	for( int i = 0 ; pstr[i]!="" ; i++) temp->subjects[i] = pstr[i];

	insert_item( temp, create_item() );
	temp = temp->link; }
fin.close();
temp = head;

while(temp!=NULL){
	if((temp->id).substr(0,4) == "2013"){
		cout<<temp->name<<'('<<temp->id<<"): "<<(temp->subjects)[0];
	for(int i = 1 ; (temp->subjects)[i]!="" ; i++)
		cout<<" & " << (temp->subjects)[i];
		cout << endl;}
	temp = temp->link; }

return 0;
}
