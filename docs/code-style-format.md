# 一、简述

在谷歌代码风格的基础上修改空格缩进为4，谷歌默认空格缩进为2

# 二、vscode 格式化配置

步骤如下：

1、安装clang-format 插件

2、ctrl + Shift + P 打开setting.json 添加如下配置启动clang-format

```
"editor.formatOnSave": true,
    "clang-format.executable": "C:/Users/温陵/.vscode/extensions/ms-vscode.cpptools-1.9.7/LLVM/bin/clang-format.exe",
    "[cpp]": {
        "editor.defaultFormatter": "xaver.clang-format"
    },
    "editor.detectIndentation": false,
    "editor.tabSize": 4
```

3、快捷键 Shift + Alt + F 格式化代码风格

# 自定义代码风格

工程目录中已添加配置文件.clang-format用于自定义代码风格（暂时使用谷歌的风格，后面按需修改）。