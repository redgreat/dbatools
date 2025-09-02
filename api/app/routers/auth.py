from fastapi import APIRouter, Depends, HTTPException, status
from sqlalchemy.orm import Session
from app.database import get_db
from app.schemas.user import UserCreate, UserLogin, UserResponse
from app.schemas.token import Token
from app.services.auth_service import AuthService
from app.services.user_service import UserService
from app.services.role_service import RoleService

router = APIRouter()

@router.post("/register", response_model=UserResponse, status_code=status.HTTP_201_CREATED)
async def register(user_create: UserCreate, db: Session = Depends(get_db)):
    """用户注册接口"""
    try:
        # 确保默认角色存在
        RoleService.init_default_roles(db)
        
        # 创建用户
        user = UserService.create_user(db, user_create)
        
        # 获取用户角色
        user_roles = UserService.get_user_roles(db, user.id)
        role_names = [role.name for role in user_roles]
        
        # 构建响应
        user_response = UserResponse(
            id=user.id,
            username=user.username,
            email=user.email,
            full_name=user.full_name,
            is_active=user.is_active,
            is_superuser=user.is_superuser,
            created_at=user.created_at,
            last_login=user.last_login,
            roles=role_names
        )
        
        return user_response
        
    except ValueError as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=str(e)
        )
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="注册失败，请稍后重试"
        )

@router.post("/login", response_model=Token)
async def login(user_login: UserLogin, db: Session = Depends(get_db)):
    """用户登录接口"""
    token = AuthService.login(db, user_login)
    if not token:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="用户名或密码错误",
            headers={"WWW-Authenticate": "Bearer"},
        )
    
    return token

@router.post("/logout")
async def logout():
    """用户登出接口"""
    # 由于使用JWT，登出主要在客户端处理（删除token）
    # 这里可以添加token黑名单逻辑（如果需要的话）
    return {"message": "登出成功"}