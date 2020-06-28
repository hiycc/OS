#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory>
#include <sys/mman.h>
using namespace std;



//缓存数据对象
class InfoNode{
    int id; //数据ID
    int size;   //数据大小
    
    int index;  //缓存区块起始序号
    int num;    //缓冲区块使用数

    InfoNode *pre;
    InfoNode *next;

    InfoNode(int id,int size,int index,int num){
        this->id = id;
        this->size = size;
        this->index = index;
        this->num = num;
        pre = NULL;
        next = NULL;
    }
};








void malloc_1024(){
    void *p;
    //分配1G内存 1G = 1024M = 1024*1024*1024
    p = malloc(1024*1024*1024);
    //从p地址开始的1G内都锁定不被换出
    mlock(p,1024*1024*1024);


}



int main(){
    //创建套接字
    int serv_sock = socket (AF_INET,SOCK_STREAM,IPPROTO_TCP);

    //将套接字和IP、端口绑定
    struct sockaddr_in serv_addr;
    memset(&serv_addr,0,sizeof(serv_addr)); //每个字节都用0填充
    serv_addr.sin_family = AF_INET; //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //具体IP地址
    serv_addr.sin_port = htons(1234);   //端口
    bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

    //进入监听状态，等待用户发起请求
    listen(serv_sock,20);


    //接受客户端请求
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_size);

    //向客户端发送数据
    char str[] = "Hello World!";
    char buffer[40];
    read(clnt_sock,buffer,sizeof(buffer)-1);

    string buf(buffer);

    //cout<<buf[0]<<endl;
    if(buf[0]=='W'){
        //通过反向寻找第一个“，”，先截取出数据段
        int data_index = buf.rfind(',');
        string data = buf.substr(data_index+1);
        cout<<data<<endl;

        //再从buf删去已经截取了数据段的字符串，再通过同样的方法截取出数据id
        buf = buf.substr(0,data_index);
        int id_index = buf.rfind(',');
        string id_str = buf.substr(id_index+1);
        int id = std::stoi(id_str);
        //cout<<id<<endl;

        
    }


    //write(clnt_sock,str,sizeof(str));
    
    // printf("Message from client: %s\n",buffer);

    // while(1){
    //     read(clnt_sock,buffer,sizeof(buffer)-1);
    //     printf("Message from client: %s\n",buffer);
    //     string flag(buffer);
    //     if(flag=="exit"){
    //         break;
    //     }
    // }

    //关闭套接字
    close(clnt_sock);
    close(serv_sock);
    
}

