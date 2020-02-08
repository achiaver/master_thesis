#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>	  
#include <string.h>	  
//#include <malloc.h>	  
#define MAX 10
#include "back_struct.h"

/*
********************************************************************************
			 Global Variables
********************************************************************************
*/
double eta, momentum, beta, error, threshold, ran3(), **ksi, **ksi_teste, 
       **desired_out, **desired_teste;

int    num_padroes, num_camadas, epocas, cam[MAX], num_teste, iseed = -17841;

float  contador_vp = 0.0, contador_fp = 0.0, contador_fn = 0.0, contador_vn = 0.0;

FILE   *fp, *dados_entrada, *dados_saida, *dados_teste, *dados_teste_saida,
       *impressao_leitura, *impressao_parametros, *impressao_leitura_teste,
       *impressao_erro, *impressao_output, *impressao_pesos, *impressao_teste,
       *confusao, *resultados_testes, *errorf;

camada_content * camada;

/*
********************************************************************************
			 Global Functions
********************************************************************************
*/

void   cabecalho(), parametros_iniciais_printar(), impressao_dos_parametros_arquivo(),
       leitura_dos_parametros(), check_allocation(), arquivos_leitura_escrita(),
       alocacao_e_leitura_conj_train_test(), treinamento_do_backpropagation(),
       impressao_dos_pesos_finais(), teste_da_rede(), respostas_da_rede_backpropagation(),
       fechar_programa();

/*
********************************************************************************
	         Main Multilayer Perceptron Simulator Program
********************************************************************************
*/

int main()
{

	/*   Leitura dos parametros da rede */

	leitura_dos_parametros ();

       /*   Alocar a rede e ler os conjuntos de train e test  */

       alocacao_e_leitura_conj_train_test();

       /*   Treinamento da Rede Backpropagation   */

       treinamento_do_backpropagation();
       
       /*   Pesos finais  */

       impressao_dos_pesos_finais();

       /*  Teste da Rede Backpropagation  */
	
        teste_da_rede();

       /*  Respostas da Rede  */

	respostas_da_rede_backpropagation();
        fechar_programa();
        
        return 0;
} /*  end main()  */



/*
********************************************************************************
			         Functions
********************************************************************************
*/



/*
********************************************************************************
called by: main.
functions called: arquivos_leitura_escrita, cabecalho, parametros_iniciais_printar,
                  impressao_dos_parametros_arquivo.

global variables: num_padroes, num_camadas, epocas, eta, momentum, beta, threshold,
                  num_teste, error, cam[MAX].

local variables: i.
 	
I/O conditions: 
********************************************************************************
*/
void leitura_dos_parametros()
{
	int i;

        arquivos_leitura_escrita();
	
        cabecalho();

	fscanf(fp, "%*s%d%*s %*s%d%*s %*s%d%*s %*s%lf%*s %*s%lf%*s %*s%lf%*s %*s%lf%*s %*s%d%*s %*s%lf%*s", 
               &num_padroes, &num_camadas, &epocas, &eta, &momentum, &beta, &threshold, &num_teste, &error);
               
	for (i = 0; i < num_camadas; i++) fscanf(fp, "%*s%d%*s", &(cam[i]));

        fclose(fp);
        
        parametros_iniciais_printar();

        impressao_dos_parametros_arquivo();
}

