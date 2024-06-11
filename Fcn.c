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

FILE* userFile;
FILE* classFile;
FILE* bookmarkFile;
FILE* distanceFile;
FILE* NodeFile;

char id[20], pw[20], building[20];

int v[BUILDNUM]; //확정된 노드
int d[BUILDNUM]; //거리

typedef struct Nayeol {
	int dis; //비교할 거리
	int count;
}Na;

Na N[BUILDNUM] = { {INF,0}, {INF,0}, {INF,0}, {INF,0} };

int final[BUILDNUM];
char answer[20]; //후에 문자형(혹은 문자열)으로 변경 예정
int check = -1; //아이디에 해당하는 비밀번호, 전공건물을 찾기위한 인덱스
int maxIndex = -1;

void square(int width, int height, int x, int y); //ui 그리는 함수 1
void square3(int width, int height, int x, int y); //ui 그리는 함수2
void square_char(int width, int height, int x, int y, char* msg); //ui그리는 함수 + 가운데 정렬 글씨
void goto_xy(int x, int y); //위치 이동 함수
void start_ui(void); //시작 화면 - 로그인 & 회원가입
void start_fcn(void); //시작 기능
void login_ui(void); //로그인 ui
void join_ui(void); //회원가입 ui
void main_ui(void); //선택화면
void Build_ui(void); //건물 선택 화면 1.전공건물 / 2.복수전공건물
void BuildPrint_ui(int num); //건물 목록 출력 화면 1.등록 / 2.수정(삭제 포함)
void BuildSign_ui(void); //건물 등록 화면
void BuildModify_ui(int num); //건물 수정 화면

void ClassSuggest_ui(void); //교양 추천 화면
void ClassBuild_ui(void); //교양 조회 건물 선택 화면
void ClassResearch_ui(void); //교양 조회 화면
void FieldChoice_ui(void); //분야 선택 화면

void bookMark_ui(void); //북마크 조회 화면

void UserFileOpen(void);
void classFileOpen(void);

void distanceFileOpen(void);
void NodeFileOpen(void); //노드 파일 열고 읽기

int getBuildingIndex(const char* buildName); //건물의 이름을 전달받으면 건물에 해당하는 숫자 리턴

int getSmallIndex(void);
void daikstra(int start);
void Ques(char name[]); //다익스트라 거리 계산 (함수)
void printQues(void); //다익스트라 거리 기준을 통해 출력하기 (함수)
void print_Daikstra(int num); //건물 목록 출력 후 기준을 통해 출력하기 (본체)

void login_fcn(void); //로그인 함수 (ui호출 포함)
void join_fcn(void); //회원가입 함수 (ui호출 포함)
void Build_fcn(void); //건물 선택 함수 (ui호출 포함)
void BuildPrint_fcn(int num); //건물 목록 함수 (ui호출 포함) / 다익스트라 기준 건물 출력
void buildSign_fcn(int num); //건물 등록 함수 (ui호출 포함)
void buildSign(void); //건물 등록 파일처리 함수 (건물 등록 함수에서 호출될 예정)
void BuildModify_fcn(int num); //건물 수정 함수 (ui호출 미포함)

void ClassSuggest_fcn(void); //교양 추천 함수
void ClassResearch_fcn(int num, int product); //교양 조회 함수
void ClassBuild_fcn(void); //교양 조회 건물 선택 함수
void ClassPrint(Class str[], int product); //교양 출력 함수
void FieldChoice_fcn(void); //분야 선택 함수
void getClassBuild(int num); //건물 기준으로 정렬된 교양 목록을 얻는 함수
void getClassField(char str[]); //입력 받은 교양(str)기준으로 정렬된 교양 목록을 얻는 함수

void addBookMark(Class str[], int num, int product, int productNum); //북마크 추가 함수
void deleteBookMark(int productNum); //북마크 삭제 함수
void printBookMark();
void bookMark_fcn(void);

void printBuild(void); //건물 등록 X시의 건물 출력 함수

int main(void)
{
	system("mode con: cols=80 lines=57");
	UserFileOpen();
	NodeFileOpen();
	distanceFileOpen();
	classFileOpen();

	start_fcn();

	return 0;
}

