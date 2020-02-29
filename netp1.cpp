
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hfilter.h"

#pragma comment(lib,"ws2_32")
char local='c';
static DWORD CALLBACK FdWorker(IN LPVOID lpCtx){
	
	SOCKET   fd      = (SOCKET)lpCtx;
	H_FILTER hFilter;
	hFilter.InitFilter(fd);
	//if(temp == 'y')
	{
	/////	printf("cpp local:%c temp:%c\n",local,temp);////当失败的连接尝试时，直接返回
		return 0;
	}
	
//	printf("cpp local:%c\n",local);
	return 0;
}
int main(int argc, char* argv[]){

	WSADATA  wsaData ;
	INT      i       ;
	SOCKET   lfd,fd  ;
	SOCKADDR_IN addr ;

	memset(&wsaData,0,sizeof(WSADATA));
	i = WSAStartup(0x0808,&wsaData);
	assert(NO_ERROR == i);
	
	lfd = socket(AF_INET,SOCK_STREAM,0);
	 char name[255];//定义用于存放获得的主机名的变量
	 char *ip; //定义IP地址变量
	 PHOSTENT hostinfo; 
	 if( gethostname ( name, sizeof(name)) == 0) 
	 { //如果成功地将本地主机名存放入由name参数指定的缓冲区中 
		 if((hostinfo = gethostbyname(name)) != NULL)
		 { 
			 ip = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list); 
			 
			 printf("%s\n",ip);
		 }
	 }


	
	addr.sin_addr.S_un.S_addr = inet_addr(ip);
	addr.sin_family           = PF_INET;
	addr.sin_port             = htons(80);

	i = bind(lfd,(const sockaddr*)&addr,sizeof(SOCKADDR_IN));
	assert(NO_ERROR == i);
	listen(lfd,54);
	int k=0;
	char test[123]="dadadadadadadada";
	int l =strlen(test);
	do {
		i   = sizeof(SOCKADDR_IN);
		memset(&addr,0,i);
		fd  = accept(lfd,(sockaddr*)&addr,&i);

		if (INVALID_SOCKET != fd){
			k++;
	////		printf("before use cpp local:%c k:%d\n",local,k);/////结果显示浏览器连接时进行多次连接尝试
			
			{
			HANDLE  hThread = CreateThread(NULL,
				                           0,
										   FdWorker,
										   (LPVOID)fd,
										   0,
										   NULL
										   );
			if (hThread){
				CloseHandle(hThread);
			} else {
				//
				// out of memory,and have to close this connection now
				//
				closesocket(fd);
			}
			}	
		} else {
			
			//
			// Oops,why do we get here ???
			//
		}
	} while ("Continue");



	
	WSACleanup();

	return 0;
}