/*
********************************************************************************
called by: leitura_dos_parametros.

functions called: 

global variables: fp, dados_entrada, dados_saida, dados_teste, dados_teste_saida,
       impressao_leitura, impressao_parametros, impressao_leitura_teste,
       impressao_erro, impressao_output, impressao_pesos, impressao_teste,
       confusao, resultados_testes, errorf;

local variables:
 	
I/O conditions: 
********************************************************************************
*/
void arquivos_leitura_escrita()
{
        fp = fopen("in_parametros.dat", "r");
        impressao_parametros = fopen("./Respostas_da_rede/parametros_entrada.txt", "w");
        resultados_testes = fopen("./Respostas_da_rede/respostas_testes.txt", "a");
        impressao_leitura = fopen("./Respostas_da_rede/leitura_conj_treinamento.txt", "w");
        impressao_leitura_teste = fopen("./Respostas_da_rede/leitura_conj_teste.txt", "w");
        confusao = fopen("./Respostas_da_rede/respostas_da_rede_backpropagation.txt", "w");
        errorf = fopen("./Respostas_da_rede/erro_alocacao.txt", "w");
	impressao_erro = fopen("./Respostas_da_rede/erro_da_rede.txt", "w");
	impressao_output = fopen("./Respostas_da_rede/resultado_treinamento.txt", "w");	
        impressao_pesos = fopen("./Respostas_da_rede/pesos_iniciais_finais.txt", "w");
        impressao_teste = fopen("./Respostas_da_rede/resultado_teste.txt", "w");
   	dados_entrada = fopen("./conjunto_de_treinamento/BNDES_30percent_var9_var10_procv_entrada.txt","r");
        dados_saida = fopen("./conjunto_de_treinamento/BNDES_30percent_var9_var10_procv_saida.txt","r");
        dados_teste = fopen("./conjunto_de_teste/BNDES_70percent_var9_var10_procv_entrada.txt","r");
        dados_teste_saida = fopen("./conjunto_de_teste/BNDES_70percent_var9_var10_procv_saida.txt","r");
}

/*
********************************************************************************
called by: leitura_dos_parametros.

functions called: 

global variables:

local variables:
 	
I/O conditions: 
********************************************************************************
*/
void cabecalho()
{
	printf("\n\t UERJ");
        printf("\n\t Mestrado de Ciencias Computacionais");
        printf("\n\t Aluno: Raphael Silva de Figueiredo");
	printf("\n\t Perceptron Multicamadas - Backpropagation\n");
}

/*
********************************************************************************
called by: leitura_dos_parametros.

functions called: 

global variables: num_padroes, epocas, eta, momentum, beta, threshold, num_teste,
                  error, num_camadas, cam[MAX].

local variables: i.
 	
I/O conditions: 
********************************************************************************
*/
void parametros_iniciais_printar()
{
	int i;
	printf("\n\t numero de padroes do conj.treinamento : %d", num_padroes);
	printf("\n\t numero de epocas                      : %d", epocas);
	printf("\n\t taxa de aprendizagem                  : %0.3lf", eta);
	printf("\n\t termo de momento                      : %0.1lf", momentum);
        printf("\n\t valor de beta                         : %0.1lf", beta);
        printf("\n\t valor do threshold                    : %0.1lf", threshold);
	printf("\n\t numero de padroes do conj.teste       : %d", num_teste);
        printf("\n\t valor de erro maximo                  : %lf\n", error);
	printf("\n\t numero de camadas                     : %d\n\n", num_camadas);

        for (i = 0; i < num_camadas; i++) 
	{
	    printf("\t neuronios da camada %d : %d\n", i, cam[i]); 		
	}
} 

/*
********************************************************************************
called by: leitura_dos_parametros.

functions called: 

global variables: num_padroes, epocas, eta, momentum, beta, threshold, num_teste,
                  error, num_camadas, cam[MAX].

local variables: i.
 	
I/O conditions: 
********************************************************************************
*/
void impressao_dos_parametros_arquivo()
{
	int i;
	fprintf(impressao_parametros,"\n UERJ");
	fprintf(impressao_parametros,"\n Mestrado de Ciencias Computacionais");
	fprintf(impressao_parametros,"\n Aluno: Raphael Silva de Figueiredo");
	fprintf(impressao_parametros,"\n Perceptron Multicamadas - Backpropagation\n");
	
	fprintf(impressao_parametros,"\n numero de padroes do conj.treinamento : %d", num_padroes);
	fprintf(impressao_parametros,"\n numero de epocas                      : %d", epocas);
	fprintf(impressao_parametros,"\n taxa de aprendizagem                  : %0.3lf", eta);
	fprintf(impressao_parametros,"\n termo de momento                      : %0.1lf", momentum);
	fprintf(impressao_parametros,"\n valor de beta                         : %0.1lf", beta);
	fprintf(impressao_parametros,"\n valor do threshold                    : %0.1lf", threshold);
	fprintf(impressao_parametros,"\n numero de padroes do conj.teste       : %d", num_teste);
	fprintf(impressao_parametros,"\n valor de erro maximo                  : %lf\n", error);
	fprintf(impressao_parametros,"\n numero de camadas                     : %d\n\n", num_camadas);
           
        for (i = 0; i < num_camadas; i++) 	
           fprintf(impressao_parametros," neuronios da camada %d: %d\n", i, cam[i]);
           fclose(impressao_parametros);
}

