from fastapi import APIRouter, Depends, HTTPException, status, Query
from sqlalchemy.orm import Session
from typing import List
from app.database import get_db
from app.schemas.role import RoleResponse, RoleCreate, RoleUpdate
from app.models.user import User
from app.services.role_service import RoleService
from app.services.user_service import UserService
from app.auth.jwt import get_current_active_user

router = APIRouter()

@router.get("/", response_model=List[RoleResponse])
async def get_roles(
    skip: int = Query(0, ge=0, description="跳过的记录数"),
    limit: int = Query(100, ge=1, le=1000, description="返回的记录数"),
    current_user: User = Depends(get_current_active_user),
    db: Session = Depends(get_db)
):
    """获取角色列表"""
    roles = RoleService.get_roles(db, skip=skip, limit=limit)
    
    return [
        RoleResponse(
            id=role.id,
            name=role.name,
            display_name=role.display_name,
            description=role.description,
            is_active=role.is_active,
            created_at=role.created_at
        )
        for role in roles
    ]

@router.get("/{role_id}", response_model=RoleResponse)
async def get_role(
    role_id: int,
    current_user: User = Depends(get_current_active_user),
    db: Session = Depends(get_db)
):
    """获取指定角色信息"""
    role = RoleService.get_role_by_id(db, role_id)
    if not role:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="角色不存在"
        )
    
    return RoleResponse(
        id=role.id,
        name=role.name,
        display_name=role.display_name,
        description=role.description,
        is_active=role.is_active,
        created_at=role.created_at
    )

@router.post("/", response_model=RoleResponse, status_code=status.HTTP_201_CREATED)
async def create_role(
    role_create: RoleCreate,
    current_user: User = Depends(get_current_active_user),
    db: Session = Depends(get_db)
):
    """创建角色（需要管理员权限）"""
    # 检查权限
    user_roles = UserService.get_user_roles(db, current_user.id)
    role_names = [role.name for role in user_roles]
    
    if "admin" not in role_names and not current_user.is_superuser:
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="权限不足"
        )
    
    try:
        role = RoleService.create_role(db, role_create)
        
        return RoleResponse(
            id=role.id,
            name=role.name,
            display_name=role.display_name,
            description=role.description,
            is_active=role.is_active,
            created_at=role.created_at
        )
        
    except ValueError as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=str(e)
        )

@router.put("/{role_id}", response_model=RoleResponse)
async def update_role(
    role_id: int,
    role_update: RoleUpdate,
    current_user: User = Depends(get_current_active_user),
    db: Session = Depends(get_db)
):
    """更新角色信息（需要管理员权限）"""
    # 检查权限
    user_roles = UserService.get_user_roles(db, current_user.id)
    role_names = [role.name for role in user_roles]
    
    if "admin" not in role_names and not current_user.is_superuser:
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="权限不足"
        )
    
    role = RoleService.update_role(db, role_id, role_update)
    if not role:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="角色不存在"
        )
    
    return RoleResponse(
        id=role.id,
        name=role.name,
        display_name=role.display_name,
        description=role.description,
        is_active=role.is_active,
        created_at=role.created_at
    )

@router.delete("/{role_id}")
async def delete_role(
    role_id: int,
    current_user: User = Depends(get_current_active_user),
    db: Session = Depends(get_db)
):
    """删除角色（需要管理员权限）"""
    # 检查权限
    user_roles = UserService.get_user_roles(db, current_user.id)
    role_names = [role.name for role in user_roles]
    
    if "admin" not in role_names and not current_user.is_superuser:
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="权限不足"
        )
    
    success = RoleService.delete_role(db, role_id)
    if not success:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="角色不存在"
        )
    
    return {"message": "角色删除成功"}

@router.post("/users/{user_id}/assign/{role_id}")
async def assign_role_to_user(
    user_id: int,
    role_id: int,
    current_user: User = Depends(get_current_active_user),
    db: Session = Depends(get_db)
):
    """为用户分配角色（需要管理员权限）"""
    # 检查权限
    user_roles = UserService.get_user_roles(db, current_user.id)
    role_names = [role.name for role in user_roles]
    
    if "admin" not in role_names and not current_user.is_superuser:
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="权限不足"
        )
    
    success = UserService.assign_role_to_user(db, user_id, role_id, current_user.id)
    if not success:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="分配角色失败，请检查用户和角色是否存在"
        )
    
    return {"message": "角色分配成功"}

@router.delete("/users/{user_id}/remove/{role_id}")
async def remove_role_from_user(
    user_id: int,
    role_id: int,
    current_user: User = Depends(get_current_active_user),
    db: Session = Depends(get_db)
):
    """移除用户角色（需要管理员权限）"""
    # 检查权限
    user_roles = UserService.get_user_roles(db, current_user.id)
    role_names = [role.name for role in user_roles]
    
    if "admin" not in role_names and not current_user.is_superuser:
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="权限不足"
        )
    
    success = UserService.remove_role_from_user(db, user_id, role_id)
    if not success:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="移除角色失败，请检查用户和角色是否存在"
        )
    
    return {"message": "角色移除成功"}