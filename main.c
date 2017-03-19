#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define FILE_NAME_LENGTH 255
#define FILE_CONTENT_LENGTH 255

typedef struct {
    int childProcessID;
    int fileDescriptorID[2];
} PipeWithFork ;

/*Fonksiyon prototipleri*/
int listDir(char* path);
char* readFile(char* fileName);
void pipeFunction(char *directoryPath);


int main(int argc, char *argv[]) {
    //Usage
    if(argc!=2)
    {
        fprintf(stdout,"Usage:%s processes \n",argv[0]);
        return 1;
    }
    pipeFunction(argv[1]);
}
/*Dosyalar arasindaki degerleri pipe ile aktarmak icin kullanilan fonksiyon*/
void pipeFunction(char *directoryPath)
{
    PipeWithFork *pipeConnectionArray;
    int fileCount=0;
    char buf[]="g";
    int comeIn=0;
    pid_t childpid=0;
    pid_t cPid=0;
    pid_t returnPid;
    //pipe icin fileDescriptor
    int fd[2];
    int fileDescriptorID=0;
    int i,n,j;
    int forkIndex;
    int index=0;
    int status=0;
    int listCount=0;
    DIR *d;
    struct dirent *dir;
    char* ch;
    char* dizi=NULL;
    int forkCount=0;
    //Malloc ile yer alinan matrisler
    char **fileNamesMatrix;
    char **WriteFileAfterPipe;

    fileCount=listDir(directoryPath);

    /*PipeWithFork Struct'ımıza ait dinamik Array olusturmak*/
    pipeConnectionArray=(PipeWithFork*)malloc(sizeof(PipeWithFork)*fileCount);


    /******************************************************************/

    /*-----------------DosyaIsimleriArrayeYerlestirildi---------------*/

    /******************************************************************/
    /*Dosya isimlerini tutabilmek icin malloc ile yer acildi*/
    fileNamesMatrix = (char**) malloc(fileCount * sizeof(char*));

    if(NULL == fileNamesMatrix){
        free(fileNamesMatrix);
        printf("fileNamesMatrix[] için hafıza ayırırken hata oluştu.n");
        exit(-1);
    }

    for (i=0;i<fileCount;i++){
        fileNamesMatrix[i] = (char*) malloc(FILE_NAME_LENGTH * sizeof(char));
        if(NULL == fileNamesMatrix[i]){
            free(fileNamesMatrix[i]);
            printf("fileNamesMatrix[i][] için hafıza ayırırken hata oluştu.n");
            exit(-1);
        }
    }
    //fprintf(stdout,"------");
    /*fileNameMatrix directorynin icindeki file isimleri ile dolduruldu*/
    i=0;
    d = opendir(directoryPath);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if(strcmp(dir->d_name, ".") != 0  && strcmp(dir->d_name, "..") != 0 && strcmp(dir->d_name,"...")!=0)
            {
                strcpy(fileNamesMatrix[i],dir->d_name);
                //printf("File Names---->%s\n",fileNamesMatrix[i]);
                ++i;
            }
        }
        closedir(d);
    }
    printf("i=%d",i);
    //writePipeFile(fileNamesMatrix,i);
    /*************************************************************************/

    /*-----------------------ForWriteToFileFromPipe--------------------------*/

    /*************************************************************************/
    /*Pipe dan gelen degerleri dosyalara kaydetmek icin malloc ile yer acildi*/
    WriteFileAfterPipe = (char**) malloc(fileCount * sizeof(char*));

    if(NULL == WriteFileAfterPipe){
        free(WriteFileAfterPipe);
        printf("WriteFileAfterPipe[] için hafıza ayırırken hata oluştu.n");
        exit(-1);
    }

    for (i=0;i<fileCount;i++){
        WriteFileAfterPipe[i] = (char*) malloc(FILE_CONTENT_LENGTH * sizeof(char));
        if(NULL == WriteFileAfterPipe[i]){
            free(WriteFileAfterPipe[i]);
            printf("WriteFileAfterPipe[i][] için hafıza ayırırken hata oluştu.n");
            exit(-1);
        }
    }
    /****************************************************************************/

    for(i=0;i<fileCount;++i){
        if(pipe(pipeConnectionArray[i].fileDescriptorID)==-1)
        {
            perror("Failed to create the synchronization pipe");
            exit(1);
        }
    }

    for(i=0;i<fileCount;++i)
    {
        int j=0;
        int writeFD;
        if((childpid=fork())==0){
            char ar[255];

            cPid=getpid();
            close(pipeConnectionArray[i].fileDescriptorID[0]);
            writeFD = pipeConnectionArray[i].fileDescriptorID[1];
            printf("IN:%d, OUT:%d\n",0,writeFD);
            //ch=readFile(fileNamesMatrix[i]);


            sprintf(ar,"child:%d",getpid());
            write(writeFD,ar,255);

            close(writeFD);
            exit(1);
        }else if(childpid >0){
            pipeConnectionArray[i].childProcessID = childpid;
            close(pipeConnectionArray[i].fileDescriptorID[1]);
        }
    }

    while((returnPid = wait(&status))!=-1){

        int readFD;
        char ar[255];
        for(j=0;j<fileCount;++j){
            if(returnPid == pipeConnectionArray[j].childProcessID)
                readFD = pipeConnectionArray[j].fileDescriptorID[0];
        }

        printf("ReadFD:%d\n",readFD);


        while(read(readFD,ar,255) >0){
            printf("Parent Read: %s\n",ar );
        }

        close(readFD);

    }
    //printf("ddddd");

    /*
    dizi=(char *)malloc(((listCount+1)*sizeof(char)));
    //printf("%s",argv[0]);
    i=0;
    if (d)
    {
      while ((dir = readdir(d)) != NULL)
      {
        dizi[i]=dir->d_name;
        //printf("%s\n", dir->d_name);
        ++i;
      }
      closedir(d);
    }

    n=atoi(argv[1]);
    //printf("n=%d",n);
    //pipe aciyoruz, hatali durum olustuysa burada kontrol ediyoruz
    if(pipe(fd)==-1)
    {
      perror("Failed to create the synchronization pipe");
      return 1;
    }

    for(i=1;i<listCount;++i)
    {
      if((childpid=fork())<=0)
        ch=readFile(dizi[i-1]);
        write(fd[1],ch,1);
    }
    if(childpid>0)
    {
      for(i=1;i<listCount;++i)
      {
        if(read(fd[0],buf,1)!=1)
        {
          perror("Failed to write synchronization characters");
          printf("Buf[%d]=%s\n",i,buf);
        }
        printf("Buf[%d]=%s\n",i,buf);
      }
    }
    //fprintf(stderr,"a");
  /*
    if(read(fd[0],buf,1)!=1)
      perror("Failed to read synchronization characters");
  */
