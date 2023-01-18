#include<iostream>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<fstream>
using namespace std;

#define MAX_COURSE_COUNT 20
#define MAX_COURSE_NAME_LENGTH 10

typedef struct Graph_Vertex{
    char name[32];
    char color[32];
    bool visited;
}Vertex;

FILE *readFile(FILE *, char *);
bool isCourseExist(char *);
int **createEdges(FILE *);
int **createEdgeBetween(int **, char coursesOneStudentTakes[MAX_COURSE_COUNT][MAX_COURSE_NAME_LENGTH], int );
void readFromFile(FILE *);
int getAdjUnvisitedVertex(int , int **, Vertex **);
void push(int);
int pop();
int peek();
bool isStackEmpty();
int determineColor(const int *);
Vertex *coloring(Vertex **, int **);
Vertex *DFS(Vertex **, int **, int);
Vertex *createVertices(Vertex **);
Vertex *addVertex(char *);

//Course initialization.
int courses[MAX_COURSE_COUNT][MAX_COURSE_NAME_LENGTH];
int courseCount = 0;

//Necessary variables for DFS.
int stack[MAX_COURSE_COUNT];
int top = -1;

//COLORS IN ORDER
//YELLOW - RED - GREEN - BLACK - PINK - WHITE - BLUE - BROWN - GRAY - ORANGE
char colors[10][10] = {{"Yellow"},{"Red"},{"Green"},{"Black"},{"Pink"},{"White"},{"Blue"},{"Brown"},{"Gray"},{"Orange"}};

int main() {

    int i, j, k;

    //Creating a file pointer.
    FILE *filePtr = NULL;

    filePtr = readFile(filePtr, "Data.txt");

    //Graph initializations
    Vertex *list[courseCount];
    int **adjMatrix = NULL;

    //Create vertices with classes.
    *list = createVertices(list);

    //Creating edges.
    adjMatrix = createEdges(filePtr);

    ////PRINTING THE ADJACENCY MATRIX.
   cout<<"\t\t   ADJACENCY MATRIX"<<endl;
   cout<<endl<<"\t";

    for(i = 0; i < courseCount + 1; i++){

        for(j = 0; j < courseCount + 1; j++){

            if(i == 0 && j != courseCount)
                cout<< list[j] -> name<<"\t";
            else if(j == 0)
                cout<< list[i - 1] -> name<<"\t";
            if(i != 0 && j != 0)
                cout<<"  "<<adjMatrix[i - 1][j - 1]<<"\t";

        }
       cout<<endl;
    }
   cout<<endl<<endl;
    /////PRINTING THE ADJACENCY MATRIX.

    //Coloring Algorithm.
    *list = coloring(list, adjMatrix);

    //Let's find how many colors used.
    int numberOfColorsUsed = 0;
    for(i = 0; i < courseCount; i++){

        for(j = 0; j < 10; j++)
            if(strcmp(list[i] -> color, colors[j]) == 0 && numberOfColorsUsed < j + 1)
                numberOfColorsUsed = j + 1;
    }

    /////PRINTING FINAL EXAM PERIODS
    cout<<"\t\t\tEXAM SCHEDULE"<<endl;
    for(i = 0; i < numberOfColorsUsed; i++){

        cout<<"\t\t"<<(i + 15) <<"/11/2021 -> ";

        for(j = 0; j < courseCount; j++){

            if(strcmp(list[j] -> color, colors[i]) == 0){
                cout<< list[j] -> name;

                for(k = j + 1; k < courseCount; k++){

                    if(strcmp(list[k] -> color, colors[i]) == 0){
                        cout<<", ";
                        break;
                    }
                }
            }
        }

        cout<<endl;
    }
    /////PRINTING FINAL EXAM PERIODS
    cout<<endl;
    fclose(filePtr);

    return 0;
}

FILE *readFile(FILE *fp, char *fileName){

    if(access(fileName, F_OK) != -1){

        fp = fopen(fileName, "r");
        readFromFile(fp);

    }else{

        cout<<fileName<<" does not exist. Please make sure it exists before starting this program."<<endl;
        exit(1);

    }

    return fp;
}

void readFromFile(FILE *fp){

    char input[256], word[256];
    char *token;

    while(!feof(fp)){

        if(fgets(input, 512, fp) != NULL){

            strcpy(word, strtok(input, "\n"));
            strtok(word, ":");
            token = strtok(NULL, ",");

            while(token != NULL){

                if(isCourseExist(token) == false){
                    strcpy((char *)courses[courseCount], token);
                    courseCount++;
                }

                token = strtok(NULL, ",");
            }
        }
    }
}

bool isCourseExist(char *course){

    int i;

    for(i = 0; i < courseCount; i++)
    {

        if(strcmp((const char *)courses[i], course) == 0)
            return true;

    }
    return false;
}

Vertex *createVertices(Vertex **list){

    int i;

    for(i = 0; i < courseCount; i++){

        list[i] = addVertex((char *) courses[i]);

    }

    return *list;
}

Vertex *addVertex(char *name){

    Vertex *newVertex = (Vertex *)malloc(sizeof(Vertex));
    strcpy(newVertex->name, name);
    strcpy(newVertex->color, "NULL");
    newVertex -> visited = false;

    return newVertex;
}

