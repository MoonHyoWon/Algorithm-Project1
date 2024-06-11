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

char answer[20]; //후에 문자형(혹은 문자열)으로 변경 예정
int check = -1; //아이디에 해당하는 비밀번호, 전공건물을 찾기위한 인덱스

// UI 그리기 함수들 

//시작 화면 - 로그인 & 회원가입
void start_ui(void)
{
	Sleep(600);
	system("cls");
    square(79, 52, 0, 0);
    square(79, 2, 0, 53);
	
    goto_xy(14, 4); // SWmap 로고 출력 print문
    printf("%2s■■%1s\n", " ", " ");
    goto_xy(14, 5);
    printf("■%4s■%1s■%2s■%2s■\t%2s■%2s■%2s%3s■■%2s%1s■■■%2s\n", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ");
    goto_xy(14, 6);
    printf("■%7s■%2s■%2s■\t■%2s■%2s■%1s■%4s■%1s■%4s■\n", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ");
    goto_xy(14, 7);
    printf("%2s■■%3s■%2s■%2s■\t■%2s■%2s■%1s■%4s■%1s■%4s■\n", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ");
    goto_xy(14, 8);
    printf("%6s■%1s■%2s■%2s■\t■%2s■%2s■%1s■■■■%1s■■■%2s\n", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ");
    goto_xy(14, 9);
    printf("■%4s■%1s■%2s■%2s■\t■%2s■%2s■%1s■%4s■%1s■%8s\n", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ");
    goto_xy(14, 10);
    printf("%2s■■%4s%1s■%2s■%2s\t■%2s■%2s■%1s■%4s■%1s■%8s\n", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ");

    goto_xy(31, 14);
    printf("|  교 양  추 천  |");

    square_char(40, 6, 20, 20, "1. 로 그 인");
    square_char(40, 6, 20, 35, "2. 회 원 가 입");
    goto_xy(2, 54);
    printf("번호 입력 >> ");
}

//로그인 ui
void login_ui(void)
{
	Sleep(600);
	system("cls");
    square(79, 52, 0, 0);
    goto_xy(2, 54);
    square_char(41, 2, 19, 2, "로 그 인");

    goto_xy(36, 14);
    printf("아 이 디");
    square(59, 4, 10, 16);

    goto_xy(35, 27);
    printf("비 밀 번 호");
    square(59, 4, 10, 29);

    square_char(41, 4, 19, 44, "@ 입력 시 시작화면으로 돌아갑니다.");
}

//회원가입 ui
void join_ui(void)
{
	Sleep(600);
	system("cls");
    square(79, 52, 0, 0);
    goto_xy(2, 54);
    square_char(41, 2, 19, 2, "회 원 가 입");

    goto_xy(36, 14);
    printf("아 이 디");
    square(59, 4, 10, 16);

    goto_xy(35, 27);
    printf("비 밀 번 호");
    square(59, 4, 10, 29);
	
    square_char(41, 4, 19, 44, "@ 입력 시 시작화면으로 돌아갑니다.");
}

//회원 정보 출력 박스
void user_print(void)
{
	square(59, 8, 10, 8);
	goto_xy(12, 10);
	printf(" %7s%-15s : %s", " ", "ID", user[check].u.id);
	goto_xy(12, 12);
	if (strcmp(user[check].u.building, "none") == 0) {
		printf(" %7s%s 이 등록되지 않았습니다.", " ", "전공 건물");
	}
	else {
		printf(" %7s%-15s : %s", " ", "전공 건물", user[check].u.building);
	}
	goto_xy(12, 14);
	if (strcmp(user[check].u.singularBuild, "none") == 0) {
		printf(" %7s%-15s이 등록되지 않았습니다.", " ", "복수 전공 건물");
	}
	else {
		printf(" %7s%-15s : %s", " ", "복수 전공 건물", user[check].u.singularBuild);
	}
}

//선택화면 1. 전공건물관리 / 2.교양목록 / 3. 북마크  *************** 이동경로 표시 X
void main_ui(void)
{
	if (strcmp(user[check].u.building, "none") == 0) { //전공 건물이 등록 되어 있는지 않는다면 바로 전공 등록 화면으로 이동
		BuildPrint_fcn(1);
	}

	Sleep(600);
	system("cls");
	square(79, 52, 0, 0);

	square_char(41, 2, 19, 2, "메 인 화 면");

	goto_xy(65, 1);
	printf("0 = 뒤로가기");

	square(79, 2, 0, 53);

	user_print();

	square_char(39, 6, 20, 21, "1. 나 의 전 공 건 물");
	square_char(39, 6, 20, 31, "2. 교 양 목 록");
	square_char(39, 6, 20, 41, "3. 북 마 크");

	goto_xy(2, 54);
	printf("번호 입력 >> ");
    goto_xy(15, 54);
    scanf("%s", answer);

    if (strcmp(answer, "1") == 0) {
        Build_fcn();
    }
	else if (strcmp(answer, "2") == 0) {
		if (strcmp(user[check].u.building, "none") == 0) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED);
			square(59, 8, 10, 8);
			goto_xy(25, 11);
			printf("전공 건물이 등록되지 않았습니다");
			goto_xy(27, 13);
			printf("건물을 먼저 등록하여주세요");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);

			Sleep(600);

			main_ui();
		}
		ClassSuggest_fcn();
	}
	else if (strcmp(answer, "3") == 0) {
		bookMark_fcn();
	}
    else if (strcmp(answer, "0") == 0) {
		start_fcn();
    }
	else {
		main_ui();
	}
}

//건물 선택 화면 1.전공건물 / 2.복수전공건물  ++이동경로 표시 완료
void Build_ui(void)
{
	Sleep(600);
	system("cls");
    square(79, 52, 0, 0);
    square(79, 2, 0, 53);

	goto_xy(65, 1);
	printf("0 = 뒤로가기");

    square_char(41, 2, 19, 2, "나 의 건 물 관 리");

	user_print();

    
	goto_xy(27, 50);
	printf("메인 화면 -> 나의 건물 관리");

    square_char(40, 6, 20, 20, "1. 나 의 전 공 건 물");
    square_char(40, 6, 20, 35, "2. 나 의 복 수 전 공 건 물");

	goto_xy(2, 54);
	printf("번호 입력 >> ");
}

//건물 목록 출력 화면 1.등록 / 2.수정(삭제 포함)     *************** 이동경로 표시 X
void BuildPrint_ui(int num)
{
	Sleep(600);
	system("cls");
	square(79, 52, 0, 0);
	square(79, 2, 0, 53);
	square_char(41, 2, 19, 2, "전 공 관 리");

	goto_xy(65, 1);
	printf("0 = 뒤로가기");

	square(71, 30, 4, 6); //건물 출력할 네모 화면 출력
	goto_xy(6, 37);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkGreen);
	printf("* 전공건물");
	goto_xy(18, 37);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkYellow);
	printf("* 복수전공건물");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);

	if (num == 1 && strcmp(user[check].u.building, "none") != 0) { //건물이 등록되어 있지 않으면 등록 불가(빨간색 변경)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED);
		square_char(35, 6, 4, 41, "1. 건 물 등 록");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);

	}
	else if (num == 2 && strcmp(user[check].u.singularBuild, "none") != 0) { //건물이 등록되어 있지 않으면 등록 불가(빨간색 변경)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED);
		square_char(35, 6, 4, 41, "1. 건 물 등 록");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
	}
	else square_char(35, 6, 4, 41, "1. 건 물 등 록");
	square_char(35, 6, 40, 41, "2. 건 물 수 정");

	goto_xy(2, 54);
	printf("번호 입력 >> ");
}

