from sqlalchemy import Column, String, Text, Boolean
from sqlalchemy.orm import relationship
from .base import BaseModel

class Role(BaseModel):
    """角色模型"""
    __tablename__ = "roles"
    
    name = Column(String(50), unique=True, nullable=False, index=True, comment="角色名称")
    display_name = Column(String(100), nullable=False, comment="角色显示名称")
    description = Column(Text, comment="角色描述")
    is_active = Column(Boolean, default=True, comment="是否激活")
    
    # 关联用户
    users = relationship("UserRole", back_populates="role")
    
    def __repr__(self):
        return f"<Role(name='{self.name}', display_name='{self.display_name}')>"