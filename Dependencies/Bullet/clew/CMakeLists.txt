Set(Previous_Bullet_Source ${Bullet_Source})

AddFile(Bullet_Source Previous_Bullet_Source clew.c)
AddFile(Bullet_Source Previous_Bullet_Source clew.h)

Set(Bullet_Source ${Previous_Bullet_Source} PARENT_SCOPE)
