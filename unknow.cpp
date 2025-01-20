
#include<windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include<map>
#include<cmath>
using namespace std;

// ʹ�ö�̬�滮��������������еĳ���
int longestCommonSubsequence(const std::string& str1, const std::string& str2) {
    int m = str1.length();
    int n = str2.length();

    // ����һ����ά��̬�滮����dp��dp[i][j]��ʾstr1��ǰi���ַ���str2��ǰj���ַ�������������г���
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));

    // ���dp����
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (str1[i - 1] == str2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1; // �ַ�ƥ�䣬���ȼ�1
            }
            else {
                dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]); // �ַ���ƥ�䣬ȡǰһ��״̬�����ֵ
            }
        }
    }
    // ��������������еĳ���
    return dp[m][n];
}

// ʹ�ö�̬�滮��������������еĳ��Ⱥ�����
std::pair<int, std::string> longestCommonSubsequence2(const std::string& str1, const std::string& str2) {
    int m = str1.length();
    int n = str2.length();

    // ����һ����ά��̬�滮����dp��dp[i][j]��ʾstr1��ǰi���ַ���str2��ǰj���ַ�������������г���
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));

    // ���ڻ�������������еĶ�ά����parent��parent[i][j]�洢����LCS��ǰһ��λ����Ϣ
    std::vector<std::vector<std::pair<int, int>>> parent(m + 1, std::vector<std::pair<int, int>>(n + 1, { -1, -1 }));

    // ���dp�����parent����
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (str1[i - 1] == str2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
                parent[i][j] = { i - 1, j - 1 }; // ��ǰ�ַ�ƥ�䣬��¼ǰһ��λ��
            }
            else {
                if (dp[i - 1][j] > dp[i][j - 1]) {
                    dp[i][j] = dp[i - 1][j];
                    parent[i][j] = { i - 1, j }; // ��ƥ��ʱ��ȡ����״̬�����ֵ
                }
                else {
                    dp[i][j] = dp[i][j - 1];
                    parent[i][j] = { i, j - 1 }; // ����ȡ���״̬�����ֵ
                }
            }
        }
    }

    // ��dp[m][n]��ʼ���ݹ���LCS
    std::string lcs;
    int i = m, j = n;
    while (i > 0 && j > 0) {
        if (parent[i][j].first != -1 && parent[i][j].second != -1) {
            lcs.push_back(str1[parent[i][j].first]);
            i = parent[i][j].first;
            j = parent[i][j].second;
        }
        else if (parent[i][j].first != -1) {
            // �������ʵ���ϲ��ᷢ������Ϊparent�������ǻᱻ��ȷ���
            // ��Ϊ�˴���Ľ�׳�ԣ����ǻ��Ǳ��������֧����������Զ���ᱻִ�У�
            i = parent[i][j].first;
        }
        else {
            // ͬ�������֧Ҳ���ᷢ��
            j = parent[i][j].second;
        }
        // ע�⣺�����else if��else��֧ʵ�����ǲ���Ҫ�ģ���Ϊparent����������������Ч������
        // ���ﱣ������ֻ��Ϊ��չʾһ�ֿ��ܵĴ�����˼·����ʵ�ʴ�����Ӧ��ɾ����Щ��֧��
    }
    // ע�⣺���������ǴӺ���ǰ���ݵģ�����lcs�ַ�����Ҫ��ת
    std::reverse(lcs.begin(), lcs.end());

    return { dp[m][n], lcs };
}