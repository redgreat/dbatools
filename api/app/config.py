from pydantic_settings import BaseSettings
from typing import Optional
import os

class Settings(BaseSettings):
    """应用配置类"""
    
    # 应用基础配置
    app_name: str = "DBA Tools API"
    app_version: str = "1.0.0"
    debug: bool = False
    
    # 数据库配置
    database_url: str = "postgresql://postgres:password@localhost:5432/dbatools"
    
    # JWT配置
    secret_key: str = "your-secret-key-change-this-in-production"
    algorithm: str = "HS256"
    access_token_expire_minutes: int = 30
    
    # 服务器配置
    host: str = "0.0.0.0"
    port: int = 8000
    
    # 密码配置
    pwd_context_schemes: list = ["bcrypt"]
    pwd_context_deprecated: str = "auto"
    
    class Config:
        env_file = ".env"
        case_sensitive = False

# 创建全局配置实例
settings = Settings()