//건물 등록 화면    *************** 이동경로 표시 X
void BuildSign_ui(void)
{
	Sleep(600);
	system("cls");
	square(79, 52, 0, 0);
	square_char(41, 2, 19, 2, "건 물 등 록");

	goto_xy(65, 1);
	printf("0 = 뒤로가기");

	square(71, 30, 4, 6); //건물 출력할 네모 화면 출력
	goto_xy(6, 37);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkGreen);
	printf("* 전공건물");
	goto_xy(18, 37);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkYellow);
	printf("* 복수전공건물");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);

	square(71, 2, 4, 44);
	goto_xy(6, 45);
	printf("등록할 건물 이름 입력 >> ");
}

//건물 수정 화면     *************** 이동경로 표시 X
void BuildModify_ui(int num)
{
	Sleep(600);
	system("cls");
	square(79, 52, 0, 0);

	goto_xy(65, 1);
	printf("0 = 뒤로가기");

	if (num == 1) {
		square_char(41, 2, 19, 2, "전 공 건 물 수 정");
	}
	else if (num == 2) {
		square_char(41, 2, 19, 2, "복 수 전 공 건 물 수 정");
	}

	square(71, 30, 4, 6); //건물 출력할 네모 화면 출력
	goto_xy(6, 37);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkGreen);
	printf("* 전공건물");
	goto_xy(18, 37);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkYellow);
	printf("* 복수전공건물");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);

	square(71, 2, 4, 40);
	goto_xy(6, 41);
	printf("재등록할 건물 이름 입력 >> ");

	if (num == 2) {
		square_char(71, 2, 4, 45, "@ 입력 시 복수 전공 건물 삭제");
	}
}

