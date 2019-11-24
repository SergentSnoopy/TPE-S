#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>


#define TAIILETANPOM 12

typedef struct FICHIER
{
	int desc;
	void * tanpom;
	int taille;
	int offset;
	char mode;
}FICHIER;

FICHIER* ouvrir(char *nom, char mode){
	int desc;
	switch(mode)
	{
		case 'L':desc=open(nom,O_RDONLY);break;
		case 'E':desc=open(nom,O_WRONLY);break;
		default:return NULL;break;
	}
	if (desc==-1)
	{
		return NULL;
	}
	FICHIER* f = malloc(sizeof(FICHIER));
	f->desc=desc;
	f->taille=TAIILETANPOM;
	f->offset=0;
	f->tanpom=malloc(sizeof(char) * TAIILETANPOM);
	f->mode=mode;
	return f;
}

int fermer(FICHIER *f)
{
	if (f!=NULL)
	{
		write(f->desc,f->tanpom,f->offset);
		int res=close(f->desc);
		free(f->tanpom);
		free(f);
		return res;
	}
	return -1;
}

int lire(void*p,unsigned int taille,unsigned int nbelem,FICHIER* f)
{
	if(f->mode == 'L')
	{
		
		//initialisation du tanpom
		if (f->offset==0)
		{
			read(f->desc,f->tanpom,f->taille);
		}
		
		//fin du tanpom
		if(f->offset+sizeof(char)*taille*nbelem >= f->taille)
		{
			memcpy(p,f->tanpom+f->offset,f->taille-f->offset);
			read(f->desc,f->tanpom,f->taille);
			memcpy(p+(f->taille-f->offset),f->tanpom,sizeof(char)*taille*nbelem-(f->taille-f->offset));
		}
		else{
			memcpy(p,f->tanpom+f->offset,sizeof(char)*taille*nbelem);
		}
		
		f->offset=(f->offset+sizeof(char)*taille*nbelem)%f->taille;
		return nbelem;
	}
	else return -1;
}

int ecrire(void*p,unsigned int taille,unsigned int nbelem,FICHIER* f)
{
	if(f->mode =='E')
	{
		
		if(f->offset+taille*nbelem >= f->taille)
		{
			int nbplacer=0;
			while(f->offset+taille<f->taille)
			{
				memcpy(f->tanpom+f->offset,p+(nbplacer*taille),taille);
				f->offset=f->offset+(taille);
				nbplacer++;
			}
			write(f->desc,f->tanpom,f->offset);
			memcpy(f->tanpom,p+(nbplacer*taille),taille*(nbelem-nbplacer));
			f->offset=taille*(nbelem-nbplacer);
		}
		else
		{
			memcpy(f->tanpom+f->offset,p,taille*nbelem);
			f->offset=f->offset+taille*nbelem;
		}
		
		return nbelem;
	}
	else return -1;
}

int fecriref(FICHIER* fp, char * format, ... ) {

    va_list ap;
    va_start( ap, format );

    int nb = 0;
    char* current;

    while( (*current = *format) != '\0' ) {
        format ++;
        if ( *current != '%' ) {
            ecrire(current,sizeof(char),1,fp);
            nb ++;
            continue;
        }

        switch( *format ++ ) {
            case 'c':
                {
                    int* charvalue=malloc(sizeof(char));
					*charvalue=va_arg(ap,int);
					ecrire(charvalue,sizeof(char),1,fp);
                }
                break;
            case 's':
                {
                   const char * string = (const char *) va_arg( ap, const char * );
                    while( *string != '\0' ) {
						ecrire((void *)string++,sizeof(char),1,fp);
                        nb ++;
                    }
                }
                break;
			case 'd':
                {
					//todo bug, l'int s'affiche mal
                    int *integer = malloc(sizeof(int));
					*integer=va_arg( ap, int );
					//printf("%c\n",*integer);
					ecrire( (void*)integer,sizeof(int),1,fp);
                    nb += *integer;
                }
                break;
            default:
                break;
        }
    }

    va_end( ap );
    return nb;
}


int fliref(FICHIER *fp,char *format,...){
	
	va_list ap;
    va_start( ap, format );

    int nb = 0;
    char* current=malloc(sizeof(char));
	char* waste=malloc(sizeof(char)*10);

    while( (*current = *format) != '\0' ) {
        format ++;
        if ( *current != '%' ) {
            nb ++;
            continue;
        }
		if(*format ==' ')
		{
			lire((void*)waste,sizeof(char),5,fp);
		}
        switch( *format ++ ) {
            case 'c':
                {
                    char* charvalue=va_arg(ap,char*);
					lire((void*)charvalue,sizeof(char),1,fp);
                }
                break;
            case 's':
                {
                   char * string = (char *) va_arg( ap,char * );
                    while( *string != ' ' ) {
						lire((void *)string,sizeof(char),1,fp);
                        nb ++;
                    }
					//*string='\0';
                }
                break;
			case 'd':
                {
					
                }
                break;
            default:
                break;
        }
    }

    va_end( ap );
    return nb;
	return 0;
}

int main() {
	FICHIER * f;
	FICHIER * f2;
	FICHIER * f3;
	FICHIER * f4;
	f=ouvrir("./plop.txt",'L');
	char* s=malloc(sizeof(char)*10);
	char* s2=malloc(sizeof(char)*10);
	lire((void*)s,1,10,f);
	lire((void*)s2,1,10,f);
	
	f2=ouvrir("ecriture.txt",'E');
	ecrire((void*)s,1,5,f2);
	ecrire((void*)s2,1,10,f2);
	
	f3=ouvrir("test_fecriref.txt",'E');
	//fecriref(f3," %s","bon");
	fecriref(f3," %c %s 12\n",'a', "bonjour");
	fecriref(f3," %d \n",6565);
	/*
	f4=ouvrir("test_fliref.txt",'L');
	char *c=malloc(sizeof(char)*2);
	char * str=malloc(sizeof(char)*10);
	int d;
	fliref(f4,"%c %s %d",c,str,d);
	
	printf("%c",*c);
	printf("%s",str);
	*/
	fermer(f);
	fermer(f2);
	fermer(f3);
	//fermer(f4);
	return 0;
}
