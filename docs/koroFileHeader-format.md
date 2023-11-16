# 文件注释配置方法

## 1. 简介

### 1.1 目的

使用koroFileHeader插件统一管理注释格式，插件详细配置手册：https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE

### 1.2 适用范围

本规范适用于区块链研发团队C++语言类代码的代码编写标准。适用于代码编写阶段和代码维护阶段。

### **1.3 配置步骤**

1、安装插件，略

2、打开工作区settings.json，方法如下使用vscode快捷键 ctrl+shift+p ，然后选择“首选项：打开工作区设置（json）”

3、在settings.json添加如下配置：

```
"fileheader.customMade": {
        "custom_string_obkoro1": "© COPYRIGHT 2022 Corporation CAICT All rights reserved.\n http://www.caict.ac.cn\n https://bitfactory.cn\n \n Licensed under the Apache License, Version 2.0 (the \"License\");\n you may not use this file except in compliance with the License.\n You may obtain a copy of the License at\n \n http://www.apache.org/licenses/LICENSE-2.0\n \n Unless required by applicable law or agreed to in writing, software\n distributed under the License is distributed on an \"AS IS\" BASIS,\n WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n See the License for the specific language governing permissions and\n limitations under the License.\n ",
        "Author": "git config user.email",
        "Date": "Do not edit",
        "FilePath": "only file name"
    },
    "fileheader.configObj": {
        "autoAdd": true,
        "throttleTime": 6000,
        "specialOptions": {
            "Author": "author",
            "FilePath": "file",
            "Date": "date",
        }
        "createFileTime": true
       }
```

特殊说明：

配置中“Author”：“git config user.email”会自动获取git中配置的邮箱作为文件作者，**如果在vscode中没有配置git，可以直接填写邮箱地址，如“Athor”:"maxufeng@caict.ac.cn"**