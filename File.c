#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h> //_getch() 함수를 사용하기 위해 선언

#define BUILDNUM 14 //건물 갯수
#define WEINUM 22 //거리 파일 갯수 (가중치 갯수)
#define CLASSNUM 50 //교양 파일 갯수 (교양 갯수)
#define MARKNUM 700
#define INF 10000 //무한대

typedef struct user
{
	char id[20]; //회원 아이디
	char pw[20]; //회원 비밀번호
	char building[20]; //회원 전공 건물
	char singularBuild[20]; //회원 복수 전공 건물
	//char bookMark[20][20]; //회원별 저장한 북마크
} User;
//User u[50]; //파일에서 저장할 회원 구조체
User r; //회원가입에 사용될 구조체

typedef struct Class
{
	char className[30]; //교양 이름
	char field[20]; //교양 분야
	char building[20]; //교양 건물명
} Class;
Class c[CLASSNUM]; //파일에서 저장할 교양 구조체

Class b[CLASSNUM]; //build의 축약 단어 / 건물 기준 조회에서 사용될 구조체 변수
Class f[BUILDNUM]; //field의 축약 단어 / 분야 기준 조회에서 사용될 구조체 변수

typedef struct book
{
	User u;
	Class bookMark[14][30]; //북마크는 인당 14개만 저장가능 (교양 이름이 저장될 예정)
	int BMindex;
} bookMark;
bookMark user[50];

typedef struct mark
{
	char id[20];
	char className[20];
} Mark;
Mark m[MARKNUM];

typedef struct No
{
	int num;  //건물 노드 번호
	char Build[20]; //건물명
} node;
node Node[BUILDNUM];

typedef struct distan
{
	int firstNum; //시작 건물 번호
	int finalNum; //도착 건물 번호
	int weight; //가중치
} Distan;
Distan dista[WEINUM]; //파일에서 저장할 거리(노드와 가중치) 구조체

int buildArray[BUILDNUM][BUILDNUM]; //다익스트라를 돌리기 위한 2차원 배열

FILE* userFile;
FILE* classFile;
FILE* bookmarkFile;
FILE* distanceFile;
FILE* NodeFile;

int check = -1; //아이디에 해당하는 비밀번호, 전공건물을 찾기위한 인덱스
int maxIndex = -1;

//회원 파일 열고 읽기
void UserFileOpen(void)
{
	userFile = fopen("user.txt", "r");
	if (userFile == NULL) {
		printf("파일을 열 수 없습니다.\n");
	} //파일이 존재하지 않을 경우에 출력되는 출력문

	int i = 0;
	while (fscanf(userFile, "%s %s %s %s", user[i].u.id, user[i].u.pw, user[i].u.building, user[i].u.singularBuild) != EOF) {
		maxIndex = i;
		i++;
		if (i >= 50) break;
	} //scnaf는 띄어쓰기는 읽을 수 없기에 띄어쓰기로 구분하여 파일에 있는 값을 변수에 저장 //파일에서 읽을 것이 없을 때까지 or 50개 이상일 경우 while문을 빠져나온다.
	
	fclose(userFile); //구조체에 따로 저장했기에 파일을 굳이 열어둘 필요가 없다.
}

