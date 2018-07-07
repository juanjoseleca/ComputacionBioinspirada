import numpy as np
import copy
import random
import operator
class individuo():
    def __init__(self, R, K, alpha, beta):
        self.R = R
        self.num_users, self.num_items = R.shape
        self.K = K
        self.alpha = alpha
        self.beta = beta
        self.fitness=0
        self.MejorLocal=[]
        self.inercia=1
        self.generar()
    def generar(self):
        self.P = np.random.normal(scale=1./self.K, size=(self.num_users, self.K))
        self.V_P = np.random.normal(scale=1./self.K, size=(self.num_users, self.K))
        self.Q = np.random.normal(scale=1./self.K, size=(self.num_items, self.K))
        self.V_Q = np.random.normal(scale=1./self.K, size=(self.num_items, self.K))
        self.b_u = np.zeros(self.num_users)
        self.b_i = np.zeros(self.num_items)
        self.b = np.mean(self.R[np.where(self.R != 0)])
        
        self.samples = [
            (i, j, self.R[i, j])
            for i in range(self.num_users)
            for j in range(self.num_items)
            if self.R[i, j] > 0
        ]
        self.MejorLocal.append(copy.copy(self))
    def mse(self):
        xs, ys = self.R.nonzero()
        predicted = self.full_matrix()
        error = 0
        for x, y in zip(xs, ys):
            error += pow(self.R[x, y] - predicted[x, y], 2)
        return np.sqrt(error)
    def entrenar(self):
        np.random.shuffle(self.samples)
        self.sgd()
        mse = self.mse()
        self.fitness=mse
        return mse
    def evaluar(self):
        self.fitness=self.mse()
    def sgd(self):
        for i, j, r in self.samples:
            prediction = self.get_rating(i, j)
            e = (r - prediction)
            self.b_u[i] += self.alpha * (e - self.beta * self.b_u[i])
            self.b_i[j] += self.alpha * (e - self.beta * self.b_i[j])
            self.P[i, :] += self.alpha * (e * self.Q[j, :] - self.beta * self.P[i,:])
            self.Q[j, :] += self.alpha * (e * self.P[i, :] - self.beta * self.Q[j,:])
    def get_rating(self, i, j):
        prediction = self.b + self.b_u[i] + self.b_i[j] + self.P[i, :].dot(self.Q[j, :].T)
        return prediction
    def full_matrix(self):
        return self.b + self.b_u[:,np.newaxis] + self.b_i[np.newaxis:,] + self.P.dot(self.Q.T)
class PSO():
    def __init__(self, R, K, alpha, beta, iterations):
        self.R = R
        self.num_users, self.num_items = R.shape
        self.K = K
        self.alpha = alpha
        self.beta = beta
        self.iterations = iterations
        self.Poblacion=[]
        self.MejorGlobal=[]
        self.tam_poblacion=10
        self.phi1=1
        self.phi2=1
        self.samples = [
            (i, j, self.R[i, j])
            for i in range(self.num_users)
            for j in range(self.num_items)
            if self.R[i, j] > 0
        ]
        self.generarPoblacion()
        
    def generarPoblacion(self):
        for tamano in range(self.tam_poblacion):
            nuevo = individuo(self.R,self.K,self.alpha,self.beta)
            nuevo.evaluar()
            print("Elemento nro ",tamano," : ",nuevo.fitness)
            if(len(self.MejorGlobal)==0):
                self.MejorGlobal.append(copy.copy(nuevo))
            else:
                self.MejorGlobal[0].evaluar()
                if(self.MejorGlobal[0].fitness>nuevo.fitness):
                    self.MejorGlobal.clear()
                    self.MejorGlobal.append(copy.copy(nuevo))
            self.Poblacion.append(nuevo)
        self.mostrar_mejor_global()
    def mostrar_mejor_global(self):
        if(len(self.MejorGlobal)!=0):
            print("MEJOR GLOBAL")
            print(self.MejorGlobal[0].fitness)
    def evaluar_poblacion(self):
        for ind in self.Poblacion:
            ind.evaluar()
    def entrenar(self):
        for x in self.Poblacion:
            valor=x.entrenar()
            print(valor)
    def sgd(self):
        for x in self.Poblacion:
            x.sgd()
            x.evaluar()
            print("Fitness:",x.fitness)
    def modificar_x_v(self):
        for a in range(self.tam_poblacion):
            random1=random.uniform(0,1)
            random2=random.uniform(0,1)
            self.Poblacion[a].evaluar()
            #print("Antiguo Fitness: ",self.Poblacion[a].fitness)
            for i, j, r in self.samples:
                for b in range(self.K):
                    self.Poblacion[a].V_P[i][b]=self.Poblacion[a].V_P[i][b]*self.inercia+self.phi1*random1*(self.Poblacion[a].MejorLocal[0].P[i][b]-self.Poblacion[a].P[i][b])+self.phi2*random2*(self.MejorGlobal[0].P[i][b]-self.Poblacion[a].P[i][b]);
                    self.Poblacion[a].P[i][b]+=self.Poblacion[a].V_P[i][b];
                    self.Poblacion[a].V_Q[j][b]=self.Poblacion[a].V_Q[j][b]*self.inercia+self.phi1*random1*(self.Poblacion[a].MejorLocal[0].Q[j][b]-self.Poblacion[a].Q[j][b])+self.phi2*random2*(self.MejorGlobal[0].Q[j][b]-self.Poblacion[a].Q[j][b]);
                    self.Poblacion[a].Q[j][b]+=self.Poblacion[a].V_Q[j][b];
            self.Poblacion[a].evaluar()
            if(self.Poblacion[a].fitness<self.Poblacion[a].MejorLocal[0].fitness):
                self.Poblacion[a].MejorLocal.clear()
                self.Poblacion[a].MejorLocal.append(copy.copy(self.Poblacion[a]))
            if(self.Poblacion[a].fitness<self.MejorGlobal[0].fitness):
                self.MejorGlobal.clear()
                self.MejorGlobal.append(copy.copy(self.Poblacion[a]))
            self.Poblacion[a].sgd()
            self.Poblacion[a].evaluar()
    def modificar_x_v_2(self):
        for w in range(1):
            r1=random.randint(0,5)
            r2=random.randint(0,5)
            while(r1!=r2):
                r1=random.randint(0,5)
                r2=random.randint(0,5)
            self.evaluar_poblacion()
            self.Poblacion=sorted(self.Poblacion,key=operator.attrgetter('fitness'))
            hijo=copy.copy(self.Poblacion[r1])
            for i in range(len(self.Poblacion[r1].P)):
                for j in range(len(self.Poblacion[r1].P[0])):
                    hijo.P[i][j]=(self.Poblacion[r1].P[i][j]+self.Poblacion[r2].P[i][j])/2;
            for i in range(len(self.Poblacion[r1].Q)):
                for j in range(len(self.Poblacion[r1].Q[0])):
                    hijo.Q[i][j]=(self.Poblacion[r1].Q[i][j]+self.Poblacion[r2].Q[i][j])/2;
            hijo.sgd()
            hijo.evaluar()
            self.Poblacion.append(hijo)
        #self.sgd()
        self.evaluar_poblacion()
        self.Poblacion=sorted(self.Poblacion,key=operator.attrgetter('fitness'))
        self.Poblacion=self.Poblacion[:10]
    def solucion(self):
        self.evaluar_poblacion()
        list1=[]
        list2=[]
        for iteraciones in range(self.iterations):
            self.inercia=random.uniform(0,1)
            self.modificar_x_v()
            self.evaluar_poblacion()
            list1.append(iteraciones)
            list2.append(self.MejorGlobal[0].fitness)
        self.MejorGlobal[0].evaluar()
        print("MEJOR GLOBAL:",self.MejorGlobal[0].fitness)
        print(self.MejorGlobal[0].full_matrix())
        return list1,list2
    def solucion2(self):
        self.evaluar_poblacion()
        list1=[]
        list2=[]
        for iteraciones in range(self.iterations):
            self.modificar_x_v_2()
            print("Mejor Fitness Iteracion ",iteraciones," :",self.Poblacion[0].fitness)
            self.evaluar_poblacion()
            list1.append(iteraciones)
            list2.append(self.Poblacion[0].fitness)
        return list1,list2


