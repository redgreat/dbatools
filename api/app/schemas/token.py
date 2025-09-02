from pydantic import BaseModel
from typing import Optional

class Token(BaseModel):
    """令牌响应模式"""
    access_token: str
    token_type: str = "bearer"
    expires_in: int
    user_id: int
    username: str

class TokenData(BaseModel):
    """令牌数据模式"""
    username: Optional[str] = None