//교양 추천 화면 ++이동경로 표시 완료
void ClassSuggest_ui(void)
{
	Sleep(600);
	system("cls");
	square(79, 52, 0, 0);
	square(79, 2, 0, 53);

	goto_xy(65, 1);
	printf("0 = 뒤로가기");

	square_char(41, 2, 19, 2, "교 양 추 천");

	user_print();

	goto_xy(29, 50);
	printf("메인 화면 -> 교양 추천");

	square_char(40, 6, 20, 20, "1. 건 물 기 준 추 천");
	square_char(40, 6, 20, 35, "2. 분 야 기 준 추 천");

	goto_xy(2, 54);
	printf("번호 입력 >> ");
}

//교양 조회 건물 선택 화면 ++이동경로 표시 완료
void ClassBuild_ui(void)
{
	Sleep(600);
	system("cls");
	square(79, 52, 0, 0);
	square(79, 2, 0, 53);

	goto_xy(65, 1);
	printf("0 = 뒤로가기");

	square_char(41, 2, 19, 2, "건 물 관 리");

	user_print();

	goto_xy(20, 50);
	printf("메인 화면 -> 교양 추천 -> 건물 기준 추천");

	square_char(40, 6, 20, 20, "1. 전 공 건 물 기 준");
	square_char(40, 6, 20, 35, "2. 복 수 전 공 건 물 기 준");

	goto_xy(2, 54);
	printf("번호 입력 >> ");
}

//교양 조회 화면    *************** 이동경로 표시 X
void ClassResearch_ui(void)
{
	Sleep(600);
	system("cls");
	square(79, 52, 0, 0);
	square(79, 2, 0, 53);

	goto_xy(65, 1);
	printf("0 = 뒤로가기");

	square_char(71, 2, 4, 6, "NO.  교양 이름                   분야              건물          ");
	square(71, 30, 4, 9); //교양 분야 건물 출력할 네모 화면 출력

	goto_xy(6, 40);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkGreen);
	printf("* 전공건물");
	goto_xy(18, 40);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkYellow);
	printf("* 복수전공건물");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);

	square_char(71, 2, 4, 44, "해당하는 번호 입력 시 북마크에 추가됩니다.");

	square(71, 3, 4, 47);
	goto_xy(18, 48);
	printf("a 입력 시 이전 페이지, d 입력 시 다음 페이지");
	goto_xy(26, 49);
	printf("0 입력 시 이전 화면으로 이동");

	goto_xy(2, 54);
	printf("번호 입력 > ");
}

//분야 선택 화면 ++이동경로 표시 완료
void FieldChoice_ui(void)
{
	Sleep(600);
	system("cls");
	square(79, 52, 0, 0);

	square_char(41, 2, 19, 2, "분 야 선 택");
	square(79, 2, 0, 53);

	goto_xy(65, 1);
	printf("0 = 뒤로가기");
	user_print();

	square_char(41, 4, 19, 19, "1. 문학, 예술, 문화");
	square_char(41, 4, 19, 25, "2. 역사와 철학");
	square_char(41, 4, 19, 31, "3. 정치, 경제, 사회");
	square_char(41, 4, 19, 37, "4. 과학과 기술");
	square_char(41, 4, 19, 43, "5. 실용 실천");

	goto_xy(20, 50);
	printf("메인 화면 -> 교양 추천 -> 분야 기준 추천");

	goto_xy(2, 54);
	printf("번호 입력 >> ");
}