class MF():
    
    def __init__(self, R, K, alpha, beta, iterations):
        self.R = R
        self.num_users, self.num_items = R.shape
        self.K = K
        self.alpha = alpha
        self.beta = beta
        self.iterations = iterations

    def train(self):
        self.P = np.random.normal(scale=1./self.K, size=(self.num_users, self.K))
        print("P generado inicialmente: ",self.P)
        self.Q = np.random.normal(scale=1./self.K, size=(self.num_items, self.K))
        print("Q generado inicialmente: ",self.Q)
        self.b_u = np.zeros(self.num_users)
        print("hey look:->",self.R[np.where(self.R != 0)])
        self.b_i = np.zeros(self.num_items)
        self.b = np.mean(self.R[np.where(self.R != 0)])
        self.samples = [
            (i, j, self.R[i, j])
            for i in range(self.num_users)
            for j in range(self.num_items)
            if self.R[i, j] > 0
        ]
        print("samples:",self.samples)
        training_process = []
        for i in range(self.iterations):
            np.random.shuffle(self.samples)
            print("samples2:",self.samples)
            self.sgd()
            mse = self.mse()
            training_process.append((i, mse))
            if (i+1) % 10 == 0:
                print("Iteration: %d ; error = %.4f" % (i+1, mse))
        
        return training_process

    def mse(self):
        xs, ys = self.R.nonzero()
        predicted = self.full_matrix()
        error = 0
        for x, y in zip(xs, ys):
            error += pow(self.R[x, y] - predicted[x, y], 2)
        return np.sqrt(error)

    def sgd(self):
        for i, j, r in self.samples:
            prediction = self.get_rating(i, j)
            e = (r - prediction)
            self.b_u[i] += self.alpha * (e - self.beta * self.b_u[i])
            self.b_i[j] += self.alpha * (e - self.beta * self.b_i[j])
            self.P[i, :] += self.alpha * (e * self.Q[j, :] - self.beta * self.P[i,:])
            self.Q[j, :] += self.alpha * (e * self.P[i, :] - self.beta * self.Q[j,:])

    def get_rating(self, i, j):
        print("First look P", self.P)
        print("HEY look get_rating:"," i:",i," j: ",j,"    =",self.P[i,:])
        print("Q:",self.Q)
        print("Q[j,:]",j," =",self.Q[j,:])
        print("Trans: ",self.Q[j,:].T)
        prediction = self.b + self.b_u[i] + self.b_i[j] + self.P[i, :].dot(self.Q[j, :].T)
        return prediction
    
    def full_matrix(self):
        print("PRE_RES",self.P.dot(self.Q.T))
        print("b_u",self.b_u)
        print("b_u:newaxis",self.b_u[:,np.newaxis])
        print("b_i:",self.b_i)
        print("b_i:newaxis",self.b_i[np.newaxis:,])
        return self.b + self.b_u[:,np.newaxis] + self.b_i[np.newaxis:,] + self.P.dot(self.Q.T)
