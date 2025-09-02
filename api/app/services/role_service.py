from sqlalchemy.orm import Session
from typing import Optional, List
from app.models.role import Role
from app.schemas.role import RoleCreate, RoleUpdate

class RoleService:
    """角色服务类"""
    
    @staticmethod
    def create_role(db: Session, role_create: RoleCreate) -> Role:
        """创建角色"""
        # 检查角色名是否已存在
        if RoleService.get_role_by_name(db, role_create.name):
            raise ValueError("角色名已存在")
        
        db_role = Role(
            name=role_create.name,
            display_name=role_create.display_name,
            description=role_create.description
        )
        
        db.add(db_role)
        db.commit()
        db.refresh(db_role)
        return db_role
    
    @staticmethod
    def get_role_by_id(db: Session, role_id: int) -> Optional[Role]:
        """根据ID获取角色"""
        return db.query(Role).filter(Role.id == role_id).first()
    
    @staticmethod
    def get_role_by_name(db: Session, name: str) -> Optional[Role]:
        """根据名称获取角色"""
        return db.query(Role).filter(Role.name == name).first()
    
    @staticmethod
    def get_roles(db: Session, skip: int = 0, limit: int = 100, active_only: bool = True) -> List[Role]:
        """获取角色列表"""
        query = db.query(Role)
        if active_only:
            query = query.filter(Role.is_active == True)
        return query.offset(skip).limit(limit).all()
    
    @staticmethod
    def update_role(db: Session, role_id: int, role_update: RoleUpdate) -> Optional[Role]:
        """更新角色信息"""
        db_role = RoleService.get_role_by_id(db, role_id)
        if not db_role:
            return None
        
        update_data = role_update.dict(exclude_unset=True)
        for field, value in update_data.items():
            setattr(db_role, field, value)
        
        db.commit()
        db.refresh(db_role)
        return db_role
    
    @staticmethod
    def delete_role(db: Session, role_id: int) -> bool:
        """删除角色"""
        db_role = RoleService.get_role_by_id(db, role_id)
        if db_role:
            db.delete(db_role)
            db.commit()
            return True
        return False
    
    @staticmethod
    def init_default_roles(db: Session) -> None:
        """初始化默认角色"""
        default_roles = [
            {"name": "admin", "display_name": "管理员", "description": "系统管理员，拥有所有权限"},
            {"name": "operator", "display_name": "操作员", "description": "数据库操作员，可以执行数据库操作"},
            {"name": "viewer", "display_name": "查看者", "description": "只读用户，只能查看数据"}
        ]
        
        for role_data in default_roles:
            existing_role = RoleService.get_role_by_name(db, role_data["name"])
            if not existing_role:
                role_create = RoleCreate(**role_data)
                RoleService.create_role(db, role_create)