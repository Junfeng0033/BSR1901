import pygame
import random
import math

# 初始化 pygame
pygame.init()

# 窗口设置
WIDTH, HEIGHT = 240, 240
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("240x240 Charging Animation")
clock = pygame.time.Clock()
FPS = 60

# 颜色定义
BG_COLOR = (10, 15, 30)
WATER_COLOR = (50, 220, 100)  # 绿色
WATER_LIGHT = (180, 255, 180) # 淡绿色高光
CHARGE_TEXT_COLOR = (255, 210, 0)
WHITE = (255, 255, 255)

# 电池区域参数 (适配 240x240)
batt_w = 120
batt_h = 160
batt_x = (WIDTH - batt_w) // 2
batt_y = 50
batt_top = batt_y
batt_bottom = batt_y + batt_h

# 全局电量
battery_level = 0.10  # 确保从 10% 开始
charge_speed = 0.0001 # 大幅减慢充电速度，以便观察上涨过程

# 水滴类
class WaterDrop:
    def __init__(self):
        self.x = random.randint(batt_x + 20, batt_x + batt_w - 20)
        self.y = random.randint(-30, 20)
        self.radius = random.uniform(4, 8)
        self.speed = random.uniform(2.5, 4.5)
        self.alpha = 220

    def update(self):
        self.y += self.speed
        # 落到电池液面就消失
        liquid_y = batt_bottom - battery_level * batt_h
        if self.y >= liquid_y - self.radius:
            return False
        return True

    def draw(self, surf):
        # 渐变水滴
        r = int(self.radius)
        pygame.draw.circle(surf, WATER_COLOR, (int(self.x), int(self.y)), r)
        pygame.draw.circle(surf, WATER_LIGHT, (int(self.x - r//3), int(self.y - r//3)), r//2)

# 快充闪光文字动画参数
flash_timer = 0
flash_alpha = 255
flash_dir = -8

# 水滴列表
drops = []
drop_spawn_timer = 0

running = True
while running:
    dt = clock.tick(FPS) / 1000.0
    screen.fill(BG_COLOR)

    # 事件监听
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    # 1. 生成水滴
    drop_spawn_timer += dt
    if drop_spawn_timer > 0.08:
        drops.append(WaterDrop())
        drop_spawn_timer = 0

    # 2. 更新 & 绘制水滴
    new_drops = []
    for d in drops:
        if d.update():
            new_drops.append(d)
            d.draw(screen)
    drops = new_drops

    # 3. 电量上涨
    if battery_level < 1.0:
        battery_level += charge_speed
    else:
        battery_level = 1.0

    # 4. 绘制电池外框
    pygame.draw.rect(screen, (80, 90, 110), (batt_x, batt_y, batt_w, batt_h), 4)
    # 电池顶部小凸起
    pygame.draw.rect(screen, (80, 90, 110), (WIDTH // 2 - 15, batt_y - 10, 30, 10))

    # 5. 绘制电池液体（汇聚后的电量）
    liquid_h = battery_level * batt_h
    liquid_rect = (batt_x + 3, batt_bottom - liquid_h, batt_w - 6, liquid_h)
    pygame.draw.rect(screen, WATER_COLOR, liquid_rect)
    # 液面高光
    line_y = batt_bottom - liquid_h
    pygame.draw.line(screen, WATER_LIGHT, (batt_x+5, line_y), (batt_x+batt_w-5, line_y), 3)

    # 6. 绘制电量百分比
    percent = int(battery_level * 100)
    font_small = pygame.font.SysFont("Arial", 24)
    text_pct = font_small.render(f"{percent}%", True, WHITE)
    text_rect = text_pct.get_rect(center=(WIDTH // 2, batt_y + batt_h // 2))
    screen.blit(text_pct, text_rect)

    # 7. Super Charge 快充闪烁特效
    flash_timer += dt
    flash_alpha += flash_dir
    if flash_alpha <= 60:
        flash_dir = 8
    elif flash_alpha >= 255:
        flash_dir = -8

    font_big = pygame.font.SysFont("Arial", 28, bold=True)
    charge_text = font_big.render("Super Charge", True, CHARGE_TEXT_COLOR)
    charge_text.set_alpha(flash_alpha)
    charge_rect = charge_text.get_rect(center=(WIDTH // 2, 25))
    screen.blit(charge_text, charge_rect)

    # 底部提示
    font_tip = pygame.font.SysFont("Arial", 16)
    tip = font_tip.render("Charging...", True, (180, 200, 220))
    tip_rect = tip.get_rect(center=(WIDTH // 2, HEIGHT - 15))
    screen.blit(tip, tip_rect)

    pygame.display.flip()

pygame.quit()