//시작 기능 (ui호출 포함)
void start_fcn(void)
{
	start_ui();
	goto_xy(15, 54);
	scanf("%s", answer);

	if (strcmp(answer, "1") == 0) {
		login_fcn();
	}
	else if (strcmp(answer, "2") == 0) {
		join_fcn();
	}
	else {
		start_fcn(); //자기 자신을 호출할지는 생각해봐야함~ ///////////////////////////////////////////////////////////////
	}
}

//로그인 함수 (ui호출 포함)
void login_fcn(void)
{
	login_ui();

	int num = 0, ch = 0;

	goto_xy(14, 18);
	scanf("%s", id);
	if (strcmp(id, "@") != 0) {
		for (int i = 0; i < 50; i++) {
			if (strcmp(user[i].u.id, id) == 0) {
				check = i;
				break;
			}
		}
		if (check != -1) {
			while (1) {
				char str[20] = { 0 };
				goto_xy(14, 31);
				num = 0;
				while ((ch = _getch()) != '\r') {
					str[num] = (char)ch;
					if (str[num] == '\b') { //BackSpace가 입력될 경우
						continue;
					}
					printf("*");
					num++;
				}

				for (int i = 0; i < 20; i++) {
					pw[i] = str[i];
				}

				if (strcmp(user[check].u.pw, pw) == 0) {
					break;
				}
				else if (strcmp(pw, "@") == 0) {
					start_fcn();
				}
				goto_xy(14, 34);
				printf("비밀번호가 일치하지 않습니다.");
				Sleep(800);
				goto_xy(14, 34);
				printf("                              "); //비밀번호가 일치하지 않습니다 지우기
				goto_xy(14, 31);
				printf("                          "); //비밀번호 지우기
				goto_xy(14, 31);
			}

			main_ui();
		}
		else {
			goto_xy(14, 22);
			printf("일치하는 아이디가 없습니다.");
			Sleep(600);
			start_fcn();
		}
	}
	else start_fcn();
}

//회원가입 함수 (ui호출 포함)
void join_fcn(void)
{
	int flag = 1;

	join_ui();
	goto_xy(14, 18);
	scanf("%s", r.id);
	if (strcmp(r.id, "@") != 0) {
		for (int i = 0; i < 50; i++) {
			if (strcmp(user[i].u.id, r.id) == 0) {
				flag = 0;
				break;
			}
		}
		if (flag == 0) {
			goto_xy(14, 21);
			printf("이미 존재하는 아이디입니다. 다른 아이디를 입력하세요.");
			join_fcn();
		}
		else {
			goto_xy(14, 31);
			scanf("%s", r.pw);
			if (strcmp(r.pw, "@") != 0) {
				userFile = fopen("user.txt", "a+");
				if (userFile == NULL) {
					printf("파일을 열 수 없습니다.\n");
				}

				fprintf(userFile, "\n%s %s %s %s", r.id, r.pw, "none", "none"); //전공건물은 로그인 후 등록하기에 입력 X

				fclose(userFile);

				UserFileOpen(); //파일에 업로드 해준 후 프로그램의 구조체에 다시 변수값을 입력받는다.
			}
		}
	}

	start_fcn();
}

//건물 선택 함수 (ui호출 포함)
void Build_fcn(void)
{
	Build_ui();

	goto_xy(15, 54);
	scanf("%s", answer);

	if (strcmp(answer, "1") == 0) {
		BuildPrint_fcn(1);
	}
	else if (strcmp(answer, "2") == 0) {
		BuildPrint_fcn(2);
	}
	else if (strcmp(answer, "0") == 0) {
		main_ui();
	}
	else {
		Build_fcn();
	}
}

