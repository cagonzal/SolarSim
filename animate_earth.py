# animate_orbits.py
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import seaborn as sns
sns.set_context("paper")
sns.set_theme(style='dark')
from cycler import cycler
from scipy.interpolate import CubicSpline

colors = ['xkcd:sea blue', 'xkcd:brick', 'xkcd:teal green', 'xkcd:salmon',
          'xkcd:soft purple', 'xkcd:turquoise', 'xkcd:gold', 'xkcd:cornflower',
          'xkcd:dark cyan', 'xkcd:terra cotta', 'xkcd:forest', 'xkcd:coral pink',
          'xkcd:plum', 'xkcd:azure', 'xkcd:bronze', 'xkcd:periwinkle']


LABELFONT = 10
TICKFONT = 10
LEGENDFONT = 8

plt.rcParams.update({
    # Font sizes
    'font.size': LABELFONT,                       # Base font size (matches your LABELFONT)
    'axes.labelsize': LABELFONT,                  # Label font size (your LABELFONT)
    'axes.titlesize': LABELFONT,                  # Title font size
    'xtick.labelsize': TICKFONT,                 # X tick label size (your TICKFONT)
    'ytick.labelsize': TICKFONT,                 # Y tick label size (your TICKFONT)
    'legend.fontsize': LEGENDFONT,                 # Legend font size (your LEGENDFONT)
    'legend.title_fontsize': LEGENDFONT,           # Legend title size

    'text.color': 'w',
    'axes.facecolor': 'k',
    'figure.facecolor': 'k',
    'axes.labelcolor': 'w',
    'axes.edgecolor': 'w',
    'xtick.color': 'w',
    'ytick.color': 'w',
    
    # Marker properties
    'lines.markersize': 4,                # Marker size (your ms)
    'lines.markeredgewidth': 0.5,         # Marker edge width (your mew)
    'lines.markeredgecolor': 'k',         # Default marker edge color (black)
    
    # Line properties
    'lines.linewidth': 1.0,               # Line width
    
    # Tick parameters
    'xtick.major.width': 0.8,             # Width of x ticks
    'ytick.major.width': 0.8,             # Width of y ticks
    'xtick.major.size': 3,                # Length of x ticks
    'ytick.major.size': 3,                # Length of y ticks
    
    # Spine properties (axes borders)
    'axes.linewidth': 0.8,                # Width of the axes lines
    
    # Figure properties
    'figure.dpi': 300,                    # Figure resolution
    # 'figure.constrained_layout.use': True,# Use constrained layout
    
    # Legend properties
    'legend.frameon': True,               # Show legend frame
    'legend.fancybox': True,              # Rounded corners on legend
    'legend.edgecolor': '0.8',            # Light gray edge color for legend
})

# Set the default color cycle
plt.rcParams['axes.prop_cycle'] = cycler(color=colors)
plt.rcParams["font.family"] = "serif"
plt.rcParams["mathtext.fontset"] = "cm" # computer modern

# colwidth = 384.0 pt
pt = 1./72.27 # Hundreds of years of history... 72.27 points to an inch.
# my_width = 384.0 * pt # CTR
# my_width = 510.0pt * pt # PRF  # full page width 
two_width = 510.0 * pt 
one_width = 510.0/2.0 * pt 
# my_width = 469.755 * pt # AIAA
# my_width = 229.8775 * pt # AIAA two column format 

two_width = 345.0  * pt# JCP 
golden = (1 + 5 ** 0.5) / 2

one_height = one_width / golden
two_height = two_width / golden


#===========================================
#===========================================
#===========================================

# Read data
print("Loading data...")
data = np.loadtxt('orbit.dat')

# Parse header to get planet names
with open('orbit.dat', 'r') as f:
    header = f.readline().strip()

# Extract planet names from header
parts = header.split()[2:]  # Skip "# time"
planet_names = []
for i in range(0, len(parts)-1, 2):  # Skip energy_error at end
    name = parts[i].replace('x_', '')
    planet_names.append(name)

print(f"Found planets: {planet_names}")

# Extract data
time = data[:, 0]
n_planets = len(planet_names)
n_frames = len(time)

print(f"Total frames: {n_frames}")
print(f"Time span: {time[0]:.2f} to {time[-1]:.2f} years")

# Extract positions for each planet
positions = {}
for i, name in enumerate(planet_names):
    x = data[:, 1 + 2*i]
    y = data[:, 2 + 2*i]
    positions[name] = (x, y)

cs_x = CubicSpline(time, positions['satellite'][0])
cs_y = CubicSpline(time, positions['satellite'][1])

t_fine = np.linspace(time[0], time[-1], 2 * np.size(time))
x_fine = cs_x(t_fine)
y_fine = cs_y(t_fine)

# print(f"x = {positions['satellite'][0]}")
# print(f"y = {positions['satellite'][1]}")
# print(f"xfine = {x_fine}")
# print(f"yfine = {y_fine}")

