# Dicitonary Generartor

一个高效的混淆字典生成工具，用于生成由字符 'I' 和 'l' 组成的唯一字符串字典，可用于混淆测试、安全测试等场景。

## 功能特点

* 多线程高效生成，充分利用 CPU 资源
  
* 支持自定义生成数量
  
* 两种使用模式：命令行模式和交互模式
  
* 详细的生成统计信息
  
* 支持将结果保存到文件
  
* 优化的内存使用和生成算法
  
* 支持 GNU C++ 和 MSVC 编译器
  

## 编译安装

### 编译要求

* C++17 或更高版本的编译器
  
* 支持多线程的操作系统（Windows/Linux/macOS）
  

### 编译命令

使用 g++ 编译：

    
    g++ -O2 -std=c++17 generator.cpp -o generator.exe

在 Linux/macOS 上：

    
    g++ -O2 -std=c++17 generator.cpp -o generator -pthread

使用 MSVC 编译：

    
    cl /O2 /std:c++17 generator.cpp /Fe:generator.exe

## 使用方法

### 命令行模式

基本语法：

    
    generator -count <N> [-verbose] [-outfile <filename>]

选项说明：

* `-count <N>`: 要生成的字符串数量（必需）
  
* `-verbose`: 启用详细输出模式（可选）
  
* `-outfile <file>`: 输出文件名（默认：dictionary.txt）
  
* `-help`: 显示帮助信息
  

示例：

    
    # 生成10000个字符串并保存到mydict.txt
    generator -count 10000 -outfile mydict.txt
    
    # 生成50000个字符串并启用详细输出
    generator -count 50000 -verbose
    
    # 显示帮助信息
    generator -help

### 交互模式

直接运行程序不带任何参数即可进入交互模式：

    
    generator

交互模式会引导您完成以下步骤：

1. 输入要生成的字符串数量
  
2. 选择是否启用详细输出
  
3. 输入输出文件名（或使用默认值）
  

## 示例输出

### 详细模式输出示例

    
    ==========================================
     Obfuscation Dictionary Generator
     Made by wzhy233 (support@91net.top)
    ==========================================
    [System] Available threads: 8
    [Info] Generating 10000 unique strings...
    [Info] Minimum length required: 14
    [Info] Starting generation with 4 threads...
    [Info] Thread 0 started with prefix: IIII
    [Info] Thread 1 started with prefix: IIll
    [Info] Thread 2 started with prefix: IlIl
    [Info] Thread 3 started with prefix: Illl
    [Thread 0] Generated 10000 strings, elapsed: 123ms
    [Info] Generation completed in 125ms
    
    === Generation Summary ===
    Total strings generated: 10000
    Generation time: 125ms
    Performance: 80 strings/ms
    Min string length: 14
    Max string length: 14
    Average length: 14
    Sample strings: "IIIIIIIIIIIIII" "IIIIIIIIIIIIIl" "IIIIIIIIIIIIlI" "IIIIIIIIIIIIll" "IIIIIIIIIIIlII" 
    
    Saved 10000 strings to dictionary.txt in 15ms
    
    === Total Execution Time ===
    Total time: 140ms
    ==========================================

### 生成的字典内容示例

    
    IIIIIIIIIIIIII
    IIIIIIIIIIIIIl
    IIIIIIIIIIIIlI
    IIIIIIIIIIIIll
    IIIIIIIIIIIlII
    IIIIIIIIIIIlIl
    IIIIIIIIIIIllI
    IIIIIIIIIIIlll
    IIIIIIIIIlIIII
    IIIIIIIIIlIIIl
    ...

## 许可证

本项目采用 GNU General Public License v3.0 许可证，详情请查看[LICENSE](LICENSE)文件。

    
    GNU GENERAL PUBLIC LICENSE
                           Version 3, 29 June 2007
    
     Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>
     Everyone is permitted to copy and distribute verbatim copies
     of this license document, but changing it is not allowed.

## 作者信息

* 作者：wzhy233
  
* 邮箱：[support@91net.top](mailto:support@91net.top)
  
* 项目地址：[https://github.com/wzhy233/dictionary-generator/](https://github.com/wzhy233/dictionary-generator/)
  

## 性能说明

* 生成速度：约 80-120 个字符串 / 毫秒（取决于 CPU 性能）
  
* 内存使用：生成 100 万个字符串约占用 100MB 内存
  
* 线程数：自动根据 CPU 核心数调整，最多使用 8 个线程
  

## 注意事项

* 生成的字符串仅包含字符 'I'（大写 i）和 'l'（小写 L）
  
* 生成数量必须大于 0
  
* 生成的字符串长度会自动调整以确保唯一性
  
* 在高并发环境下，建议适当减少线程数以避免资源竞争
  
  > （注：文档部分内容可能由 AI 生成）
