# DBA Tools 脚本使用指南

本目录包含了DBA Tools项目的各种维护和构建脚本。

## 📁 脚本列表

### Qt客户端相关

#### `build-qt.ps1` - Qt应用编译脚本（PowerShell版本）
**功能**: 完整的Qt应用程序编译脚本，支持自动环境检测和多种配置选项

**用法**:
```powershell
.\build-qt.ps1 [参数]
```

**参数**:
- `-BuildType <类型>` - 构建类型 (Debug/Release，默认: Release)
- `-QtPath <路径>` - Qt安装路径 (可选，自动检测)
- `-Generator <生成器>` - CMake生成器 (默认: Visual Studio 17 2022)
- `-Architecture <架构>` - 目标架构 (默认: x64)
- `-Clean` - 清理构建目录
- `-Verbose` - 详细输出
- `-Help` - 显示帮助信息

**示例**:
```powershell
# 基本构建
.\build-qt.ps1

# Debug构建，详细输出
.\build-qt.ps1 -BuildType Debug -Verbose

# 指定Qt路径，清理构建
.\build-qt.ps1 -QtPath "C:\Qt\6.5.0\msvc2022_64" -Clean

# 查看帮助
.\build-qt.ps1 -Help
```

#### `build.bat` - Qt应用编译脚本（批处理版本）
**功能**: 调用PowerShell构建脚本的简化批处理文件

**用法**:
```cmd
scripts\build.bat [PowerShell参数]
```

#### `run-qt.ps1` - Qt应用启动脚本（PowerShell版本）
**功能**: 启动编译好的Qt应用程序，自动检测依赖和配置环境

**用法**:
```powershell
.\run-qt.ps1 [参数]
```

**参数**:
- `-BuildType <类型>` - 构建类型 (Debug/Release，默认: Release)
- `-QtPath <路径>` - Qt安装路径 (可选，自动检测)
- `-Verbose` - 详细输出
- `-Help` - 显示帮助信息

**示例**:
```powershell
# 启动Release版本
.\run-qt.ps1

# 启动Debug版本，详细输出
.\run-qt.ps1 -BuildType Debug -Verbose
```

#### `run.bat` - Qt应用启动脚本（批处理版本）
**功能**: 调用PowerShell启动脚本的简化批处理文件

**用法**:
```cmd
scripts\run.bat [PowerShell参数]
```

### 安装和配置相关

#### `install-qt.bat` - Qt6安装指南
**功能**: 显示Qt6安装指南和配置说明

**用法**:
```cmd
scripts\install-qt.bat
```

### API服务相关

#### `start-api.bat` - 启动FastAPI服务
**功能**: 启动后端API服务

**用法**:
```cmd
scripts\start-api.bat
```

#### `test-api.bat` - 测试API服务
**功能**: 测试API服务的基本功能

**用法**:
```cmd
scripts\test-api.bat
```

#### `start-docker.bat` - 启动Docker服务
**功能**: 使用Docker启动完整的服务栈

**用法**:
```cmd
scripts\start-docker.bat
```

## 🚀 快速开始

### 1. 首次构建

```cmd
# 1. 查看Qt安装指南
scripts\install-qt.bat

# 2. 安装Qt6后，构建客户端
scripts\build.bat

# 3. 启动客户端
scripts\run.bat

# 4. 启动API服务（可选）
scripts\start-api.bat
```

### 2. 开发模式

```powershell
# Debug构建
scripts\build-qt.ps1 -BuildType Debug -Verbose

# 启动Debug版本
scripts\run-qt.ps1 -BuildType Debug -Verbose
```

### 3. 清理重建

```powershell
# 清理并重新构建
scripts\build-qt.ps1 -Clean -Verbose
```

## 🔧 环境要求

### 必需工具
- **CMake** 3.16+
- **Visual Studio 2022** 或 Visual Studio Build Tools
- **Qt6** (6.5+推荐)
- **PowerShell** 5.1+ (Windows 10/11自带)

### 可选工具
- **Git** (版本控制)
- **Docker** (容器化部署)

## 📝 脚本特性

### PowerShell脚本优势
- ✅ 自动环境检测
- ✅ 详细的错误处理
- ✅ 丰富的参数选项
- ✅ 彩色输出和进度提示
- ✅ 自动Qt路径检测
- ✅ 依赖项验证

### 批处理脚本优势
- ✅ 简单易用
- ✅ 兼容性好
- ✅ 无需额外配置
- ✅ 快速启动

## 🐛 故障排除

### 常见问题

1. **CMake配置失败**
   - 检查Qt6是否正确安装
   - 确认Visual Studio 2022已安装
   - 尝试指定Qt路径: `-QtPath "C:\Qt\6.5.0\msvc2022_64"`

2. **构建失败**
   - 运行 `build-qt.ps1 -Clean -Verbose` 清理重建
   - 检查错误日志中的具体错误信息

3. **应用程序无法启动**
   - 检查Qt DLL是否在PATH中
   - 运行 `run-qt.ps1 -Verbose` 查看详细信息
   - 考虑使用windeployqt工具部署应用

4. **PowerShell执行策略错误**
   ```powershell
   Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
   ```

### 获取帮助

```powershell
# 查看构建脚本帮助
.\build-qt.ps1 -Help

# 查看启动脚本帮助
.\run-qt.ps1 -Help
```

## 📚 相关文档

- [BUILD_STATUS.md](../BUILD_STATUS.md) - 项目构建状态
- [README.md](../README.md) - 项目主文档
- [api/README.md](../api/README.md) - API服务文档