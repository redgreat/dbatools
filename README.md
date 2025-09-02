# DBA Tools

一个基于Qt6开发的数据库管理员工具集合，提供各种实用的数据库管理和开发工具。

## 功能特性

- **用户认证**: 安全的登录系统，支持后台API认证
- **字符串格式化工具**: 支持多种格式的字符串格式化
  - JSON格式化
  - XML格式化
  - SQL格式化
  - HTML/CSS/JavaScript格式化
  - Base64编码/解码
  - URL编码/解码
- **模块化设计**: 易于扩展新的工具功能
- **现代化UI**: 基于Qt6的现代化用户界面

## 系统要求

- Windows 11
- Qt6 (Core, Widgets, Network)
- CMake 3.16+
- C++17编译器

## 构建说明

1. 确保已安装Qt6开发环境
2. 克隆项目到本地
3. 使用CMake构建项目:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## 配置说明

### 后台API服务器

应用程序需要连接到后台API服务器，默认地址为 `http://localhost:8080/api`

可以通过以下方式修改服务器地址:
1. 在登录界面点击"服务器设置"按钮
2. 在主界面菜单栏选择"文件" -> "设置"

### API接口规范

#### 登录接口
- **URL**: `POST /auth/login`
- **请求体**:
```json
{
  "username": "用户名",
  "password": "密码"
}
```
- **响应体**:
```json
{
  "success": true,
  "message": "登录成功",
  "token": "认证令牌"
}
```

#### 字符串格式化接口
- **URL**: `POST /tools/format-string`
- **请求头**: `Authorization: Bearer {token}`
- **请求体**:
```json
{
  "input": "要格式化的字符串",
  "type": "格式化类型(json/xml/sql等)"
}
```
- **响应体**:
```json
{
  "success": true,
  "result": "格式化后的结果",
  "error": "错误信息(如果有)"
}
```

## 项目结构

```
dbatools/
├── CMakeLists.txt          # CMake构建配置
├── README.md               # 项目说明文档
├── src/                    # 源代码目录
│   ├── main.cpp           # 应用程序入口
│   ├── loginwindow.h/cpp  # 登录窗口
│   ├── mainwindow.h/cpp   # 主窗口
│   ├── stringformatter.h/cpp # 字符串格式化工具
│   └── apimanager.h/cpp   # API管理器
└── ui/                     # UI文件目录(预留)
```

## 扩展开发

### 添加新工具

1. 创建新的工具类，继承自QWidget
2. 在MainWindow::initializeTools()中添加新工具页面
3. 如需要API支持，在ApiManager中添加相应方法

### 示例代码

```cpp
// 在mainwindow.cpp的initializeTools()方法中添加:
MyNewTool *newTool = new MyNewTool(m_apiManager, this);
m_tabWidget->addTab(newTool, "新工具");
```

## 许可证

本项目采用MIT许可证，详见LICENSE文件。

## 贡献

欢迎提交Issue和Pull Request来改进项目。
dba客户端工具