//북마크 파일 열고 읽기
void bookmarkFileOpen(void)
{
	char id[20] = { 0 };
	char mark[30] = { 0 };
	
	bookmarkFile = fopen("bookmark.txt", "r");
	if (bookmarkFile == NULL) {
		printf("파일을 열 수 없습니다.\n");
	} //파일이 존재하지 않을 경우에 출력되는 출력문

	for (int i = 0; i < BUILDNUM; i++) {
		strcpy(user[check].bookMark[i]->className, "");
		strcpy(user[check].bookMark[i]->building, "");
		strcpy(user[check].bookMark[i]->field, "");
	}

	int bmi = 0;
	while (fscanf(bookmarkFile, "%s %s", id, mark) != EOF) {
		if (strcmp(user[check].u.id, id) == 0) {
			user[check].BMindex = bmi;
			strcpy(user[check].bookMark[user[check].BMindex]->className, mark);
			for (int k = 0; k < CLASSNUM; k++) {
				if (strcmp(user[check].bookMark[user[check].BMindex]->className, c[k].className) == 0) {
					strcpy(user[check].bookMark[user[check].BMindex]->building, c[k].building);
					strcpy(user[check].bookMark[user[check].BMindex]->field, c[k].field);
				}
			}
			bmi++;
		}
	} //파일에서 읽을 것이 없을 때까지 //scnaf는 띄어쓰기는 읽을 수 없기에 띄어쓰기로 구분하여 파일에 있는 값을 변수에 저장

	fclose(bookmarkFile); //구조체에 따로 저장했기에 파일을 굳이 열어둘 필요가 없다.
}


//교양 파일 열고 읽기
void classFileOpen(void)
{
	classFile = fopen("class.txt", "rt");
	if (classFile == NULL) {
		printf("파일을 열 수 없습니다.\n");
	} //파일 안에 값이 존재하지 않거나 파일 자체가 존재하지 않을 경우 열리지 않는다.

	int i = 0;
	while (fscanf(classFile, "%s  %s  %s", &c[i].className, &c[i].field, &c[i].building) != EOF) {
		i++;
		if (i >= CLASSNUM) break;
	} //scnaf는 띄어쓰기는 읽을 수 없기에 띄어쓰기로 구분하여 파일에 있는 값을 변수에 저장

	fclose(classFile); //구조체에 따로 저장했기에 파일을 굳이 열어둘 필요가 없다.
}

//거리 파일 열고 읽기
void distanceFileOpen(void)
{
	int i, j;
	distanceFile = fopen("distance.txt", "r");
	if (distanceFile == NULL) {
		printf("파일을 열 수 없습니다.\n");
	} //파일 안에 값이 존재하지 않거나 파일 자체가 존재하지 않을 경우 열리지 않는다.

	for (i = 0; i < WEINUM; i++) {
		fscanf(distanceFile, "%d %d %d", &dista[i].firstNum, &dista[i].finalNum, &dista[i].weight);
	} //scnaf는 띄어쓰기는 읽을 수 없기에 띄어쓰기로 구분하여 파일에 있는 값을 변수에 저장

	fclose(distanceFile); //구조체와 배열에에 따로 저장했기에 파일을 굳이 열어둘 필요가 없다.

	for (i = 0; i < BUILDNUM; i++) {
		for (j = 0; j < BUILDNUM; j++) {
			buildArray[i][j] = INF;
		}
	} //다익스트라 2차원 배열을 INF(무한대)로 초기화

	for (int i = 0; i < WEINUM; i++) {
		if (dista[i].firstNum != -1 && dista[i].finalNum != -1) { //두 인덱스 값이 정상적으로 있을 때
			buildArray[dista[i].firstNum][dista[i].finalNum] = dista[i].weight;
			buildArray[dista[i].finalNum][dista[i].firstNum] = dista[i].weight;
		}
	}

	for (i = 0; i < BUILDNUM; i++) {
		buildArray[i][i] = 0;
	} //자기 자신과의 이동 가중치는 0으로 설정
}

//노드 파일 열고 읽기
void NodeFileOpen(void)
{
	int i, j;
	NodeFile = fopen("node.txt", "r");
	if (NodeFile == NULL) {
		printf("파일을 열 수 없습니다.\n");
	} //파일 안에 값이 존재하지 않거나 파일 자체가 존재하지 않을 경우 열리지 않는다.

	for (i = 0; i < BUILDNUM; i++) {
		fscanf(NodeFile, "%d %s", &Node[i].num, Node[i].Build);
	} //scnaf는 띄어쓰기는 읽을 수 없기에 띄어쓰기로 구분하여 파일에 있는 값을 변수에 저장

	fclose(NodeFile); //구조체와 배열에에 따로 저장했기에 파일을 굳이 열어둘 필요가 없다.
}