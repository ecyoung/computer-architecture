#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define PI 3.14159265358979323846

// Declare pizza struct
typedef struct pizza {
    char name[50]; //pizza name
    float diameter; //inches
    float cost; //dollars
    float costPerArea;
    struct pizza *next;
} pizza_t; //shorthand for struct pizza

// Declare function that calculates the dollar per area of a pizza
float dollarPerArea(float diameter, float cost){
    float area = 0.25* PI * (diameter*diameter);
    float calculation = 0;
    if (diameter != 0) calculation = cost/area;
    return calculation;
}

// Function to insert node
void insert(pizza_t** head, char name[], float diameter, float cost) {
    float costPerArea;
    pizza_t* pizza = (pizza_t*) malloc(sizeof(pizza_t));
    pizza_t* ptr;
    strcpy(pizza->name, name);
    pizza -> diameter = diameter;
    pizza -> cost = cost;
    costPerArea = dollarPerArea(pizza->diameter, pizza->cost);
    pizza -> costPerArea = costPerArea;
    pizza -> next = NULL;

    if (*head == NULL) *head = pizza;
    else {
        ptr = *head;
        while (ptr->next != NULL) ptr = ptr->next;
        ptr->next = pizza;
    }
}

void sortPizzaList(pizza_t* head){
    pizza_t* pizza = NULL;
    pizza_t* temp = NULL;
    float tempCostPerArea; //temp variable to store node data
    char tempName[50];
    int alphComp;
    int swap=0;
    pizza = head;
    while(pizza != NULL){
        swap=0;
        temp = pizza;

        while (temp->next != NULL){ //travel till the second last element
         if(temp->costPerArea < temp->next->costPerArea){// compare the data of the nodes
            // swap the name data
            strcpy(tempName, temp->name);
            strcpy(temp->name, temp->next->name);
            strcpy(temp->next->name, tempName);
            // swap the costPerArea data
            tempCostPerArea = temp->costPerArea;
            temp->costPerArea = temp->next->costPerArea;
            temp->next->costPerArea = tempCostPerArea;
            swap=1;
         }
         else if(temp->costPerArea == temp->next->costPerArea){
            alphComp = strcmp(temp->name, temp->next->name);
            if (alphComp>0) {
            // swap the name data
            strcpy(tempName, temp->name);
            strcpy(temp->name, temp->next->name);
            strcpy(temp->next->name, tempName);
            // swap the costPerArea data
            tempCostPerArea = temp->costPerArea;
            temp->costPerArea = temp->next->costPerArea;
            temp->next->costPerArea = tempCostPerArea;
            swap=1;
            }
        }
        temp = temp->next;    // move to the next element
        }
        if (swap==0) pizza = pizza->next;    // move to the next node
    }
}

void printList(pizza_t* pizza) {
    while (pizza != NULL){
        printf("%s %f\n", pizza->name, pizza->costPerArea);
        pizza = pizza->next;
    }
}

int main(int argc, char *argv[]){
    char pizza_name[50];
    float pizza_diameter;
    float pizza_cost;
    float pizza_costPerArea;
    int size;
    // accessing file
    FILE *fr = fopen(argv[1], "r" );
    pizza_t *head = NULL;
    pizza_t *pizza;
    // check if the file is empty
    if (fr != NULL){
        fseek(fr, 0, SEEK_END);
        size = ftell(fr);
        if(size == 0) {
            printf("PIZZA FILE IS EMPTY\n");
            fclose(fr);
            return 0;
        }
        fseek(fr, 0, SEEK_SET);
    }

    // set the first node
    while(!feof(fr)){
        fscanf(fr, "%s\n", pizza_name);
        if (strcmp(pizza_name,"DONE")==0) break;
        fscanf(fr,"%f\n", &pizza_diameter);
        fscanf(fr, "%f\n", &pizza_cost);
        // copy the data over to the pizza node
        insert(&head, pizza_name,pizza_diameter,pizza_cost);
    }
    fclose(fr);  // close the file prior to exiting the routine
    pizza = head;
    sortPizzaList(pizza);
    printList(pizza);

    // free linked list
    while (head!=NULL) {
        pizza=head;
        head=head->next;
        free(pizza);
    }

  return 0;
}
