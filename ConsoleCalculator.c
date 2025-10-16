#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

float float_check(char *value){
    char *end;
    strtof(value,&end);
    if(*end=='\0'){
        return 1;
    }
    else{
        return 0;
    }
    
}

int operator_check(char *value1){
    int count=0;
    for (int i=0;value1[i]!='\0';i++){
        if(ispunct(value1[i])){
            count++;
        }
    }
    if (count==1){
        return 1;
    }
    else{
        return 0;
    }
}

int main(){

    char choice='y';
    char operators[4]={'+','-','*','/'};
    char expo[]="**";
    char modulo[]="%";
    char operatorschoice[2];
    float number1,number2,result=0, opcheck;
    char number1c[8], number2c[8];
    printf("Welcome to Console Calculator!\n");
    printf("You can use operators to calculate your number.\nEx: + to addition, - to substractiob, * to multiply, / to divide, ** to exponantiation, %% to modulo\n");

    while(choice=='y'){
       
        printf("First number: ");
        fflush(stdout);
        scanf("%s",number1c);
        
        printf("Operator: ");
        fflush(stdout);
        scanf("%s", operatorschoice);
        
        printf("Second number: ");
        fflush(stdout);
        scanf("%s",number2c);

        if (float_check(number1c)!=1){
            printf("Wrong First Number!\nExiting!\n");
            return 0;
        }
        if (float_check(number2c)!=1){
            printf("Wrong Second Number!\nExiting!\n");
            return 0;
        }

        number1= strtof(number1c,NULL);
        number2= strtof(number2c,NULL);
        
        opcheck=operator_check(operatorschoice);
        if (opcheck!=1){
            printf("Too Much Operator!\n");
            printf("Exiting!\n");
            return 0;
        }

        if (operatorschoice[0]==operators[0]){
            result=number1+number2;
            printf("Result: %.1f\n",result);
        }
        else if (operatorschoice[0]==operators[1]){
            result=number1-number2;
            printf("Result: %.1f\n",result);
        }
        else if (operatorschoice[0]==operators[2]){
            if (strcmp(operatorschoice,expo)==0){
                result=1;
                for(int i=1;i<=(int)number2;i++){
                    result=result*number1;
                }
                printf("Result: %.1f\n",result);
            }
            else{
                result=number1*number2;
                printf("Result: %.1f\n",result);
            }
        }
        else if (operatorschoice[0]==operators[3]){
            result=number1/number2;
            printf("Result: %.1f\n",result);
        }

        else if (strcmp(operatorschoice,modulo)==0){
            int result=(int)number1%(int)number2;
            printf("Result: %d\n",result);
        }
        else{
            printf("You made a mistake, please run the program again!\n");
            break;
        }
        printf("To continue to calculate please press 'y', to quit please press 'q': ");
        fflush(stdout);
        getchar();
        scanf("%c",&choice);
        if (choice=='y'){
            continue;
        }
        else{
            printf("You Are Exiting!\n");
            break;
        }

    }

    return 0;

}