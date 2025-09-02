-- 数据库初始化脚本
-- 这个文件会在PostgreSQL容器启动时自动执行

-- 创建数据库（如果不存在）
-- CREATE DATABASE IF NOT EXISTS dbatools;

-- 设置时区
SET timezone = 'Asia/Shanghai';

-- 创建扩展（如果需要）
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";
CREATE EXTENSION IF NOT EXISTS "pg_stat_statements";

-- 创建用户（如果需要额外的用户）
-- CREATE USER IF NOT EXISTS dbatools_readonly WITH PASSWORD 'readonly123';
-- GRANT CONNECT ON DATABASE dbatools TO dbatools_readonly;
-- GRANT USAGE ON SCHEMA public TO dbatools_readonly;
-- GRANT SELECT ON ALL TABLES IN SCHEMA public TO dbatools_readonly;
-- ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT SELECT ON TABLES TO dbatools_readonly;

-- 设置连接限制
-- ALTER DATABASE dbatools SET max_connections = 100;

-- 优化配置
-- ALTER DATABASE dbatools SET shared_preload_libraries = 'pg_stat_statements';
-- ALTER DATABASE dbatools SET log_statement = 'all';
-- ALTER DATABASE dbatools SET log_min_duration_statement = 1000;

COMMIT;