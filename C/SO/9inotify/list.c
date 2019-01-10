#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N		1000
#define N_STEP	10

char filenameTmp[256];
int opTmp;

 
typedef struct list list_str;
struct list {
	char filename[256];
	int op;
	list_str *next;
};



list_str *createList(void){
	return NULL;
}

list_str *insertList(char f[256], int o, list_str *n){
	list_str *newNode = (list_str*)malloc(sizeof(list_str));
	strcpy(newNode->filename, f);
	newNode->op = o;
	newNode->next = n;
	//
	return newNode;
}


list_str *delListBegin(list_str *n){
	list_str *newNode = (list_str*)malloc(sizeof(list_str));
	newNode->next = n;
	//
	return newNode;
}

list_str *delListEnd(list_str *n){
	if(n->next == NULL){
		printf("List is empty\n");
		return NULL;
	}
	else{
		list_str *last = n->next;
		list_str *penultimate = n;
		while(last->next != NULL){
			penultimate = last;
			last = last->next;
		}

		penultimate->next = NULL;
		return last;
	}
}

void showList(list_str *list1) {
	printf("[list]");
	
	while (list1 != NULL) {
		printf(" -> (%s:%d)", list1->filename, list1->op);
		list1 = list1->next;
	}
	printf("\n");	
}


int main(void) {
	list_str *list1;
	list1 = createList();
	

	while (opTmp < 10) {
		printf("[filename]");
		scanf("%s", &filenameTmp);
		printf("[operation]");
		scanf("%d", &opTmp);
		
		list1 = insertList(filenameTmp, opTmp, list1);
	}

	showList(list1);
	delListEnd(list1);
	showList(list1);
}








