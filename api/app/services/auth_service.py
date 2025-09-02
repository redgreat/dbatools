from sqlalchemy.orm import Session
from typing import Optional
from datetime import timedelta
from app.models.user import User
from app.schemas.user import UserLogin
from app.schemas.token import Token
from app.auth.password import verify_password
from app.auth.jwt import create_access_token
from app.services.user_service import UserService
from app.config import settings

class AuthService:
    """认证服务类"""
    
    @staticmethod
    def authenticate_user(db: Session, user_login: UserLogin) -> Optional[User]:
        """用户认证"""
        user = UserService.get_user_by_username(db, user_login.username)
        if not user:
            return None
        
        if not verify_password(user_login.password, user.hashed_password):
            return None
        
        if not user.is_active:
            return None
        
        return user
    
    @staticmethod
    def create_user_token(db: Session, user: User) -> Token:
        """为用户创建访问令牌"""
        access_token_expires = timedelta(minutes=settings.access_token_expire_minutes)
        access_token = create_access_token(
            data={"sub": user.username},
            expires_delta=access_token_expires
        )
        
        # 更新用户最后登录时间
        UserService.update_last_login(db, user.id)
        
        return Token(
            access_token=access_token,
            token_type="bearer",
            expires_in=settings.access_token_expire_minutes * 60,
            user_id=user.id,
            username=user.username
        )
    
    @staticmethod
    def login(db: Session, user_login: UserLogin) -> Optional[Token]:
        """用户登录"""
        user = AuthService.authenticate_user(db, user_login)
        if not user:
            return None
        
        return AuthService.create_user_token(db, user)