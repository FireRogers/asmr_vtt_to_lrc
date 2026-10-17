#pragma once

#include <string>
#include <cstdio>

class lrcTime //把00:00:00.000转化成[00:00.00]的工具
{
    public:

    std::string ec_time = "-1"; //出错时返回

    std::string lrctime(std::string time) //把空格处理完后的时间string丢进来处理,返回[00:00.00]
    {
        long s_time = toTime(time);
    
        if(s_time == -1) return "-1";
    
        s_time = (s_time+5)/10 *10;
        time = finishString(s_time);
        return time;
    }

private:
    long toTime(std::string str) //把vtt的时间转化为总毫秒数
    {
        int h=0,m=0,s=0,ms=0;
        if(std::sscanf(str.c_str(),"%d:%d.%d",&m,&s,&ms) == 3 || 
        std::sscanf(str.c_str(),"%d:%d:%d.%d",&h,&m,&s,&ms) == 4 )
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
            for(int k = j - 1 ; k >0 ; k--)
            {
                p*=10;
            }
            s += (char)(a/p +'0');
            a %= p;
        }
    
        return s;
    }
    
    std::string finishString(long time) //最后转化时间为[00:00.00]
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

};
