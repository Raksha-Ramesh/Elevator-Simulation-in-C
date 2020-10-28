#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#define MAX 12
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"

typedef struct button
{
	int press;					
	int flooroc;
	struct button* next;
	struct button* prev;
}button;

typedef struct elevator
{
	int occupants;
	int* floordisp;
	button* head, *tail;		//ordered doubly linked list
	char dxn; 					//'u' or 'd'
}elevator;

button* createNode(int press,int flooroc)
{
	button* b=(button*)malloc(sizeof(button));
	b->press=press;
	b->flooroc=flooroc;
	b->prev=b->next=NULL;
	return b;
}

char pressButton()					
{
	char press[3];
	printf(YELLOW "\n\nPress button: ");
	scanf("%s",press);
	printf(RESET);
	return press[0];
}

void delNode(elevator* e,button* b)
{
	if(!e->head)
		return;
	if(b==e->head)
		e->head=b->next;
	if(b==e->tail)
		e->tail=b->prev;
	if(b->next)
		b->next->prev=b->prev;
	if(b->prev)
		b->prev->next=b->next;
	e->floordisp[b->press]=0;
	free(b);
	b=NULL;
}

void insert_update(elevator* e,button* b)		
{
	e->floordisp[b->press]=1;
	if(!e->head)								//empty list
	{
		e->head=e->tail=b;
	}
	
	else if(b->press < e->head->press)  		//insert at head
	{
		b->next=e->head;
		e->head->prev=b;
		e->head=b;
	}
	else if(b->press > e->tail->press)			//insert at tail
	{
		e->tail->next=b;
		b->prev=e->tail;
		e->tail=b;
	}
	else										//insert in middle
	{
		button* cur=e->head->next;
		while(b->press > cur->press)
			cur=cur->next;
		if(b->press == cur->press)				//update flooroc
		{
			cur->flooroc+=b->flooroc;
		}
		cur->prev->next=b;
		b->prev=cur->prev;
		b->next=cur;
		cur->prev=b;
	}
}

void printgrid(int curfloor,elevator* e)
{
	printf("\n\n\n");
	e->floordisp[curfloor]=2;
	int x=9;
	for(int i=1;i<=3;i++)
	{
		for(int j=1;j<=3;j++)
		{
			if(e->floordisp[x]==2)			//curfloor
				printf(CYAN);
			else if(e->floordisp[x]==1)		//pressed button
				printf(YELLOW);
			else							//unpressed button
				printf(RESET);
			printf("%8d",x);
			x--;
		}
		printf("\n\n\n");
	}
	if(e->floordisp[0]==2) printf(CYAN); 
	else if(e->floordisp[0]==1) printf(YELLOW);
	else printf(RESET);
	printf("%16d",x);
	printf(RESET);
	printf("\n");
	printf(GREEN);
	for(int i=0;i<e->occupants;i++)
	{
		if(i%3==0)
			printf("\n      ");
		printf("\\O/     ");
	}
	printf(RESET "\n\n\n");
}

void delayrun(int milliseconds)
{
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
        now = clock();    
}

void countdelay(int curfloor,int nextfloor)
{
	printf(YELLOW "Moving...\n");
	delayrun(1000*abs(curfloor-nextfloor));
}

void enterLift(elevator* e)
{
	int n,check;
	int press;
	button* b;
	int exceed;
	printf(YELLOW);
	while(1)
	{
		exceed=0;
		if(e->occupants==MAX)
		{
			printf("\nMax capacity reached. Closing lift...\n");
			delayrun(2000);
			break;
		}
		press=pressButton();
		if(press=='x')
		{
			printf("\nClosing lift...\n"); 
			delayrun(2000);
			break;
		}
		if(press=='!')
		{		
			printf(RED "ALERT!!!\n");
			printf(RESET "Shutting down elevator.\nPlease remain clam.\n\n");	
			exit(1);
		}
		if(press>=48 && press<=58)		
		{
			while(1)
			{
				printf("People getting off on floor %d: ",press-48); scanf("%d",&n);
				if(n<0)
				{
					printf(YELLOW "That can't be right! Please recount.\n" RESET);
					continue;
				}
				break;
			}
			if(e->occupants+n>MAX)
			{	
				exceed=e->occupants+n-MAX;
				printf(RED "Max capacity exceeded.\n%d of you will have to wait for the next lift.\n",exceed);			
			}
			printf(YELLOW);
			b=createNode(press-48,n-exceed);		
			insert_update(e,b);
			e->occupants+=b->flooroc;
		}
		else
		{
			printf("Invalid button. Please re-enter.\n");
		}
	}
}
		
void move(elevator* e)
{
	int curfloor=0,nextfloor=0,press=0,n;
	button* b=e->head;			
	button* temp=NULL;
	
	while(1)
	{
		curfloor=b->press;
		printf(GREEN "\nDing!\n");
		printf(CYAN "Floor: %d\n",b->press);
		printf(RESET);	
		if(e->occupants)
		{
			e->occupants-=b->flooroc;
		}
		printgrid(b->press,e);
		
		enterLift(e); printf(RESET "Lift closed.\n\n");
		
		switch(e->dxn)
		{
			case 'u':	
				if(b->next)
				{
					temp = b->next;
					countdelay(curfloor,b->next->press);
					delNode(e, b);
					b = temp;
				}
				else if(b->prev)
				{
					temp = b->prev;
					countdelay(curfloor,b->prev->press);
					delNode(e, b);
					b = temp;
					e->dxn = 'd';
				}
				break;
			case 'd':	
				if(b->prev)
				{
					countdelay(curfloor,b->prev->press);
					temp = b->prev;
					delNode (e, b);
					b = temp;
				}
				else if(b->next)
				{
					countdelay(curfloor,b->next->press);
					temp = b->next;
					delNode(e, b);
					b = temp;
					e->dxn = 'u';
				}
				break;
		}	
		
	}
}

int main()
{
	elevator* e=(elevator*)malloc(sizeof(elevator));
	e->head=createNode(0,0);
	e->tail=e->head;
	e->dxn='u';
	e->floordisp=(int*)calloc(10,sizeof(int));
	move(e);
	return 0;
}
