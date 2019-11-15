#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>

typedef struct FICHIER
{
	int desc;
	void * tanpom;
	int taille;
	int offset;
	int debut;
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
	f->taille=INT_MAX;
	f->offset=0;
	f->tanpom=malloc(sizeof(char) * INT_MAX);
	f->debut=0;
	f->mode=mode;
	return f;
}

int fermer(FICHIER *f)
{
	if (f!=NULL)
	{
		int res=close(f->desc);
		free(f->tanpom);
		free(f);
		return res;
	}
	return -1;
}

int lire(void*p,unsigned int taille,unsigned int nbelem,FICHIER* f)
{
	//initialisation du tanpom
	if (f->offset==f->debut)
	{
		read(f->desc,f->tanpom,INT_MAX);
	}
	
	//fin du tanpom
	if(f->offset+sizeof(char)*taille*nbelem >= f->taille)
	{
		f->debut=f->offset;
		
	}
	
	memcpy(p,f->tanpom+f->offset,sizeof(char)*taille*nbelem);
	f->offset=f->offset+sizeof(char)*taille*nbelem;
	return nbelem;
	
}

int ecrire(void*p,unsigned int taille,unsigned int nbelem,FICHIER* f)
{
	memcpy(f->tanpom+f->offset,p,sizeof(char)*taille*nbelem);
	f->offset=f->offset+sizeof(char)*taille*nbelem;
	int r=write(f->desc,f->tanpom+f->debut,f->offset);
	printf("%i",r);
	return nbelem;
}

int main() {
	FICHIER * f;
	FICHIER * f2;
	f=ouvrir("./plop.txt",'L');
	printf("%i\n",f->desc);
	char* s=malloc(sizeof(char)*10);
	lire((void*)s,1,4,f);
	printf("%s",s);
	lire((void*)s,1,5,f);
	printf("%s",s);
	f2=ouvrir("ecriture.txt",'E');
	ecrire((void*)s,1,5,f2);
	fermer(f);
	fermer(f2);
	return 0;
}
