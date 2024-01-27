#include "tetris.h"

char Table[ROWS][COLUMNS] = {0};
int final = 0;
char GameOn = TRUE;
suseconds_t timer = 400000;
int decrease = 1000;

typedef struct {
    char **array;
    int width, row, col;
} s_tetromino;

s_tetromino current;

const s_tetromino tetrominoes[7]= {
	{(char *[]){(char []){0,1,1},(char []){1,1,0}, (char []){0,0,0}}, 3},
	{(char *[]){(char []){1,1,0},(char []){0,1,1}, (char []){0,0,0}}, 3},
	{(char *[]){(char []){0,1,0},(char []){1,1,1}, (char []){0,0,0}}, 3},
	{(char *[]){(char []){0,0,1},(char []){1,1,1}, (char []){0,0,0}}, 3},
	{(char *[]){(char []){1,0,0},(char []){1,1,1}, (char []){0,0,0}}, 3},
	{(char *[]){(char []){1,1},(char []){1,1}}, 2},
	{(char *[]){(char []){0,0,0,0}, (char []){1,1,1,1}, (char []){0,0,0,0}, (char []){0,0,0,0}}, 4}
};

s_tetromino copy_tetromino(s_tetromino shape){
	s_tetromino new_shape = shape;
	char **copyshape = shape.array;
	new_shape.array = (char**)malloc(new_shape.width*sizeof(char*));
    int i, j;
    for(i = 0; i < new_shape.width; i++){
		new_shape.array[i] = (char*)malloc(new_shape.width*sizeof(char));
		for(j=0; j < new_shape.width; j++) {
			new_shape.array[i][j] = copyshape[i][j];
		}
    }
    return new_shape;
}

void destroy_tetromino(s_tetromino shape){
    int i;
    for(i = 0; i < shape.width; i++){
		free(shape.array[i]);
    }
    free(shape.array);
}

int check_position(s_tetromino shape){
	char **array = shape.array;
	int i, j;
	for(i = 0; i < shape.width;i++) {
		for(j = 0; j < shape.width ;j++){
			if((shape.col+j < 0 || shape.col+j >= COLUMNS || shape.row+i >= ROWS)){
				if(array[i][j])
					return FALSE;
				
			}
			else if(Table[shape.row+i][shape.col+j] && array[i][j])
				return FALSE;
		}
	}
	return TRUE;
}

void rotate_shape(s_tetromino shape){
	s_tetromino temp = copy_tetromino(shape);
	int i, j, k, width;
	width = shape.width;
	for(i = 0; i < width ; i++){
		for(j = 0, k = width-1; j < width ; j++, k--){
				shape.array[i][j] = temp.array[k][i];
		}
	}
	destroy_tetromino(temp);
}

void print_table(){
	char Buffer[ROWS][COLUMNS] = {0};
	int i, j;
	for(i = 0; i < current.width ;i++){
		for(j = 0; j < current.width ; j++){
			if(current.array[i][j])
				Buffer[current.row+i][current.col+j] = current.array[i][j];
		}
	}
	clear();
	for(i=0; i<COLUMNS-9; i++)
		printw(" ");
	printw("42 Tetris\n");
	for(i = 0; i < ROWS ;i++){
		for(j = 0; j < COLUMNS ; j++){
			printw("%c ", (Table[i][j] + Buffer[i][j])? '#': '.');
		}
		printw("\n");
	}
	printw("\nScore: %d\n", final);
}

struct timeval before_now, now;
int has_time_elapsed(){
	return ((suseconds_t)(now.tv_sec*1000000 + now.tv_usec) -((suseconds_t)before_now.tv_sec*1000000 + before_now.tv_usec)) > timer;
}

void set_timeout(int time) {
	time = 1;
	timeout(1);
}

