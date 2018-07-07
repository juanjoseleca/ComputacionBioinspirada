import numpy as np
from mf import MF
from mf import PSO
from mf import individuo
import matplotlib.pyplot as plt
import re
 
matrix=[]
for i in range(51):  
	matrix.append([0]*7)
file=open("silabuz.txt","r")
for z in file:
	w=z.split(',')
	matrix[int(w[0])][int(w[1])]=int(w[2])
print(matrix)
R = np.array(matrix)
mi_pso2=PSO(R, K=2, alpha=0.1, beta=0.01, iterations=50)
l3,l4=mi_pso2.solucion2()
plt.plot(l3,l4,'bo')
plt.show()
