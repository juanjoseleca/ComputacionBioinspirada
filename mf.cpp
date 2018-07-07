#include <iostream>
#include <stdio.h>      
#include <stdlib.h>     
#include <time.h> 
#include <vector>
#include <algorithm>
#include <mutex>
#include "timer.h"
#define num_threads 4
#include <thread>
//#define e 2.7182818
#define TAM_X 2
#define TAM_V 2
#define l_i 10
#define l_s 10
#define phi1 0.2
#define phi2 0.2


using namespace std;
typedef double DATO;
struct  valoracion
{
	int i;
	int j;
	int v;
};
void hi(int a, int b, int c)
{
	cout<<a+b+c<<endl;
}

class individuo
{
public:
	DATO fitness;
	DATO x[TAM_X];
	DATO v[TAM_V];
	DATO velocidad[TAM_V];
	individuo * MejorLocal;
	//FUERZAS
	DATO inercia; ///????
	DATO mejor_global;
	DATO mejor_local;

	DATO aceleracion;
	individuo()
	{
		for(int i=0;i<TAM_X;i++)
		{
			x[i]=-5+((double) rand() / (RAND_MAX))*(10);
			v[i]=-1+((double) rand() / (RAND_MAX))*(2);
		}
		this->MejorLocal=this;
		this->inercia=((double) rand() / (RAND_MAX)) ;
		//Inercia es el vector de velocidad
	}
	void set_fitness()
	{
		this->fitness=pow(x[0],2)+pow(x[1],2);
	}
	void mostrar()
	{
		cout<<"x1"<<" ="<<x[0]<<" x2="<<x[1]<<" v1: "<<v[0]<<" v2: "<<v[1]<<endl;
		cout<<"Fitness: "<<fitness<<endl;
	}
};
//pBesti = xi
//el mejor global se actualiza, el grupo confia en el lider
//en el mejor local, para la primera iteracion es el mismo
//solo se almacena el mejor y el actual para cada particula
//xi=xi+vi
//luego de la actualizacion verificar para hallar el mejor global
// antes de actualizar las pos, se actualiza la velocidad
class PSO
{
public:
	int tam_poblacion;
	vector<individuo*> miPoblacion;
	individuo * MejorGlobal;
	DATO inercia;
	PSO(int tam_poblacion)
	{
		this->tam_poblacion=tam_poblacion;
		this->MejorGlobal=0;
		generarPoblacion();
	}
	void mostrar_Poblacion()
	{
		cout<<"Cumulo de particulas Siguientes:"<<endl;
		for(int i=0;i<tam_poblacion;i++)
		{
			cout<<"x1"<<" ="<<miPoblacion[i]->x[0]<<" x2="<<miPoblacion[i]->x[1]<<" v1: "<<miPoblacion[i]->v[0]<<" v2: "<<miPoblacion[i]->v[1]<<endl;
			cout<<"Fitness: "<<miPoblacion[i]->fitness<<endl;
			cout<<endl;
		}
	}
	void generarPoblacion()
	{
		for(int i=0;i<tam_poblacion;i++)
		{
			individuo * nuevo = new individuo();
			miPoblacion.push_back(nuevo);
		}
	}
	void evaluar_poblacion()
	{

		for(int i=0;i<tam_poblacion;i++)
		{
			miPoblacion[i]->set_fitness();
			if(MejorGlobal!=0)
			{
				if(MejorGlobal->fitness>miPoblacion[i]->fitness)
				{
					cout<<"MG_F: "<<MejorGlobal->fitness<<"  vs  MP: "<<miPoblacion[i]->fitness<<endl;
					MejorGlobal=miPoblacion[i];
					cout<<"Se encontro una mejor solucion Global:"<<MejorGlobal->fitness<<endl;
				}
			}
			else
			{
				MejorGlobal=miPoblacion[i];
			}

		}
		cout<<"Mejor Solucion despues de evaluacion: "<<MejorGlobal->fitness<<endl;


	}
	void modificar_x_v()
	{
		double random1,random2;
		for(int i=0;i<tam_poblacion;i++)
		{

			for(int j=0;j<TAM_V;j++)
			{
				random1=((double) rand() / (RAND_MAX)) ;
				random2=((double) rand() / (RAND_MAX)) ;
				miPoblacion[i]->v[j]=miPoblacion[i]->v[j]*inercia+phi1*random1*(miPoblacion[i]->MejorLocal->x[j]-miPoblacion[i]->x[j])+phi2*random2*(MejorGlobal->x[j]-miPoblacion[i]->x[j]);
				miPoblacion[i]->x[j]+=miPoblacion[i]->v[j];
				miPoblacion[i]->set_fitness();
				if(miPoblacion[i]->fitness<miPoblacion[i]->MejorLocal->fitness)
				{
					miPoblacion[i]->MejorLocal=miPoblacion[i];
				}
			}
			
		}
	}

