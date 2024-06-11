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

enum ColorType {
	BLACK,  	//0
	darkBLUE,	//1
	DarkGreen,	//2
	darkSkyBlue,    //3
	DarkRed,  	//4
	DarkPurple,	//5
	DarkYellow,	//6
	GRAY,		//7
	DarkGray,	//8
	BLUE,		//9
	GREEN,		//10
	SkyBlue,	//11
	RED,		//12
	PURPLE,		//13
	YELLOW,		//14
	WHITE		//15
} COLOR;

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

char id[20], pw[20], building[20];

int v[BUILDNUM]; //확정된 노드
int d[BUILDNUM]; //거리

typedef struct Nayeol {
	int dis; //비교할 거리
	int count;
}Na;

Na N[BUILDNUM] = { {INF,0}, {INF,0}, {INF,0}, {INF,0} };

int final[BUILDNUM];
int check = -1; //아이디에 해당하는 비밀번호, 전공건물을 찾기위한 인덱스
//다익스트라가 사용되지 않은 기본 건물 목록 출력
void printBuild(void)
{
	for (int j = 0; j < BUILDNUM; j++) {
		if (strcmp(Node[j].Build, user[check].u.building) == 0) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkGreen); //전공건물일 때 초록색으로 표시
			goto_xy(7, 6 + (j + 1) * 2);
			printf(" ▶ %s", Node[j].Build);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		}
		else {
			goto_xy(7, 6 + (j + 1) * 2);
			printf(" ▶ %s", Node[j].Build);
		}
	}
}

//가장 작은 가중치 판별 함수
int getSmallIndex(void) {
	int min = INF; //가중치를 비교할 변수 선언, 가장 작은 값을 구할 것이기에 무한대로 초기화 
	int index = 0; //인덱스는 0부터 시작
	for (int i = 0; i < BUILDNUM; i++) { //i가 0부터 빌딜 갯수(14)까지 반복
		if (d[i] < min && v[i] != 1) { //거리가 min보다 작고, 확정된 노드가 아닐때
			min = d[i]; //min에 i번째 거리 값을 집어넣고
			index = i; //index 값에 i 값을 집어넣는다
		}
	}

	return index; //index를 반환해준다.
}

//다익스트라 거리 계산
void daikstra(int start) {
	for (int i = 0; i < BUILDNUM; i++) {
		d[i] = buildArray[start][i]; //다익스트라 2차원 배열의 원소를 거리 값으로 채워준다.
		v[i] = 0; //확정된 노드는 없다.
	}
	v[start] = 1; //v[0] = 1로 방문하였다는 표시를 해준다.
	for (int i = 0; i < BUILDNUM - 2; i++) { //이중 for문을 사용할 예정이기에 최종 건물 갯수에서 -2를 해준다. (총 12번 반복)
		int index = getSmallIndex(); //가장 작은 인덱스를 반환해주는 함수를 호출하고 인덱스를 변수에 저장한다.
		v[index] = 1; //v[인덱스]는 방문한 노드이다.
		for (int j = 0; j < BUILDNUM; j++) { //j는 0부터 최종 건물 갯수까지 반복한다(0 ~ 13까지 반복)
			if (v[j] != 1) { //v[j]가 방문한 노드가 아니라면
				if (d[index] + buildArray[index][j] < d[j]) { //거리[인덱스] + 2차원 배열[인덱스][j] < 거리[j]에 해당한다면
					d[j] = d[index] + buildArray[index][j];
					buildArray[start][j] = d[index] + buildArray[index][j];
				}
			}
		}
	}
}

//다익스트라 거리 계산 후 기준을 통해 출력하기 (함수)
void Ques(char name[]) { 
	int i, j;
	int index;
	int stmp;

	for (i = 0; i < BUILDNUM; i++) {
		if (strcmp(Node[i].Build, name) == 0) { //건물 이름 중 (전공건물/복수전공건물)이 있다면 
			index = i;                     //인덱스에 i를 집어넣는다.
		}
	}

	for (i = 0; i < BUILDNUM; i++) {       //건물의 갯수(14)만큼 반복
		N[i].dis = buildArray[index][i];   //i번째 거리는 2차원 배열의[index][i]번째에 있는 정수 값이다
		N[i].count = i;                    //현재 순서는 배열에서 저장된 i 순서를 따른다
	}

	for (j = 0; j < BUILDNUM; j++) {                //건물의 갯수(14)만큼 반복
		for (int k = j + 1; k < BUILDNUM; k++) {    //j+1부터 건물의 갯수만큼 반복 (최대 13번, j가 커질수록 반복하는 횟수가 줄어든다
			if (N[k].dis < N[j].dis) {              //나열할 거리 두개를 비교하여 인덱스 k 거리보다 인덱스 j 거리가 더 짧다면
				stmp = N[j].dis;                    //k의 거리와 j의 거리를 바꾸어주고
				N[j].dis = N[k].dis;
				N[k].dis = stmp;

				stmp = N[j].count;                  //count 또한 서로 바꾸어준다.
				N[j].count = N[k].count;
				N[k].count = stmp;
			}                                       //최종적으로 거리가 짧은 순으로 count가 정렬된다
		}
		final[j] = N[j].count;                      //count를 final(최종 정렬본) 변수에 저장한다
	}
}
//원래 아래랑 합쳐있는 거였는데 출력이 따로 필요해서 분리함!

//다익스트라 거리 기준을 통해 출력하기
void printQues(void)
{
	for (int j = 0; j < BUILDNUM; j++) {
		if (strcmp(Node[final[j]].Build, user[check].u.building) == 0) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkGreen);
			goto_xy(7, 6 + (j + 1) * 2);
			printf("%2d. %s", j + 1, Node[final[j]].Build);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		}
		else if (strcmp(Node[final[j]].Build, user[check].u.singularBuild) == 0) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkYellow);
			goto_xy(7, 6 + (j + 1) * 2);
			printf("%2d. %s", j + 1, Node[final[j]].Build);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		}
		else {
			goto_xy(7, 6 + (j + 1) * 2);
			printf("%2d. %s", j + 1, Node[final[j]].Build);
		}
	}
}

//건물 목록 출력 후 기준을 통해 출력하기 (본체)
void print_Daikstra(int num)
{
	int i;

	for (i = 0; i < BUILDNUM; i++) {
		daikstra(i);                    //0부터 13까지의 2차원 다익스트라 배열을 정리해준다
	}

	if (num == 1) {
		Ques(user[check].u.building);  //배열을 전공 건물에 가까운 순으로 정렬해주고
		printQues();                   //정렬된 값을 통해 출력해준다
	}
	else if (num == 2) {
		Ques(user[check].u.singularBuild);  //배열을 복수 전공 건물에 가까운 순으로 정렬해주고
		printQues();                        //정렬된 값을 통해 출력해준다   
	}
}