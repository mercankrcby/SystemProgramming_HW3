#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h> /* isdigit */


#define MAX_PATH_LEN 255
#define MAX_NAME_LEN 255
#define FIFO_PERM  (S_IRUSR | S_IWUSR) /* taked from book */

/*Fonksiyon Prototipleri*/
int fileOperations(const char *path, const char *fname,int pipeFD,const char *word);

int search(const char *dirname, const char *word);

int findOccurences(const char *array, const char *word, const char *fname, int pipeFD);

int isDir(const char *path);

int isReg(const char *path);

int combineLogs(const char *fileName, const char *logName);

int findDirCountInDirectory(const char *path);

typedef struct {
    int childProcessID;
    int fileDescriptorID[2];
} PipeWithFork;

typedef struct {
    pid_t pid;
    char fifoName[MAX_NAME_LEN];
} FifoCon_t;

/*Fonksiyon prototipleri*/
int listDir(const char *path);

int main(int argc, char **argv) {

    int dirCount=0;
    int total = 0;
    char tempLogName[MAX_PATH_LEN];
    if (argc != 3) {
        fprintf(stdout, "Usage: %s string dirname\n", argv[0]);
        return 1;
    }
    /*total = search(argv[2], argv[1]);*/
    /*printf("Total occurences of %s is %d\n",argv[1],total);*/
    dirCount=listDir(argv[2]);
    printf("DirCount=%d",dirCount);
    /*sprintf(tempLogName, "...%d", getpid());
    rename(tempLogName, "log.log");*/
    return total;
}

/*Directory'nin icindeki file'lari bulan fonksiyon*/
int listDir(const char *path) {
    int fileCount = 0;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG && strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0 &&
                strcmp(ent->d_name, "...") != 0) {
                /*printf("Ent->d_name:%s",ent->d_name);*/
                fileCount++;
                listDir(ent->d_name);
            }
        }
        closedir(dir);
    }
    return fileCount;
}
/*Directory'nin icindeki directory'leri bulmak icin*/
int findDirCountInDirectory(const char *path) {
    int DirectoryCount = 0;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            printf("Directory icerisindeyim\n");
            printf("%s\n",ent->d_name);
            if (ent->d_type == DT_DIR || strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0 ||
                strcmp(ent->d_name, "...") == 0) {
                DirectoryCount++;
                /*printf("Directory Names:%s",ent->d_name);*/
                if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0 ||
                   strcmp(ent->d_name, "...") == 0)
                {
                    DirectoryCount--;
                }
            }
        }
        closedir(dir);
    }
    return DirectoryCount;
}


/*Directory olup olmadigini anlamak icin path alarak onu kontrol eder*/
int isDir(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) == -1) {
        return 0;
    }
    return S_ISDIR(statbuf.st_mode);
}

/*TextFile olup olmadigin directory fonksiyonunda oldugu gibi path alarak kontrol eder*/
int isReg(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) == -1) {
        return 0;
    }
    return S_ISREG(statbuf.st_mode);
}

