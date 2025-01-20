
#include<windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include<map>
#include<cmath>
using namespace std;

// 使用动态规划计算最长公共子序列的长度
int longestCommonSubsequence(const std::string& str1, const std::string& str2) {
    int m = str1.length();
    int n = str2.length();

    // 创建一个二维动态规划数组dp，dp[i][j]表示str1的前i个字符和str2的前j个字符的最长公共子序列长度
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));

    // 填充dp数组
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (str1[i - 1] == str2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1; // 字符匹配，长度加1
            }
            else {
                dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]); // 字符不匹配，取前一个状态的最大值
            }
        }
    }
    // 返回最长公共子序列的长度
    return dp[m][n];
}

// 使用动态规划计算最长公共子序列的长度和内容
std::pair<int, std::string> longestCommonSubsequence2(const std::string& str1, const std::string& str2) {
    int m = str1.length();
    int n = str2.length();

    // 创建一个二维动态规划数组dp，dp[i][j]表示str1的前i个字符和str2的前j个字符的最长公共子序列长度
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));

    // 用于回溯最长公共子序列的二维数组parent，parent[i][j]存储构建LCS的前一个位置信息
    std::vector<std::vector<std::pair<int, int>>> parent(m + 1, std::vector<std::pair<int, int>>(n + 1, { -1, -1 }));

    // 填充dp数组和parent数组
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (str1[i - 1] == str2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
                parent[i][j] = { i - 1, j - 1 }; // 当前字符匹配，记录前一个位置
            }
            else {
                if (dp[i - 1][j] > dp[i][j - 1]) {
                    dp[i][j] = dp[i - 1][j];
                    parent[i][j] = { i - 1, j }; // 不匹配时，取上面状态的最大值
                }
                else {
                    dp[i][j] = dp[i][j - 1];
                    parent[i][j] = { i, j - 1 }; // 或者取左边状态的最大值
                }
            }
        }
    }

    // 从dp[m][n]开始回溯构建LCS
    std::string lcs;
    int i = m, j = n;
    while (i > 0 && j > 0) {
        if (parent[i][j].first != -1 && parent[i][j].second != -1) {
            lcs.push_back(str1[parent[i][j].first]);
            i = parent[i][j].first;
            j = parent[i][j].second;
        }
        else if (parent[i][j].first != -1) {
            // 这种情况实际上不会发生，因为parent数组总是会被正确填充
            // 但为了代码的健壮性，我们还是保留这个分支（尽管它永远不会被执行）
            i = parent[i][j].first;
        }
        else {
            // 同理，这个分支也不会发生
            j = parent[i][j].second;
        }
        // 注意：上面的else if和else分支实际上是不必要的，因为parent数组总是有两个有效的索引
        // 这里保留它们只是为了展示一种可能的错误处理思路（在实际代码中应该删除这些分支）
    }
    // 注意：由于我们是从后往前回溯的，所以lcs字符串需要反转
    std::reverse(lcs.begin(), lcs.end());

    return { dp[m][n], lcs };
}