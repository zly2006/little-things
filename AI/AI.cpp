#include<iostream>
#include<vector>
#include<string>
#include<fstream>
using namespace std;
//-files命令未完成，只能输入一个文件
using uint = unsigned int;
namespace std {
    class tastring :public string {
    public:
        tastring(const char* y) :string(y) {}
        tastring(char* y) :string(y) {}
        tastring(string y) :string(y) {}
        tastring() :string("") {}
        tastring operator*(int b) {
            tastring ta;
            for (int i = 0; i < b; i++) {
                ta += *this;
            }
            return ta;
        }
    };
    tastring operator*(int b, tastring a) {
        tastring ta;
        for (int i = 0; i < b; i++) {
            ta += a;
        }
        return ta;
    }
}
double ReLU(double x) {
    return x > 0 ? x : 0;
}
int main(int argc, char** argv) {
    double a;
    ifstream fin;
    ofstream fout;
    fstream fi;
    vector<double> result;//期待结果
    vector<vector<double>> file;//文件类矩阵
    vector<vector<vector<double>>> weights;//权重
    vector<double> biases;//偏移量
    vector<double> templist;
    if (argc == 1) {
        cout << "命令：[]为可选的,|为需要在若干项中选择的,<>为必填的\n格式：AI [-help|-h|-?] <-confing...> <-files...|-file...|-f...>"\
            "\n-confing <filename>\t\t\t计算所用配置"\
            "\n-help、-h或-?\t\t\t\t获取帮助，不输入任何参数也有同等效果，并将中断其他命令的执行"\
            "\n-file <filename>或-f <filename>\t\t使用AI输入文件"\
            "\n-files <filename1>[+filename2][+...]\t训练AI输入文件"\
            "\n" << endl;
        system("pause>nul");
        return 0;
    }
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (argv[i] == "-file") {
                fin.open(string(argv[++i]));
                templist.clear();
                while (fin) {
                    fin >> a;
                    if (fin.fail()) {
                        cout << "文件" + string(argv[i]) + "格式不正确！";
                        break; 
                    }
                    templist.push_back(a);
                }
                if (fin.fail())
                    fin.sync();
                else {
                    if (sqrt(templist.size()) != (int)sqrt(templist.size())) {
                        cout << "文件" + string(argv[i]) + "格式不正确！";
                        fin.close();
                        continue;
                    }
                    else {
                        int c = 0;
                        for (int i = 0; i < sqrt(templist.size()); i++) {
                            for (int j = 0; j < sqrt(templist.size()); j++) {
                                file[i][j] = templist[c];
                                ++c;
                            }
                        }
                    }
                    for (uint i = 0; i < weights.size(); i++) {
                        a = 0;
                        for (uint j = 0; j < file.size(); j++) {
                            for (uint k = 0; k < file.size(); k++) {
                                a += file[j][k] * weights[i][j][k];
                            }
                        }
                        a += biases[i];
                        a = ReLU(a);
                        cout << "result[" << i << "]=" << a;
                    }
                }
                fin.close();
            }
            else if (argv[i] == "-confing") {
                int c;
                fin.open(string(argv[++i]));
                templist.clear();
                if (fin)fin >> c;
                if (fin.fail()) {
                    fin.close();
                    cout << "文件" + string(argv[i]) + "格式不正确！";
                    continue;
                }
                while (fin) {
                    fin >> a;
                    if (fin.fail()) {
                        cout << "文件" + string(argv[i]) + "格式不正确！";
                        break;
                    }
                    templist.push_back(a);
                }
                if (fin.fail())
                    fin.sync();
                else {
                    if (sqrt(templist.size() / c - 1) != (int)sqrt(templist.size() / c - c)) {
                        cout << "文件" + string(argv[i]) + "格式不正确！";
                        fin.close();
                        continue;
                    }
                    else {
                        int b = 0;
                        for (int i = 0; i < c; i++) {
                            for (int j = 0; j < sqrt(templist.size() / c - 1); j++) {
                                for (int k = 0; k < sqrt(templist.size() / c - 1); k++) {
                                    weights[i][j][k] = templist[b++];
                                }
                            }
                        }
                        for (int i = 0; i < c; i++) {
                            biases[i] = templist[b++];
                        }
                    }
                }
                fin.close();
            }
            else if (argv[i] == "-files") {
                fin.open(string(argv[++i]));
                templist.clear();
                while (fin) {
                    fin >> a;
                    if (fin.fail()) {
                        cout << "文件" + string(argv[i]) + "格式不正确！";
                        break;
                    }
                    templist.push_back(a);
                }
                if (fin.fail()) {
                    fin.sync();
                    fin.close();
                }
                else {
                    if (sqrt(templist.size()) != (int)sqrt(templist.size())) {
                        cout << "文件" + string(argv[i]) + "格式不正确！";
                        fin.close();
                        continue;
                    }
                    else {
                        int c = 0;
                        for (int i = 0; i < sqrt(templist.size()); i++) {
                            for (int j = 0; j < sqrt(templist.size()); j++) {
                                file[i][j] = templist[c++];
                            }
                        }
                    }

                    double temp1, temp2;
                    fout.open("log", ios::app);
                    fout << "[learn]start\n[learn]s=";
                    temp1 = 0;
                    for (uint i = 0; i < weights.size(); i++) {
                        a = 0;
                        for (uint j = 0; j < file.size(); j++) {
                            for (uint k = 0; k < file.size(); k++) {
                                a += file[j][k] * weights[i][j][k];
                            }
                        }
                        a += biases[i];
                        a = ReLU(a);
                        temp1 += pow((a - result[i]), 2);
                    }
                    temp1 = sqrt(temp1);
                    fout << temp1 << endl;//标准差，倍数除以10
                    vector<vector<vector<double>>> _weight;
                    _weight = weights;
                    for (uint i = 0; i < weights.size(); i++) {
                        a = 0;
                        for (uint j = 0; j < file.size(); j++) {
                            for (uint k = 0; k < file.size(); k++) {
                                a += file[j][k] * weights[i][j][k];
                            }
                        }
                        a += biases[i];
                        a = ReLU(a);
                        fout << "差值" << i << ":" << (result[i] - a);
                        fout << "biases[" << i << "]+=差值*标准差/100=" << (result[i] - a) * temp1 / 100;
                        for (uint j = 0; j < file.size(); j++) {
                            for (uint k = 0; k < file.size(); k++) {
                                _weight[i][j][k] += temp2 =
                                    (result[i] - a) * temp1 * file[j][k] / 750;
                                fout << "weights[" << i << "][" << j << "][" << k << "]+=差值*标准差*"
                                    << file[j][k] << "/750=" << temp2 << endl;
                            }
                        }
                    }
                    temp2 = 0;
                    for (uint i = 0; i < weights.size(); i++) {
                        a = 0;
                        for (uint j = 0; j < file.size(); j++) {
                            for (uint k = 0; k < file.size(); k++) {
                                a += file[j][k] * weights[i][j][k];
                            }
                        }
                        a += biases[i];
                        a = ReLU(a);
                        temp2 += pow((a - result[i]), 2);
                    }
                    temp2 = sqrt(temp2);
                    fout << "当前标准差=" << temp2;
                    temp1 -= temp2;
                    fout << ",下降" << temp1 << endl << "[learn]succees:" << (temp1 > 0);
                    fout.close();
                    fin.close();
                    if (temp1 > 0) {
                        weights = _weight;
                        fout.open(argv[i]);
                        fout << weights.size();
                        for (uint i = 0; i < weights.size(); i++) {
                            for (uint j = 0; j < file.size(); j++) {
                                for (uint k = 0; k < file.size(); k++) {
                                    fout << weights[i][j][k] << " ";
                                }
                            }
                        }
                        for (uint i = 0; i < weights.size(); i++) {
                            fout << biases[i] << " ";
                        }
                    }
                }
            }
            else if (argv[i] == "-help") {
                cout << "帮助\n";
                cout << "命令：[]为可选的,|为需要在若干项中选择的,<>为必填的\n格式：AI [-help|-h|-?] <-confing...> <-files...|-file...|-f...>"\
                    "\n-confing <filename>\t\t\t计算所用配置"\
                    "\n-help、-h或-?\t\t\t\t获取帮助，不输入任何参数也有同等效果，并将中断其他命令的执行"\
                    "\n-file <filename>或-f <filename>\t\t使用AI输入文件"\
                    "\n-files <filename1>[+filename2][+...]\t训练AI输入文件"\
                    "\n" << endl;
                system("pause>nul");
                return 0;
            }
        }
    }

    cout << "按任意键结束程序";
    system("pause>nul");
    
    return 0;
}
