#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#define PORT 10000

char hi_buffer[100] = "안녕하세요.만나서 반가워요.";
char rcvBuffer[100];
char name_buffer[100] ="내 이름은 서윤재야"; 
char age_buffer[100]="나는 22살이야";
char sep_buffer[100]; 
char *cmpArr[3];
char* cmp_buffer;  char* result;
char* res_buffer;



int main(){
	int c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	int len;
	int n,i;

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
		printf("클라이언트 접속 허용\n");
	while(1){
		n = read(c_socket, rcvBuffer, sizeof(rcvBuffer));
		printf("rcvBuffer: %s\n", rcvBuffer);
		if(strncasecmp(rcvBuffer,"quit",4)==0||strncasecmp(rcvBuffer,"kill server",11) == 0)
			break;
		if(strncasecmp(rcvBuffer,"안녕하세요.",16)==0){
			write(c_socket,hi_buffer,strlen(hi_buffer));
			continue;}
		if(strncasecmp(rcvBuffer,"이름이 뭐야?",17)==0){
			write(c_socket,name_buffer,strlen(name_buffer));
			continue;}
		if(strncasecmp(rcvBuffer,"몇 살이야?",14)==0){
			write(c_socket,age_buffer,strlen(age_buffer));
			continue;}
		if(strncasecmp(rcvBuffer,"strlen",6)==0){
			sprintf(sep_buffer,"문자열 길이 = %d",strlen(rcvBuffer)-7);
			write(c_socket,sep_buffer,strlen(sep_buffer));
			continue;}
		if(strncasecmp(rcvBuffer,"strcmp",6)==0){
			cmp_buffer = strtok(rcvBuffer," ");
			i=0;
			while(cmp_buffer != NULL){
				cmpArr[i] = cmp_buffer;
				i++;
				cmp_buffer = strtok(NULL," ");
			}
			if(strcmp(cmpArr[1],cmpArr[2])==0){
				*result = '0';}
			else{
				*result = '1';}
			write(c_socket,result,strlen(result));
			continue;}
		write(c_socket, rcvBuffer, n); //클라이언트에게 buffer의 내용을 전송함
	}
		close(c_socket);
		if(strncasecmp(rcvBuffer, "kill server", 11)==0)
				break;
	}
	close(s_socket);
	return 0;	
}