/*Directory name ve word alarak verilen directory icindeki file'lar icin ve ayni zamanda
kendi directorysi altindaki directory'lerin icindeki file'larda arama yapar. Yaptigi arama
sonucunda kac tane buldugunu da return eder.Fonksiyonun icerisinde fork yapilarak processler
iki katina cikarilir ayni zamanda , recursive kol yapılarak da directoryler arasinda gezilir*/
int search(const char *dirpath, const char *word) {

    /*Directory'ler icin ozel olarak tanimlanmis struct*/
    struct dirent *m_dirent = NULL;
    DIR *m_dir;
    char path[MAX_PATH_LEN];
    pid_t m_childpid;
    int numofoccs = 0;
    char fname[MAX_PATH_LEN];
    int fileCount=0;
    int dirCount=0;
    int i;
    pid_t childpid;
    int fileIndex = 0;
    int dirIndex = 0;
    pid_t returnPid;
    char fifoName[MAX_NAME_LEN];
    PipeWithFork *pipeConnectionArray;
    FifoCon_t *openedFifos;

    fileCount = listDir(dirpath);
    /*dirCount=findDirCountInDirectory(dirpath);*/
    /*fprintf(stdout,"DirCount:%d",dirCount);*/

    openedFifos = (FifoCon_t *) malloc(sizeof(FifoCon_t)*dirCount);
    pipeConnectionArray = (PipeWithFork *) malloc(sizeof(PipeWithFork) * fileCount);
    /* open pipes */
    for (i = 0; i < fileCount; ++i) {
        if (pipe(pipeConnectionArray[i].fileDescriptorID) == -1) {
            perror("Failed to create the synchronization pipe");
            exit(1);
        }
    }


    for(i=0;i<dirCount;++i){
        char name[MAX_NAME_LEN];
        sprintf(name,".%d_%d.fifo",getpid(),i);
        if(mkfifo(name,FIFO_PERM)<0){
            fprintf(stderr,"Fifo open error");
            exit(1);
        }
    }


    /*Directory acildi mi diye baslangicta kontrol edilir, base case */
    if ((m_dir = opendir(dirpath)) != NULL) {
        /* initialize path*/
        strcpy(path, dirpath);
        strcat(path, "/");
        /*Directory icerisine girilip okundugu surece while icerisinde kontroller yapilir ,fork olusturulur*/
        while ((m_dirent = readdir(m_dir)) != NULL) {
            if (strcmp(m_dirent->d_name, ".") != 0 &&
                strcmp(m_dirent->d_name, "..") != 0 &&
                strncmp(m_dirent->d_name, "...", 3) != 0) { /* special log files*/
                char tempPath[MAX_PATH_LEN];
                strcpy(tempPath, path);
                strcat(tempPath, m_dirent->d_name);
                strcpy(fname, m_dirent->d_name);
                /*Text File ya da Directory olup olmadigi kontrolu yapilir */

                if (isReg(tempPath)) {
                    int j = 0;
                    int writeFD;
                    if ((childpid = fork()) == 0) {
                        char ar[255];

                        close(pipeConnectionArray[fileIndex].fileDescriptorID[0]);
                        writeFD = pipeConnectionArray[fileIndex].fileDescriptorID[1];


                        fileOperations(tempPath,fname,writeFD,word);

                        write(writeFD, ar, 255);

                        close(writeFD);
                        exit(1);
                    } else if (childpid > 0) {
                        pipeConnectionArray[fileIndex].childProcessID = childpid;
                        close(pipeConnectionArray[fileIndex].fileDescriptorID[1]);
                        ++fileIndex;
                    }

                }


                if (isDir(tempPath)) {
                    m_childpid = fork();
                    /*Hatali fork olusumu */
                    if (m_childpid == -1) {
                        fprintf(stderr, "Fork error. Emergency exit\n");
                        exit(EXIT_FAILURE);
                    }
                        /*Child Process*/
                    else if (m_childpid == 0) {
                        strcat(path, m_dirent->d_name);
                        m_dirent = NULL;

                        closedir(m_dir);
                        m_dir = NULL;
                        break;
                    }
                }
            }
        }

        while ((returnPid = wait(NULL)) != -1) {
            int j;
            int readFD;
            char ar[255];
            for (j = 0; j < fileCount; ++j) {
                if (returnPid == pipeConnectionArray[j].childProcessID)
                    readFD = pipeConnectionArray[j].fileDescriptorID[0];
            }

            printf("ReadFD:%d\n", readFD);

            while (read(readFD, ar, 255) > 0) {
                printf("Parent Read: %s\n", ar);
            }

            close(readFD);

        }

        /*Parent Process İslemleri*/
/*
        pid_t returnPid;
        char procLogName[MAX_PATH_LEN];
        int status = 0;
        int totalOcc = 0;
        FILE *myLog;

        sprintf(procLogName, "...%d", getpid());

        while ((returnPid = wait(&status)) != -1) {
            char childLogname[MAX_PATH_LEN];
            sprintf(childLogname, "...%d", returnPid);
            totalOcc += combineLogs(procLogName, childLogname);
        }

        if ((myLog = fopen(procLogName, "a+")) == NULL) {
            fprintf(stderr, "Failed to open childLog\n");
            exit(EXIT_FAILURE);
        }

        fprintf(myLog, "%d %s were found in total.\n", totalOcc, word);
        Dosya ve directory kapatilir
        fclose(myLog);
        closedir(m_dir);
        */
        return 0;

    } else {
        fprintf(stderr, "Directory open error. DirectoryName:%s\n", dirpath);
        exit(EXIT_FAILURE);
    }
}