//건물 목록 함수 (ui호출 포함) / 다익스트라 기준 건물 출력
void BuildPrint_fcn(int num)
{
	BuildPrint_ui(num);

	if (num == 1) {
		if (strcmp(user[check].u.building, "none") != 0) { //건물이 등록 되어 있는지 확인
			print_Daikstra(1);
		}
		else { //없을 경우 그냥 목록 출력하는 반복문
			printBuild();
		}
	}
	else if (num == 2) {
		if (strcmp(user[check].u.singularBuild, "none") != 0) { //건물이 등록 되어 있는지 확인
			print_Daikstra(2);
		}
		else { //없을 경우 그냥 목록 출력하는 반복문
			printBuild();
		}
	}

	goto_xy(15, 54);
	scanf("%s", answer);

	if (strcmp(user[check].u.building, "none") == 0) {
		if (strcmp(answer, "1") != 0) {
			BuildPrint_fcn(1);
		}
	}
	if (strcmp(answer, "1") == 0) {
		if (num == 1 && strcmp(user[check].u.building, "none") != 0) { //건물이 등록 되어 있는지 확인
			goto_xy(4, 45);
			square_char(71, 30, 4, 6, "건물이 이미 등록 되어 있습니다.");
			//건물 출력할 네모 화면에 맞춰 글씨 출력
			//printf("건물이 등록 되어 있습니다.");
			Sleep(1000);
			BuildPrint_fcn(1); //등록되어 있을 경우 다시 원점으로 복귀
			
		}
		else if (num == 2 && strcmp(user[check].u.singularBuild, "none") != 0) { //건물이 등록 되어 있는지 확인
			goto_xy(4, 45);
			square_char(71, 30, 4, 6, "건물이 이미 등록 되어 있습니다.");
			//건물 출력할 네모 화면에 맞춰 글씨 출력
			//printf("건물이 등록 되어 있습니다.");
			Sleep(1000);
			BuildPrint_fcn(2); //등록되어 있을 경우 다시 원점으로 복귀

		}
		else {
			buildSign_fcn(num); //등록이 안 됐을 경우 등록 함수로 이동
		}
	}
	else if (strcmp(answer, "2") == 0) {
		BuildModify_fcn(num); //수정 함수로 이동
	}
	else if (strcmp(answer, "0") == 0) {
		Build_fcn();
	}
	else {
		BuildPrint_fcn(num); //잘못 입력시 자기 자신으로 복귀
	}
}

//건물 등록 함수
void buildSign_fcn(int num)
{
	BuildSign_ui();

	int check_building = 0;
	printBuild();

	goto_xy(31, 45);
	scanf("%s", answer);

	if (num == 1) {
		for (int i = 0; i < 14; i++) {
			if (strcmp(Node[i].Build, answer) == 0) {
				check_building = 1;
				break;
			}
		}
		if (check_building == 1) {
			strcpy(user[check].u.building, answer);
			buildSign();
		}
		else {
			goto_xy(6, 48);
			printf("올바르지 않은 건물명입니다.");
		}
	}
	else if (num == 2) {
		for (int i = 0; i < 14; i++) {
			if (strcmp(Node[i].Build, answer) == 0) {
				check_building = 1;
				break;
			}
		}
		if (check_building == 1) {
			if (strcmp(user[check].u.building, answer) == 0) {
				goto_xy(6, 48);
				printf("전공 건물을 선택하였습니다.");
			}
			else {
				strcpy(user[check].u.singularBuild, answer);
				buildSign();
			}
		}
		else {
			goto_xy(6, 48);
			printf("올바르지 않은 건물명입니다.");
		}
	}

	Sleep(1000);
	BuildPrint_fcn(num); //추가 연산 이후 목록으로 복귀
}

//건물 등록 파일 처리 함수 (건물 등록 함수에서 호출될 예정)
void buildSign(void)
{
	FILE* signFile;

	signFile = fopen("sign.txt", "w+");
	if (signFile == NULL) {
		printf("파일을 열 수 없습니다.\n");
	}

	for(int i=0; i<=maxIndex;i++) {
		fprintf(signFile, "%s %s %s %s\n", user[i].u.id, user[i].u.pw, user[i].u.building, user[i].u.singularBuild);
		if (i >= 50) break;
	}

	fclose(signFile);

	remove("user.txt");             // 원래 파일 삭제
	rename("sign.txt", "user.txt"); // 임시 파일을 원래 파일로 이름 변경

	UserFileOpen(); //파일에 업로드 해준 후 프로그램의 구조체에 다시 변수값을 입력받는다.
}