//북마크 조회 화면  *************** 이동경로 표시 X
void bookMark_ui(void)
{
	Sleep(600);
	system("cls");
	square(79, 52, 0, 0);
	square(79, 2, 0, 53);

	goto_xy(65, 1);
	printf("0 = 뒤로가기");

	square_char(41, 2, 19, 2, "북 마 크 목 록");

	square_char(71, 2, 4, 6, "NO.  교양 이름                   분야              건물          ");
	square(71, 30, 4, 9); //건물 출력할 네모 화면 출력

	goto_xy(6, 40);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkGreen);
	printf("* 전공건물");
	goto_xy(18, 40);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkYellow);
	printf("* 복수전공건물");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);

	square_char(71, 2, 4, 44, "해당하는 번호 입력 시 북마크에서 삭제됩니다.");

	square(71, 3, 4, 47);
	//goto_xy(18, 48);
	//printf("a 입력 시 이전 페이지, d 입력 시 다음 페이지");
	goto_xy(26, 49);
	printf("0 입력 시 이전 화면으로 이동");

	goto_xy(2, 54);
	printf("번호 입력 >> ");
}



void square(int width, int height, int x, int y)
{
    goto_xy(x, y);
    for (int i = 0; i < width; i++) {   //첫 가로 라인 그리기
        if (i == 0) printf("┏");
        else if (i == width - 1) printf("┓\n");
        else  printf("━");
    }
    goto_xy(x, y + 1);
    for (int j = 0; j < height; j++) {  //세로 라인 그리기
        printf("┃");
        for (int k = 0; k < width - 2; k++) {
            printf(" ");
        }
        printf("┃\n");
        goto_xy(x, y + (j + 1));
    }
    for (int m = 0; m < width; m++) {   //마지막 가로 라인 그리기
        if (m == 0) printf("┗");
        else if (m == width - 1) printf("┛\n");
        else printf("━");
    }
}

void square3(int width, int height, int x, int y)
{
    goto_xy(x, y);
    for (int i = 0; i < width; i++) {
        if (i == 0) printf("┣");
        else if (i == width - 1) printf("┨\n");
        else  printf("━");
    }
    goto_xy(x, y + 1);
    for (int j = 0; j < height; j++) {
        printf("┃");
        for (int k = 0; k < width - 2; k++) {
            printf(" ");
        }
        printf("┃\n");
        goto_xy(x, y + (j + 1));
    }
    for (int m = 0; m < width; m++) {
        if (m == 0) printf("┗");
        else if (m == width - 1) printf("┛\n");
        else printf("━");
    }
}

void square_char(int width, int height, int x, int y, char* msg)
{
    goto_xy(x, y);
    for (int i = 0; i < width; i++) {
        if (i == 0) printf("┏");
        else if (i == width - 1) printf("┓\n");
        else  printf("━");
    }
    goto_xy(x, y + 1);
    for (int j = 0; j < height; j++) {
        printf("┃");
        for (int k = 0; k < width - 2; k++) {
            if (j == height / 2 && k == ((width)-strlen(msg)) / 2) {
                goto_xy(x + ((width)-strlen(msg)) / 2 + 1, y + height / 2);
                printf("%s", msg);
                goto_xy(x + width - 1, y + j);
            }
            else if (j != height / 2) printf(" ");
        }

        printf("┃");
        goto_xy(x, y + (j + 1));
    }
    for (int m = 0; m < width; m++) {
        if (m == 0) printf("┗");
        else if (m == width - 1) printf("┛\n");
        else printf("━");
    }
}

void goto_xy(int x, int y)
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos;
    pos.X = x;
    pos.Y = y;
    SetConsoleCursorPosition(handle, pos);
}