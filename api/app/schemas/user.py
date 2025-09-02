from pydantic import BaseModel, EmailStr, Field
from typing import Optional, List
from datetime import datetime

class UserBase(BaseModel):
    """用户基础模式"""
    username: str = Field(..., min_length=3, max_length=50, description="用户名")
    email: EmailStr = Field(..., description="邮箱地址")
    full_name: Optional[str] = Field(None, max_length=100, description="全名")

class UserCreate(UserBase):
    """用户创建模式"""
    password: str = Field(..., min_length=6, max_length=100, description="密码")

class UserLogin(BaseModel):
    """用户登录模式"""
    username: str = Field(..., description="用户名")
    password: str = Field(..., description="密码")

class UserUpdate(BaseModel):
    """用户更新模式"""
    email: Optional[EmailStr] = Field(None, description="邮箱地址")
    full_name: Optional[str] = Field(None, max_length=100, description="全名")
    is_active: Optional[bool] = Field(None, description="是否激活")

class UserResponse(UserBase):
    """用户响应模式"""
    id: int
    is_active: bool
    is_superuser: bool
    created_at: datetime
    last_login: Optional[datetime] = None
    roles: List[str] = []
    
    class Config:
        from_attributes = True