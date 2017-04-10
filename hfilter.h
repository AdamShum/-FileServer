#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <ctype.h>
#include <stdlib.h>
using namespace std;
#undef _UNICODE
class H_FILTER {

public:

	enum { MAX_REQ_SIZE = (1024 * 512 - 1) };
	char         *m_reqBuffer;
	INT           m_reqLength;
	H_FILTER()
	{
	}
	virtual ~H_FILTER()
	{
	}
	typedef unsigned char BYTE;
	inline BYTE toHex(const BYTE &x)
	{
		return x > 9 ? x + 55 : x + 48;
	}
	void urlEncoding(char *sIn)
	{
		int i = 0;
		int t;
		char sOut[1024] = "";
		for (int ix = 0; ix < (int)strlen(sIn); ix++)
		{
			t = 0;
			BYTE buf[4];
			memset(buf, 0, 4);
			if (isalnum((BYTE)sIn[ix]))
			{
				buf[0] = sIn[ix];
			}
			else if (isspace((BYTE)sIn[ix]))
			{
				buf[0] = '%';
				buf[1] = '2';
				buf[2] = '0';
			}
			else
			{
				if (sIn[ix] == '.')
				{
					buf[0] = '.';
				}
				else{
					buf[0] = '%';
					buf[1] = toHex((BYTE)sIn[ix] >> 4);
					buf[2] = toHex((BYTE)sIn[ix] % 16);
				}
			}
			for (t; t<(int)strlen((char *)buf); t++)
			{
				sOut[i++] = buf[t];
			}
		}
		sOut[i] = '\0';
		for (t = 0; t<i + 1; t++)
		{
			sIn[t] = sOut[t];
		}
	}
	void urldecode(char *p)
	{
		int i = 0;
		while (*(p + i))
		{
			if ((*p = *(p + i)) == '%')
			{
				*p = *(p + i + 1) >= 'A' ? ((*(p + i + 1) & 0XDF) - 'A') + 10 : (*(p + i + 1) - '0');
				*p = (*p) * 16;
				*p += *(p + i + 2) >= 'A' ? ((*(p + i + 2) & 0XDF) - 'A') + 10 : (*(p + i + 2) - '0');
				i += 2;
			}
			else if (*(p + i) == '+')
			{
				*p = ' ';
			}
			p++;
		}
		*p = '\0';
		return;
	}
	char InitFilter(SOCKET fd){
		char	local;
		BOOL     fYES = FALSE;
		int      i;
		int      off = 0;
		m_reqBuffer = new char[MAX_REQ_SIZE + 1];

		while (TRUE){
			i = recv(fd, &m_reqBuffer[off], MAX_REQ_SIZE - off, 0);
			if (i <= 0){
				break;
			}
			off += i;
			if ('\n' == m_reqBuffer[off - 1]){
				break;
			}

			if (i <= 0){
				break;
			}
		}
		i = off;
		m_reqBuffer[i] = '\0';
		if (strlen(m_reqBuffer) == 0)
		{
			///	printf("刚才出错的地方\n");/////失败的连接
			return 'y';
		}
		fYES = TRUE;
		printf("%s", m_reqBuffer);/////////////////////////////////////////
		if (fYES){
			PCHAR  Ptr = m_reqBuffer;
			PCHAR  Prev;
			while (*Ptr && ' ' != *Ptr++){
			}
			Prev = Ptr;
			while (*Ptr && ' ' != *Ptr){
				++Ptr;
			}
			char    szUrl[4096];
			i = (INT)(Ptr - Prev);
			memcpy(szUrl, Prev, i);
			szUrl[i] = '\0';

			Ptr = szUrl;
			while (*Ptr){
				if ('/' == *Ptr){
					*Ptr = '\\';
				}
				Ptr++;
			}
			//		printf ("解码前Url:[%s] LOCAL:%c\r\n",szUrl,local);
			//		printf("strlen(szUrl) %d\n",strlen(szUrl));
			urldecode(szUrl);
					printf ("\r\n解码后Url:[%s] \r\n",szUrl);
			//if (strlen(szUrl)>2 && szUrl[2] == '\\')
			//{
			//	local = szUrl[1];
			//	//	printf("local = szUrl[1] %c\n",local);
			//	Ptr = &szUrl[2];
			//	//	printf("ptr:%s\n",Ptr);
			//	memcpy(szUrl, Ptr, i - 2);
			//	szUrl[i - 2] = '\0';
			//	//		printf("szUrl:%s\n",szUrl);

			//}

			if (strlen(szUrl)==1&&'\\'==szUrl[0]){
				printf("root_handler\n");
				root_handler(fd, szUrl);
			}
			else{
				if (strstr(szUrl,".")&&szUrl[strlen(szUrl)-1]!='\\'){
					if (strstr(m_reqBuffer, "Range")){
						char * a = strstr(m_reqBuffer, "Range");
						char * b = strstr(a, "\r\n");
						char c[256] = { 0 };
						memcpy(c, a, b - a);
						printf(c);
						printf("\n");
					}
					printf("file_handler\n");
					file_handler(fd, szUrl);
				}
				else {
					printf("directory_handler\n");
					directory_handler(fd, szUrl);
				}
			}
			
		
		}
		delete[]m_reqBuffer;
		closesocket(fd);
		return 0;
	}
protected:

	void root_handler(SOCKET fd, IN PCHAR szUrl){
		
		char	Buffer[MAX_PATH];
		char	s[MAX_PATH] = {0};
		int		k = 0; 
		//设置初始界面	<img src="logo/logo_jw.png">
		k += sprintf(&Buffer[k], "<html>\r\n"
			"	<head>\r\n"
			"		<title>Local Disk</title>\r\n"
			"		<link rel=\"stylesheet\" type=\"text/css\" href=\"/d/webserver/css/root_handler.css\"/>\r\n"
			"	</head>\r\n"
			"	<body>\r\n"
			"		<div>\r\n"
			"			<div id=\"div_disknav\">\r\n"
			"				<ul class=\"local\"  style=\"display:block;\"> \r\n"
			"					<li class=\"tile\">\r\n"
			"						<a class=\"link\" href=\"/C\" > \r\n"
			"							<span class=\"span1\"><i>C</i></span>"
			"							<p>本地磁盘(C:)</p>\r\n"
			"						</a>\r\n"
			"					</li>\r\n"
			"					<li class=\"tile\">\r\n"
			"						<a class=\"link\" href=\"/D\" > \r\n"
			"							<span class=\"span2\"><i>D</i></span>"
			"							<p>本地磁盘(D:)</p>\r\n"
			"						</a>\r\n"
			"					</li>\r\n"
			"					<li class=\"tile\">\r\n"
			"						<a class=\"link\" href=\"/E\" > \r\n"
			"							<span class=\"span3\"><i>E</i></span>"
			"							<p>本地磁盘(E:)</p>\r\n"
			"						</a>\r\n"
			"					</li>\r\n"
			"					<li class=\"tile\">\r\n"
			"						<a class=\"link\" href=\"/F\" > \r\n"
			"							<span class=\"span4\"><i>F</i></span>"
			"							<p>本地磁盘(F:)</p>\r\n"
			"						</a>\r\n"
			"					</li>\r\n"
			"				</ul>\r\n"
			"			</div>\r\n"
		//	"<video src = \"http://192.168.2.227/d/FTN-024.mp4\" controls = \"controls\">< / video>"
			"		</div>\r\n"
			"	</body>\r\n"
			"</html>\r\n"
								 );
		int	i = sprintf(s,
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"charset = utf8\r\n"
			"Content-Length: %d\r\n"
			"Connection:close\r\n\r\n"
			"%s",
			k,
			Buffer
			);
		send(fd, s, i, 0);

		return;

	}

	//if chose local disk


