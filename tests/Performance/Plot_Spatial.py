import numpy as np
import matplotlib.pyplot as plt

# Load data 
data = np.loadtxt('../build/spatial_error_map.txt', comments='#')
x = data[:, 0]
y = data[:, 1]
logerr = data[:, 2]

# Assume grid is square
ngrid = int(np.sqrt(len(x)))
xg = x.reshape((ngrid, ngrid))
yg = y.reshape((ngrid, ngrid))
errg = logerr.reshape((ngrid, ngrid))

plt.figure(figsize=(7, 6))
levels = np.linspace(np.nanmin(errg), np.nanmax(errg), 20)
contour = plt.contourf(xg, yg, errg, levels=levels, cmap='copper')
cbar = plt.colorbar(contour)
cbar.set_label("log$_{10}$ (Relative Total Error)", fontsize=14)
plt.xlabel(r'$x$', fontsize=14)
plt.ylabel(r'$y$', fontsize=14)
plt.title("Spatial Distribution of Relative Total Error")
plt.tight_layout()
plt.savefig("fmm_spatial_error_map.png", dpi=150)