/* Her dosya icin olusturulmus log file'lari birlestirir*/
int combineLogs(const char *parentLogPath, const char *childLogPath) {
    FILE *childLog;
    FILE *parentLog;
    char line[MAX_PATH_LEN];
    int total = 0;
    /*Read modunda cocuk processler tarafindan dosyadan alinan bilgiler okunur*/
    if ((childLog = fopen(childLogPath, "r")) == NULL) {
        fprintf(stderr, "Failed to open childLog\n");
        exit(EXIT_FAILURE);
    }

    if ((parentLog = fopen(parentLogPath, "a+")) == NULL) {
        fprintf(stderr, "Fail to open parentLog:%s\n", parentLogPath);
        exit(EXIT_FAILURE);
    }
    /*bir bir kopyasi log dosyasina eklendi*/

    while (fgets(line, MAX_PATH_LEN, childLog) != NULL) {
        if (isdigit(line[0])) {
            int num;
            sscanf(line, "%d", &num);
            total += num;
        } else {
            fprintf(parentLog, "%s", line);
        }
    }


    fclose(childLog);
    fclose(parentLog);
    unlink(childLogPath);
    return total;
}

/* Dosya okuma islemi yaparak oncelikle dosyanin kac harften olustugunu bulur daha sonrasinda
malloc ile yer alarak tek boyutlu array de saklar. Fonksiyonun sonunda dosyada kac tane word
gectigini bulduguna dair ekrana cikti basar*/

int fileOperations(const char *path, const char *fname,int pipeFD, const char *word) {
    FILE *dosya = NULL;
    char ch;
    int letterNumber = 0;
    char *dizi = NULL;
    int totalOccNum;


    if ((dosya = fopen(path, "r")) != NULL) {
        while (fscanf(dosya, "%c", &ch) != EOF) {
            ++letterNumber;
        }
        fclose(dosya);
        /*printf("Maximum Column Number = %d\n Maximum Row Number = %d ",maxNumber,rowNumber);*/
    } else {
        fprintf(stderr, "File Read error!");
        return 1;
    }

    dizi = (char *) malloc(((letterNumber + 1) * sizeof(char)));

    letterNumber = 0;
    if ((dosya = fopen(path, "r")) != NULL) {
        while (fscanf(dosya, "%c", &ch) != EOF) {
            dizi[letterNumber] = ch;
            ++letterNumber;
        }
        dizi[letterNumber] = '\0'; /* end of char array*/
        fclose(dosya);
        dosya = NULL;
    } else {
        fprintf(stderr, "File Read error!");
        /*free taken memories*/
        free(dizi);
        dizi = NULL;
        return 1;
    }

    totalOccNum = findOccurences(dizi, word, fname, pipeFD);
    fprintf(stderr, "%d %s were found in total.\n", totalOccNum,word);

    free(dizi);
    dizi = NULL;

    return totalOccNum;
}

/*FindOccurences fonksiyonu cagirilarak dosyada word'un kac kez gectigi bilgisi
ogrenilerek ekrana basilir*/
int findOccurences(const char *array, const char *word, const char *fname, int pipeFD) {
    int i = 0, rowNumber = 0, currentNum = 0;
    int foundColumn = 0, foundRow = 0;
    int found = 0;
    int arraySize = 0, wordSize = 0;
    int search = 0;
    int index = 0;

    arraySize = strlen(array);
    wordSize = strlen(word);

    for (i = 0; i < arraySize; ++i) {
        if (array[i] == word[0]) {
            foundColumn = i;
            foundRow = rowNumber;
            index = i + 1;
            currentNum = 1;
            search = 1; /* search status */

            while (search && index < arraySize) {
                if (array[index] == word[currentNum]) {
                    /* printf("%c - %c\n",array[index],word[currentNum]); */
                    currentNum += 1;
                } else if (array[index] == ' ' || array[index] == '\t' || array[index] == '\n') {
                    /* do nothing */
                } else {
                    search = 0;
                }

                if (wordSize == currentNum) {
                    char message[MAX_PATH_LEN];
                    search = 0;
                    ++found;
                    sprintf(message, "%s: [%d-%d] %s first character is found.\n", fname, foundRow + 1, foundColumn + 1,
                            word);
                    write(pipeFD,message,MAX_PATH_LEN);
                }
                ++index;
            }
        } else if (array[i] == '\n') {
            ++rowNumber;
        }
    }

    return found;
}
