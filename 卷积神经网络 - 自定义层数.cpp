#include <iostream>
#include <ctime>
#include <cmath>
#include <vector>
#include <fstream>
#include <string>
using namespace std;

class AI {
private:
	//data
	string confingname;//
	vector<vector<double>>templ;//
	vector<vector<double>>files;//files[文件编号][序列]
	//weights [ 层数 ] [ 下一层序列 ] [ 对应本层序列 ]
	//文件层数由此定义
	vector<vector<vector<double>>>weights;
	vector<vector<vector<double>>>biases;//biases [ 层数 ] [ 下一层序列 ] [ 对应本层序列 ]
	//stream
	ifstream fin;
	ofstream fout;
	ofstream log;


	void _readAll(string fn) {
		templ[0].clear();

	}
	template <class ...T>
	void _build(int a,T...args) {

	}
	template <class T>
	void _build(int a, T args) {

	}
public:
	bool strict = false;
	bool fail = false;
	//从confing读取数据
	AI(string filename) {

	}
	// T 必须为int
	template <class ...T>
	AI(int a, T...args) {
		_build(a, args);
	}
	~AI() {

	}
};
int main()
{
	cout << "不要点击关闭，这将不会保存任何更改！";
	string cmd();
}