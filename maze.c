#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

//define node struct
typedef struct node{
  int xcoord, ycoord; //holds an x and y
  struct node* next; //link to next node
}NODE;

//list struct
typedef struct LIST{
  NODE* head; //list with head pointer
}LIST;

//maze struct
typedef struct mazeStruct
{
 char **arr;           /* allows for a dynamic 2-D maze of any size */
 int xsize, ysize;
 int xstart, ystart;
 int xend, yend;
} maze;

//initialize list
LIST* init(){
 LIST* tmp = (LIST*)malloc(sizeof(LIST)); //allocate memory
 tmp->head = NULL; 
 return tmp; //return new list
}

//push function on the stack
void push(LIST *l, int x, int y, int debugMode) {
NODE *p =(NODE*) malloc(sizeof(NODE)); //create new node

  p->xcoord = x; 
  p->ycoord = y; 
  p->next = l->head; //link it to current front of list
  if(debugMode == TRUE){ //debugger information
      printf("Pushing coordinate %d, %d onto the stack \n", x,y);
  }
  l->head = p; //this node is now front of list
}

//Function to remove the front node of the list
void pop(LIST *l, int debugMode){
  NODE* tmp = l->head;  //hold the node needed to be freed
  l->head = l->head->next; // front of the list will move over by one
  if(debugMode == TRUE){ //debugger information
      printf("Popping coordinate %d, %d off the stack \n", tmp->xcoord, tmp->ycoord);
  }
  free(tmp); //free the node
}

//check if the list is empty
int isEmpty(LIST *l){
    if(l->head == NULL)
        return TRUE;
    else
        return FALSE;
}

//Recursive print function to print the solution from the start to finish
void lst_print(NODE *head) {
NODE *p = head;
  if(p == NULL){ //reached end of list?
    return;
  }

  lst_print(p->next); //keep going till you reach end of list

  printf("(%d, %d) ", p->xcoord, p->ycoord); //print everything in reverse order, otherwise from start to finish
}

//list reset function
void lst_reset(LIST* l){
NODE *p = l->head;
NODE *tmp;
  while(p != NULL){ //while the list is not empty
     tmp = p;
     p = p->next;
     free(tmp); //deallocate memory
  }
  l->head = NULL;
}

//maze reset function
void maze_reset(maze* m){
int i;
char** temp = m->arr;
  /* for each malloc we did in our function we need to free it */
  for(i=1; i< (m->xsize+2); i++){  
    free(temp[i]);  //loop to free every char* 
  }
  free(m->arr); //free the final char**
  /* just set the parameters to invalid ones */
  m->xsize = -1;
  m->ysize = -1;
  m->xend = -1;
  m->yend = -1;
  m->xstart = -1;
  m->ystart = -1;
}

/* Depth first search algorithm.  Takes the maze and grabs the start 
 * grabs the start value and pushes it onto the stack.  Then it marks the
 * spot as "*" aka walled off or visited for this case.  The algorithm implemented
 * will first check up,then down, then right, then left.  It will move to that spot
 * change the variables keeping track of where were at and mark the spot as visited, 
 * and also push it onto the stack.  The way this function terminates is if a solution
 * is found or if the stack is left empty.  I.e. there's no more moves to be done.
 */
void search(LIST *l, maze* m, int debugMode){
  int xcurr, ycurr; //tracking current array indexes
  /* set it to start coordinates */
  xcurr = m->xstart;  
  ycurr = m->ystart;
  push(l,m->xstart, m->ystart, debugMode); //push start onto stack
  m->arr[xcurr][ycurr] = '*'; //mark start as visited;
  while(!(isEmpty(l)) && !((l->head->xcoord == m->xend)&&(l->head->ycoord == m->yend))){ //loop until stack is empty or solution found
      if(m->arr[xcurr+1][ycurr] != '*'){ //check up
          xcurr = xcurr+1;
          push(l,xcurr,ycurr, debugMode);
          m->arr[xcurr][ycurr] = '*';
      }
      else if(m->arr[xcurr-1][ycurr] != '*'){ //check down
          xcurr = xcurr-1;
          push(l,xcurr,ycurr, debugMode);
          m->arr[xcurr][ycurr] = '*';
      }
      else if(m->arr[xcurr][ycurr+1] != '*'){ //check right
          ycurr = ycurr+1;
          push(l,xcurr,ycurr,debugMode);
          m->arr[xcurr][ycurr] = '*';
      }
      else if(m->arr[xcurr][ycurr-1] != '*'){ //check left
          ycurr = ycurr-1;
          push(l,xcurr,ycurr, debugMode);
          m->arr[xcurr][ycurr] = '*';
      }
      else{ //otherwise the spot were at cant make a move so pop it off the stack
          pop(l, debugMode);
          if(l->head != NULL){ //if the stack isnt empty revert our xcurr and ycurr to previous node
            xcurr = l->head->xcoord;
            ycurr = l->head->ycoord;
          } 
      }
  }
}

