/*
Este programa implementa un sencillo compresor
de Huffman, que podria usarse para un programa
de mayor envergadura.

comprimir: huf C entrada salida
descompr.: huf D entrada salida
info:      huf I entrada
*/

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

struct nodo
{
    struct nodo *der,*izq,*arr;  /* forma el nodo */
    int cuenta;                  /* apariciones del carÃƒÂ¡cter */
    char bit;                    /* 0 o 1 */
    unsigned char karacter;      /* el carÃƒÂ¡cter (para la descompresiÃƒÂ³n */
    char *codigo;                /* cadena de ceros y unos con la codificaciÃƒÂ³n */
    char nbits;                  /* me apunto el numero de bits que codifican el carÃƒÂ¡cter */
}HOJAS[256],*TELAR[256],*MENOR,*SEGUNDO;

int NSIMB=0,nsimb;
FILE *f,*g;
int NBYTES=0;

/*--------------------------------
preparar las hojas
--------------------------------*/
int preparar_hojas(char *archivo)
{
    int j;
    for(j=0;j<256;++j){
        HOJAS[j].der=HOJAS[j].izq=HOJAS[j].arr=NULL;
        HOJAS[j].cuenta=0;
        HOJAS[j].karacter=j;
        HOJAS[j].codigo=NULL;
    }
    if ((f=fopen(archivo,"rb"))!=NULL){
        while ((j=fgetc(f))!=EOF){
            ++HOJAS[j].cuenta;
            ++NBYTES;
        }
        fclose(f);
    }
    else
    {
        return(1);
    }
    for(j=0;j<256;++j){
        if (HOJAS[j].cuenta!=0)
            ++NSIMB;
    }
    nsimb=NSIMB;
    return(0);
}

/*--------------------------------
preparar telar
--------------------------------*/
void preparar_telar()
{
    int j;
    for(j=0;j<256;++j){
        TELAR[j]=&(HOJAS[j]);
    }
    return;
}

/*--------------------------------
tejer el ÃƒÂ¡rbol
--------------------------------*/
void tejer()
{
    int menor=-1;     /* guarda indice */
    int segundo=-1;   /* guarda indice */
    int temporal;     /* guarda cuenta */
    int j;
    struct nodo *P;   /* nuevo nodo */

    if (nsimb==1) return;

    /* buscar menor valor */
    for(j=0;j<256;++j){
        if (TELAR[j]==NULL) continue;
        if (TELAR[j]->cuenta==0) continue;
        if (menor==-1){
            menor=j;
            temporal=TELAR[j]->cuenta;
        } else
        {
            if (TELAR[j]->cuenta<temporal){
                menor=j;
                temporal=TELAR[j]->cuenta;
            }
        }
    }

    /* buscar segundo menor */
    for(j=0;j<256;++j){
        if (TELAR[j]==NULL) continue;
        if (TELAR[j]->cuenta==0) continue;
        if (j==menor) continue;
        if (segundo==-1){
            segundo=j;
            temporal=TELAR[j]->cuenta;
        } else
        {
            if (TELAR[j]->cuenta<temporal){
                segundo=j;
                temporal=TELAR[j]->cuenta;
            }
        }
    }

    /* tejer un nuevo nodo */
    P=(struct nodo *)malloc(sizeof(struct nodo));
    TELAR[menor]->arr=P;
    TELAR[segundo]->arr=P;
    P->izq=TELAR[menor];
    P->der=TELAR[segundo];
    P->arr=NULL;
    TELAR[menor]->bit=0;
    TELAR[segundo]->bit=1;
    P->cuenta=TELAR[menor]->cuenta+TELAR[segundo]->cuenta;
    TELAR[menor]=NULL;
    TELAR[segundo]=P;
    --nsimb;

    /* sigue tejiendo hasta que sÃƒÂ³lo quede un nodo */
    tejer();
}

/*--------------------------------
Una vez construido el ÃƒÂ¡rbol, puedo codificar
cada carÃƒÂ¡cter. Para eso recorro desde la hoja
a la raÃƒÂ­z, apunto 0 o 1 en una pila y luego
paso la pila a una cadena. Un 2 determina el
fin de la cadena.
--------------------------------*/
void codificar()
{
    char pila[64];
    char tope;
    int j;
    char *w;
    struct nodo *P;
    for(j=0;j<256;++j){
        if (HOJAS[j].cuenta==0) continue;
        P=(struct nodo *)(&(HOJAS[j]));
        tope=0;
        while (P->arr!=NULL){
            pila[tope]=P->bit;
            ++tope;
            P=P->arr;
        }
        HOJAS[j].nbits=tope;
        HOJAS[j].codigo=(char *)malloc((tope+1)*sizeof(char));
        w=HOJAS[j].codigo;
        --tope;
        while (tope>-1){
            *w=pila[tope];
            --tope;
            ++w;
        }
        *w=2;
    }
    return;
}


