//在知乎提问
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

	string confingname;
	vector<vector<double>>templ;
	vector<vector<double>>files;//files [ 文件编号 ] [ 序列 ]
	//weights [ 层数 ] [ 下一层序列 ] [ 对应本层序列 ]
	//文件层数由此定义
	vector<vector<vector<double>>>weights;
	vector<vector<double>>biases;//biases [ 层数 ] [ 下一层序列 ]
	vector<double>result;

	//stream

	ifstream fin;
	ofstream log;

#define TODO log<<"调用TODO代码，开启strict，设置fail=true"<<endl;\
strict=fail=1;
#define EXIT if(strict&&fail)return;
	void _readAll(string fn) {
		log << "readAll start:" << fn << endl;
		templ[0].clear();
		char temp[8];
		fin.open(fn, ios::binary);
		fin.seekg(ios::beg);
		while (!fin.eof()) {
			fin.read(temp, 8);
			templ[0].push_back(*(double*)temp);
		}
		fin.close();
		log << "readAll finish" << endl;
	}
public:
	double delta = 1;
	//严格模式：长度检查，及时中断，防止对已有内容发生破坏
	//但会显著降低效率，若已对内容进行封装，请设为false
	bool strict = true;
	bool fail = false;
	//从confing读取数据
	AI(string filename) {
		templ.push_back(vector<double>((0)));
		log.open("AI.log");
		log << endl << endl << "AI,start confing= " << filename << " ,初始化templ" << endl;
		_readAll(filename);
		confingname = filename;
		weights.clear();
		biases.clear();
		int pos = (int)templ[0][0] + 2;
		if (strict) {
			int a = 0;
			for (int i = 0; i <= templ[0][0]; i++) {
				a += templ[0][i + 1];
			}
			if (a != templ.size() - (templ[0][0] + 2)) {
				log << "Error at load confing:长度理论值= " << a << " 实际值= " << templ.size() - (templ[0][0] + 2) << " ,templ[0][0]= " << templ[0][0] << endl;
				fail = true;
			}
		}
		for (int i = 0; i < templ[0][0]; i++) {
			biases.push_back(vector<double>(0));
			weights.push_back(vector<vector<double>>(0));
			for (int j = 0; j < templ[0][i + 2]; j++) {
				weights[i].push_back(vector<double>(0));
				for (int k = 0; k < templ[0][i + 1]; k++) {
					weights[i][j].push_back(templ[0][pos++]);
				}
				biases[i].push_back(templ[0][pos++]);
			}
		}
		log << "配置已读取" << endl;
	}
	//使用给定的每层个数创建confing
	AI(vector<int> count, string confing) :confingname(confing) {
		log.open("AI.log");
		templ.push_back(vector<double>(0));
		log << endl << endl << "AI,start confing= " << confing << " ,初始化templ" << endl;
		log << "配置：" << count.size() << endl;
		log << count[0] << endl;
		srand(clock() | time(0));
		weights.clear();
		biases.clear();
		for (int i = 0; i < count.size() - 1; i++) {
			weights.push_back(vector<vector<double>>(0));
			biases.push_back(vector<double>(0));
			log << count[i + 1] << endl;
			for (int j = 0; j < count[i + 1]; j++) {
				weights[i].push_back(vector<double>(0));
				for (int k = 0; k < count[i]; k++) {
					weights[i][j].push_back((double)(rand() % 100 + 1) / 100);
				}
				biases[i].push_back((double)(rand() % 201 - 100) / 100);
			}
		}
	}
	~AI() {
		ofstream fout(confingname);
		double temp;
		temp = weights.size();
		fout.write((char*)&temp, 8);
		temp = weights[0][0].size();
		fout.write((char*)&temp, 8);
		for (int i = 0; i < weights.size(); i++) {
			temp = weights[i].size();
			fout.write((char*)&temp, 8);
		}
		for (int i = 0; i < weights.size(); i++) {
			for (int j = 0; j < weights[i].size(); j++) {
				for (int k = 0; k < weights[i][j].size(); k++) {
					temp = weights[i][j][k];
					fout.write((char*)&temp, 8);
				}
				temp = biases[i][j];
				fout.write((char*)&temp, 8);
			}
		}
		fout.close();
		log << "析构结束，confing保存" << endl;
	}
	vector<double> compute(string filename) {
		log << "compute start:" << filename << endl;
		_readAll(filename);
		if (strict && templ[0].size() != weights[0][0].size()) {
			log << "Error at compute:file size=" << templ.size() << ",but confing size=" << weights[0][0].size() << endl;
			return vector<double>(0);
		}
		for (int i = 0; i < weights.size(); i++) {
			for (int j = 0; j < weights[i].size(); j++) {
				for (int k = 0; k < weights[i][j].size(); k++) {
					templ[1][j] = templ[0][k] * weights[i][j][k];
				}
				templ[1][j] -= biases[i][j];
			}
			templ[0] = templ[1];
		}
		log << "compute finish" << endl;
		return templ[0];
	}
	void train(string filename) {
		log << "train start:" << filename << endl;
		log << "test:call compute from train" << endl;
		compute(filename);
		templ[1] = templ[0];
		log << "move result" << endl;
		_readAll(filename);
		if (strict) {
			log << "strict:检测";
			if (templ[0].size() != weights[0][0].size() + templ[1].size()) {
				log << "Error at train:文件数据大小= " << templ[0].size() << " ;需要 =" << weights[0][0].size() + templ[1].size() << endl;
				fail = true;
				return;
			}
		}
		//如果训练效果不佳，复位并调节delta
		vector<vector<vector<double>>>_weights(weights);
		vector<vector<double>>_biases(biases);
		for (int i = 0; i < templ[1].size(); i++) {
			result[i] = templ[0][templ[0].size() - templ[1].size() + i];
		}
		cout << "目标：\n";
		for (int i = 0; i < templ[1].size(); i++) {
			cout << "[" << i << "]=" << result[i] << endl;
			log << "result[" << i << "]=" << result[i] << endl;
		}
		cout << "计算结果：\n";
		for (int i = 0; i < templ[1].size(); i++) {
			cout << "[" << i << "]=" << templ[1][i] << endl;
			log << "templ[1]:compute[" << i << "]=" << templ[1][i] << endl;
		}
		//TODO
		for (int i = 0; i < templ[1].size(); i++) {

		}
		log << "train finish" << endl;
	}
	//TODO
	//所有文件->一组文件->某个文件
	void traingroup(vector<vector<string>> filenames) {
		TODO
	}
#undef TODO
#undef EXIT
};
int main()
{
	cout << "不要点击关闭，这将不会保存任何更改！" << endl;

	AI ai({1000000,10000,100,10},"confing");
}