int main(int argc, char** argv){
  int i,j;
  int debugMode = FALSE;
  FILE *src;
  maze m1;
  int flag = 0;
  int xpos, ypos;
  /* check for debugging flag */
  for(i = 0; i < argc; i++){
      if(strcmp(argv[i], "-d")==0)
          debugMode = TRUE;
  }
  if(debugMode == TRUE)
    printf(" Debugging Information \n");
  for(i = 1; i < argc; i++){ //loop through arguments and find first file to read 
      if((src = fopen(argv[i], "r")) == NULL){
        if(!(strcmp(argv[i], "-d")==0))  
          fprintf (stderr, "Can't open input file: %s\n", argv[i] );
      }
      else{ 
          flag = 1; //if file found flag is true
          break; //stop searching
      }
  }

  
  int argindex = i; //keep track of what argument we were checking to check for multiple inputs
  if (flag == 0 ){ //never found a file
    fprintf (stderr, "ERROR: NO VALID INPUT FILES WERE ENTERED EXITING...\n" );
    return 0;
  }
  /* Keep executing program until we finished through all the arguments */
  while(argindex < argc){
  
          if(strcmp(argv[argindex], "-d")==0){ //if its a debugger flag found after the input files just go back to start of while loop
              argindex++; 
              continue;
          }
          else if (( src = fopen( argv[argindex], "r" )) == NULL ){ //open file, if it cant be opened go into if statement
              fprintf(stderr, "Can't open input file: %s\n", argv[argindex]); //print error
              argindex++; //increment
              continue; // go to start of loop
          }
          
          /* begin scanning information for maze struct */
          if(fscanf (src, "%d %d", &m1.xsize, &m1.ysize) == EOF){
             fprintf(stderr, "ERROR: Reached end of file without valid maze size parameters.  Exiting... \n");
             return 0;
          }
          while(m1.ysize <= 0||m1.ysize>30||m1.xsize<=0||m1.xsize>30){ //check if it was a valid size input
              fprintf(stderr, "Invalid maze size.  Try again \n");
              if(fscanf (src, "%d %d", &m1.xsize, &m1.ysize) == EOF){ //if we reach end of file without a valid input then exit
                 fprintf(stderr, "ERROR: Reached end of file without valid maze size parameters.  Exiting... \n");
                 return 0;
              }
          }
          if(fscanf (src, "%d %d", &m1.xstart, &m1.ystart) == EOF){ //check if we reached the end of file for start
             fprintf(stderr, "ERROR: Reached end of file without valid start coordinates.  Exiting... \n");
             return 0;
          }
          while(m1.ystart>m1.ysize||m1.ystart<=0||m1.xstart>m1.xsize||m1.xstart<=0){ //check for valid start input
              fprintf(stderr, "Invalid start coordinate.  Try again \n");
              if(fscanf (src, "%d %d", &m1.xstart, &m1.ystart) == EOF){
                 fprintf(stderr, "ERROR: Reached end of file without valid start coordinates.  Exiting... \n");
                 return 0;
              }
          }
          if(fscanf (src, "%d %d", &m1.xend, &m1.yend) == EOF){
              fprintf(stderr, "ERROR: Reached end of file without valid end coordinates.  Exiting... \n");
              return 0;
          }
          while(m1.xend>m1.xsize||m1.xend<=0||m1.yend<=0||m1.yend>m1.ysize||(m1.yend == m1.ystart && m1.xend == m1.xstart)){ //check for valid end input make sure end doesnt overlap with start point
              fprintf(stderr, "Invalid end coordinate.  Try again\n");
              if(fscanf (src, "%d %d", &m1.xend, &m1.yend) == EOF){ 
                  fprintf(stderr, "ERROR: Reached end of file without valid end coordinates.  Exiting... \n");
                  return 0;
              }
          }
          /* print them out to verify the input */
          printf ("size: %d, %d\n", m1.xsize, m1.ysize);
          printf ("start: %d, %d\n", m1.xstart, m1.ystart);
          printf ("end: %d, %d\n", m1.xend, m1.yend);

          /* allocate the maze */
          m1.arr = (char **) malloc (sizeof(char *) * (m1.xsize+2));
          for (i = 0; i < (m1.xsize+2); i++){
             m1.arr[i] = (char *) malloc (sizeof(char ) * (m1.ysize+2));
          }
          /* initialize the maze to empty */
          for (i = 0; i < m1.xsize+2; i++)
             for (j = 0; j < m1.ysize+2; j++)
               m1.arr[i][j] = '.';

          /* mark the borders of the maze with *'s */
          for (i=0; i < m1.xsize+2; i++){
             m1.arr[i][0] = '*';
             m1.arr[i][m1.ysize+1] = '*';
          }
          for (i=0; i < m1.ysize+2; i++){
             m1.arr[0][i] = '*';
             m1.arr[m1.xsize+1][i] = '*';
          }

          /* mark the starting and ending positions in the maze */
          m1.arr[m1.xstart][m1.ystart] = 's';
          m1.arr[m1.xend][m1.yend] = 'e';
                  
          /* mark the blocked positions in the maze with *'s */
          while (fscanf (src, "%d %d", &xpos, &ypos) != EOF){
            if((xpos<=m1.xsize && xpos>0)&&(ypos<=m1.ysize && ypos>0)){  //check if its within the bounds of the maze
              if(m1.arr[xpos][ypos] == '.'){ //check if the location is a 'free' location
                m1.arr[xpos][ypos] = '*';
              }
              else
                  fprintf(stderr, "Cant block an already filled spot.  Try again\n"); //if not we cant overwrite the spot
            }
            else{
                fprintf(stderr, "Not a valid location for wall.  Try again\n");
            }
            
          }

          /* print out the initial maze */
          printf("\n");
          for (i = 0; i < m1.xsize+2; i++){
             for (j = 0; j < m1.ysize+2; j++)
               printf ("%c", m1.arr[i][j]);
             printf("\n");
          }

          printf("\n");
          LIST* list = init(); //create the list
          search(list, &m1, debugMode); //search algorithm call 
          if(list->head == NULL){ //if the list is empty we never found a solution
            printf("The maze has no solution\n");
          }
          else{ //otherwise print the solution
            lst_print(list->head);
            printf("\n");
          }
          argindex++; //increment argindex
          lst_reset(list); //free list and reset it
          maze_reset(&m1); //reset the maze
          printf("\n\n"); //format
  }
}
