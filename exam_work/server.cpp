#include <stdio.h>
#include <iostream>
#include <string>
#include <cmath>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory>
#include <cstddef>
#include <sys/mman.h>
#include <list>
#include <unordered_map>
using namespace std;
#define BUF_SIZE 256



const int NUM = 256*1024*1024;
// //缓存数据对象
// class InfoNode{
//     int id; //数据ID
//     int size;   //数据大小
    
//     int index;  //缓存区块起始序号
//     int num;    //缓冲区块使用数

//     InfoNode *pre;
//     InfoNode *next;

//     InfoNode(int id,int size,int index,int num){
//         this->id = id;
//         this->size = size;
//         this->index = index;
//         this->num = num;
//         pre = NULL;
//         next = NULL;
//     }
// };

struct InfoNode{
            int id;
            int size;
            int index;
            int num;
            InfoNode(int i,int s,int ind,int n):id(i),size(s),index(ind),num(n){}
            InfoNode* getself(){return this;}
        };
class LRUCache{
        
    public:
        LRUCache(int c):capacity(c){}

        InfoNode* get(int key){
            if(cacheMap.find(key)==cacheMap.end()){
                InfoNode *p = new InfoNode(-1,0,0,0);
                return p;
            }else{
                cachelist.splice(cachelist.begin(),cachelist,cacheMap[key]);
                cacheMap[key] = cachelist.begin();
                return cacheMap[key]->getself();
            }
        }

        void set(int id,int size,int index,int num){
            if(cacheMap.find(id)==cacheMap.end()){
                //淘汰最后一个，然后将其加入到第一个位置
                if(cachelist.size()==capacity){
                    cacheMap.erase(cachelist.back().id);
                    cachelist.pop_back();
                }
                cachelist.push_front(InfoNode(id,size,index,num));
                cacheMap[id] = cachelist.begin();
            }else{
                //更新节点的值，并将其加到第一个位置
                cacheMap[id]->size = size;
                cacheMap[id]->id = id;
                cacheMap[id]->index = index;
                cacheMap[id]->num = num;
                cachelist.splice(cachelist.begin(),cachelist,cacheMap[id]);
                cacheMap[id] = cachelist.begin();
            }
        }

    private:
        int capacity;
        list<InfoNode> cachelist;
        unordered_map<int,list<InfoNode>::iterator> cacheMap;

};









int Index = 0;
LRUCache *lrucache = new LRUCache(NUM);

int main(){
    char *p = new char[256ULL*1024ULL*1024ULL];
    mlock(p,256ULL*1024ULL*1024ULL);

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

    char buffer[BUF_SIZE];
    while(1){
        
        int clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_size);
        //接受客户端数据
        
        read(clnt_sock,buffer,sizeof(buffer)-1);
        
        string buf(buffer);
        string b(buf);

        if(buf[0]=='W'){
            //通过反向寻找第一个“，”，先截取出数据段
            int data_index = buf.rfind(',');
            string data = buf.substr(data_index+1);
            //cout<<data<<endl;

            //再从buf删去已经截取了数据段的字符串，再通过同样的方法截取出数据id
            buf = buf.substr(0,data_index);
            int size_index = buf.rfind(',');
            string size_str = buf.substr(size_index+1);
            int size = std::stoi(size_str);
            //cout<<size<<endl;

            buf = buf.substr(0,size_index);
            int id_index = buf.rfind(':');
            string id_str = buf.substr(id_index+1);
            //cout<<"id_str:"<<id_str<<endl;
            int id = std::stoi(id_str);
            //cout<<id<<endl;

            int num = ceil(size/64)+1;

            lrucache->set(id,size,Index,num);
            char *data_pin = &data[0];
            memcpy(p+Index*64,data_pin,64);

            Index +=num;

            

            // char bufSend[BUFSIZ];
            string bufSend = std::to_string(id);
            bufSend.append(",ok");
            //cout<<bufSend<<endl;
            char *bufsend = &bufSend[0];
            write(clnt_sock,bufsend,strlen(bufsend));
            
        }else if(buf[0]=='R'){
            int id_index = buf.rfind(':');
            string id_str = buf.substr(id_index+1);
            int id = std::stoi(id_str);

            InfoNode *node = lrucache->get(id);
            if(node->id==-1){
                //说明待读出的ID还没写入
                string bufSend("FFFFFFFF");
                char *bufsend = &bufSend[0];
                write(clnt_sock,bufsend,strlen(bufsend));
            }else{
                int index = node->index;
                cout<<index<<endl;
                int size = node->size;
                cout<<size<<endl;
                char *data = new char[size];
                strncpy(data,p+index*64,size);
                //string data(p,index*64,size);
                string bufSend = std::to_string(id);

                bufSend.append(","+string(data));
                char *bufsend = &bufSend[0];
                write(clnt_sock,bufsend,strlen(bufsend));
            }
        }

        
        close(clnt_sock);
        memset(buffer, 0, BUF_SIZE);  //重置缓冲区
    }

    delete []p;
    close(serv_sock);

    
    //接受客户端数据
    //char buffer[40];
    //read(clnt_sock,buffer,sizeof(buffer)-1);

    // string buf(buffer);
    // string b(buf);

    //cout<<buf[0]<<endl;
    // if(buf[0]=='W'){
    //     //通过反向寻找第一个“，”，先截取出数据段
    //     int data_index = buf.rfind(',');
    //     string data = buf.substr(data_index+1);
    //     cout<<data<<endl;

    //     //再从buf删去已经截取了数据段的字符串，再通过同样的方法截取出数据id
    //     buf = buf.substr(0,data_index);
    //     int size_index = buf.rfind(',');
    //     string size_str = buf.substr(size_index+1);
    //     int size = std::stoi(size_str);
    //     cout<<size<<endl;

    //     buf = buf.substr(0,size_index);
    //     int id_index = buf.rfind(':');
    //     string id_str = buf.substr(id_index+1);
    //     //cout<<"id_str:"<<id_str<<endl;
    //     int id = std::stoi(id_str);
    //     cout<<id<<endl;

    //     int num = ceil(size/64);

    //     lrucache->set(id,size,Index,num);
    //     Index +=num;

    //     char *data_pin = &data[0];
    //     memcpy(p,data_pin,size);

    //     for(int i=0;i<size;i++){
    //         cout<<p[i];
    //     }
    //     cout<<endl;


    //     // char* flag = &b[0];
    //     // memcpy(p,flag,13);

    //     // char* storebox;
    //     // memcpy(storebox,p,13);
    //     // cout<<storebox<<endl;
        
        
    // }

    // delete []p;
    // //write(clnt_sock,str,sizeof(str));
    
    // // printf("Message from client: %s\n",buffer);

    // // while(1){
    // //     read(clnt_sock,buffer,sizeof(buffer)-1);
    // //     printf("Message from client: %s\n",buffer);
    // //     string flag(buffer);
    // //     if(flag=="exit"){
    // //         break;
    // //     }
    // // }

    // //关闭套接字
    // close(clnt_sock);
    // close(serv_sock);
    
}