//건물 수정 함수
void BuildModify_fcn(int num)
{
	BuildModify_ui(num);

	int check_building = 0;
	if (num == 1) {
		if (strcmp(user[check].u.building, "none") != 0) { //건물이 등록 되어 있는지 확인
			print_Daikstra(1);
		}
	}
	else if (num == 2) {
		if (strcmp(user[check].u.singularBuild, "none") != 0) { //건물이 등록 되어 있는지 확인
			print_Daikstra(2);
		}
	}

	goto_xy(33, 41);
	scanf("%s", answer);

	if (num == 2 && strcmp(answer, "@") == 0) {
		strcpy(user[check].u.singularBuild, "none");
		buildSign();
	}
	if (num == 1) {
		for (int i = 0; i < 14; i++) {
			if (strcmp(Node[i].Build, answer) == 0) {
				check_building = 1;
				break;
			}
		}
		if (check_building == 1) {
			strcpy(user[check].u.building, answer);
			buildSign();
		}
		else {
			goto_xy(6, 44);
			printf("올바르지 않은 건물명입니다.");
		}
	}
	else if (num == 2) {
		for (int i = 0; i < 14; i++) {
			if (strcmp(Node[i].Build, answer) == 0) {
				check_building = 1;
				break;
			}
		}
		if (strcmp(user[check].u.building, answer) == 0) {
			goto_xy(6, 48);
			printf("전공 건물을 선택하였습니다.");
		}
		else if (check_building == 1) {
			strcpy(user[check].u.singularBuild, answer);
			buildSign();
		}
		else {
			if (strcmp(answer, "@") == 0) {
				goto_xy(6, 48);
				printf("건물이 삭제되었습니다.");
			}
			else {
				goto_xy(6, 48);
				printf("올바르지 않은 건물명입니다.");
			}
		}
	}

	Sleep(1000);
	BuildPrint_fcn(num);
}

//건물 삭제 함수
void BuildDelete_fcn(void)
{
	FILE* signFile;

	signFile = fopen("sign.txt", "w+");
	if (signFile == NULL) {
		printf("파일을 열 수 없습니다.\n");
	}

	for (int i = 0; i <= maxIndex; i++) {
		fprintf(signFile, "%s %s %s %s\n", user[i].u.id, user[i].u.pw, user[i].u.building, user[i].u.singularBuild);
		if (i >= 50) break;
	}

	fclose(signFile);

	remove("user.txt");             // 원래 파일 삭제
	rename("sign.txt", "user.txt"); // 임시 파일을 원래 파일로 이름 변경

	UserFileOpen(); //파일에 업로드 해준 후 프로그램의 구조체에 다시 변수값을 입력받는다.
}

//교양 추천 함수
void ClassSuggest_fcn(void)
{
	ClassSuggest_ui();

	goto_xy(15, 54);
	scanf("%s", answer); //몇번 선택할 것인지 선택

	if (strcmp(answer, "1") == 0) {
		ClassBuild_fcn(); //건물 기준 조회
	}
	else if (strcmp(answer, "2") == 0) {
		FieldChoice_fcn(); //분야 기준 조회
	}
	else if (strcmp(answer, "0") == 0) {
		main_ui(); //메인화면으로 복귀
	}
	else ClassSuggest_fcn;
}

//건물 기준 조회 건물 선택 함수 ++++++++++++++++++++
void ClassBuild_fcn(void)
{
	ClassBuild_ui();
	goto_xy(15, 54);
	scanf("%s", answer); //몇번 선택할 것인지 선택

	if (strcmp(answer, "1") == 0) {
		getClassBuild(1); //전공건물 선택
		ClassResearch_fcn(1, 0);
	}
	else if (strcmp(answer, "2") == 0) {
		getClassBuild(2); //복수 전공 건물 선택
		ClassResearch_fcn(1, 0);
	}
	else ClassBuild_fcn();
}

