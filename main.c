#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>

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
		
		
		if(f->offset+sizeof(char)*taille*nbelem >= f->taille)
		{
			int nbplacer=0;
			while(f->offset+sizeof(char)*taille<f->taille)
			{
				memcpy(f->tanpom+f->offset,p+(nbplacer*sizeof(char)*taille),sizeof(char)*taille);
				f->offset=f->offset+(sizeof(char)*taille);
				nbplacer++;
			}
			write(f->desc,f->tanpom,f->offset);
			memcpy(f->tanpom,p+(nbplacer*sizeof(char)*taille),sizeof(char)*taille*(nbelem-nbplacer));
			f->offset=sizeof(char)*taille*(nbelem-nbplacer);
		}
		else
		{
			memcpy(f->tanpom+f->offset,p,sizeof(char)*taille*nbelem);
			f->offset=f->offset+sizeof(char)*taille*nbelem;
		}
		
		return nbelem;
	}
	else return -1;
}

int main() {
	FICHIER * f;
	FICHIER * f2;
	f=ouvrir("./plop.txt",'L');
	char* s=malloc(sizeof(char)*10);
	char* s2=malloc(sizeof(char)*10);
	lire((void*)s,1,10,f);
	lire((void*)s2,1,10,f);
	f2=ouvrir("ecriture.txt",'E');
	ecrire((void*)s,1,5,f2);
	ecrire((void*)s2,1,10,f2);
	fermer(f);
	fermer(f2);
	return 0;
}
