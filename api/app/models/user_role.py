from sqlalchemy import Column, Integer, ForeignKey, DateTime, func
from sqlalchemy.orm import relationship
from .base import BaseModel

class UserRole(BaseModel):
    """用户角色关联模型"""
    __tablename__ = "user_roles"
    
    user_id = Column(Integer, ForeignKey("users.id", ondelete="CASCADE"), nullable=False, comment="用户ID")
    role_id = Column(Integer, ForeignKey("roles.id", ondelete="CASCADE"), nullable=False, comment="角色ID")
    assigned_at = Column(DateTime(timezone=True), server_default=func.now(), comment="分配时间")
    assigned_by = Column(Integer, ForeignKey("users.id"), comment="分配者ID")
    
    # 关联关系
    user = relationship("User", back_populates="roles", foreign_keys=[user_id])
    role = relationship("Role", back_populates="users")
    assigner = relationship("User", foreign_keys=[assigned_by])
    
    def __repr__(self):
        return f"<UserRole(user_id={self.user_id}, role_id={self.role_id})>"