//교양 조회 함수
void ClassResearch_fcn(int num, int product)
{
	ClassResearch_ui();

	if (num == 1) {
		square_char(41, 2, 19, 2, "건 물 기 준 교 양 목 록");
		ClassPrint(b, product);
	}
	else if (num == 2) {
		square_char(41, 2, 19, 2, "분 야 기 준 교 양 목 록");
		ClassPrint(f, product);
	}

	goto_xy(14, 54);
	scanf("%s", answer);

	int choiceNum = 0;
	choiceNum = atoi(answer) + (product * 14);

	if (strcmp(answer, "0") == 0) {
		ClassSuggest_fcn(); //메인화면으로 복귀
	}
	else if (strcmp(answer, "a") == 0) {
		if (num == 1) { //건물 기준 조회
			if (product > 0) {
				ClassResearch_fcn(1, product - 1);
			}
			else ClassResearch_fcn(1, product);
		}
		else if (num == 2) { //분야 기준 조회
			if (product > 0) {
				ClassResearch_fcn(2, product - 1);
			}
			else ClassResearch_fcn(2, product);
		}
	}
	else if (strcmp(answer, "d") == 0) {
		if (num == 1) { //건물 기준 조회
			if (product < 3) {
				ClassResearch_fcn(1, product + 1);
			}
			else ClassResearch_fcn(1, product);
		}
		else if (num == 2) { //분야 기준 조회
			ClassResearch_fcn(2, product);
		}
	}
	else if (choiceNum % 14 >= 0 && choiceNum % 14 <= 13) {
		if (choiceNum % 14 == 0) {
			choiceNum = 14;
		}
		else choiceNum = choiceNum % 14;
		if (num == 1) {
			addBookMark(b, num, product, choiceNum % 15); // 14까지 제대로 인식하기 위해
		}
		else if (num == 2) {
			addBookMark(f, num, product, choiceNum % 15);
		}
	}
	else ClassResearch_fcn(num, product);
}

//분야 조회 선택 함수
void FieldChoice_fcn(void)
{
	FieldChoice_ui();

	goto_xy(15, 54);
	scanf("%s", answer);

	if (strcmp(answer, "1") == 0) {
		getClassField("문학,예술,문화");
	}
	else if (strcmp(answer, "2") == 0) {
		getClassField("역사와철학");
	}
	else if (strcmp(answer, "3") == 0) {
		getClassField("정치,경제,사회");
	}
	else if (strcmp(answer, "4") == 0) {
		getClassField("과학과기술");
	}
	else if (strcmp(answer, "5") == 0) {
		getClassField("실용실천");
	}
	else if (strcmp(answer, "0") == 0) {
		ClassSuggest_fcn();
	}

	ClassResearch_fcn(2, 0);
}

//건물 기준으로 정렬된 교양 목록을 얻는 함수
void getClassBuild(int num)
{
	int i, j, k = 0;
	for (i = 0; i < BUILDNUM; i++) {
		daikstra(i);
	}

	if (num == 1) {
		Ques(user[check].u.building);

		for (i = 0; i < BUILDNUM; i++) { //건물 갯수만큼 반복
			for (j = 0; j < CLASSNUM; j++) { //교양 갯수만큼 반복
				if (strcmp(Node[final[i]].Build, c[j].building) == 0) {
					strcpy(b[k].building, c[j].building);
					strcpy(b[k].className, c[j].className);
					strcpy(b[k].field, c[j].field);
					k++;
				}
			}
		}
	}
	else if (num == 2) {
		if (strcmp(user[check].u.singularBuild, "none") == 0) {
			ClassBuild_fcn();
		}
		else {
			Ques(user[check].u.singularBuild);

			for (i = 0; i < BUILDNUM; i++) { //건물 갯수만큼 반복
				for (j = 0; j < CLASSNUM; j++) { //교양 갯수만큼 반복
					if (strcmp(Node[final[i]].Build, c[j].building) == 0) {
						strcpy(b[k].building, c[j].building);
						strcpy(b[k].className, c[j].className);
						strcpy(b[k].field, c[j].field);
						k++;
					}
				}
			}
		}
	}
}

//분야 기준으로 교양 목록 획득 함수
void getClassField(char str[])
{
	int i, j, k = 0;
	for (i = 0; i < BUILDNUM; i++) {
		daikstra(i);
	}

	Ques(user[check].u.building);

	for (int i = 0; i < BUILDNUM; i++) {
		strcpy(f[i].building, "");
		strcpy(f[i].className, "");
		strcpy(f[i].field, "");
	}

	for (i = 0; i < BUILDNUM; i++) { //건물 갯수만큼 반복
		for (j = 0; j < CLASSNUM; j++) { //교양 갯수만큼 반복
			if (strcmp(c[j].field, str) == 0) { //교양의 분야가 입력 받은 분야와 일치한다면
				if (strcmp(Node[final[i]].Build, c[j].building) == 0) { //정렬된 건물과 일치하는 것을 대입
					strcpy(f[k].building, c[j].building);
					strcpy(f[k].className, c[j].className);
					strcpy(f[k].field, c[j].field);
					k++;
				}
			}
		}
	}
}