/*--------------------------------
debug. Imprime la info sobre cada
carÃƒÂ¡cter, como nÃƒÂºmero de apariciones
y cadena con que se codifica
--------------------------------*/
void debug()
{
    int j,k;
    char *w;
    int tam_comprimido=0;
    for(j=0;j<256;++j){
        if (HOJAS[j].cuenta==0) continue;
        tam_comprimido+=(HOJAS[j].cuenta*HOJAS[j].nbits);
        printf("%3d %6d ",j,HOJAS[j].cuenta);
        w=HOJAS[j].codigo;
        while (*w!=2){
            printf("%c",48+(*w));
            ++w;
        }
        printf("\n");
    }
    //printf("NSIMB: %d\n",NSIMB);
    printf("NUMERO DE BYTES (ORIGINAL): %d\n",NBYTES);
    printf("TAMAÑO COMPRIMIDO: %d\n",tam_comprimido/8+1);
    return;
}

/*--------------------------------
Escribe la cabecera del archivo de
destino. La cabecera contiene: el
nÃƒÂºmero de bytes del archivo origen,
el nÃƒÂºmero de caracteres distintos
en ese archivo y una lista de parejas
nÃƒÂºmero de carÃƒÂ¡cter-cuenta de ese
carÃƒÂ¡cter. Eso es suficiente para la
descompresiÃƒÂ³n
--------------------------------*/
int escribe_cabecera(char *destino)
{
    int j,k;
    FILE *g;

    char *p=(char *)(&NBYTES);
    if ((g=fopen(destino,"wb"))==NULL) return(1);
    for(j=0;j<4;++j){
        fputc(*p,g);
        ++p;
    }

    p=(char *)(&NSIMB);
    fputc(*p,g);

    for(j=0;j<256;++j){
        if (HOJAS[j].cuenta==0) continue;
        fputc(j,g);
        p=(char *)(&(HOJAS[j].cuenta));
        for(k=0;k<4;++k){
            fputc(*p,g);
            ++p;
        }
    }
    fclose(g);
    return(0);
}

/*--------------------------------
Una vez construido el ÃƒÂ¡rbol y codificado
cada carÃƒÂ¡cter se puede proceder a la
compresiÃƒÂ³n: se tomarÃƒÂ¡ carÃƒÂ¡cter a carÃƒÂ¡cter
del archivo origen y se usarÃƒÂ¡ la cadena
de codificaciÃƒÂ³n para ir escribiendo
bits en un buffer de un carÃƒÂ¡cter, que
cada vez que quede lleno se pasarÃƒÂ¡ al
archivo de destino
--------------------------------*/
int comprimir(char *origen, char *destino)
{
    unsigned char d=0;
    int x;
    char nbit=0;
    char *p;

    if ((f=fopen(origen,"rb"))==NULL) return(1);
    if ((g=fopen(destino,"ab"))==NULL) return(2); /* ya esta la cabecera */

    while ((x=fgetc(f))!=EOF){
        p=HOJAS[x].codigo;
        while (*p!=2){
            if (nbit==8){
                nbit=0;
                fputc(d,g);
                d=0;
            } else
            {
                if (*p==1){
                    d|=(1<<nbit);
                }
                ++nbit;
                ++p;
            }
        }
    }
    fputc(d,g);
    fclose(f);
    fclose(g);
    return(0);
}