# Set up the figure
fig, ax = plt.subplots(figsize=(10, 10), facecolor='black', dpi=300)

# Determine plot limits based on outermost planet
max_dist = 0
for name in planet_names:
    x, y = positions[name]
    max_dist = max(max_dist, np.max(np.sqrt(x**2 + y**2)))

earth_name = 'earth'
margin = 1.1
R_min = 0.004 #AU ~ moon orbit scale 
R_max = max_dist * margin
zoom_start = 0.0 
zoom_duration = time[-1]

def smooth_step(t):
    return t * t * (3 - 2 * 2)

# ax.set_xlim(-max_dist * margin, max_dist * margin)
# ax.set_ylim(-max_dist * margin, max_dist * margin)
ax.set_xlim(0, max_dist * margin)
ax.set_ylim(0, max_dist * margin)
ax.set_aspect('equal')
ax.set_xlabel(r'$x$ (AU)', fontsize=12)
ax.set_ylabel(r'$y$ (AU)', fontsize=12)
# ax.grid(True, alpha=0.3)

# Dictionary to store plot elements for each planet
planet_plots = {}
trail_plots = {}
trail_length = 10  # Number of previous positions to show

# Initialize plots for each planet
for i, name in enumerate(planet_names):
    if name.lower() == 'sun':
        # Sun: large yellow circle, no trail
        planet_plots[name] = ax.plot([], [], 'o', color='gold', 
                                      markersize=15, label='Sun')[0]
    else:
        # Planet: smaller circle with trail
        trail_plots[name] = ax.plot([], [], '-', color=colors[i], 
                                    alpha=0.5)[0]
        planet_plots[name] = ax.plot([], [], 'o', color=colors[i], 
                                      markersize=6, label=name.capitalize())[0]

# Time text
time_text = ax.text(0.02, 0.98, '', transform=ax.transAxes, 
                    fontsize=14, verticalalignment='top',
                    bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))

ax.legend(loc='upper right', fontsize=10)

# Animation initialization
def init():
    for name in planet_names:
        planet_plots[name].set_data([], [])
        if name.lower() != 'sun' and name in trail_plots:
            trail_plots[name].set_data([], [])
    time_text.set_text('')
    return list(planet_plots.values()) + list(trail_plots.values()) + [time_text]

# Animation update function
def animate(frame):

    # earth position for camera center 
    xE, yE = positions[earth_name]
    cx = xE[frame]
    cy = yE[frame]

    t = time[frame] 
    if t <= zoom_start:
        s = 0.0 
    elif t >= zoom_start + zoom_duration:
        s = 1.0 
    else:
        s = (t - zoom_start) / zoom_duration
    # s = smooth_step(s)

    # R = R_min + s * (R_max - R_min)
    R = R_min

    xmin, xmax = sorted((cx - R, cx + R))
    ymin, ymax = sorted((cy - R, cy + R))
    ax.set_xlim(xmin, xmax)
    ax.set_ylim(ymin, ymax)

    # ax.set_xlim(cx - R, cx+ R)
    # ax.set_ylim(cy - R, cy+ R)
    # Update each planet
    for name in planet_names:
        x, y = positions[name]
        
        xp = x[frame]  
        yp = y[frame] 
        # Update planet position
        planet_plots[name].set_data([xp], [yp])
        
        # Update trail (if not sun)
        if name.lower() != 'sun' and name in trail_plots:
            trail_start = max(0, frame - trail_length)
            xt = x[trail_start:frame+1] 
            yt = y[trail_start:frame+1] 
            trail_plots[name].set_data(xt, yt)
    
    # Update time display
    time_text.set_text(f'Time: {time[frame]:.2f} years')
    
    return list(planet_plots.values()) + list(trail_plots.values()) + [time_text]

# Create animation
print("Creating animation...")
# Use every nth frame to speed up animation if you have many frames
frame_skip = max(1, n_frames // 500)  # Target ~500 frames for reasonable file size
frames_to_use = range(0, n_frames, frame_skip)

# anim = animation.FuncAnimation(fig, animate, init_func=init,
#                                frames=frames_to_use, 
#                                interval=20,  # 20ms between frames = 50 fps
#                                blit=True, repeat=True)
anim = animation.FuncAnimation(fig, animate, init_func=init,
                               frames=frames_to_use, 
                               interval=200,  # 20ms between frames = 50 fps
                               blit=True, repeat=True)

# Save animation
print("Saving animation (this may take a minute)...") 

anim.save('earth_zoom.mp4', writer='ffmpeg', fps=30, dpi=150, bitrate=1800)
print("Animation saved as 'earth_zoom.mp4'")
# anim.save('solar_system.gif', writer='pillow', fps=30, dpi=150, bitrate=1800)
# print("Animation saved as 'solar_system.gif'")

# Also show it
# plt.show()