//북마크 추가 함수
void addBookMark(Class str[], int num, int product, int productNum)
{
	int findNum = ((product * 14) + productNum)-1;
	int flag = 0;

	if(user[check].BMindex == -1)
		user[check].BMindex++; //첫 북마크 기능의 경우 bmIndex == 0

	if (user[check].BMindex >= 0 && user[check].BMindex < BUILDNUM -1) { //북마크 저장의 갯수 14개(0 ~ 13까지)
		for (int i = 0; i <= user[check].BMindex; i++) {
			if (strcmp(str[findNum].className, user[check].bookMark[i]->className) == 0) {
				flag = 1; //이미 교양이 북마크에 존재할 경우
				goto_xy(6, 42);
				printf("%d번 교양이 이미 북마크에 존재합니다.", findNum + 1);
				Sleep(1600);
				goto_xy(6, 42);
				printf("                                        ");
				break;
			}
		}
		if (flag == 0) {
			strcpy(user[check].bookMark[user[check].BMindex]->className, str[findNum].className);
			strcpy(user[check].bookMark[user[check].BMindex]->building, str[findNum].building);
			strcpy(user[check].bookMark[user[check].BMindex]->field, str[findNum].field);
			user[check].BMindex++;

			bookmarkFile = fopen("bookmark.txt", "a+");
			if (bookmarkFile == NULL) {
				printf("파일을 열 수 없습니다.\n");
			}
			else {
				fprintf(bookmarkFile, "%s %s\n", user[check].u.id, user[check].bookMark[user[check].BMindex - 1]->className);

				fclose(bookmarkFile);

				bookmarkFileOpen(); //파일에 업로드 해준 후 프로그램의 구조체에 다시 변수값을 입력받는다.

				goto_xy(6, 42);
				printf("%d번 교양이 북마크에 추가되었습니다.", findNum + 1);
				Sleep(1600);
				goto_xy(6, 42);
				printf("                                       ");
			}
		}
	}
	else {
		square_char(71, 30, 4, 9, "북마크가 꽉 찼습니다.");
		goto_xy(21, 25);
		printf("사용하지 않는 북마크 목록을 비워주세요.");
		Sleep(3000);
	}

	ClassResearch_fcn(num, product);
}

//북마크 삭제를 묻는 함수
int deleteQuest(int productNum)
{
	square_char(71, 30, 4, 9, "해당 교양을 북마크에서 삭제하시겠습니까? (y/n)");
	goto_xy((80 - (strlen(user[check].bookMark[productNum]->className)) - 3) / 2, 22);
	printf("[ %s ]", user[check].bookMark[productNum]->className);
	goto_xy(36, 26);
	printf(">> ");
	scanf("%s", answer);
	Sleep(1000);

	if (strcmp(answer, "y") == 0 || strcmp(answer, "Y") == 0) {
		return 1;
	}
	else return 0;
}

//북마크 삭제 함수
void deleteBookMark(int productNum)
{
	char id[20] = { 0 };
	char mark[30] = { 0 };
	
	bookmarkFile = fopen("bookmark.txt", "r");
	if (bookmarkFile == NULL) {
		printf("파일을 열 수 없습니다.\n");
	} //파일이 존재하지 않을 경우에 출력되는 출력문

	FILE* markNotFile;

	markNotFile = fopen("marknot.txt", "w+");
	if (markNotFile == NULL) {
		printf("파일을 열 수 없습니다.\n");
	}

	int i = 0;
	while(fscanf(bookmarkFile, "%s %s", m[i].id, m[i].className) != EOF) {
		if (strcmp(user[check].bookMark[productNum]->className, m[i].className) == 0 &&
			strcmp(user[check].u.id, m[i].id) == 0) {
			continue; //
		}
		else fprintf(markNotFile, "%s %s\n", m[i].id, m[i].className);
		i++;
	} //////////////// ***********************************************************************************여기 수정해야됨! 0605 문제 없는 것 같은데?

	fclose(bookmarkFile); //구조체에 따로 저장했기에 파일을 굳이 열어둘 필요가 없다.
	fclose(markNotFile);

	remove("bookmark.txt");             // 원래 파일 삭제
	rename("marknot.txt", "bookmark.txt"); // 임시 파일을 원래 파일로 이름 변경

	goto_xy(6, 42);
	printf("%s 교양이 북마크에서 삭제되었습니다.", user[check].bookMark[productNum]->className);
	Sleep(1600);
	goto_xy(6, 42);
	printf("                                                               ");

	bookmarkFileOpen(); //파일에 업로드 해준 후 프로그램의 구조체에 다시 변수값을 입력받는다.
}

