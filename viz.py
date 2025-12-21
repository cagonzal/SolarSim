import numpy as np 
import matplotlib.pyplot as plt 
import seaborn as sns
sns.set_context("paper")
sns.set_theme(style='ticks')
from cycler import cycler

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
    
    # Marker properties
    'lines.markersize': 4,                # Marker size (your ms)
    'lines.markeredgewidth': 0.5,         # Marker edge width (your mew)
    'lines.markeredgecolor': 'k',         # Default marker edge color (black)
    
    # Line properties
    'lines.linewidth': 1.5,               # Line width
    
    # Tick parameters
    'xtick.major.width': 0.8,             # Width of x ticks
    'ytick.major.width': 0.8,             # Width of y ticks
    'xtick.major.size': 3,                # Length of x ticks
    'ytick.major.size': 3,                # Length of y ticks
    
    # Spine properties (axes borders)
    'axes.linewidth': 0.8,                # Width of the axes lines
    
    # Figure properties
    'figure.dpi': 300,                    # Figure resolution
    'figure.constrained_layout.use': True,# Use constrained layout
    
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

data = np.loadtxt('orbit.dat')

# Parse header to get planet names
with open('orbit.dat', 'r') as f:
    header = f.readline().strip()

# Extract planet names from header (format: "# time x_sun y_sun x_earth y_earth ...")
parts = header.split()[2:]  # Skip "# time"
planet_names = []
for i in range(0, len(parts)-1, 2):  # Skip energy_error at end
    name = parts[i].replace('x_', '')
    planet_names.append(name)

print(f"Found planets: {planet_names}")

# Extract data
time = data[:, 0]
n_planets = len(planet_names)
energy_error = data[:, -1]

plt.figure(figsize=(two_width,two_height), dpi=200, constrained_layout=True)
for i, name in enumerate(planet_names):

    x = data[:, 1 + 2*i]
    y = data[:, 2 + 2*i]

    if name.lower() == 'sun':
        plt.plot(x, y, 'o', color='gold', label=name.capitalize())
    else:
        plt.plot(x, y, '-', color=colors[i], label=name.capitalize(), alpha=0.7)
        plt.plot(x[-1], y[-1], 'o', color=colors[i])  # Mark final position

plt.xlabel(r"$x$")
plt.ylabel(r"$y$")
plt.legend()
plt.title("Orbital Path")

plt.savefig('orbital_path.pdf', bbox_inches='tight')

plt.figure(figsize=(two_width,two_height), dpi=200, constrained_layout=True)
plt.plot(time, energy_error)
plt.xlabel(r"$t$")
plt.ylabel(r"$\epsilon$")
plt.title("Energy Conservation")
plt.savefig('energy.pdf', bbox_inches='tight')



