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

FILE* userFile;

char id[20], pw[20], building[20];
int check = -1; //아이디에 해당하는 비밀번호, 전공건물을 찾기위한 인덱스

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
