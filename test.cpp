#include <iostream>
#include <string>

int main()
{

    std::string input = "  15168484865   \"";

    std::size_t s = input.find_first_not_of(" \t\r\n");
    
    if(s != std::string::npos) input = input.substr(s,input.size() - s);

    s = input.find_last_not_of(" \t\r\n\"");

    if(s != std::string::npos) input = input.substr(0,s+1);

    std::cout << input;
}