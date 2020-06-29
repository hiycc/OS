#include <iostream>
#include <fstream>
#include <stdio.h>
using namespace std;
char *rand_str(char *str,const int len)
{
    srand(time(NULL));
    int i;
    for(i=0;i<len;++i)
    {
        switch((rand()%3))
        {
            case 1:
            str[i]='A'+rand()%26;
       break;
        case 2:
        str[i]='a'+rand()%26;
        break;
     default:
        str[i]='0'+rand()%10;
       break;
         }
    }
     str[++i]='\0';
    return str;
}
int main () {
    ofstream out("out.txt");
    if (out.is_open()) {
        for(int i=0;i<1024;i++){
            for(int k=0;k<4;k++){
                char* str = new char[34];
                out<<i+k<<"."<<rand_str(str,34)<<"\n";
            }
            
        }
        out.close();
    }
    return 0;
}