/*
  fprintf(stderr,"\ni:%d process ID:%ld parent ID:%ld child ID:%ld\n",
                    i,(long)getpid(),(long)getppid(),(long)childpid);
  //printf("buf:%s",buf);
  exit(EXIT_FAILURE);
  return (childpid==-1);*/
}
/*Dosyadan iki adet string okuyacak sekilde tasarlandi*/
char* readFile(char* fileName)
{
    FILE *dosya;
    char k1[10],k2[10];
    char result[30];
    //result[0]='1';
    char *type ;
    int resultSize=0;
    int i;
    strcpy(result,"1");
    //printf("FileName1:%s",fileName);
    for(i=0;i<3;++i)
    {
        if(fileName[strlen(fileName)-1]=='t' && fileName[strlen(fileName)-2]=='x' && fileName[strlen(fileName)-3]=='t')
        {

            if((dosya =fopen(fileName,"r"))!=NULL)
            {
                fscanf(dosya,"%s",k1);
                fscanf(dosya,"%s",k2);
                //printf("%s %s ",k1,k2);
            }
            else
            {
                printf("Dosya Okunamadi.");
            }
            strcat(result,k1);
            strcat(result,k2);
            //result[29]='\0';
            //printf("File Name:%s ",fileName);
            //printf("-Result String:%s",result);
            resultSize=strlen(result);
            type= (char*)malloc(resultSize*sizeof(char));
            type=result;
            fclose(dosya);
            return type;
        }
        else{
            strcpy(result,"1");
            resultSize=strlen(result);
            type= (char *)malloc(resultSize*sizeof(char));
            type=result;
            return type;
        }
    }

}
/*Directory'nin icindeki file'lari bulan fonksiyon*/
int listDir(char* path){
    int fileCount=0;
    DIR* dir;
    struct dirent *ent;
    if((dir=opendir(path)) != NULL){
        while (( ent = readdir(dir)) != NULL){
            if(ent->d_type == DT_REG && strcmp(ent->d_name, ".") != 0  && strcmp(ent->d_name, "..") != 0 && strcmp(ent->d_name,"...")!=0){
                fileCount++;
                listDir(ent->d_name);
            }
        }
        closedir(dir);
    }
    return fileCount;
}
