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

class EyeBlink:
    def __init__(self):
        self.state = NOBLINK
        self.start_time = 0
        self.duration = 0
        self.last_blink_time = 0
        # Initial blink very soon (500ms) to show it's working
        self.next_blink_interval = 500000 
        
    def update(self, current_time_us):
        if self.state == NOBLINK:
            if current_time_us - self.last_blink_time >= self.next_blink_interval:
                self.state = ENBLINK
                self.start_time = current_time_us
                self.duration = random.randint(100000, 150000)
                
        elif self.state == ENBLINK: 
            if current_time_us - self.start_time >= self.duration:
                self.state = DEBLINK
                self.start_time = current_time_us
                self.duration = random.randint(120000, 200000)
                
        elif self.state == DEBLINK: 
            if current_time_us - self.start_time >= self.duration:
                self.state = NOBLINK
                self.last_blink_time = current_time_us
                # Normal interval for subsequent blinks
                self.next_blink_interval = random.randint(2000000, 4000000)

    def get_upper_threshold(self, current_time_us):
        if self.state == NOBLINK:
            return 0
            
        elapsed = current_time_us - self.start_time
        if elapsed >= self.duration:
            return 254 if self.state == ENBLINK else 0
            
        s = (elapsed * 255) // self.duration
        
        if self.state == ENBLINK:
            return (254 * s) // 255
        else:
            return (254 * (255 - s)) // 255

    def get_lower_threshold(self, current_time_us):
        uT = self.get_upper_threshold(current_time_us)
        return (uT * 30) // 100

class BlinkSimulator:
    def __init__(self, root):
        self.root = root
        self.root.title("Realistic Eye Blink Simulation")
        
        # Black background like the image
        self.canvas = tk.Canvas(root, width=SCREEN_WIDTH, height=SCREEN_HEIGHT, bg="black", highlightthickness=0)
        self.canvas.pack()
        
        self.eye = EyeBlink()
        self.start_perf = time.perf_counter()
        
        # Eye geometry parameters - More symmetrical and elongated as in the new image
        self.center_x = SCREEN_WIDTH // 2
        self.center_y = SCREEN_HEIGHT // 2
        self.eye_w = 230
        self.eye_h_top = 70   
        self.eye_h_bottom = 65 
        
        # 1. Draw Sclera (Lavender-white)
        self.sclera_points = self.get_almond_points(self.center_x, self.center_y, self.eye_w, self.eye_h_top, self.eye_h_bottom)
        self.sclera = self.canvas.create_polygon(self.sclera_points, fill="#f5f5ff", outline="") 
        
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
        
        # 5. Eyelids (Pure Black to blend with background)
        self.skin_color = "#000000" 
        self.upper_lid = self.canvas.create_polygon([0,0,0,0], fill=self.skin_color, outline="")
        self.lower_lid = self.canvas.create_polygon([0,0,0,0], fill=self.skin_color, outline="")
        
        # 6. Static Mask Overlay
        self.draw_overlay()
        
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

    def draw_overlay(self):
        # Create a large black rectangle covering everything, then "cut out" the almond hole
        screen_poly = [
            -10, -10,
            SCREEN_WIDTH + 10, -10,
            SCREEN_WIDTH + 10, SCREEN_HEIGHT + 10,
            -10, SCREEN_HEIGHT + 10,
            -10, -10
        ]
        # Get smooth almond points
        hole_points = self.get_almond_points(self.center_x, self.center_y, self.eye_w, self.eye_h_top, self.eye_h_bottom)
        # Reverse to create hole effect
        screen_poly.extend(hole_points[::-1])
        self.canvas.create_polygon(screen_poly, fill="black", outline="", smooth=True)

    def update_simulation(self):
        try:
            current_time_us = int((time.perf_counter() - self.start_perf) * 1_000_000)
            self.eye.update(current_time_us)
            uT = self.eye.get_upper_threshold(current_time_us)
            lT = self.eye.get_lower_threshold(current_time_us)
            
            # 1. Pupil jitter amplitude
            jitter_x = math.sin(current_time_us / 300000) * 10.0
            jitter_y = math.cos(current_time_us / 250000) * 6.0
            
            # Update Iris and Pupil positions with jitter
            iris_cx = self.center_x + jitter_x
            iris_cy = self.center_y + 2 + jitter_y
            r_i = 62
            r_p = 42
            self.canvas.coords(self.iris, iris_cx - r_i, iris_cy - r_i, iris_cx + r_i, iris_cy + r_i)
            self.canvas.coords(self.pupil, iris_cx - r_p, iris_cy - r_p, iris_cx + r_p, iris_cy + r_p)
            
            # Update highlights
            self.canvas.coords(self.highlights[0], iris_cx + 5, iris_cy - 45, iris_cx + 45, iris_cy - 5)
            self.canvas.coords(self.highlights[1], iris_cx + 42, iris_cy - 5, iris_cx + 52, iris_cy + 5)
            self.canvas.coords(self.highlights[2], iris_cx + 35, iris_cy + 12, iris_cx + 42, iris_cy + 19)
            self.canvas.coords(self.highlights[3], iris_cx - 25, iris_cy + 15, iris_cx - 12, iris_cy + 28)

            # 2. Update eyelid polygons
            u_move_range = self.eye_h_top + 25
            u_offset = (uT * u_move_range) / 254
            l_move_range = self.eye_h_bottom + 15
            l_offset = (lT * l_move_range) / 254
            
            u_lid_pts = []
            steps = 50
            for i in range(steps + 1):
                x = self.center_x - self.eye_w//2 + (self.eye_w * i / steps)
                angle = math.pi * i / steps
                y_curve = self.center_y - (self.eye_h_top * math.sin(angle))
                u_lid_pts.extend([x, y_curve])
            y_edge = self.center_y - self.eye_h_top + u_offset
            u_lid_pts.extend([self.center_x + self.eye_w//2 + 50, y_edge, 
                             self.center_x + self.eye_w//2 + 50, self.center_y - self.eye_h_top - 50,
                             self.center_x - self.eye_w//2 - 50, self.center_y - self.eye_h_top - 50,
                             self.center_x - self.eye_w//2 - 50, y_edge])
            self.canvas.coords(self.upper_lid, *u_lid_pts)
            
            l_lid_pts = []
            for i in range(steps, -1, -1):
                x = self.center_x - self.eye_w//2 + (self.eye_w * i / steps)
                angle = math.pi * i / steps
                y_curve = self.center_y + (self.eye_h_bottom * math.sin(angle))
                l_lid_pts.extend([x, y_curve])
            y_edge = self.center_y + self.eye_h_bottom - l_offset
            l_lid_pts.extend([self.center_x - self.eye_w//2 - 50, y_edge,
                             self.center_x - self.eye_w//2 - 50, self.center_y + self.eye_h_bottom + 50,
                             self.center_x + self.eye_w//2 + 50, self.center_y + self.eye_h_bottom + 50,
                             self.center_x + self.eye_w//2 + 50, y_edge])
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
