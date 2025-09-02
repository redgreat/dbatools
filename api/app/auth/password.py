from passlib.context import CryptContext
from app.config import settings

# 创建密码上下文
pwd_context = CryptContext(
    schemes=settings.pwd_context_schemes,
    deprecated=settings.pwd_context_deprecated
)

def verify_password(plain_password: str, hashed_password: str) -> bool:
    """验证密码"""
    return pwd_context.verify(plain_password, hashed_password)

def get_password_hash(password: str) -> str:
    """生成密码哈希"""
    return pwd_context.hash(password)