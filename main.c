#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<limits.h>
#include<time.h>
typedef struct{
 int id;char title[50];char category[50];float amount;char date[25];
}Expense;
void activitylogs(const char *logs_file,const char *modification,Expense *e){
    FILE *fp=fopen(logs_file,"a");
    if(fp==NULL){printf("Cannot open the logs file !\n");return ;}
    time_t now=time(NULL);
    struct tm *t = localtime(&now);
    char str_time[20];
    strftime(str_time,sizeof(str_time),"%Y-%m-%d %H:%M:%S",t);
    // or you can use asctime(struct *tm)
    strcpy(e->date,str_time);
    fprintf(fp,"[%s]: %s expense ID : %d\n",e->date,modification,e->id);
    fclose(fp);
}
int intParse(int *n){
    char buffer[100],*end;errno=0;
    if(fgets(buffer,sizeof(buffer),stdin)==NULL) return 0;
    buffer[strcspn(buffer,"\n")]='\0';
    long number=strtol(buffer,&end,10);
    if(end==buffer||*end!='\0'||errno==ERANGE||number<INT_MIN||number>INT_MAX)return 0;
    *n=(int)number;return 1;
} 
float floatParse(float *f){
    char buffer[100],*end;errno=0;
    if(fgets(buffer,sizeof(buffer),stdin)==NULL) return 0;
    buffer[strcspn(buffer,"\n")]='\0';
    double number=strtod(buffer,&end);
    if(end==buffer||*end!='\0'||errno==ERANGE)return 0;
    *f=(float)number;return 1;
}
void enterAmount(float* amount){
    while(!floatParse(amount)||*amount<=0){
        printf("Error! Please enter a valid positive amount\n   Amount: ");
    }
}
void enterOption(int *option,int a,int b){
    while(!intParse(option)||*option<a||*option>b){
        printf("Error! Please enter a valid option(%d-%d)\n\toption: ",a,b);
    }
}
int getMenuChoice(){
    int choice;
    printf("1. Add expense.\n2. Display all expenses.\n3. Search expense\n4. Modify expense.\n5. Delete expense.\n6. Total expenses.\n7. Show the most expensive expense.\n8. Activity logs.\n9. File information.\n10. Exit.\n\toption: ");
    enterOption(&choice,1,10);
    return choice;
}
void addExpense(const char *expenses_file,const char *logs_file){
   FILE *fp=fopen(expenses_file,"ab+");
   if (fp==NULL ){printf("Cannot open the file!\n.");return ;}
   Expense e;
   printf("ID: ");while(!intParse(&e.id)||e.id<=0) printf("Error! Please enter a valid ID.\n    ID: ");// check if the id is correct
   rewind(fp) ;// moves the file pointer to the begining to read all records equivalent to fseek(fp,0,SEEK_SET)
   // check that is no duplicate IDs exist
   Expense e_check;// I added this expense just for checking 
   while(fread(&e_check,sizeof(Expense),1,fp)==1){
    if(e_check.id==e.id){
        printf("This ID belongs to an expense already exist.!\n");
        fclose(fp);return ;
    }
   }
   printf("Title: ");fgets(e.title,sizeof(e.title),stdin);e.title[strcspn(e.title,"\n")]='\0';
   printf("Categorie: ");fgets(e.category,sizeof(e.category),stdin);e.category[strcspn(e.category,"\n")]='\0';
   printf("Amount: ");enterAmount(&e.amount);
   activitylogs(logs_file,"ADDED",&e);
   if(fwrite(&e,sizeof(Expense),1,fp)!=1){
    printf("Cannot write into the file!\n");fclose(fp);return ;
   }
   fclose(fp);
   printf(" The expense was added successfuly.\n");
}
void displayAllExpenses(const char *filename){
    printf("\n==================== DISPLAY ALL EXPENSES =======================\n");
    FILE *file=fopen(filename,"rb");
    if(file==NULL){printf("Cannot open the file\n");return ;}
    Expense e;int record=0;
    while(fread(&e,sizeof(Expense),1,file)==1){
       record=1;
       printf("ID: %d \nTITLE: %s \nCATEGORIE: %s \nAMOUNT: $%.2f \nDATE : %s\n-----------------------------\n",e.id,e.title,e.category,e.amount,e.date);
    }
    if(!record){printf("No expense recorded yet! \n");fclose(file);return;}
    fclose(file);
}
void searchByID(const char *filename){
    FILE *fp=fopen(filename,"rb");
    if(fp==NULL){printf("Cannot open the file!\n");return ;}
    int search_id;printf("Enter searched ID: ");
    while(!intParse(&search_id)||search_id<=0){
        printf("Error! Please enter a valid ID\n    ID: ");
    }
    Expense e;
    int found=0,record=0;
    while(fread(&e,sizeof(Expense),1,fp)==1) {
        record=1;
          if(e.id==search_id){
            printf("Expense found successfuly.\n");
            printf("ID: %d \nTITLE: %s \nCATEGORIE: %s \nAMOUNT: %.2f \nDATE : %s\n--------------------------------\n",e.id,e.title,e.category,e.amount,e.date);
            fclose(fp);
            found=1;
            return ;
          }
    }
    if(!record){printf("No Expense recorded yet!\n");fclose(fp);return ;}
    if(!found){printf("Expense not found or doesn't exist!\n");fclose(fp);return;}
    fclose(fp);
}
void searchByTitle(const char *filename){
    FILE *fp=fopen(filename,"rb");
    if(fp==NULL){printf("Cannot open the file!\n");return ;}
    char search_title[50];printf("Enter the searched title: ");
    if(fgets(search_title,sizeof(search_title),stdin)==NULL){printf("Something wrong!...Try again.\n");fclose(fp);return ;}
    search_title[strcspn(search_title,"\n")]='\0';
    Expense e;int record=0,found=0;
    while(fread(&e,sizeof(Expense),1,fp)==1){
        record=1;
        if(strcmp(search_title,e.title)==0){
            printf("Expense found successfuly.\n");
            printf("ID: %d \nTITLE: %s \nCATEGORIE: %s \nAMOUNT: %.2f \nDATE : %s\n--------------------------------\n",e.id,e.title,e.category,e.amount,e.date);
            fclose(fp);
            found=1;
            return ;
        }
    }
    if(!record){printf("No Expense recorded yet!\n");fclose(fp);return ;}
    if(!found){printf("Expense not found or doesn't exist!\n");fclose(fp);return;}
    fclose(fp);
}
void searchByCategorie(const char *filename){
    FILE *fp=fopen(filename,"rb");
    if(fp==NULL){printf("Cannot open the file!\n");return ;}
    char search_Categorie[50];printf("Enter the searched categorie: ");
    if(fgets(search_Categorie,sizeof(search_Categorie),stdin)==NULL){printf("Something wrong!...Try again.\n");fclose(fp);return ;}
    search_Categorie[strcspn(search_Categorie,"\n")]='\0';
    Expense e;int record=0,found=0;
    while(fread(&e,sizeof(Expense),1,fp)==1){
        record=1;
        if(strcmp(search_Categorie,e.category)==0){
            printf("Expense found successfuly.\n");
            printf("ID: %d \nTITLE: %s \nCATEGORIE: %s \nAMOUNT: %.2f \nDATE : %s\n--------------------------------\n",e.id,e.title,e.category,e.amount,e.date);
            fclose(fp);
            found=1;
            return ;
        }
    }
    if(!record){printf("No Expense recorded yet!\n");fclose(fp);return ;}
    if(!found){printf("Expense not found or doesn't exist!\n");fclose(fp);return;}
    fclose(fp);
}
void searchExpense(const char *filename){
    printf("\n=================== SEARCH EXPENSE =====================\n");
    int sub_option;
    printf("1. Search by ID.\n2. Search by Title.\n3. Search by Categorie.\n4. Back.\n  option: ");
    enterOption(&sub_option,1,4);
    switch(sub_option){
        case 1: searchByID(filename);break;
        case 2: searchByTitle(filename);break;
        case 3: searchByCategorie(filename);break;
        case 4: printf("Canceling the process...\n");break;
    }
}
void modifyExpense(const char *filename,const char *logs_file){
    printf("\n=================== MODIFY EXPENSE =====================\n");
    FILE *fp=fopen(filename,"rb+");
    if(fp==NULL){printf("Cannot open the file!\n");return ;}
    int search_id;printf("Enter searched ID: ");
    while(1){
        if(!intParse(&search_id)||search_id<=0)printf("Error! Please enter a valid ID\n    ID: ");
        else break;
    }
    Expense e;
    int found=0,record=0;
    fseek(fp,0,SEEK_SET);//moves the cursor's position to the begin of the file
    while(fread(&e,sizeof(Expense),1,fp)==1) {
        record=1;
          if(e.id==search_id){
            printf("Expense found successfuly.\n");
            printf("new Title: ");if(fgets(e.title,sizeof(e.title),stdin)==NULL) { printf("Something wrong!....\n");fclose(fp);return ;}
            e.title[strcspn(e.title,"\n")]='\0';
            printf("new Categorie: ");if(fgets(e.category,sizeof(e.category),stdin)==NULL) {printf("Something wrong!....\n");fclose(fp);return ;}
            e.category[strcspn(e.category,"\n")]='\0';
            printf("new Amount: ");enterAmount(&e.amount);
            fseek(fp,-(long)sizeof(Expense),SEEK_CUR);// moves one record backward
            size_t written=fwrite(&e,sizeof(Expense),1,fp);
            if(!written){printf("Cannot write into the file!.\n");fclose (fp);return ; }
            printf("The expense was modified successfuly!\n");
            fclose(fp);
            activitylogs(logs_file,"MODIFIED",&e);
            found=1;
            return ;
          }
    }
    if(!record){printf("No Expense recorded yet!\n");fclose(fp);return ;}
    if(!found){printf("Expense not found or doesn't exist!\n");fclose(fp);return;}
}
void deleteExpense(const char *expenses_file,const char *logs_file)
{
    printf("\n=================== DELETE EXPENSE =======================\n");
    FILE *fp=fopen(expenses_file,"rb");
    if(fp==NULL){printf("Cannot open the file!\n");return ;}
    int target_id;printf("Enter expense ID: ");
    while(!intParse(&target_id)||target_id<=0){printf("Error! Please enter a valid ID.\n    ID: ");}
    rewind(fp);// moves the file pointer to the begining of the file equivalent to fseek(fp,0,SEEK_SET)
    Expense e;int record=0,found=0;
    // create a temporary file to store the expenses that we want to not delete(or preserve) 
    FILE *temp=fopen("temporary.dat","wb");
    if(temp==NULL){printf("Cannot open temporary file to store expenses.\n");fclose(fp);return;}
    while(fread(&e,sizeof(Expense),1,fp)==1){
        record=1;
        if(target_id==e.id){
            printf("Expense found! \n");found=1;
            continue;
        }
        if(fwrite(&e,sizeof(Expense),1,temp)!=1){
            printf("Cannot write into the temporary file!\n");
            fclose(fp);
            fclose(temp);
            remove("temporary.dat");
            return ;
        }
    }
    fclose(temp);fclose(fp);
    if(!record){printf("No expense recorded yet!\n");remove("temporary.dat");return ;}
    if(!found){printf("This expense not found or It doesn't exist !\n");remove("temporary.dat");return ;}
    if(remove(expenses_file)!=0) { printf("Cannot removing the expenses file . !\n");return ;}
    if(rename("temporary.dat",expenses_file)!=0){ printf("Cannot renaming the temporary file . !\n");return ;}
    activitylogs(logs_file,"DELETED",&e);
    printf("Deleting process...done.\n");
}
void totalExpenses(const char *expenses_file){
    printf("\n=============== TOTAL EXPENSES ====================\n");
    FILE *fp=fopen(expenses_file,"rb");
    if(fp==NULL){printf("Cannot open the expenses file!\n");return ;}
    Expense e;
    int record =0;float total_expenses=0;
    while(fread(&e,sizeof(Expense),1,fp)==1){
       record ++;
       total_expenses+=e.amount;
    }
    if(record==0){printf("No expense recorded yet!\n");fclose(fp);return ;}
    printf("Total expenses : %d\nTotal spent : $%.2f\nAverage expense: $%.2f\n----------------------------------------\n",record,total_expenses,total_expenses/record);
    fclose(fp);
}
void MostExpensive(const char *expenses_file){
    printf("\n==================== Most Expensive Expense ============================\n");
    FILE *fp=fopen(expenses_file,"rb");
    if(fp==NULL){printf("Cannot open the expenses file! ");return ;}
    Expense e,Expensive;
    Expensive.amount=-1.0f;
    int record =0;
    while(fread(&e,sizeof(Expense),1,fp)==1){
        record =1;
        if(e.amount>Expensive.amount) Expensive=e;
    }
    if(!record){printf("No expense recorded yet! \n");fclose(fp);return ;}
    printf("ID: %d \nTITLE: %s \nCATEGORIE: %s \nAMOUNT: $%.2f \nDATE : %s\n-----------------------------\n",Expensive.id,Expensive.title,Expensive.category,Expensive.amount,Expensive.date);
    fclose(fp);
}
void displayAcitivityLogs(const char *logs_file){
    printf("\n========================= DISPLAY ACTIVITY LOGS =======================\n");
    FILE *fp=fopen(logs_file,"r");
    if(fp==NULL){printf("Cannot open the activity logs file!\n");return ;}
    char buffer[1024];// create a buffer can store the file size
    rewind(fp);// Moves the pointer to the very beginning of the logs file for reading but it still unneccessary in this case 
    while (fgets(buffer,sizeof(buffer),fp)!=NULL) printf("%s",buffer);
    printf("\n------------------------------\n");
    fclose(fp);
}
void fileInformation(const char *expenses_file){
    printf("\n============= FiLE INFORMATION ===============\n");
    FILE *fp=fopen(expenses_file,"rb");
    if(fp==NULL){printf("Cannot open the file!\n");return ;}
    int records=0;Expense e;
    while(fread(&e,sizeof(Expense),1,fp)==1){
        records++;
    }
    if(records==0){printf("No expense recorded yet!\n");fclose(fp);return ;}
    printf("Number of records : %d\nFile size: %ld Bytes\nCurrent position: %ld\n---------------------------\n",records,ftell(fp),ftell(fp));
    fclose(fp);
}
void createBackup(const char *expenses_file){
    printf("\n================= CREATE BACK-UP ========================\n");
    FILE *exp_fp=fopen(expenses_file,"rb");
    if(exp_fp==NULL){printf("Cannot open expenses file ! \n");return;}
    FILE *backUp_fp=fopen("expenses_backup.dat","wb");
    if(backUp_fp==NULL){printf("Cannot open the back up file ! \n");fclose(exp_fp);return ;}
    Expense e;
    int record=0;
     while(fread(&e,sizeof(Expense),1,exp_fp)==1){
        record=1;
        size_t written = fwrite(&e,sizeof(Expense),1,backUp_fp);
        if(!written){printf("Cannot write into the backup file!");fclose(backUp_fp);fclose(exp_fp);remove("expenses_backup.dat");return;}
     }
     if(!record){printf("No Expense recorded yet!\n");fclose(backUp_fp);fclose(exp_fp);remove("expenses_backup.dat");return;}
    fclose(backUp_fp);fclose(exp_fp);
    printf("The back-up file has created successfuly!.\n");
}
int main()
{
    printf("=========================================\n\tPersonal expense\n=========================================\n");
    // modifyExpense("expenses.dat","activity.log");
    // searchExpense("expenses.dat");
    // deleteExpense("expenses.dat","activity.log");
    // displayAllExpenses("expenses.dat");
    // totalExpenses("expenses.dat");
    // MostExpensive("expenses.dat");
    // displayAllExpenses("expenses.dat");
    // displayAcitivityLogs("activity.log");
    // fileInformation("expenses.dat");
    //createBackup("expenses.dat");
    //displayAllExpenses("expenses_backup.dat");
    // I don't want to add create backup function to the menu 
    int choice=0;
    do{
       choice=getMenuChoice();
       switch(choice){
        case 1: addExpense("expenses.dat","activity.log");break;
        case 2: displayAllExpenses("expenses.dat");break;
        case 3: searchExpense("expenses.dat");break;
        case 4: modifyExpense("expenses.dat","activity.log");break;
        case 5: deleteExpense("expenses.dat","activity.log");break;
        case 6: totalExpenses("expenses.dat");break;
        case 7: MostExpensive("expenses.dat");break;
        case 8: displayAcitivityLogs("activity.log");break;
        case 9: fileInformation("expenses.dat");break;
        case 10: printf("System shutting down... See you again!\n");break;
       }
    }while(choice!=10);
}