/*
********************************************************************************
called by: alocacao_e_leitura_conj_train_test.

functions called:
 
global variables: iseed, ran3.

local variables: inext, inextp, ma, iff, idum, mj, mk, i, ii, k, MBIG, MSEED,
                 MZ, FAC. 
 	
I/O conditions: 
********************************************************************************
*/
/* Gerador de números aleatórios */
#define	MBIG	1000000000
#define	MSEED	161803398
#define	MZ	0
#define	FAC	(1.0/MBIG)

int	inext, inextp;
long	ma[56];
int	iff = 0;

double
ran3(idum)
int	*idum;
{
/*	static int	inext, inextp;
	static long	ma[56];
	static int	iff = 0;  */
	long		mj, mk;
	int		i, ii, k;

	if (*idum < 0 || iff == 0)
	{
		iff = 1;
		mj = MSEED - (*idum < 0 ? -*idum : *idum);
		mj %= MBIG;
		ma[55] = mj;
		mk = 1;
		for (i = 1; i <= 54; i++)
		{
			ii = (21 * i) % 55;
			ma[ii] = mk;
			mk = mj - mk;
			if (mk < MZ) mk += MBIG;
			mj = ma[ii];	
		}   /* end for  */
		
		for (k = 1; k <= 4; k++)
			for (i = 1; i <= 55; i++) 
			{
				ma[i] -= ma[1+(i + 30) % 55];
				if (ma[i] < MZ) ma[i] += MBIG;
			} /*  end for  */
		inext = 0;
		inextp = 31;
		*idum = 1;
	} /*  end if  */

	if (++inext == 56) inext = 1;	
	if (++inextp == 56) inextp = 1;
	mj = ma[inext] - ma[inextp];
	if (mj < MZ) mj += MBIG;
	ma[inext] = mj;
        if (mj * FAC == 1.0) return (0.0);
	else	return (mj * FAC);
} /*  end ran3  */

/*
********************************************************************************
called by: alocacao_e_leitura_conj_train_test

functions called: 

global variables:

local variables: pt, in_funct, in_var.
 	
I/O conditions: 
********************************************************************************
*/
/*    Check allocation      */
	void check_allocation (pt, in_funct, in_var)
	void	*pt;
	char	in_funct[];
	char	in_var[];
	{
		if (pt == NULL)
		{

		  fprintf(errorf, "Run-time memory allocation error...\n");
		  fprintf(errorf, "In function: %s, variable: %s\n", in_funct, in_var);
		  fprintf(errorf, "...now exiting to system...\n");
		  exit(0);

                fclose(errorf);
		}

	}   /*  check_allocation ()   */

