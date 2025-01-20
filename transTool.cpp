#include<windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include<map>
#include<cstdlib>
#include <sstream>
#include <unordered_set>
#include <algorithm>
#include <cctype>    
#include <commdlg.h>
#include<tchar.h>

using namespace std;
enum MyEnum
{
    TransLateHead,//翻译段落的头
    Eng,//英文原文
    SpaceLine,//完全的空行
    Wait, //等待翻译的
    Chinese,//已经翻译好的
    Unknow
};
struct textinfo
{
    wstring content;//整行的内容
    wstring sub;//提炼出来的内容
    MyEnum type = Unknow;
};

map<int, textinfo>Alline;//原文的所有行
//匹配待填翻译的行
bool isSpaceLine(const std::wstring& line) {
    wstring l = line;
    l.erase(l.begin(), std::find_if(l.begin(), l.end(), [](int ch) {
        return !std::isspace(ch);
        }));
    if (l == L"\"\"")
        return true;
    // 查找空格的位置
    size_t spacePos = l.find(' ');
    if (spacePos != std::string::npos) {
        // 查找空格后紧跟两个引号的位置
        size_t firstQuotePos = l.find('"', spacePos + 1);
        if (firstQuotePos != std::string::npos && firstQuotePos + 1 < l.size() && l[firstQuotePos + 1] == '"')
            return true;// 找到了空格后紧跟两个引号的模式
    }
    return false;
}
void read(std::wifstream& inputFile) {
    int cnt = 1;
    std::wstring l;
    while (std::getline(inputFile, l)) {
        textinfo t;
        t.content = l;
        t.type = Unknow;
        Alline[cnt++] = t;
    }
    for (auto it = Alline.begin();it != Alline.end();it++) {
        if (it->first == 19789)
            int z = 0;
        wstring& s = it->second.content;
        if (s[0] == 't' && s.find(L"translate") != string::npos &&
            s.find(L"chinese") != string::npos) {
            it->second.type = TransLateHead;
            //下面第二行一定是英文原本
            Alline[it->first + 2].type = Eng;
            wstring& engStr = Alline[it->first + 2].content;
            // 查找双引号位置提取对话内容
            size_t firstQuotePos = engStr.find('"');
            if (firstQuotePos != std::string::npos) {
                size_t lastQuotePos = engStr.rfind('"');
                if (lastQuotePos != std::string::npos && lastQuotePos > firstQuotePos) {
                    std::wstring quotedContent = engStr.substr(firstQuotePos + 1, lastQuotePos - firstQuotePos - 1);
                    Alline[it->first + 2].sub = quotedContent;
                }
            }
            //那么再下一行一定是翻译文本,检查是空的还是已经翻译好的
            if (isSpaceLine(Alline[it->first + 3].content))
                Alline[it->first + 3].type = Wait;
            else
                Alline[it->first + 3].type = Chinese;
        }
    }
}
// 将字符串转换为小写
std::wstring toLowerCase(const std::wstring& str) {
    std::wstring lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return lowerStr;
}
// 获取句子中的所有单词（不区分大小写）
std::unordered_set<std::wstring> getWords(const std::wstring& sentence) {
    std::unordered_set<std::wstring> words;
    std::wistringstream stream(sentence);
    std::wstring word;
    while (stream >> word) {
        words.insert(toLowerCase(word)); // 插入小写形式的单词以进行不区分大小写的比较
    }
    return words;
}
// 计算两个句子中相同单词的数量
int countCommonWords(const std::wstring& a, const std::wstring& b) {
    std::unordered_set<std::wstring> wordsA = getWords(a);
    std::unordered_set<std::wstring> wordsB = getWords(b);
    int commonWordCount = 0;
    for (const auto& word : wordsA) {
        if (wordsB.find(word) != wordsB.end()) {
            ++commonWordCount;
        }
    }
    return commonWordCount;
}
int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    OPENFILENAME ofn;       // common dialog box structure
    wchar_t szFile[MAX_PATH]; // buffer for file name, MAX_PATH is defined in Windows headers
    HWND hwnd = NULL;       // owner window (NULL for no owner)
    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = L'\0';
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
    ofn.lpstrFilter = L"All Files\0*.*\0Text Files\0*.txt\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    // Display the Open dialog box.
    GetOpenFileName(&ofn);
    std::wifstream inputFile(szFile, std::ios::out | std::ios::binary); // 输入文件名
    if (!inputFile.is_open()) {
        std::cerr << "无法打开输入文件" << std::endl;
        return 1;
    }
    Alline.clear();
    read(inputFile);
    int beginLine = 1;
    std::cout << "查找ToDO标记...." << endl;
    for (int i = 1;i < Alline.size() + 1;i++) {
        wstring& str = Alline[i].content;
        if (str.find(L"# TODO: Translation updated at") != string::npos) {
            beginLine = i+1;
            break;
        }
    }
    std::cout << "开始替换...." << endl;
    std::wofstream fileXiugai("xiugai.rpy");
    for (int i = beginLine;i < Alline.size() + 1;i++) {
        wstring& str = Alline[i].content;
        if (Alline[i].type != Wait)
            continue;
        //读取上一行的英文原文
        wstring& oriLine = Alline[i - 1].sub;
        //尝试找到前面就出现过的最相似的英文
        int size0 = oriLine.size();
        int matchLine = -1;//最后匹配结果在第几行
        int maxCcw = 0;//最大单词相同数目
        int disLen = 99999;//最大长度差距
        //查找已经出现过的最相似的英文原文
        for (int j = 1;j < Alline.size() + 1 && j != i-1; j++) {
            if (Alline[j].type != Eng)
                continue;
            wstring& cpstr = Alline[j].sub;
            int ccw = countCommonWords(cpstr, oriLine);
            if (ccw == 0 || ccw < maxCcw)
                continue;
            int curdisLen = oriLine.size() - cpstr.size();
            if (ccw == maxCcw) {
                if (curdisLen > disLen)
                    continue;
                disLen = curdisLen;
            }
            maxCcw = ccw;
            matchLine = j;
        }
        if (matchLine > Alline.size())
            continue;
        else if(Alline[matchLine + 1].type != Chinese)
            continue;
        std::cout << i << "/" << Alline.size() << endl;
        wcout << "eng: " << Alline[i - 1].content;
        wcout << Alline[i].content << "------" << Alline[matchLine + 1].content << endl;
        Alline[i].content = Alline[matchLine + 1].content;
       fileXiugai << i-1 << " eng:";
       fileXiugai << Alline[i - 1].content;
       fileXiugai << "from " << matchLine << " line:" << Alline[matchLine].content << "\n";
       fileXiugai << i << "--change: " << Alline[i].content << "\n\n";
       fileXiugai << "\n";
    }
    std::wofstream outFile("out.rpy");
    for (int i = 1;i < Alline.size() + 1;i++)
        outFile << Alline[i].content << "\n";
    outFile.close();
    fileXiugai.close();
    inputFile.close();
    std::cout << "执行完成." << endl;
    return 0;
}