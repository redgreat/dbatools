from pydantic import BaseModel, Field
from typing import Optional
from datetime import datetime

class RoleBase(BaseModel):
    """角色基础模式"""
    name: str = Field(..., min_length=2, max_length=50, description="角色名称")
    display_name: str = Field(..., min_length=2, max_length=100, description="角色显示名称")
    description: Optional[str] = Field(None, description="角色描述")

class RoleCreate(RoleBase):
    """角色创建模式"""
    pass

class RoleUpdate(BaseModel):
    """角色更新模式"""
    display_name: Optional[str] = Field(None, min_length=2, max_length=100, description="角色显示名称")
    description: Optional[str] = Field(None, description="角色描述")
    is_active: Optional[bool] = Field(None, description="是否激活")

class RoleResponse(RoleBase):
    """角色响应模式"""
    id: int
    is_active: bool
    created_at: datetime
    
    class Config:
        from_attributes = True