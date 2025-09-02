from fastapi import APIRouter, Depends, HTTPException, status, Query
from sqlalchemy.orm import Session
from typing import List
from app.database import get_db
from app.schemas.user import UserResponse, UserUpdate
from app.models.user import User
from app.services.user_service import UserService
from app.auth.jwt import get_current_active_user

router = APIRouter()

@router.get("/me", response_model=UserResponse)
async def get_current_user_info(
    current_user: User = Depends(get_current_active_user),
    db: Session = Depends(get_db)
):
    """获取当前用户信息"""
    user_roles = UserService.get_user_roles(db, current_user.id)
    role_names = [role.name for role in user_roles]
    
    return UserResponse(
        id=current_user.id,
        username=current_user.username,
        email=current_user.email,
        full_name=current_user.full_name,
        is_active=current_user.is_active,
        is_superuser=current_user.is_superuser,
        created_at=current_user.created_at,
        last_login=current_user.last_login,
        roles=role_names
    )

@router.get("/", response_model=List[UserResponse])
async def get_users(
    skip: int = Query(0, ge=0, description="跳过的记录数"),
    limit: int = Query(100, ge=1, le=1000, description="返回的记录数"),
    current_user: User = Depends(get_current_active_user),
    db: Session = Depends(get_db)
):
    """获取用户列表（需要管理员权限）"""
    # 检查权限
    user_roles = UserService.get_user_roles(db, current_user.id)
    role_names = [role.name for role in user_roles]
    
    if "admin" not in role_names and not current_user.is_superuser:
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="权限不足"
        )
    
    users = UserService.get_users(db, skip=skip, limit=limit)
    user_responses = []
    
    for user in users:
        user_roles = UserService.get_user_roles(db, user.id)
        role_names = [role.name for role in user_roles]
        
        user_responses.append(UserResponse(
            id=user.id,
            username=user.username,
            email=user.email,
            full_name=user.full_name,
            is_active=user.is_active,
            is_superuser=user.is_superuser,
            created_at=user.created_at,
            last_login=user.last_login,
            roles=role_names
        ))
    
    return user_responses

@router.get("/{user_id}", response_model=UserResponse)
async def get_user(
    user_id: int,
    current_user: User = Depends(get_current_active_user),
    db: Session = Depends(get_db)
):
    """获取指定用户信息"""
    # 用户只能查看自己的信息，除非是管理员
    user_roles = UserService.get_user_roles(db, current_user.id)
    role_names = [role.name for role in user_roles]
    
    if user_id != current_user.id and "admin" not in role_names and not current_user.is_superuser:
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="权限不足"
        )
    
    user = UserService.get_user_by_id(db, user_id)
    if not user:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="用户不存在"
        )
    
    user_roles = UserService.get_user_roles(db, user.id)
    role_names = [role.name for role in user_roles]
    
    return UserResponse(
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

@router.put("/{user_id}", response_model=UserResponse)
async def update_user(
    user_id: int,
    user_update: UserUpdate,
    current_user: User = Depends(get_current_active_user),
    db: Session = Depends(get_db)
):
    """更新用户信息"""
    # 用户只能更新自己的信息，除非是管理员
    user_roles = UserService.get_user_roles(db, current_user.id)
    role_names = [role.name for role in user_roles]
    
    if user_id != current_user.id and "admin" not in role_names and not current_user.is_superuser:
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="权限不足"
        )
    
    user = UserService.update_user(db, user_id, user_update)
    if not user:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="用户不存在"
        )
    
    user_roles = UserService.get_user_roles(db, user.id)
    role_names = [role.name for role in user_roles]
    
    return UserResponse(
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