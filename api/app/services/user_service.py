from sqlalchemy.orm import Session
from sqlalchemy import and_
from typing import Optional, List
from app.models.user import User
from app.models.role import Role
from app.models.user_role import UserRole
from app.schemas.user import UserCreate, UserUpdate
from app.auth.password import get_password_hash
from datetime import datetime

class UserService:
    """用户服务类"""
    
    @staticmethod
    def create_user(db: Session, user_create: UserCreate) -> User:
        """创建用户"""
        # 检查用户名是否已存在
        if UserService.get_user_by_username(db, user_create.username):
            raise ValueError("用户名已存在")
        
        # 检查邮箱是否已存在
        if UserService.get_user_by_email(db, user_create.email):
            raise ValueError("邮箱已存在")
        
        # 创建用户
        hashed_password = get_password_hash(user_create.password)
        db_user = User(
            username=user_create.username,
            email=user_create.email,
            full_name=user_create.full_name,
            hashed_password=hashed_password
        )
        
        db.add(db_user)
        db.commit()
        db.refresh(db_user)
        
        # 自动分配查看者角色
        viewer_role = db.query(Role).filter(Role.name == "viewer").first()
        if viewer_role:
            UserService.assign_role_to_user(db, db_user.id, viewer_role.id)
        
        return db_user
    
    @staticmethod
    def get_user_by_id(db: Session, user_id: int) -> Optional[User]:
        """根据ID获取用户"""
        return db.query(User).filter(User.id == user_id).first()
    
    @staticmethod
    def get_user_by_username(db: Session, username: str) -> Optional[User]:
        """根据用户名获取用户"""
        return db.query(User).filter(User.username == username).first()
    
    @staticmethod
    def get_user_by_email(db: Session, email: str) -> Optional[User]:
        """根据邮箱获取用户"""
        return db.query(User).filter(User.email == email).first()
    
    @staticmethod
    def get_users(db: Session, skip: int = 0, limit: int = 100) -> List[User]:
        """获取用户列表"""
        return db.query(User).offset(skip).limit(limit).all()
    
    @staticmethod
    def update_user(db: Session, user_id: int, user_update: UserUpdate) -> Optional[User]:
        """更新用户信息"""
        db_user = UserService.get_user_by_id(db, user_id)
        if not db_user:
            return None
        
        update_data = user_update.dict(exclude_unset=True)
        for field, value in update_data.items():
            setattr(db_user, field, value)
        
        db.commit()
        db.refresh(db_user)
        return db_user
    
    @staticmethod
    def update_last_login(db: Session, user_id: int) -> None:
        """更新用户最后登录时间"""
        db_user = UserService.get_user_by_id(db, user_id)
        if db_user:
            db_user.last_login = datetime.utcnow()
            db.commit()
    
    @staticmethod
    def assign_role_to_user(db: Session, user_id: int, role_id: int, assigned_by: Optional[int] = None) -> UserRole:
        """为用户分配角色"""
        # 检查是否已经分配了该角色
        existing = db.query(UserRole).filter(
            and_(UserRole.user_id == user_id, UserRole.role_id == role_id)
        ).first()
        
        if existing:
            raise ValueError("用户已拥有该角色")
        
        user_role = UserRole(
            user_id=user_id,
            role_id=role_id,
            assigned_by=assigned_by
        )
        
        db.add(user_role)
        db.commit()
        db.refresh(user_role)
        return user_role
    
    @staticmethod
    def remove_role_from_user(db: Session, user_id: int, role_id: int) -> bool:
        """移除用户角色"""
        user_role = db.query(UserRole).filter(
            and_(UserRole.user_id == user_id, UserRole.role_id == role_id)
        ).first()
        
        if user_role:
            db.delete(user_role)
            db.commit()
            return True
        return False
    
    @staticmethod
    def get_user_roles(db: Session, user_id: int) -> List[Role]:
        """获取用户角色列表"""
        return db.query(Role).join(UserRole).filter(UserRole.user_id == user_id).all()