/*
********************************************************************************
called by: main.

functions called: check_allocation.

global variables: camada, num_camadas,cam[MAX], num_padroes, ran3, ksi, desired_out,
                  ksi_teste, desired_teste, num_teste.

local variables: i, j, k, p, t;
 	
I/O conditions: 
********************************************************************************
*/
void alocacao_e_leitura_conj_train_test()
{
int    i, j, k, p, t;
/* Alocar a Rede Neuronal */
       camada = (camada_content *) malloc (num_camadas * sizeof(camada_content)); 
       check_allocation( (void *)camada, "alocacao_e_leitura_conj_train_test()", "camada");

       for ( i = 0 ; i < num_camadas ; i++ ) 
         camada[i].num_neuronios = cam[i];

       for ( i = 0 ; i < num_camadas ; i++ )
       {
         camada[i].neuronio = (neuronio_content *) malloc ((camada[i].num_neuronios + 1) * sizeof(neuronio_content));
         check_allocation( (void *)camada[i].neuronio, "alocacao_e_leitura_conj_train_test()", "camada[i].neuronio");
         for (j = 0 ; j <= camada[i].num_neuronios ; j++ ) 
         {
           camada[i].neuronio[j].peso =  (double *) malloc (camada[i+1].num_neuronios * sizeof(double));
           check_allocation( (void *)camada[i].neuronio[j].peso, "alocacao_e_leitura_conj_train_test()", "camada[i].neuronio[j].peso");
           for (k = 0; k < camada[i+1].num_neuronios; k++)
	     camada[i].neuronio[j].peso[k] = ran3(&iseed);          
         }
       }   /* end for num_camadas */

/* Alocar conjunto treinamento  */
       ksi = (double **) malloc ( num_padroes * sizeof(double));
       check_allocation( (void *)ksi, "alocacao_e_leitura_conj_train_test()", "ksi");
       for (i = 0; i <  num_padroes; i++) 
       {
         ksi[i] = (double *) malloc ((camada[0].num_neuronios + 1)  * sizeof(double));
         check_allocation( (void *)ksi[i], "alocacao_e_leitura_conj_train_test()", "ksi[i]");
       }

       desired_out = (double **) malloc ( num_padroes * sizeof(double));
       check_allocation( (void *)desired_out, "alocacao_e_leitura_conj_train_test()", "desired_out");
       for (i = 0; i <  num_padroes; i++)
       {
         desired_out[i] = (double *) malloc (camada[num_camadas - 1].num_neuronios  * sizeof(double));
         check_allocation( (void *)desired_out[i], "alocacao_e_leitura_conj_train_test()", "desired_out[i]");
       }

/* Leitura do conjunto de treinamento  */
        fprintf(impressao_leitura,"\n");
        for (p = 0 ; p < num_padroes ; p++ )
        {
          fprintf(impressao_leitura," %-5d", p);
          for (j = 0; j < camada[0].num_neuronios ; j++)
          { 
              fscanf(dados_entrada,"%lf", &ksi[p][j]);  
              fprintf(impressao_leitura," %+lf ", ksi[p][j]);
          } 
            for (k = 0; k < camada[num_camadas - 1].num_neuronios ; k++)
            {
              fscanf(dados_saida,"%lf", &desired_out[p][k]);
              fprintf(impressao_leitura," %+lf " , desired_out[p][k]);
	    }  
        fprintf(impressao_leitura,"\n");
        }

/* Alocar conjunto teste  */
       ksi_teste = (double **) malloc ( num_teste * sizeof(double));
       check_allocation( (void *)ksi_teste, "alocacao_e_leitura_conj_train_test()", "ksi_teste");
       for (i = 0; i < num_teste; i++)
       { 
         ksi_teste[i] = (double *) malloc ((camada[0].num_neuronios + 1) * sizeof(double));
         check_allocation( (void *)ksi_teste[i], "alocacao_e_leitura_conj_train_test()", "ksi_teste[i]");
       }

       desired_teste = (double **) malloc ( num_teste * sizeof(double));
       check_allocation( (void *)desired_teste, "alocacao_e_leitura_conj_train_test()", "desired_teste");
       for (i = 0; i < num_teste; i++)
       { 
         desired_teste[i] = (double *) malloc (camada[num_camadas - 1].num_neuronios  * sizeof(double));
         check_allocation( (void *)desired_teste[i], "alocacao_e_leitura_conj_train_test()", "desired_teste[i]");
       }

/* Leitura do conjunto de teste  */
        fprintf(impressao_leitura_teste,"\n");
        for (t = 0 ; t < num_teste ; t++ )
        {
          fprintf(impressao_leitura_teste," %-5d", t);
          for (j = 0; j < camada[0].num_neuronios ; j++)
          { 
              fscanf(dados_teste,"%lf", &ksi_teste[t][j]); 
              fprintf(impressao_leitura_teste," %+lf ", ksi_teste[t][j]);
          } 
            for (k = 0; k < camada[num_camadas - 1].num_neuronios ; k++)
            {
              fscanf(dados_teste_saida,"%lf", &desired_teste[t][k]);
              fprintf(impressao_leitura_teste," %+lf " , desired_teste[t][k]);
	    }  
         fprintf(impressao_leitura_teste,"\n");
        }
}

