import copy
import operator
class punto():
	def __init__(self,x,y):
		self.x=x
		self.y=y
		self.mi_list=[]
		punto_copy=copy.copy(self)
		self.mi_list.append(punto_copy)
	def mostrar(self):
		print(self.x,self.y)
lista_p=[]
punto_n=punto(6,2)
punto2=punto(4,3)
punto3=punto(2,3)
lista_p.append(punto_n)
lista_p.append(punto2)
lista_p.append(punto3)
for a in lista_p:
	print("uns: ",a.x)
s_lista=sorted(lista_p,key=operator.attrgetter('x'))
for a in s_lista:
	print("s: ",a.x)
punto_n.mostrar()
punto_c=copy.copy(punto_n)
punto_c.mostrar()
punto_n.x=3
punto_n.mostrar()
punto_c.mostrar()
print("hey")
punto_n.mi_list[0].x=5
punto_n.mostrar()
punto_n.mi_list[0].mostrar()
