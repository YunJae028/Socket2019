#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#define PORT 10000
#define BUFSIZE 10000
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>


char buffer[BUFSIZE];
char rcvBuffer[BUFSIZE];
int numClient=0;

void do_service(int c_socket);
void sig_handler();

int main(){
	int c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	int len;
	int n;

	signal(SIGCHLD, sig_handler); //첫번째 인자: 시그널번호, 두번째 인자:첫번째 인자의  시그널이 발생했을 때 실행되는 함수
 
	
	// 1. 서버 소켓 생성
	//서버 소켓 = 클라이언트의 접속 요청을 처리(허용)해 주기 위한 소켓
	s_socket = socket(PF_INET, SOCK_STREAM, 0); //TCP/IP 통신을 위한 서버 소켓 생성
	
	//2. 서버 소켓 주소 설정
	memset(&s_addr, 0, sizeof(s_addr)); //s_addr의 값을 모두 0으로  초기화
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY); //IP 주소 설정
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);

	//3. 서버 소켓바인딩
	if(bind(s_socket,(struct sockaddr *) &s_addr, sizeof(s_addr)) == -1){ 
		//바인딩 작업 실패 시, Cannot Bind 메시지 출력 후 프로그램 종료
		printf("Cannot Bind\n");
		return -1;
	}
	
	//4.listen() 함수 실행
	if(listen(s_socket, 5) == -1){
		printf("listen Fail\n");
		return -1;
	}

	//5. 클라이언트 요청 처리
	// 요청을 허용한 후, Hello World 메세지를 전송함
	
	while(1){ //무한 루프
		len = sizeof(c_addr);
		printf("클라이언트 접속을 기다리는 중....\n");
		c_socket = accept(s_socket, (struct sockaddr *)&c_addr, &len); 
		//클라이언트의 요청이 오면 허용(accept)해 주고, 해당 클라이언트와 통신할 수 있도록 클라이언트 소켓(c_socket)을 반환함.	
		printf("/client is connected\n");
		numClient++;
		printf("[%d]th_client connected\n",numClient);		
		printf("클라이언트 접속 허용\n");
		
		int pid;
		if((pid = fork()) > 0){
			close(c_socket);
			continue;
		}else if(pid == 0){
			close(s_socket);		
			do_service(c_socket);
			exit(0);
		}
		else{
			printf("fork() Fail");
			exit(0);}
	}
	close(s_socket);
	return 0;	
}


void do_service(int c_socket){
	int n;
	while(1){
		n = read(c_socket, rcvBuffer, sizeof(rcvBuffer)); //n string의 길이 
		printf("rcvBuffer: %s\n", rcvBuffer);
		rcvBuffer[n-1] = '\0'; //개행 문자 삭제 
		if(strncasecmp(rcvBuffer,"quit",4)==0||strncasecmp(rcvBuffer,"kill server",11) == 0)
			break;
		else if(!strncasecmp(rcvBuffer,"안녕하세요.",strlen("안녕하세요")))
			//strncasecmp(rcvBuffer,"안녕하세요.",strlen("안녕하세요"))==0 
				strcpy(buffer,"안녕하세요. 만나서 반가워요");
		else if(!strncasecmp(rcvBuffer,"이름이 뭐야?",strlen("이름이 뭐야?")))
				strcpy(buffer,"내 이름은 서윤재");	
		else if(!strncasecmp(rcvBuffer,"몇 살이야?",strlen("몇 살이야?")))
				strcpy(buffer,"22살이야");	
		else if(!strncasecmp(rcvBuffer,"strlen ",strlen("strlen ")))
				//문자열의 길이는  xx입니다.
				sprintf(buffer, "문자열의 길이는 %d입니다.", strlen(rcvBuffer)-7);
		else if(!strncasecmp(rcvBuffer,"strcmp ",strlen("strcmp "))){
			int i;
			char* cmpArr[3];
			char* token;
			token = strtok(rcvBuffer," ");
			i=0;
			while(token != NULL){
				cmpArr[i] = token;
				i++;
				token = strtok(NULL," ");
			}
			if(i < 3)
				strcpy(buffer,"문자열을 비교하기 위해서 두 문자열이 필요합니다.");		
			else if(!strcmp(cmpArr[1],cmpArr[2]))
				sprintf(buffer,"%s와 %s는 같은 문자열입니다.",cmpArr[1],cmpArr[2]);
			else
				sprintf(buffer,"%s와 %s는 다른 문자열입니다.",cmpArr[1],cmpArr[2]);
			}
		else if(!strncasecmp(rcvBuffer,"readfile ",strlen("readfile "))){
			char* fn;
			FILE* fp;
			char* str[10];
			int cnt = 0;
			fn = strtok(rcvBuffer," "); //token = readfile
			while(fn !=NULL){
				str[cnt] = fn; //str[0] = readfile, str[1]= <파일명>,....
				cnt++;
				fn = strtok(NULL," "); //token = <파일명>
			} 
			if(cnt < 2){
				strcpy(buffer, "파일명을 입력해주세요");
			}else{
				fp = fopen(str[1],"r"); 
				if(fp){ ////정상적으로  파일이 오픈 되었다면,
					char tempStr[BUFSIZE];
					memset(buffer, 0 ,BUFSIZE); //buffer 초기
					while(fgets(tempStr,BUFSIZE,(FILE *)fp)){
						strcat(buffer,tempStr); //여러 줄의 내용을 하나의  buffer에 저장하기 위해  strcat()함수 사용
					}			
				fclose(fp); }	
			else
				strcpy(buffer,"해당 파일은 존재하지 않습니다.");}
		}
		else if(!strncasecmp(rcvBuffer,"exec ",strlen("exec "))){
			char* sn;

			sn = strtok(rcvBuffer," ");
			sn = strtok(NULL,"\0"); //exec 뒤에 모든 문자열을 sn으로 저장
			printf("command : %s\n", sn);
			int result = system(sn);
			if(!result)
				sprintf(buffer,"%s command is executed",sn);
			else 
				sprintf(buffer,"%s command failed.",sn);
		}
		else
			strcpy(buffer,"무슨 말인지 모르겠습니다.");

		write(c_socket,buffer,strlen(buffer)); //클라이언트에게 buffer의 내용을 전송함
		}

		close(c_socket);

}

void sig_handler(int signo){
	int pid;
	int status;
	pid = wait(&status);
	printf("pid[%d] is terminated.status = %d\n",pid,status);
	numClient--;
	printf("현재 프로세스 수 : %d \n", numClient);
}