/*
********************************************************************************
called by: main.

functions called: 

global variables: camada, num_camadas, cam[MAX], epocas, num_padroes, ksi,
                  threshold, eta, momentum, desired_out, error.

local variables: i, j, k, e, p, delta_w, erro, g_ativ, dg_ativ, sum.
 	
I/O conditions: 
********************************************************************************
*/
void treinamento_do_backpropagation()
{
int    i, j, k, e, p;
double delta_w, erro, g_ativ, dg_ativ, sum;
/* Impressao dos pesos iniciais */

       fprintf(impressao_pesos,"\nPesos Iniciais\n\n");
       for ( i = num_camadas - 1; i > 0; i-- )
       {
         for (j = 0 ; j < camada[i].num_neuronios ; j++ )
         {
           for (k = 0 ; k < camada[i-1].num_neuronios +1 ; k++ )       
           {
             fprintf(impressao_pesos,"%+lf\t", camada[i-1].neuronio[k].peso[j]);
           }
         fprintf(impressao_pesos,"\n");
         }
       }

       for (e = 0; e < epocas; e++)
       {
 	 fprintf(impressao_output,"\n\n epoca %d \n", e+1);
         erro = 0.0;
         for ( p = 0 ; p < num_padroes ; p++ )
	 {
           /*       Para frente   */						   	
           for ( i = 0 ; i < num_camadas ; i++ )
           {
	     for (j = 0 ; j < camada[i].num_neuronios ; j++ )
	     {	
	       camada[i].neuronio[j].h_in = 0.0;
	
	       if (i == 0) 
               {
                  camada[i].neuronio[j].V = ksi[p][j];
               }
               else     /* Estado do neuronio  */
	       {
	         for (k = 0 ; k < camada[i-1].num_neuronios + 1 ; k++ )
		   camada[i].neuronio[j].h_in += (camada[i-1].neuronio[k].peso[j] * camada[i-1].neuronio[k].V); 
	
	       camada[i].neuronio[j].V = tanh(beta * camada[i].neuronio[j].h_in);	  
	       } /*  end (else)  */			
	     } /*  end for (j)  */
           camada[i].neuronio[camada[i].num_neuronios].V = -threshold; 	      
           } /*  end for (i)  terminou de subir  */

           /*   Voltando: Backpropagation   */
		   
	   for ( i = num_camadas - 1; i > 0; i-- )      /*  Calculo dos Deltas - descida  */
           {
             for (j = 0 ; j < camada[i].num_neuronios ; j++ )
	     {
               camada[i].neuronio[j].delta = 0.0;
	    
	       if (i == num_camadas -1)	     /*  Calculo do delta para ultima camada  */
	       {
	         g_ativ = camada[i].neuronio[j].V;
                 dg_ativ = beta * (1 - (g_ativ * g_ativ));
	         camada[i].neuronio[j].delta =  dg_ativ * (desired_out[p][j] - camada[i].neuronio[j].V);
	       }	
	       else
	       {
                 sum = 0.0;
	         for (k = 0 ; k < camada[i+1].num_neuronios ; k++ )        /*  Calculo dos deltas das camadas intermediarias  */
                   sum += (camada[i].neuronio[j].peso[k] * camada[i+1].neuronio[k].delta);

                   g_ativ = camada[i].neuronio[j].V;
                   dg_ativ = beta * (1 - (g_ativ * g_ativ));
                   camada[i].neuronio[j].delta = ( dg_ativ * sum );
               }  /* end else   */
   	     }  /*  end for  num_neuronios  camada i */
	   }  /* end for num_camadas  */

	   for ( i = num_camadas - 1; i > 0; i-- )       /*  Atualizacao dos pesos após o calculo dos deltas  */
           {
             for (j = 0 ; j < camada[i].num_neuronios  ; j++ )
             {
               for (k = 0 ; k < camada[i-1].num_neuronios + 1 ; k++ )       
               { 
                 delta_w = eta * (camada[i].neuronio[j].delta * camada[i-1].neuronio[k].V);
                 camada[i-1].neuronio[k].peso[j] = camada[i-1].neuronio[k].peso[j] + momentum * delta_w;
               }
             }
           }  /*  end for num_camadas  */
	
           /* Impressao do output */

           for (j = 0 ; j < camada[num_camadas-1].num_neuronios ; j++ )
             fprintf(impressao_output,"\n padrao %d -  desired %+-12lf:   output = %+lf", p+1, desired_out[p][j], camada[num_camadas-1].neuronio[j].V);

           /* Impressao dos erros */

           for (j = 0 ; j < camada[num_camadas-1].num_neuronios ; j++ )
             erro += 0.5 * ( (desired_out[p][j] - camada[num_camadas-1].neuronio[j].V) * (desired_out[p][j] - camada[num_camadas-1].neuronio[j].V) );

         }/*  end for (p) num_padroes */ 

           fprintf(impressao_erro,"\n epoca = %-5d:   erro = %lf",e+1, erro);

          if (erro < error ) break;

       /*  Impressao do progresso de epocas   */

       printf("\b\b\b\b\b\b %d", e+1); fflush(stdout);

       }/*  end for (e)  */
printf("\n");
}