	//
	void directory_handler(SOCKET fd, IN PCHAR szUrl){
		char name[1024];
		char   *szHtml = new char[1024 * 1024];
		int		t;
		int		i;
		int     off = 0; 
		char local = szUrl[1];
		char* szUrl1;
		printf("directory_handler szurl:%s\n", szUrl);
		printf("d-handle local:%c\n", local);
		off += sprintf(&szHtml[off],
			"<html><head><title>%s</title>"
			"<link rel=\"stylesheet\" type=\"text/css\" href=\"/d/webserver/css/directory_handler.css\"/>"
			"</head><body>\r\n"
			" <div>\r\n"
			"			<div id=\"div_disknav\">\r\n"
			"				<div class=\"div_directory\"  style=\"display:block;\"> \r\n",
			szUrl
			);
		WIN32_FIND_DATAA fData;
		char szFinder[1024];
		if (szUrl[2]!='\0')
		 szUrl1 = szUrl + 3;
		else szUrl1 = szUrl + 2;
		if (szUrl[2] == '\0'){
			sprintf(szFinder, "%c:\\*.*", local);
		}
		else{
			sprintf(szFinder, "%c:\\%s\\*.*", local, szUrl+2);
		}
		printf("szFinder:%s\n", szFinder);
		HANDLE   hFinder = FindFirstFileA(szFinder, &fData);
		if (hFinder == INVALID_HANDLE_VALUE)
		{
			printf("Error:INVALID_HANDLE_VALUE\n");
			return ;
		}
		urlEncoding(szUrl1);
		do {
			if (0 != (FILE_ATTRIBUTE_DIRECTORY & fData.dwFileAttributes)){
				if ('.' != fData.cFileName[0])
				{

					for (t = 0; t<strlen(fData.cFileName); t++)
					{
						name[t] = fData.cFileName[t];
					}
					name[t] = '\0';
					
					//文件夹
					
					urlEncoding(fData.cFileName);
					
					off += wsprintfA(&szHtml[off],	
						"					<div class=\"tile\">\r\n"
						"						<a class=\"directory\" href=\"/%c/%s%s/\" > \r\n"
						"							<span class=\"span1\"><i>    </i></span>"
						"							<p>%s</p>\r\n"
						"						</a>\r\n"
						"					</div>\r\n",
						local,szUrl1, fData.cFileName, name);
				}
			}
			else { // file
				/////////////////////////////////////////////////////文件

				for (t = 0; t<strlen(fData.cFileName); t++)
				{
					name[t] = fData.cFileName[t];
				}
				name[t] = '\0';
				urlEncoding(fData.cFileName);
				off += wsprintfA(&szHtml[off],
					"					<div class=\"tile\">\r\n"
					"						<a class=\"file\" href=\"/%c/%s%s\" > \r\n"
					"							<span class=\"span2\"><i>    </i></span>"
					"							<p>%s</p>\r\n"
					"						</a>\r\n"
					"					</div>\r\n",
					local, szUrl1, fData.cFileName, name);
				/*off += wsprintfA(&szHtml[off],
					"<a class=\"file\" href=\"/%c/%s%s\">%s</a>\r\n", 
					local, szUrl1,fData.cFileName, name);*/
				//printf("test:%s", szHtml);
				//////////////////////////////////////////尝试尝试尝试尝试尝试尝试
				//	off += wsprintf(&szHtml[off],"</br><a href=\"测试\">测试</a>\r\n");	
			}
		//	off += sprintf(&szHtml[off], "<br />\r\n");
		} while (FindNextFileA(hFinder, &fData));
		if (off > 0){
			off += sprintf(&szHtml[off], "\r\n</div></div></div>\r\n</body></html>\r\n");
			char *t = new char[off + 2048];
			i = sprintf(t,
				"HTTP/1.1 200 OK\r\n"
				"Content-Type: text/html\r\n"
				"Charset:UTF8"
				"Content-Length: %d\r\n"
				"Connection: close\r\n\r\n"
				"%s",
				off,
				szHtml
				);
			send(fd, t, i, 0);
			delete[]t;
		}
		delete[]szHtml;
		
		return ;
	}

	//file handler
	char file_handler(SOCKET fd, IN PCHAR szUrl){
		//	if(0==strcmp("\\favicon.ico",szUrl))////当浏览器请求favicon.ico时，忽略其请求
		//	{
		//		return local;
		//	}
		//	printf("file_handler szUrl:%s\n",szUrl);/////////////////////无本地磁盘的路径
		char local = szUrl[1];
		unsigned long k;
		char  szTemp[MAX_PATH];
		sprintf(szTemp, "%c:%s", local, szUrl+2);
		printf("file_handler %s\n", szTemp);//////////////绝对路径
		unsigned long length;
		FILE *fp = fopen(szTemp, "rb");
		if (NULL == fp)
		{
			//	fclose(fp);
			closesocket(fd);
			printf("Error:File can't be opened!\n");
			return local;
		}
		fseek(fp, 0L, SEEK_END);
		length = ftell(fp);
		if (length>1024 * 1024 * 1024)
		{
			printf("%ldGB\n", length / 1024 / 1024 / 1024);
		}
		else if (length>1024 * 1024 && length<1024 * 1024 * 1024)
		{
			printf("%ldMB\n", length / 1024 / 1024);
		}
		else
		{
			printf("%ldKB\n", length / 1024);
		}
		fseek(fp, 0L, SEEK_SET);
		int i;
		char Temp[4096];
		i = sprintf(Temp,
			"HTTP/1.1 200 OK\r\n"
		//	
			"Content-Lenght: %d\r\n"
			"Content-Type:aduio/mpeg application/octet-stream image/gif image/jpeg image/x-xbitmap image/pjpeg\r\n"
			"Content-Range:bytes 0-%d/%d\r\n"
			"Connection: close\r\n\r\n",
			length, length-1, length
			);
		i = send(fd, Temp, i, 0);//文件大小
		/*	if(length == 0)
		{
		closesocket(fd);
		fclose(fp);
		return local;
		}
		else	if(length<4096)
		{
		fread(Temp,length,1,fp);
		Temp[length] = '\0';
		send(fd,Temp,length,0);
		}
		else
		while(fread(Temp,4096,1,fp) == 1)
		{
		//	printf("%s",Temp);
		send(fd,Temp,4096,0);
		}*/

		do
		{
			i = 0;
			k = length - ftell(fp);
			if (k == 0)
			{
				break;
			}
			if (k<4096)
			{
				fread(Temp, k, 1, fp);
				Temp[k] = '\0';
				send(fd, Temp, k + 1, 0);
			}
			else
			{
				fread(Temp, 4096, 1, fp);
				send(fd, Temp, 4096, 0);
				i = 1;
			}
			
		} while (i == 1);
		printf("传输完毕....!\n");
		closesocket(fd);
		fclose(fp);
		return local;
	}
};


