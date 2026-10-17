#pragma once

#include "lrcTime.h"

#include <iostream>
#include <filesystem>
#include <string>
#include <windows.h> 
#include <fstream>
#include <vector>
#include <QDebug>

namespace fs= std::filesystem;

const std::string photoEX = ".jpg.jpeg.png.gif.bmp.webp.JPG.JPEG.PNG.GIF.BMP.WEBP"; //常见图片后缀名

class orgenizeRj : private lrcTime //转化Rj下的vtt成lrc,把图片转移的工具
{ 
    public:
    
    std::string organizePath(std::string input) //对path路径进行修整,去掉开头的空格,去掉开头结尾的双引号
    {
        std::size_t s = input.find_first_not_of(" \t\r\n");
        
        if(s != std::string::npos) input = input.substr(s,input.size() - s);
    
        if(input.size() >= 2 && input.front() == '"' && input.back() == '"') input = input.substr(1,input.size() - 2);
    
        return input;
    }

    bool organizeRj(std::string input,std::string photo,int file) //主函数,第一个输入文件路径,第二个输入图片存放路径,第三个文件类型,1为只搞vtt,2是要搞图片
    {
        std::string path;
        if(file == 2 || file == 1) //需要创建一个文件夹,把图片文件统一存在那里,删除所有空文件夹
        {
            std::string photopos;//图片的储存位置

            if(photo.empty())
            {
                photopos = input;
            }
            else if(fs::path(photo).is_absolute())
            {
                photopos = photo;
            }
            else
            {
                if(photo.front() != '\\')
                {
                    photopos = input + '\\' + photo;
                }
                else
                {
                    photopos = input + photo;
                }
            }

            char id[7] = "000000";
            fs::create_directories(photopos);

            for (const auto& entry : fs::recursive_directory_iterator(input))
            {
                path = entry.path().string();
                qInfo() << QString::fromStdString(path + "\n");

                if(fs::is_directory(path)) continue;
                else if(!photo.empty() && path.find(photopos) != std::string::npos) continue;
                else if(fs::is_regular_file(path) && (entry.path().extension() == ".vtt" || entry.path().extension() == ".VTT"))
                {
                    if(!change_wav_vtt(path)){
                        qWarning() << QString::fromStdString(path + " wav转化vtt失败\n");
                        continue;
                    }
                    
                }
                else if(file != 1 && fs::is_regular_file(path) && photoEX.find(entry.path().extension().string()) != std::string ::npos)
                {
                    while(1)
                    {
                        for(int i = sizeof(id)-2; i >= 0;i--)
                        {
                            if(id[i] < '9')
                            {
                                id[i]++;
                                break;
                            }
                            id[i] = '0';
                        }

                        if(!fs::is_regular_file(fs::path(photopos) / (std::string(id) + entry.path().extension().string())))
                        {
                            fs::rename(path,fs::path(photopos) / (std::string(id) + entry.path().extension().string()));
                            break;
                        }
                    }
                }
            }

            std::vector<fs::path> v; //所有文件夹位置

            for (const auto& entry : fs::recursive_directory_iterator(input))
            {
                if(!fs::is_regular_file(entry.path()))
                {
                    v.push_back(entry.path());
                }
            }
            if(v.size() != 0)
            {
                for(std::size_t i = v.size(); i-- > 0;)
                {
                    if(fs::is_empty(v[i]))
                    {
                        fs::remove(v[i]);
                        qInfo() << QString::fromStdString(v[i].string() + " 空文件夹已被删除\n");
                    }
                }
            }

        }
        else
        {
            fs::path p = input;
            if(p.extension() != ".vtt")
            {
                qInfo() << QString::fromStdString(input + " 该文件不为.vtt格式\n");
                return 0;
            }
            if(!change_wav_vtt(input))
            {
                qWarning() << " wav转vtt失败";
                return 0;
            }
        } 
        return 1;
    }
    
    private:
    
    bool change_wav_vtt(std::string input)
    {
        std::string path = vttfile(input);
        qInfo() << QString::fromStdString(input + " 正在生成lrc\n");
        if(vtt_to_lrc(path.substr(0,path.size()-4)))
        {
            return 1;
        }
        return 0;
    }

    bool vtt_to_lrc(std::string path) //转化path路径(不包含后缀名)的vtt文件为lrc
    {
        qInfo() << QString::fromStdString(path + ".vtt" + "\n");
        
        std::ifstream old_vtt(fs::path(path + ".vtt"));
    
        if (!old_vtt) {
        std::cerr << "打不开文件: " << fs::absolute(path + ".vtt") << "\n";
        return 0;
        }
    
        std::ofstream new_lrc(fs::path(path + ".lrc"));
    
        std::string line;
    
        lrcTime lrctime;
        bool first = true;
    
        std::string name;
        name = fs::path(path).filename().string();
        new_lrc << "[ti:" << name << "]\n";
    
        std::string start_time,end_time;//一段开始时间
    
        while(std::getline(old_vtt , line))
        {
            qInfo() << QString::fromStdString(line + "\n");
            std::string text;//台词文本
    
            std::size_t pos = line.rfind("-->");
    
            if(pos == std::string::npos) continue;
    
            start_time = line.substr(0,pos);//把箭头前的时间截出来
            if(start_time.empty())
            {
                qCritical() << "开始时间格式为空";
                return 0;
            }
            while(start_time.back() == ' ') start_time.pop_back(); //去除空格
            start_time = lrctime.lrctime(start_time);
            if(start_time == lrctime.ec_time)
            {
                qCritical() << "开始时间格式错误";
                return 0;
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
            qInfo() << QString::fromStdString(start_time + text + '\n');
            first = false;
    
            end_time = line.substr(pos + 3,line.size() -pos -3);
            if(end_time.empty())
            {
                qCritical() <<"结束时间格式为空";

                return 0;
            }
            while(end_time.front() == ' ') end_time.erase(0,1);
            end_time = lrctime.lrctime(end_time);
            if(end_time == lrctime.ec_time)
            {
                qCritical() <<"某处的结束时间格式错误";
                return 0;
            }
        }
    
        new_lrc << end_time << "\n";
        return 1;
    }

    std::string vttfile(std::string s) //生成vtt文件,返回路径
    {
        qInfo() << QString::fromStdString( s + " 开始\n");
    
        fs::path path = s;
        if(s.size() >= 9 && (s.rfind(".wav.vtt") == s.size() - 8 || s.rfind(".mp3.vtt") == s.size() - 8))
        {
            s = s.substr(0,s.size() - 8);
            fs::path new_path = s + ".vtt";
            fs::rename(path,new_path);
            return new_path.string();
        } 
        return s;
    } 
};