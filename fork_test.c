#include <stdio.h>

int main(){
	int a = 10;
	int b = 100;
	
	a = a+10;
	
	sleep(10);

	int pid = fork();
	//fork() 함수는 현재 구동중인 프로세스의 복제본을 생성
	// 현재 프로세스 = 부모 프로세스
	// 생성된 프로세스 = 자식 프로세스 
	// 리턴값 ( pid) = 0, if 자식프로세스
	// 리턴값 = 자식프로세스의  pid값  if 부모 프로세스
	// 만약 fork() 함수 실패시, -1 값을 리턴
	if(pid > 0){ //부모 프로세스 
		printf("부모 프로세스 \n");
		a = a+10;
		printf("[Parent] a = %d, b = %d\n", a, b);
		sleep(20);
	}else if(pid == 0){ //자식 프로세스
		printf("자식 프로세스\n");
		b = b*10;
		printf("[Chid] a = %d, b = %d\n", a, b);
		sleep(20);
	}else { //fork() 실패
		printf("fork() 실패 \n");
	}

}