int **createEdges(FILE *fp){

    rewind(fp);

    //Creating an adj matrix.
    int **adjMatrix, i, j;
    adjMatrix = (int **)malloc(sizeof(int*) * courseCount);

    for(i = 0; i < courseCount; i++)
        adjMatrix[i] = (int *)malloc(sizeof(int*) * courseCount);

    for(i = 0; i < courseCount; i++){
        for(j = 0; j < courseCount; j++)
            adjMatrix[i][j] = 0;
    }

    char input[256], word[256], *token;
    char coursesOneStudentTakes[MAX_COURSE_COUNT][MAX_COURSE_NAME_LENGTH];
    int numberOfCoursesOneStudentTakes = 0;

    while(!feof(fp)){

        if(fgets(input, 512, fp) != NULL){

            numberOfCoursesOneStudentTakes = 0;

            strcpy(word, strtok(input, "\n"));
            strtok(word, ":");
            token = strtok(NULL, ",");

            while(token != NULL){

                strcpy(coursesOneStudentTakes[numberOfCoursesOneStudentTakes], token);
                numberOfCoursesOneStudentTakes++;

                token = strtok(NULL, ",");
            }

            adjMatrix = createEdgeBetween(adjMatrix, coursesOneStudentTakes, numberOfCoursesOneStudentTakes);
        }
    }

    return adjMatrix;
}

int **createEdgeBetween(int **adjMatrix, char coursesOneStudentTakes[MAX_COURSE_COUNT][MAX_COURSE_NAME_LENGTH], int numberOfCoursesOneStudentTakes){

    int i, j, correctIndexes[courseCount];

    for(i = 0; i < courseCount; i++){

        correctIndexes[i] = 0;

    }

    for(i = 0; i < courseCount; i++){

        for(j = 0; j < numberOfCoursesOneStudentTakes; j++){

            if(strcmp((char *)courses[i], coursesOneStudentTakes[j]) == 0){

                correctIndexes[i] = 1;

            }
        }
    }

    for(i = 0; i < courseCount; i++){

        if(correctIndexes[i] == 1){

            for(j = 0; j < courseCount; j++) {

                if(correctIndexes[j] == 1 && i != j) {

                    adjMatrix[i][j] = 1;
                    adjMatrix[j][i] = 1;

                }

            }
        }
    }

    return adjMatrix;
}

//Stack Functions.
void push(int item){

    stack[++top] = item;

}

int pop(){

    return stack[top--];
}

int peek(){

    return stack[top];
}

bool isStackEmpty(){

    return top == -1;
}

int getAdjUnvisitedVertex(int vertexIndex, int **adjMatrix, Vertex **list){

    int i;

    for(i = 0; i < courseCount; i++){

        if(adjMatrix[vertexIndex][i] == 1 && list[i] -> visited == false)
            return i;

    }

    return -1;
}

Vertex *coloring(Vertex **list, int **adjMatrix){

    int i;

    for(i = 0; i < courseCount; i++){

        if(list[i] -> visited == false)
            *list = DFS(list, adjMatrix, i);

    }

    return *list;
}

Vertex *DFS(Vertex **list, int **adjMatrix, int firstIndex){

    //An array to hold the colors around the vertex.
    int colorsAround[10];

    //I will traverse the graph using Depth First Search(DFS) algorithm.
    int i, j;
    //First, we mark the first vertex as visited.
    list[firstIndex] -> visited = true;

    //Set the first vertexes color to yellow.
    strcpy(list[firstIndex] -> color, colors[0]);

    //Pushing the first vertexes index into stack.
    push(firstIndex);

    while(!isStackEmpty()){

        //Initializing colorsAround array.
        for(i = 0; i < 10; i++)
            colorsAround[i] = 0;

        //Get the unvisited vertex of vertex which is at the top of the stack.
        int unvisitedVertex = getAdjUnvisitedVertex(peek(), adjMatrix, list);

        //If no adjacent vertex found...
        if(unvisitedVertex == -1)
            pop();
        else{

            list[unvisitedVertex] -> visited = true;
            push(unvisitedVertex);
            //Now, we need to determine this vertexes color.
            for(i = 0; i < courseCount; i++){

                if(adjMatrix[unvisitedVertex][i] == 1){

                    for(j = 0; j < 10; j++){

                        if(strcmp(list[i] -> color, colors[j]) == 0)
                            colorsAround[j]++;

                    }
                }
            }

            if(determineColor(colorsAround) == -1)
                strcpy(list[unvisitedVertex] -> color, "NULL");
            else
                strcpy(list[unvisitedVertex] -> color, colors[determineColor(colorsAround)]);

        }
    }

    return *list;
}

int determineColor(const int *colorsAround){

    if(colorsAround[0] != 0)
        if(colorsAround[1] != 0)
            if(colorsAround[2] != 0)
                if(colorsAround[3] != 0)
                    if(colorsAround[4] != 0)
                        if(colorsAround[5] != 0)
                            if(colorsAround[6] != 0)
                                if(colorsAround[7] != 0)
                                    if(colorsAround[8] != 0)
                                        if(colorsAround[9] != 0)
                                            return -1;
                                        else
                                            return 9;
                                    else
                                        return 8;
                                else
                                    return 7;
                            else
                                return 6;
                        else
                            return 5;
                    else
                        return 4;
                else
                    return 3;
            else
                return 2;
        else
            return 1;
    return  0;
}
