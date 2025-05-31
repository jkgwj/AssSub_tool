#pragma once
#include<iostream>
#include<fstream>
#include <iomanip>
class AssSubTime
{
private:
	int T_h = 0, T_m = 0;double T_s = 0.10;//微调使用
	int hour, minute;
	double sec;
    void _add_hour(int H) {
		hour = hour + H+T_h;
		if (hour >= 60) {
			std::cout << "溢出" << std::endl;
		}
	}
	void _add_minute(int M) {
		minute = minute + M+T_m;
		if (minute >= 60) {
			minute = minute - 60;
			_add_hour(1);
		}
	}
	void _add_sec(double S) {
		sec = sec + S+T_s;
		if (sec >= 60.00) {
			sec = sec - 60.00;
			_add_minute(1);
		}
	}
public:
	AssSubTime() { hour = minute = 0; sec = 0.00; }
	AssSubTime(int H, int M, double S) {
		hour = H;
		minute = M;
		sec = S;
	}
	void set_time_hour(int);
	void set_time_minute(int);
	void set_time_sec(double);
	int get_hour();
	int get_minute();
	double get_sec();
	void show();
	AssSubTime operator+(const AssSubTime& T) {
		AssSubTime ret(this->hour, this->minute, this->sec);
		
		ret._add_sec(T.sec);
		ret.set_time_sec(ret.sec);
		
		ret._add_minute(T.minute);
		ret.set_time_minute(ret.minute);
		
		ret._add_hour(T.hour);
		ret.set_time_hour(ret.hour);
		return ret;

	}
	//AssSubTime operator-(const AssSubTime& T) {}


};
void AssSubTime::set_time_hour(int H) {
	hour = H;
}
void AssSubTime::set_time_minute(int M) {
	minute = M;
}
void AssSubTime::set_time_sec(double S) {
	sec = S;
}
 void AssSubTime::show() {
	std::cout << std::setw(1) << std::setfill('0') << hour
		      << ":"<<std::setw(2) << std::setfill('0')  << minute
		      << ":" <<std::fixed << std::setw(5) << std::setfill('0') << std::setprecision(2) << sec<<std::endl;
}
 int AssSubTime::get_hour() {
	 return hour;
 }
 int AssSubTime::get_minute() {
	 return minute;
 }
 double AssSubTime::get_sec() {
	 return sec;
 }