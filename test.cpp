#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>


int main()
{
    char id[5] = "0909";

    for(int i = sizeof(id)-2; i > 0;i--)
    {
        if(id[i] < '9')
        {
            id[i]++;
            break;
        }
        id[i] = '0';
    }

    std::cout << id;
}