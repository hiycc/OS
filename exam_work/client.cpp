#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
using namespace std;
#define BUF_SIZE 256


int main(){
    
    

    //向服务器(特定的IP和端口)发送请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr,0,sizeof(serv_addr));//每个字节都用0填充
    serv_addr.sin_family = AF_INET; //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //具体的IP地址
    serv_addr.sin_port = htons(1234);   //端口

    char bufSend[BUF_SIZE];
    char bufRev[BUF_SIZE];
    // char buffer[64];

    // ifstream in("out.txt");
    // if(!in.is_open()){
    //     cout<<"Read test data error!"<<endl;
    // }
    // int i = 0;
    // while(!in.eof()){
    //     in.getline(bufSend,64);
    //     //cout<<buffer;
    //     //创建套接字
    //     int sock = socket(AF_INET,SOCK_STREAM,0);   //SOCK_STREAM表示使用面向连接的数据传输方式
    //     connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

    //     string flag = "W:";
    //     flag.append(std::to_string(i)+","+std::to_string(34)+",");
    //     char *buf = &flag[0];
    //     write(sock,buf,flag.size());
    //     write(sock,bufSend,strlen(bufSend));
    //     read(sock,bufRev,BUF_SIZE);
    //     cout<<bufRev<<endl;

    //     memset(bufSend,0,BUF_SIZE);
    //     memset(bufRev,0,BUF_SIZE);

    //     close(sock);
    //     i++;
    // }


    while(1){
        //创建套接字
        int sock = socket(AF_INET,SOCK_STREAM,0);   //SOCK_STREAM表示使用面向连接的数据传输方式
        connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

        cin>>bufSend;
        write(sock,bufSend,strlen(bufSend));
        read(sock,bufRev,BUF_SIZE);
        cout<<bufRev<<endl;

        memset(bufSend,0,BUF_SIZE);
        memset(bufRev,0,BUF_SIZE);

        close(sock);
    }

    // for(int i=0;i<1024;i++){
    //     for(int j=0;j<1024;j++){
    //         for(int k=0;k<4;k++){
    //             int sock = socket(AF_INET,SOCK_STREAM,0);   //SOCK_STREAM表示使用面向连接的数据传输方式
    //             connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

    //         }
    //     }
    // }

    //connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));



    

}