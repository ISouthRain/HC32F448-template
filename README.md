# 项目说明
[[小华半导体](https://www.xhsc.com.cn/index.aspx)]  
小华芯片 *HC32F448* 芯片系列  
库来源: HC32F448_DDL_Rev1.1.0  
该例程默认使用 HC32F448KCTI 芯片  
项目包含 HC32F448_DDL_Rev1.1.0 所有驱动  
user/* 目录源码文件为 swdt 例程
# 如何使用
## 使用环境
- 工作区编辑器  
  [Vscode](https://code.visualstudio.com/)
  - 打开 vscode 软件下载 扩展  
    [EIDE](https://github.com/github0null/eide/tree/master)  
    更多说明 [EIDE 文档](https://em-ide.com/docs/intro)
- 编译工具链(可选的, 除非你有 keil 或 iar)  
  默认使用 [GNU Arm tools](https://developer.arm.com/downloads/-/gnu-rm)  
- 调试扩展
  [Cotex-M debgug](https://em-ide.com/docs/advance/debug_project)

## 使用步骤
1. 下载本 仓库
2. 使用 Vscode 打开 HC32F448.code-workspace
3. 按键盘: F7 直接编译试试
4. enjoy :)

## 定制 EIDE 项目
1. 修改 芯片支持包 为你个芯片
2. 选择 构建配置: 选择你的编译工具链, 默认使用 GNU Arm(开源免费)
3. 修改 烧录配置: 选择你的 烧录器
4. 项目属性->预处理宏定义: 定义你自己的全局宏

# HC32F448 芯片常见问题
## 编译出来的 程序(bin)文件 前面部分 包含 无效数据
一般是和 IAP 程序搭配使用才考虑到这个  
解决方法: 将 LL_ICG_ENABLE 关闭, 如需该功能请详细阅读该功能  
#define LL_ICG_ENABLE                               (DDL_OFF)

# 如果该仓库对你有用, 请点击由上角 star

# ChangLog
## 2024-03-29
新版发行