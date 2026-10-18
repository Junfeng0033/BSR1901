import tkinter as tk
import random
import time
import math

# --- Configuration from eye_blink_m0.h ---
SCREEN_WIDTH = 240
SCREEN_HEIGHT = 240
IRIS_MIN = 100
IRIS_MAX = 500

# --- State Constants ---
NOBLINK = 0
ENBLINK = 1  # Closing
DEBLINK = 2  # Opening

# --- Blink Types ---
BLINK_NATURAL = 0   # 自然眨眼：合眼快睁眼慢
BLINK_QUICK = 1     # 快速眨眼：像被什么东西惊到
BLINK_SLOW = 2      # 缓慢眨眼：疲劳/困倦

class EyeBlink:
    def __init__(self):
        self.state = NOBLINK
        self.start_time = 0
        self.duration = 0
        self.last_blink_time = 0
        self.blink_type = 0
        self.next_blink_interval = 800000
        self.consecutive_blinks = 0
        self.max_consecutive = 3
        self.focus_level = 0.5
        self.focus_change_time = 0
        self.focus_duration = 0
        self.close_duration_min = 80000
        self.close_duration_max = 150000
        self.open_duration_min = 120000
        self.open_duration_max = 250000
        
    def _get_interval(self):
        base_min = 2000000
        base_max = 6000000
        focus_factor = 1.0 - self.focus_level * 0.5
        if self.consecutive_blinks > 0:
            cluster_factor = 1.0 + (self.consecutive_blinks * 0.5)
        else:
            cluster_factor = 1.0
        interval_min = int(base_min * focus_factor * cluster_factor)
        interval_max = int(base_max * focus_factor * cluster_factor)
        return random.randint(interval_min, interval_max)

    def _pick_blink_type(self):
        r = random.random()
        if self.focus_level < 0.3:
            if r < 0.5:
                return 2
            elif r < 0.8:
                return 0
            else:
                return 1
        else:
            if r < 0.7:
                return 0
            elif r < 0.9:
                return 1
            else:
                return 2

    def update(self, current_time_us):
        if current_time_us - self.focus_change_time > self.focus_duration:
            self.focus_level = random.uniform(0.2, 0.9)
            self.focus_change_time = current_time_us
            self.focus_duration = random.randint(10000000, 30000000)

        if self.state == NOBLINK:
            if current_time_us - self.last_blink_time >= self.next_blink_interval:
                self.state = ENBLINK
                self.start_time = current_time_us
                self.blink_type = self._pick_blink_type()
                self.duration = random.randint(self.close_duration_min, self.close_duration_max)
        elif self.state == ENBLINK:
            if current_time_us - self.start_time >= self.duration:
                self.state = DEBLINK
                self.start_time = current_time_us
                self.duration = random.randint(self.open_duration_min, self.open_duration_max)
        elif self.state == DEBLINK:
            if current_time_us - self.start_time >= self.duration:
                self.state = NOBLINK
                self.last_blink_time = current_time_us
                self.consecutive_blinks += 1
                if self.consecutive_blinks < self.max_consecutive and random.random() < 0.3:
                    self.next_blink_interval = random.randint(300000, 1000000)
                else:
                    self.consecutive_blinks = 0
                    self.next_blink_interval = self._get_interval()

    def _ease_value(self, t):
        """正弦平滑缓动：先慢后快再慢，更自然"""
        return -(math.cos(math.pi * t) - 1) / 2

    def get_progress(self, current_time_us):
        """Get normalized progress (0.0 to 1.0) of current blink phase"""
        if self.state == NOBLINK:
            return 0.0
        elapsed = current_time_us - self.start_time
        if elapsed >= self.duration:
            return 1.0
        return elapsed / self.duration

    def get_upper_threshold(self, current_time_us):
        """Get upper eyelid position (0-254, higher = more closed)"""
        if self.state == NOBLINK:
            return 0

        elapsed = current_time_us - self.start_time
        if elapsed >= self.duration:
            return 254 if self.state == ENBLINK else 0

        # Apply easing for natural acceleration/deceleration
        progress = elapsed / self.duration
        eased = self._ease_value(progress)

        if self.state == ENBLINK:
            # Closing: 0 -> 254
            return int(254 * eased)
        else:
            # Opening: 254 -> 0
            return int(254 * (1.0 - eased))

    def get_lower_threshold(self, current_time_us):
        """Get lower eyelid position (0-254, higher = more closed)"""
        uT = self.get_upper_threshold(current_time_us)
        # Lower lid moves less than upper lid (about 35%)
        return int((uT * 35) // 100)

    def is_blinking(self):
        return self.state != NOBLINK

class BlinkSimulator:
    def __init__(self, root):
        self.root = root
        self.root.title("Realistic Eye Blink Simulation")
        
        # White background
        self.canvas = tk.Canvas(root, width=SCREEN_WIDTH, height=SCREEN_HEIGHT, bg="white", highlightthickness=0)
        self.canvas.pack()
        
        self.eye = EyeBlink()
        self.start_perf = time.perf_counter()
        
        # Eye geometry parameters - Elliptical shape
        self.center_x = SCREEN_WIDTH // 2
        self.center_y = SCREEN_HEIGHT // 2
        self.eye_w = 220
        self.eye_h = 130
        
        # 1. Draw Sclera (Lavender-white) - Elliptical
        self.sclera = self.canvas.create_oval(
            self.center_x - self.eye_w//2, self.center_y - self.eye_h//2,
            self.center_x + self.eye_w//2, self.center_y + self.eye_h//2,
            fill="#f5f5ff", outline=""
        ) 
        
        # 2. Draw Iris (Bright Purple, Centered)
        iris_cx = self.center_x
        iris_cy = self.center_y + 2
        iris_radius = 62
        self.iris = self.canvas.create_oval(
            iris_cx - iris_radius, iris_cy - iris_radius,
            iris_cx + iris_radius, iris_cy + iris_radius,
            fill="#7b68ee", outline="#483d8b", width=1
        )
        
        # 3. Draw Pupil (Large and Black)
        pupil_radius = 42
        self.pupil = self.canvas.create_oval(
            iris_cx - pupil_radius, iris_cy - pupil_radius,
            iris_cx + pupil_radius, iris_cy + pupil_radius,
            fill="#080808", outline=""
        )

        # 4. Add Highlights (Exact placement based on the reference)
        self.highlights = []
        # Main large highlight (Top-Right)
        self.highlights.append(self.canvas.create_oval(iris_cx + 5, iris_cy - 45, iris_cx + 45, iris_cy - 5, fill="white", outline=""))
        # Two small ones (Right side)
        self.highlights.append(self.canvas.create_oval(iris_cx + 42, iris_cy - 5, iris_cx + 52, iris_cy + 5, fill="white", outline=""))
        self.highlights.append(self.canvas.create_oval(iris_cx + 35, iris_cy + 12, iris_cx + 42, iris_cy + 19, fill="white", outline=""))
        # One small one (Bottom-Left)
        self.highlights.append(self.canvas.create_oval(iris_cx - 25, iris_cy + 15, iris_cx - 12, iris_cy + 28, fill="white", outline=""))
        
        # 5. Eyelids (Black)
        self.skin_color = "#000000" 
        self.upper_lid = self.canvas.create_polygon([0,0,0,0], fill=self.skin_color, outline="")
        self.lower_lid = self.canvas.create_polygon([0,0,0,0], fill=self.skin_color, outline="")
        
        self.update_simulation()

        
    def get_almond_points(self, cx, cy, w, h_top, h_bottom):
        points = []
        # Increased points for smoothness (0.5 degree steps)
        steps = 200
        # Upper curve
        for i in range(steps + 1):
            x = cx - w//2 + (w * i / steps)
            angle = math.pi * i / steps
            y = cy - (h_top * math.sin(angle))
            points.extend([x, y])
        # Lower curve
        for i in range(steps, -1, -1):
            x = cx - w//2 + (w * i / steps)
            angle = math.pi * i / steps
            y = cy + (h_bottom * math.sin(angle))
            points.extend([x, y])
        return points

    def get_ellipse_points(self, cx, cy, w, h):
        points = []
        steps = 100
        for i in range(steps + 1):
            angle = 2 * math.pi * i / steps
            x = cx + (w/2) * math.cos(angle)
            y = cy + (h/2) * math.sin(angle)
            points.extend([x, y])
        return points

    def update_simulation(self):
        try:
            current_time_us = int((time.perf_counter() - self.start_perf) * 1_000_000)
            self.eye.update(current_time_us)
            uT = self.eye.get_upper_threshold(current_time_us)
            lT = self.eye.get_lower_threshold(current_time_us)
            
            # 1. Natural eye micro-movements (random walk + gentle drift)
            if not hasattr(self, '_target_jx'):
                self._target_jx = 0
                self._target_jy = 0
                self._curr_jx = 0
                self._curr_jy = 0
                self._jitter_change_time = 0

            # Change target jitter target every 1-3 seconds
            if current_time_us - self._jitter_change_time > random.randint(1000000, 3000000):
                self._target_jx = random.uniform(-8, 8)
                self._target_jy = random.uniform(-5, 5)
                self._jitter_change_time = current_time_us

            # Smoothly interpolate toward target
            self._curr_jx += (self._target_jx - self._curr_jx) * 0.05
            self._curr_jy += (self._target_jy - self._curr_jy) * 0.05

            # Subtle continuous micro-tremor
            tremor_x = math.sin(current_time_us / 80000) * 0.8
            tremor_y = math.cos(current_time_us / 60000) * 0.5

            # When blinking, eyeball tends to roll up slightly (Bell's phenomenon)
            blink_lift = 0
            if self.eye.is_blinking():
                blink_lift = -2.0

            iris_cx = self.center_x + self._curr_jx + tremor_x
            iris_cy = self.center_y + 2 + self._curr_jy + tremor_y + blink_lift
            r_i = 62
            r_p = 42
            self.canvas.coords(self.iris, iris_cx - r_i, iris_cy - r_i, iris_cx + r_i, iris_cy + r_i)
            self.canvas.coords(self.pupil, iris_cx - r_p, iris_cy - r_p, iris_cx + r_p, iris_cy + r_p)

            # Update highlights relative to iris position
            self.canvas.coords(self.highlights[0], iris_cx + 5, iris_cy - 45, iris_cx + 45, iris_cy - 5)
            self.canvas.coords(self.highlights[1], iris_cx + 42, iris_cy - 5, iris_cx + 52, iris_cy + 5)
            self.canvas.coords(self.highlights[2], iris_cx + 35, iris_cy + 12, iris_cx + 42, iris_cy + 19)
            self.canvas.coords(self.highlights[3], iris_cx - 25, iris_cy + 15, iris_cx - 12, iris_cy + 28)

            # 2. Update eyelid polygons - natural curved closure
            # When closing, eyelids curve inward from the sides, not just a flat line
            u_move_range = self.eye_h/2 + 25
            u_offset = (uT * u_move_range) / 254
            l_move_range = self.eye_h/2 + 20
            l_offset = (lT * l_move_range) / 254

            # Upper eyelid: curves down from center when closing
            # Extend beyond eye corners to fully cover sclera at the edges
            u_lid_pts = []
            steps = 60
            lid_extend = 10  # 眼角正好在屏幕边界 (center_x=120, eye_w=220 → 边界在0和240)
            for i in range(steps + 1):
                t = i / steps
                x = self.center_x - self.eye_w//2 - lid_extend + (self.eye_w + 2*lid_extend) * t
                angle = math.pi * i / steps
                # Base upper curve
                y_base = self.center_y - (self.eye_h/2 * math.sin(angle))
                # When closing, pull the lid down more in the center
                center_pull = math.sin(angle)
                y_closed = y_base + u_offset * center_pull * 1.2
                u_lid_pts.extend([x, y_closed])
            # Close the polygon at the bottom (beyond eye corners)
            u_lid_pts.extend([self.center_x + self.eye_w//2 + lid_extend, self.center_y - self.eye_h/2 - 50,
                             self.center_x - self.eye_w//2 - lid_extend, self.center_y - self.eye_h/2 - 50])
            self.canvas.coords(self.upper_lid, *u_lid_pts)

            # Lower eyelid: curves up from center when closing
            l_lid_pts = []
            for i in range(steps, -1, -1):
                t = i / steps
                x = self.center_x - self.eye_w//2 - lid_extend + (self.eye_w + 2*lid_extend) * t
                angle = math.pi * i / steps
                y_base = self.center_y + (self.eye_h/2 * math.sin(angle))
                center_pull = math.sin(angle)
                y_closed = y_base - l_offset * center_pull * 1.2
                l_lid_pts.extend([x, y_closed])
            # Close the polygon at the top (beyond eye corners)
            l_lid_pts.extend([self.center_x - self.eye_w//2 - lid_extend, self.center_y + self.eye_h/2 + 50,
                             self.center_x + self.eye_w//2 + lid_extend, self.center_y + self.eye_h/2 + 50])
            self.canvas.coords(self.lower_lid, *l_lid_pts)
            
        except Exception as e:
            print(f"Animation Error: {e}")
            
        self.root.after(16, self.update_simulation)

def main():
    root = tk.Tk()
    sim = BlinkSimulator(root)
    root.mainloop()

if __name__ == "__main__":
    main()