/*--------------------------------
Descomprime el archivo. El primer paso
es leer la cabecera, paso previo a la
descompresiÃƒÂ³n. Recuerdo formato de
la cabecera:
NBYTES|NSIMB|(char,cuenta)*
--------------------------------*/
int descomprimir(char *origen, char *destino)
{
    char *p;
    int j,k,n,m;
    unsigned char x,nbit;
    struct nodo *P,*Q;

    if ((g=fopen(origen,"rb"))==NULL) return(1);
    if ((f=fopen(destino,"wb"))==NULL) return(2);

    /* leer NBYTES */
    p=(char *)(&n);
    for(j=0;j<4;++j){
        *p=(unsigned char)fgetc(g);
        ++p;
    }
    NBYTES=n;

    /* leer NSIMB */
    NSIMB=nsimb=fgetc(g);

    /* preparar las hojas */
    for(j=0;j<256;++j){
        HOJAS[j].cuenta=0;
        HOJAS[j].izq=HOJAS[j].der=HOJAS[j].arr=NULL;
        HOJAS[j].karacter=j;
    }
    for(j=0;j<NSIMB;++j){
        n=fgetc(g);
        p=(char *)(&m);
        for(k=0;k<4;++k){
            *p=(unsigned char)fgetc(g);
            ++p;
        }
        HOJAS[n].cuenta=m;
    }

    /* construyo el ÃƒÂ¡rbol */
    preparar_telar();
    tejer();

    /* apunto a la raÃƒÂ­z del ÃƒÂ¡rbol */
    j=0;
    while (HOJAS[j].cuenta==0) ++j;
    P=(struct nodo *)(&(HOJAS[j]));
    while (P->arr!=NULL) P=P->arr;

    /* ahora ya se puede descomprimir */
    j=0;
    x=fgetc(g);
    nbit=0;
    Q=P;
    while(j<NBYTES){
        if (Q->izq==NULL){
            fputc(Q->karacter,f);
            Q=P;
            ++j;
        } else
        if (nbit==8){
            x=fgetc(g);
            nbit=0;
        } else
        {
            if (x&(1<<nbit)){
                Q=Q->der;
            }
            else
            {
                Q=Q->izq;
            }
            ++nbit;
        }
    }
    fclose(f);
    fclose(g);
    return(0);
}

void main()
{

    //int j;

    int opc=0;
    int aux=0;
    int aux1=0;
    char archivo_des [100];
    char archivo_com [100];
    do {

    printf("\t     ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ» \n");
    printf("\t     º                      HUFFMAN                        º \n");
    printf("\t     º          -------------------------------            º \n") ;
    printf("\t     ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍŒ\n\n\n");
    printf("1.) Comprimir.\n");
    printf("2.) Descomprimir.\n");
    printf("3.) Mostrar archvo a comprimir.\n");
    printf("4.) Mostrar archivo a descomprimir.\n");
    printf("5.) Mostrar archvio von datos decomprimidos.\n");
    printf("6.) Salir\n");
    printf("\nIngrese su opcion: ");
    scanf("%d",&opc);

    //if (argc<2) return;
    if (opc == 1){ /* comprimir */
        //if (argc!=4) return;
        printf("Ingrese el nombre del archivo a comprimir\n");
        char cadena [100];
        
        fflush(stdin);
        gets(cadena);
        strcat(cadena,".txt");
		strcpy(archivo_com,cadena);//copiamos depues de concatenar
        if (preparar_hojas(cadena)){
            printf("error abriendo archivo\n");
            return;
        }
        preparar_telar();
        tejer();
        codificar();
        if (escribe_cabecera("salida.txt")){
            printf("error abriendo archivo\n");
            return;
        }
        if (comprimir(cadena,"salida.txt")){
            printf("error abriendo archivo\n");
            return;
        }
        //INFO
        preparar_telar();
        tejer();
        codificar();
        debug();
       
        printf("COMPRIMIDO****\n");
        aux++;
    }
    else if (opc == 2){ /* descomprimir */
        //if (argc!=4) return;
        printf("Ingrese el nombre del archivo descoprimir: \n");
        char cadena [100];
        fflush(stdin);
        gets(cadena);
        strcat(cadena,".txt");
        
		strcpy(archivo_des,cadena);
        if (descomprimir(cadena,"Archivo_Descomprimido.txt")){
            printf("error abriendo archivo\n");
            return;
        }
        printf("DESCOMPRIMIDO****\n");
        aux1++;
    }
    else if(opc==3){
    	if(aux!=0){
    		system(archivo_com);
		}else{
			printf("\n Tien que seleciionar un archivo primero....\n");
		}
    	
	}
	else if(opc==4){
		if(aux1!=0){
    		system(archivo_des);
		}else{
			printf("\n Tien que seleciionar un archivo primero....\n");
		}
		
	}
	else if(opc==5){
		system("Archivo_Descomprimido.txt");
	}
    
    }while(opc != 6);
}