int main() {
    srand(time(0));
    final = 0;
    int c;
    initscr();
	gettimeofday(&before_now, NULL);
	set_timeout(1);
	s_tetromino new_shape = copy_tetromino(tetrominoes[rand()%7]);
    new_shape.col = rand()%(COLUMNS-new_shape.width+1);
    new_shape.row = 0;
    destroy_tetromino(current);
	current = new_shape;
	if(!check_position(current)){
		GameOn = FALSE;
	}
    print_table();
	while(GameOn){
		if ((c = getch()) != ERR) {
			s_tetromino temp = copy_tetromino(current);
			switch(c){
				case 's':
					temp.row++;  //move down
					if(check_position(temp))
						current.row++;
					else {
						int i, j;
						for(i = 0; i < current.width ;i++){
							for(j = 0; j < current.width ; j++){
								if(current.array[i][j])
									Table[current.row+i][current.col+j] = current.array[i][j];
							}
						}
						int n, m, sum, count=0;
						for(n=0;n<ROWS;n++){
							sum = 0;
							for(m=0;m< COLUMNS;m++) {
								sum+=Table[n][m];
							}
							if(sum==COLUMNS){
								count++;
								int l, k;
								for(k = n;k >=1;k--)
									for(l=0;l<COLUMNS;l++)
										Table[k][l]=Table[k-1][l];
								for(l=0;l<COLUMNS;l++)
									Table[k][l]=0;
								timer-=decrease--;
							}
						}
						final += 100*count;
						s_tetromino new_shape = copy_tetromino(tetrominoes[rand()%7]);
						new_shape.col = rand()%(COLUMNS-new_shape.width+1);
						new_shape.row = 0;
						destroy_tetromino(current);
						current = new_shape;
						if(!check_position(current)){
							GameOn = FALSE;
						}
					}
					break;
				case 'd':
					temp.col++;
					if(check_position(temp))
						current.col++;
					break;
				case 'a':
					temp.col--;
					if(check_position(temp))
						current.col--;
					break;
				case 'w':
					rotate_shape(temp);
					if(check_position(temp))
						rotate_shape(current);
					break;
			}
			destroy_tetromino(temp);
			print_table();
		}
		gettimeofday(&now, NULL);
		if (has_time_elapsed()) {
			s_tetromino temp = copy_tetromino(current);
			switch('s'){
				case 's':
					temp.row++;
					if(check_position(temp))
						current.row++;
					else {
						int i, j;
						for(i = 0; i < current.width ;i++){
							for(j = 0; j < current.width ; j++){
								if(current.array[i][j])
									Table[current.row+i][current.col+j] = current.array[i][j];
							}
						}
						int n, m, sum, count=0;
						for(n=0;n<ROWS;n++){
							sum = 0;
							for(m=0;m< COLUMNS;m++) {
								sum+=Table[n][m];
							}
							if(sum==COLUMNS){
								count++;
								int l, k;
								for(k = n;k >=1;k--)
									for(l=0;l<COLUMNS;l++)
										Table[k][l]=Table[k-1][l];
								for(l=0;l<COLUMNS;l++)
									Table[k][l]=0;
								timer-=decrease--;
							}
						}
						s_tetromino new_shape = copy_tetromino(tetrominoes[rand()%7]);
						new_shape.col = rand()%(COLUMNS-new_shape.width+1);
						new_shape.row = 0;
						destroy_tetromino(current);
						current = new_shape;
						if(!check_position(current)){
							GameOn = FALSE;
						}
					}
					break;
				case 'd':
					temp.col++;
					if(check_position(temp))
						current.col++;
					break;
				case 'a':
					temp.col--;
					if(check_position(temp))
						current.col--;
					break;
				case 'w':
					rotate_shape(temp);
					if(check_position(temp))
						rotate_shape(current);
					break;
			}
			destroy_tetromino(temp);
			print_table();
			gettimeofday(&before_now, NULL);
		}
	}
	destroy_tetromino(current);
	endwin();
	int i, j;
	for(i = 0; i < ROWS ;i++){
		for(j = 0; j < COLUMNS ; j++){
			printf("%c ", Table[i][j] ? '#': '.');
		}
		printf("\n");
	}
	printf("\nGame over!\n");
	printf("\nScore: %d\n", final);
    return 0;
}