void printBookMark(void)
{
	int value = 0; //최종 값
	int choice = 0; //선택한 번호 값
	for (int i = 1; i <= 14; i++) {
		value = i;
		choice = value - 1;
		if (choice >= CLASSNUM) {
			break;
		}
		//전공건물에 속한 교양일때
		if (strcmp(user[check].bookMark[choice]->building, user[check].u.building) == 0) {
			goto_xy(8, 11 + (i - 1) * 2);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkGreen);
			printf("%-4d %-28s %-18s %-12s", value, user[check].bookMark[choice]->className, user[check].bookMark[choice]->field, user[check].bookMark[choice]->building);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		}
		//복수전공건물에 속한 교양일때
		else if (strcmp(user[check].bookMark[choice]->building, user[check].u.singularBuild) == 0) {
			goto_xy(8, 11 + (i - 1) * 2);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkYellow);
			printf("%-4d %-28s %-18s %-12s", value, user[check].bookMark[choice]->className, user[check].bookMark[choice]->field, user[check].bookMark[choice]->building);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		}
		//그 외의 건물에 속한 교양일때
		else {
			goto_xy(8, 11 + (i - 1) * 2);
			printf("%-4d %-28s %-18s %-12s", value, user[check].bookMark[choice]->className, user[check].bookMark[choice]->field, user[check].bookMark[choice]->building);
		}
		//%-ns는 왼쪽 정렬된 n칸 출력하는 문자열이다.
	}
}

void bookMark_fcn(void)
{
	bookmarkFileOpen(); //북마크 파일 읽기 함수
	bookMark_ui(); //북마크 UI
	printBookMark(); //북마크 내용 출력 함수

	goto_xy(15, 54);
	scanf("%s", answer);

	int choiceNum = atoi(answer);

	if (strcmp(answer, "0") == 0) {
		main_ui();
	}
	else if (choiceNum % 15 >= 1 && choiceNum % 15 <= 14) {
		int QuestNum = deleteQuest(choiceNum % 15 - 1);
		if (QuestNum == 1) {
			deleteBookMark(choiceNum % 15 - 1);
		}
		else {
			goto_xy(6, 42);
			printf("북마크 삭제가 취소 되었습니다.");
			Sleep(1600);
			goto_xy(6, 42);
			printf("                                                               ");
		}
		bookMark_fcn();
	}

}

//교양 출력 함수
void ClassPrint(Class str[], int product) //교양 정보가 담긴 str배열 / 페이지를 설정할 product
{
	int value = 0; //최종 값
	int choice = 0; //선택한 번호 값
	for (int i = 1; i <= 14; i++) {
		value = i + (14 * product);
		choice = value - 1;
		if (choice >= CLASSNUM) {
			break;
		}
		//전공건물에 속한 교양일때
		if (strcmp(str[choice].building, user[check].u.building) == 0) {
			goto_xy(8, 11 + (i - 1) * 2);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkGreen);
			printf("%-4d %-28s %-18s %-12s", value, str[choice].className, str[choice].field, str[choice].building);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		}
		//복수전공건물에 속한 교양일때
		else if (strcmp(str[choice].building, user[check].u.singularBuild) == 0) {
			goto_xy(8, 11 + (i - 1) * 2);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkYellow);
			printf("%-4d %-28s %-18s %-12s", value, str[choice].className, str[choice].field, str[choice].building);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		}
		//그 외의 건물에 속한 교양일때
		else {
			goto_xy(8, 11 + (i - 1) * 2);
			printf("%-4d %-28s %-18s %-12s", value, str[choice].className, str[choice].field, str[choice].building);
		}
		//%-ns는 왼쪽 정렬된 n칸 출력하는 문자열이다.
	}
}