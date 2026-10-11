#include <iostream>
#include <filesystem>
#include <string>
#include <windows.h> 
#include <fstream>

namespace fs = std::filesystem;

long toTime(std::string str) //把vtt的时间转化为总毫秒数
{
    int h=0,m=0,s=0,ms=0;
    if(std::sscanf(str.c_str(),"%d:%d:%d.%d",&h,&m,&s,&ms) == 4 || 
    std::sscanf(str.c_str(),"%d:%d.%d",&m,&s,&ms) == 3)
    {
        return (h*3600000 + m*60000 + s*1000 + ms);
    }
    return -1;
}

std::string int_to_string(int a) //把时间转化为00
{
    if(a == 0) return "00";

    int temp = a;
    int i = 0; //a有几位数
    std::string s;
    while(temp != 0)//算出有几位数
    {
        temp /= 10;
        i++;
    }

    if(i == 1)
    {
        s = '0';
        s += (char)(a+'0'); 
        return s;
    }

    for(int j = i;j > 0;j--)
    {
        int p = 1;
        for(int k = j -1 ; k >0 ; k--)
        {
            p*=10;
        }
        s += (char)(a/p +'0');
        a %= p;
    }

    return s;
}

std::string finishString(long time) //最终时间的[00:00.00]
{
    int m=0,s=0,ms=0;

    ms = time%1000;
    ms /= 10;  //把毫秒变为两位数

    m = time/60000;
    s = (time % 60000) / 1000;

    std::string str;
    str += '[' + int_to_string(m) + ':' + int_to_string(s) + '.' + int_to_string(ms) + ']';
    return str;
}

std::string lrctime(std::string time) //把空格处理完后的时间丢进来处理
{
    long s_time = toTime(time);

    if(s_time == -1) return "-1";

    s_time = (s_time+5)/10 *10;
    time = finishString(s_time);
    return time;
}

void vtt_to_lrc(std::string path) //转化path路径(不包含后缀名)的vtt文件为lrc
{
    std::cout << path + ".vtt"<< "\n";
    
    std::ifstream old_vtt(fs::path(path + ".vtt"));

    if (!old_vtt) {
    std::cerr << "打不开文件: " << fs::absolute(path + ".vtt") << std::endl;
    return;
    }

    std::ofstream new_lrc(fs::path(path + ".lrc"));

    std::string line;

    bool first = true;

    std::string name;
    int o = path.rfind('\\') + 1;
    name = path.substr(o,path.size()-o);

    new_lrc << "[ti:" << name <<"]\n";

    std::string start_time,end_time;//一段开始时间

    while(std::getline(old_vtt , line))
    {
        std::cout << line << "\n";
        std::string text;//台词文本

        std::size_t pos = line.rfind("-->");

        if(pos == std::string::npos) continue;

        start_time = line.substr(0,pos);//把箭头前的时间截出来

        while(start_time.back() == ' ') start_time.pop_back(); //去除空格

        start_time = lrctime(start_time);

        if(start_time == "-1")
        {
            printf("某处的开始时间格式错误");
            system("pause");
            exit(1);
        }

        std::string temp;
        if(first || (!end_time.empty() && start_time == end_time))
        {
            while(std::getline(old_vtt,temp))
            {
                if(!temp.empty() && temp.back() == '\r') temp.pop_back();
                if(temp.empty()) break;
                if(!text.empty()) text += ' ';
                text += temp;
            }
            new_lrc << start_time << text << '\n';
        }
        else if(start_time != end_time)
        {
            new_lrc << end_time << '\n';
            while(std::getline(old_vtt,temp))
            {
                if(!temp.empty() && temp.back() == '\r') temp.pop_back();
                if(temp.empty()) break;
                if(!text.empty()) text += ' ';
                text += temp;
            }
            new_lrc << start_time << text << '\n';
        }
        std::cout << start_time << text << '\n';
        first = false;

        end_time = line.substr(pos + 3,line.size() -pos -3);
        while(end_time.front() == ' ') end_time.erase(0,1);
        end_time = lrctime(end_time);
        if(end_time == "-1")
        {
            printf("某处的结束时间格式错误");
            system("pause");
            exit(1);
        }
    }

    new_lrc << end_time << "\n";
}

std::string organizepath(std::string input) //对path路径进行修整,去掉开头的空格,去掉开头结尾的双引号
{
    std::size_t s = input.find_first_not_of(" \t\r\n");
    
    if(s != std::string::npos) input = input.substr(s,input.size() - s);

    if(input.front() == '"' && input.back() == '"') input = input.substr(1,input.size() - 2);

    return input;
}

std::string vttfile(std::string s) //生成vtt文件,返回路径
{
    std::cout << s<< " 开始\n";

    fs::path path =s;
    if(s.rfind(".wav.vtt") != std::string::npos || s.rfind(".mp3.vtt") != std::string::npos)
    {
        s = s.substr(0,s.size() - 8);
        fs::path new_path = s + ".vtt";
        fs::rename(path,new_path);
        return s;
    } 
    else if(s.rfind(".vtt") != std::string::npos || s.rfind(".vtt") != std::string::npos)
    {
        return s.substr(0,s.size() - 4);
    }
    
    return "-1";
} 

int main()
{
    SetConsoleOutputCP(CP_UTF8);  // 输出代码页设为 UTF-8
    SetConsoleCP(CP_UTF8);        // 输入代码页设为 UTF-8

    std::string input; //输入的地址
    std::string path; //修完后的地址

    bool file;//输入的是文件夹还是vtt文件,true为文件夹
    
    while(1)
    {
        std::cout << "请输入你要转换的文件绝对地址,或者文件夹绝对地址\n";
        std::cout << "如果完成请输入0\n";

        getline(std::cin,input);

        if(input == "0") break;
        else if(input.size() < 3) {
            std::cout << "输入地址过短\n";
            system("puase");system("cls");
            continue;
        }

        input = organizepath(input);

        fs::path p = input;

        if(fs::exists(p) && fs::is_directory(p)) file = true;
        else if(fs::exists(p) && fs::is_regular_file(p)) file = false;
        else{
            std::cout << input << " 输入的地址不存在\n";
            system("puase");system("cls");
            continue;
        } 

        if(file)
        {
            for (const auto& entry : fs::directory_iterator(input))
            {

                if(fs::is_directory(entry)) 
                {
                    std::cout << entry << " 是文件夹,不进行读取改变\n";
                    continue;
                }
                else if(fs::is_regular_file(entry))
                {
                    std::cout << entry << " 正在生成lrc\n";
                    path = vttfile(entry.path().string());
                    if(path == "-1") 
                    {
                        std::cout << input << " 该文件不为.vtt文件\n";
                        continue; 
                    }
                    vtt_to_lrc(path);
                }
            }

        }
        else
        {
            path = vttfile(input);
            if(path == "-1") 
            {
                std::cout << input << " 该文件不为.vtt文件\n";
                continue; 
            }
            vtt_to_lrc(path);
        } 
        
        std::cout << path <<" 转换完毕\n";
        system("pause");
        system("cls");
    }
    
    std::cout <<"感谢你的使用\n";

    system("pause");

    return 0;
}