	void solucion(int iteraciones)
	{
		evaluar_poblacion();
		for(int i=0;i<iteraciones;i++)
		{
			inercia=((double) rand() / (RAND_MAX)) ;
			cout<<"****ITERACION "<<i<<endl;
			cout<<"Mejor: Fitness: "<<MejorGlobal->fitness<<" ;x1: "<<MejorGlobal->x[0]<<" ;x2: "<<MejorGlobal->x[1]<<endl;
			mostrar_Poblacion();
			modificar_x_v();
			evaluar_poblacion();
		}
		cout<<"Mejor solucion: "<<endl;
		MejorGlobal->mostrar();
	}
};



class MF
{
public:
	mutex mu;
	thread t[num_threads];
	DATO ** Ratings;
	int K;
	DATO alpha;
	DATO beta;
	int n_iteraciones;
	int n_usuarios;
	int n_items;
	DATO * b_u; //bias de usuarios
	DATO * b_i; // bias de items
	DATO b;
	DATO ** P;
	DATO ** Q;
	DATO ** T_Q;
	DATO ** VE_Rating;
	double start, finish, elapsed;
	vector<valoracion*> samples;
	MF(DATO ** Ratings,int n_usuarios,int n_items,int K, DATO alpha, DATO beta, int n_iteraciones)
	{
		
		this->Ratings=Ratings;
		this->K=K;
		this->alpha=alpha;
		this->beta=beta;
		this->n_iteraciones=n_iteraciones;
		this->n_usuarios=n_usuarios;
		this->n_items=n_items;
		this->P=new DATO*[n_usuarios];
		this->Q=new DATO*[n_items];
		//this->b_u=(DATO*)malloc(sizeof(DATO)*n_usuarios);
		this->b_u=new DATO[n_usuarios];
		this->b_i=new DATO[n_items];
		this->VE_Rating= new DATO*[n_usuarios];
		for(int i=0;i<n_usuarios;i++)
		{
			P[i]=new DATO[K];
			VE_Rating[i]=new DATO[n_items];
		}
		for(int i=0;i<n_items;i++)
		{
			Q[i]=new DATO[K];
		}
		//mostrar_matriz(Ratings,n_usuarios,n_items);

	}
	void setP_Q()
	{
		for(int i=0;i<n_usuarios;i++)
		{
			for (int j = 0; j <K; j++)
			{
				/* code */
				P[i][j]=((double) rand() / (RAND_MAX)); //random 
				
			}
			
		}
		for (int i = 0; i <n_items; i++)
		{
			for (int j = 0; j <K; j++)
			{
				/* code */
				Q[i][j]=((double) rand() / (RAND_MAX)); // random
			}
			/* code */
		}
	}
	void showBias()
	{
		cout<<"Bias para usuario:"<<endl;
		for (int i = 0; i < n_usuarios; ++i)
		{
			cout<<b_u[i]<<"  ";
			/* code */
		}	
		cout<<endl<<"Bias para items:"<<endl;
		for (int i = 0; i < n_items; ++i)
		{
			cout<<b_i[i]<<"  ";
			/* code */
		}
		cout<<endl;
	}
	void set_Bias()
	{

		for (int i = 0; i < n_usuarios; ++i)
		{
			b_u[i]=0;
			/* code */
		}	
		for (int i = 0; i < n_items; ++i)
		{
			b_i[i]=0;
			/* code */
		}
		DATO sum=0;
		int contador=0;
		for(int i=0;i<n_usuarios;i++)
			for(int j=0;j<n_items;j++)
				if(Ratings[i][j]!=0)
				{
					sum+=Ratings[i][j];
					contador++;
				}
		this->b=sum/contador;

	}
	void set_samples()
	{
		for(int i=0;i<n_usuarios;i++)
			for(int j=0;j<n_items;j++)
				if(Ratings[i][j]!=0)
				{
					valoracion * nueva = new valoracion;
					nueva->i=i;
					nueva->j=j;
					nueva->v=Ratings[i][j];
					samples.push_back(nueva);
				}

	}
	void parallel_sgd(vector<valoracion*> * trabajo, int thread_)
	{

		DATO prediccion;
		DATO e;
		vector<valoracion*>::iterator it;
		for(it = trabajo->begin() ; it != trabajo->end(); ++it)
		{
			int i=(*it)->i;
			int j=(*it)->j;
			int v=(*it)->v;
			//cout<<"Thread("<<thread_<<"): "<<i<<" "<<j<<" "<<v<<endl;
			prediccion=get_rating(i,j);
			e = (v-prediccion);	
			//omp_set_lock(&writelock);
			mu.lock();
			this->b_u[i]+=this->alpha*(e-this->beta*this->b_u[i]);
			this->b_i[j]+=this->alpha*(e-this->beta*this->b_i[j]);
			mu.unlock();
			//omp_unset_lock(&writelock);
			//showBias();
			for(int a=0;a<K;a++)
				{
					//omp_set_lock(&writelock);
					mu.lock();	
					P[i][a]+=alpha*(2*e*Q[a][j]-beta*P[i][a]);
					Q[a][j]+=alpha*(2*e*P[i][a]-beta*Q[a][j]);
					mu.unlock();
					//omp_unset_lock(&writelock);
				}
		}
		
	}
	void sgd()
	{
		
		vector<valoracion*>::iterator it;
		vector<valoracion*> ** trabajo = new vector<valoracion*>*[num_threads];
		for(int i=0;i<num_threads;i++)
		{
			trabajo[i]=new vector<valoracion*>;
		}
	int estado=0;
	for(it = samples.begin() ; it != samples.end(); ++it){
		//cout<<"estado: "<<estado<<endl;
			trabajo[estado]->push_back((*it));
			if(estado>=3)
				estado=0;
			else
				estado++;
			
			//omp_unset_lock(&writelock);
    	//}
		}
		for(int a=0;a<num_threads;a++)
			{
				t[a]=thread(&MF::parallel_sgd,this,trabajo[a],a);
			}
			for(int a=0;a<num_threads;a++)
				t[a].join();

			
	
	}
	void static_sgd()
	{
		vector<valoracion*>::iterator it;
		DATO prediccion;
		DATO e;
		for(it = samples.begin() ; it != samples.end(); ++it)
		{
			int i=(*it)->i;
			int j=(*it)->j;
			int v=(*it)->v;
			prediccion=get_rating(i,j);
			e = (v-prediccion);	
			this->b_u[i]+=this->alpha*(e-this->beta*this->b_u[i]);
			this->b_i[j]+=this->alpha*(e-this->beta*this->b_i[j]);
			for(int a=0;a<K;a++)
				{
					P[i][a]+=alpha*(e*Q[j][a]-beta*P[i][a]);
					Q[j][a]+=alpha*(e*P[i][a]-beta*Q[j][a]);
				}
		}
	}
	DATO get_rating(int i, int j)
	{
		DATO sum_P=0;
		for(int a=0;a<K;a++)
		{
			sum_P+=P[i][a]*Q[j][a];
		}
		DATO prediccion = this->b+this->b_u[i]+this->b_i[j]+sum_P;
		return prediccion;
	}
	void shuffle()
	{
		int dato1;
		int dato2;
		for(int i=0;i<n_usuarios/2;i++)
		{

			do
			{
				dato1=rand() % n_usuarios;
				dato2=rand() % n_usuarios;
			}while(dato1!=dato2);
			swap(Ratings[dato1],Ratings[dato2]);
		}
	}
	void entrenamiento()
	{
		
		setP_Q();
		set_Bias();
		shuffle();
		set_samples();
		GET_TIME(start);
		for(int i=0;i<n_iteraciones;i++)
		{
			random_shuffle ( samples.begin(), samples.end() );
			static_sgd();
		}
		GET_TIME(finish);
  		elapsed = finish - start;
  		cout<<"Tiempo transcurrido para sgd:"<<elapsed<<endl;


		
	}
	void mostrar_P_Q()
	{
		cout<<"P:"<<endl;
		for (int i = 0; i < n_usuarios; ++i)
		{
			for (int j=0;j<K;j++)
			{
				cout<<P[i][j]<<"   ";
				/* code */
			}
			cout<<endl;
			/* code */
		}
		cout<<"Q"<<endl;
		for (int i = 0; i <K; ++i)
		{
			for (int j=0;j<n_items;j++)
			{
				cout<<Q[i][j]<<"   ";
				/* code */
			}
			cout<<endl;
			/* code */
		}
	}
	void generar_matriz()
	{
		DATO ** transpuesta;
		transpuesta = new DATO*[K];
		for(int i=0;i<K;i++)
		{
			transpuesta[i]=new DATO[n_items];
		}
		for (int i = 0; i < n_items; i++)
		{
			for(int j=0;j<K;j++)
			{
				transpuesta[j][i]=Q[i][j];
			}
			/* code */
		}
		DATO aux_suma;
		for(int i=0;i<n_usuarios;i++)
		{
			//cout<<"i:"<<i<<endl;
			for(int j=0;j<n_items;j++)
			{
				aux_suma=0;
				for(int a=0;a<K;a++)
				{
					//cout<<"j:"<<j<<endl;
					//cout<<P[i][a]<<"*"<<transpuesta[a][j]<<endl;
					aux_suma+=P[i][a]*transpuesta[a][j];
				}
					
				VE_Rating[i][j]=aux_suma;
			}
			//cout<<endl;
		}
		//mostrar_matriz(P,n_usuarios,K);
		//mostrar_matriz(Q,n_items,K);
		//mostrar_P_Q();
		//mostrar_matriz(VE_Rating,n_usuarios,n_items);



	}
	void mostrar_matriz(DATO ** M, int a, int b)
	{
		for (int i = 0; i < a; i++)
		{
			for(int j=0;j<b;j++)
			{
				cout<<M[i][j]<<"    ";
			}
			cout<<endl;
			/* code */
		}
	}
};
int main()
{
	srand (time(NULL));
	int n_usuarios=3000;
	int n_items=3000;
	int K=2;
	DATO alpha=0.0001;
	DATO beta=0.0001;
	int n_iteraciones=10;
	DATO ** R = new DATO*[n_usuarios];
	for(int i=0;i<n_usuarios;i++)
	{
		R[i]=new DATO[n_items];
		for(int j=0;j<n_items;j++)
		{
			R[i][j]=rand() % 5;
			//cout<<R[i][j]<<"   "<<endl; 
		}
	}

	MF mimatrix(R,n_usuarios,n_items,K,alpha,beta,n_iteraciones);
	mimatrix.entrenamiento();
	mimatrix.generar_matriz();
	return 0;
}