/*
********************************************************************************
called by: main.

functions called: 

global variables: num_camadas, cam[MAX].

local variables: i, j, k.
 	
I/O conditions: 
********************************************************************************
*/
void impressao_dos_pesos_finais()
{
int    i, j, k;
       /* Impressao dos pesos finais */   

       fprintf(impressao_pesos,"\nPesos Finais\n\n");
       for ( i = num_camadas - 1; i > 0; i-- )
       {
         for (j = 0 ; j < camada[i].num_neuronios ; j++ )
         {
           for (k = 0 ; k < camada[i-1].num_neuronios +1 ; k++ )       
           {
             fprintf(impressao_pesos,"%+lf\t", camada[i-1].neuronio[k].peso[j]);
           }
         fprintf(impressao_pesos,"\n");
         }
       }
}

/*
********************************************************************************
called by: main.

functions called: 

global variables: num_teste, num_camadas, cam[MAX], ksi_teste, camada, contador_vp,
                  contador_fn, contador_fp, contador_vn, desired_teste.

local variables: t, i, j, k.
 	
I/O conditions: 
********************************************************************************
*/
void teste_da_rede()
{
int    t, i, j, k;
        for (t = 0 ; t < num_teste ; t++ )
        {
          for ( i = 0 ; i < num_camadas ; i++ )
          {	
	     for (j = 0 ; j < camada[i].num_neuronios ; j++ )
	     {	
	       camada[i].neuronio[j].h_in = 0.0;
	       if (i == 0) 
               {
                  camada[i].neuronio[j].V = ksi_teste[t][j];
               }
               else
	       {
	         for (k = 0 ; k < camada[i-1].num_neuronios ; k++ )
                   camada[i].neuronio[j].h_in += (camada[i-1].neuronio[k].peso[j] * camada[i-1].neuronio[k].V); 
	           camada[i].neuronio[j].V = tanh(beta * camada[i].neuronio[j].h_in);
               }
             }
          }
          for (j = 0 ; j < camada[num_camadas-1].num_neuronios ; j++ )
          {
             if (camada[num_camadas-1].neuronio[j].V >= 0.0) 
             {
                camada[num_camadas-1].neuronio[j].V2 = 1.0;
             }
             else
             {
                camada[num_camadas-1].neuronio[j].V2 = -1.0;
             }
             if (desired_teste[t][j] == 1.0 && camada[num_camadas-1].neuronio[j].V2 == 1.0) 
             {
		contador_vp = contador_vp + 1;
             }
             else if (desired_teste[t][j] == 1.0 && camada[num_camadas-1].neuronio[j].V2 == -1.0) 
             {
		contador_fn = contador_fn + 1;
             }
             else if (desired_teste[t][j] == -1.0 && camada[num_camadas-1].neuronio[j].V2 == 1.0) 
             {
		contador_fp = contador_fp + 1;
             }
             else if (desired_teste[t][j] == -1.0 && camada[num_camadas-1].neuronio[j].V2 == -1.0) 
             {
		contador_vn = contador_vn + 1;
             }
           fprintf(impressao_teste,"\npadrao_teste %-7d -  desired %+-12lf:   output = %+lf", t+1, desired_teste[t][j], camada[num_camadas-1].neuronio[j].V);
           }
        }
}

