# DBA Tools API

基于 FastAPI 的数据库管理工具后端 API 服务。

## 功能特性

- 用户注册和登录
- JWT 认证
- 角色管理（admin、operator、viewer）
- 密码加密存储
- 支持 PostgreSQL 和 MySQL 数据库
- 自动 API 文档生成

## 技术栈

- **框架**: FastAPI
- **数据库**: SQLAlchemy + PostgreSQL/MySQL
- **认证**: JWT (JSON Web Tokens)
- **密码加密**: bcrypt
- **数据验证**: Pydantic
- **数据库迁移**: Alembic

## 快速开始

### 1. 环境准备

确保已安装 Python 3.8+

### 2. 安装依赖

```bash
cd api
pip install -r requirements.txt
```

### 3. 环境配置

复制环境配置文件并修改：

```bash
cp .env.example .env
```

编辑 `.env` 文件，配置数据库连接等信息：

```env
# 数据库配置
DATABASE_URL=postgresql://username:password@localhost:5432/dbatools
# 或者使用 MySQL
# DATABASE_URL=mysql://username:password@localhost:3306/dbatools

# JWT 配置
SECRET_KEY=your-secret-key-here
ALGORITHM=HS256
ACCESS_TOKEN_EXPIRE_MINUTES=30

# 应用配置
APP_NAME=DBA Tools API
APP_VERSION=1.0.0
DEBUG=true

# 服务器配置
HOST=0.0.0.0
PORT=8000
```

### 4. 数据库初始化

启动应用时会自动创建数据库表和默认角色。

### 5. 启动服务

```bash
uvicorn main:app --host 0.0.0.0 --port 8000 --reload
```

服务启动后，可以访问：

- API 文档: http://localhost:8000/docs
- 替代文档: http://localhost:8000/redoc
- 健康检查: http://localhost:8000/health

## API 接口

### 认证接口

- `POST /auth/register` - 用户注册
- `POST /auth/login` - 用户登录
- `POST /auth/logout` - 用户登出

### 用户管理

- `GET /users/me` - 获取当前用户信息
- `GET /users/` - 获取用户列表（管理员）
- `GET /users/{user_id}` - 获取指定用户信息
- `PUT /users/{user_id}` - 更新用户信息

### 角色管理

- `GET /roles/` - 获取角色列表
- `GET /roles/{role_id}` - 获取指定角色信息
- `POST /roles/` - 创建角色（管理员）
- `PUT /roles/{role_id}` - 更新角色（管理员）
- `DELETE /roles/{role_id}` - 删除角色（管理员）
- `POST /roles/users/{user_id}/assign/{role_id}` - 分配角色（管理员）
- `DELETE /roles/users/{user_id}/remove/{role_id}` - 移除角色（管理员）

## 默认角色

系统会自动创建以下默认角色：

- **admin**: 管理员，拥有所有权限
- **operator**: 操作员，拥有数据库操作权限
- **viewer**: 查看者，只有查看权限

新注册用户默认分配 `viewer` 角色。

## 开发

### 项目结构

```
api/
├── app/
│   ├── auth/          # 认证相关
│   ├── database/      # 数据库配置
│   ├── models/        # 数据库模型
│   ├── routers/       # API 路由
│   ├── schemas/       # Pydantic 模型
│   ├── services/      # 业务逻辑
│   └── config.py      # 配置文件
├── main.py            # 应用入口
├── requirements.txt   # 依赖包
├── .env.example       # 环境配置示例
└── README.md          # 项目说明
```

### 数据库迁移

如果需要使用 Alembic 进行数据库迁移：

```bash
# 初始化迁移
alembic init alembic

# 创建迁移文件
alembic revision --autogenerate -m "Initial migration"

# 执行迁移
alembic upgrade head
```

## 部署

### Docker 部署

参考 `docker-compose.yml` 文件进行容器化部署。

### 生产环境

1. 设置环境变量 `DEBUG=false`
2. 使用强密码作为 `SECRET_KEY`
3. 配置反向代理（如 Nginx）
4. 使用进程管理器（如 Supervisor）

## 许可证

MIT License