/*
********************************************************************************
called by: main.

functions called: 

global variables: contador_vp, contador_fn, contador_fp, contador_vn.

local variables: acerto, sensibilidade, especificidade, vpp, vpn.
 	
I/O conditions: 
********************************************************************************
*/
void respostas_da_rede_backpropagation()
{

float  acerto, sensibilidade, especificidade, vpp, vpn;

        acerto = ((contador_vp + contador_vn)/(contador_vp + contador_vn + contador_fn + contador_fp))*100;
	sensibilidade = (contador_vp/(contador_vp + contador_fn))*100;
	especificidade = (contador_vn/(contador_vn + contador_fp))*100;
	vpp = (contador_vp/(contador_vp + contador_fp))*100;
	vpn = (contador_vn/(contador_vn + contador_fn))*100;

	fprintf(confusao,"\n VP %-5.0f FP %.0f" , contador_vp, contador_fp);
        printf("\n VP %-5.0f FP %.0f" , contador_vp, contador_fp);
	fprintf(confusao,"\n FN %-5.0f VN %.0f" , contador_fn, contador_vn);
        printf("\n FN %-5.0f VN %.0f" , contador_fn, contador_vn);

	fprintf(confusao,"\n\n acurácia: %.2f %%" , acerto);
        printf("\n\n acuracia: %.2f %%" , acerto);
	fprintf(confusao,"\n erro: %.2f %%" , 100 - acerto);
        printf("\n erro: %.2f %%" , 100 - acerto);

	fprintf(confusao,"\n\n sensibilidade: %.2f %%" , sensibilidade);
        printf("\n\n sensibilidade: %.2f %%" , sensibilidade);
	fprintf(confusao,"\n especificidade: %.2f %%" , especificidade);
        printf("\n especificidade: %.2f %%" , especificidade);

	fprintf(confusao,"\n\n vpp: %.2f %%" , vpp);
        printf("\n\n vpp: %.2f %%" , vpp);
	fprintf(confusao,"\n vpn: %.2f %%" , vpn);
        printf("\n vpn: %.2f %% \n\n" , vpn);

        fprintf(resultados_testes,"\n%d	%d	%.2f	%.2f	%.2f	%.2f	%.2f	%.3f   %d", cam[1], cam[2], acerto, sensibilidade, especificidade, vpp, vpn, eta, epocas);
}

/*
********************************************************************************
called by: main.

functions called:
 
global variables: resultados_testes, impressao_leitura, impressao_leitura_teste,
                  confusao, impressao_erro, impressao_output, impressao_pesos,
                  impressao_teste, dados_entrada, dados_saida.

local variables:
 	
I/O conditions: 
********************************************************************************
*/
void fechar_programa()
{
       fclose(resultados_testes);
       fclose(impressao_leitura);
       fclose(impressao_leitura_teste);
       fclose(confusao);
       fclose(impressao_erro);
       fclose(impressao_output);
       fclose(impressao_pesos);
       fclose(impressao_teste);
       fclose(dados_entrada);
       fclose(dados_saida);
}

