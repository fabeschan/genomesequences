# include<stdio.h>
# include<stdlib.h>

# define F 0
# define T 1

class match;

struct NODE
{
	int Info;
	int Flag;
	struct  NODE *Left_Child;
	struct  NODE *Right_Child;
	match *matchp;
};

struct NODE *Binary_Tree (int , struct NODE *, int *, match *m);
void Output(struct NODE *, int );
struct  NODE *Balance_Right_Heavy(struct NODE *, int *);
struct NODE *Balance_Left_Heavy(struct NODE *, int *);
struct NODE *DELETE(struct NODE *, struct NODE *, int *);
struct NODE *Delete_Element(struct NODE *, int , int *);

/* Function to insert an element into tree */

struct NODE *  Binary_Tree (int Info, struct NODE *Parent, int *H, match *m)
{
	struct NODE *Node1;
	struct NODE *Node2;
	if(!Parent)
	{
		Parent = (struct NODE *) malloc(sizeof(struct NODE));
		Parent->Info = Info;
		Parent->Left_Child = NULL;
		Parent->Right_Child = NULL;
		Parent->Flag = 0;
		Parent->matchp = m;
		*H = T;
		return (Parent);
	}

	if(Info < Parent->Info)
	{
		Parent->Left_Child = Binary_Tree(Info, Parent->Left_Child, H, m);
		if(*H)
		/* Left branch has grown higher */
		{
			switch(Parent->Flag)
			{
			case 1: /* Right heavy */
				Parent->Flag = 0;
				*H = F;
				break;
			case 0: /* Balanced tree */
				Parent->Flag = -1;
				break;
			case -1: /* Left heavy */
				Node1 = Parent->Left_Child;
				if(Node1->Flag == -1)
				{
					////printf("\n Left to Left Rotation\n");
					Parent->Left_Child= Node1->Right_Child;
					Node1->Right_Child = Parent;
					Parent->Flag = 0;
					Parent = Node1;
				}
				else
				{
					////printf("\n Left to right rotation\n");
					Node2 = Node1->Right_Child;
					Node1->Right_Child = Node2->Left_Child;
					Node2->Left_Child = Node1;
					Parent->Left_Child = Node2->Right_Child;
					Node2->Right_Child = Parent;
					if(Node2->Flag == -1)
						Parent->Flag = 1;
					else
						Parent->Flag = 0;
					if(Node2->Flag == 1)
						Node1->Flag = -1;
					else
						Node1->Flag = 0;
					Parent = Node2;
				}

				Parent->Flag = 0;
				*H = F;
			}
		}
	}

	if(Info > Parent->Info)
	{
		Parent->Right_Child = Binary_Tree(Info, Parent->Right_Child, H, m);
		if(*H)
		/* Right branch has grown higher */
		{
			switch(Parent->Flag)
			{
			case -1: /* Left heavy */
				Parent->Flag = 0;
				*H = F;
				break;
			case 0: /* Balanced tree */
				Parent->Flag = 1;
				break;

			case 1: /* Right heavy */
				Node1 = Parent->Right_Child;
				if(Node1->Flag == 1)
				{
					//printf("\n Right to Right Rotation\n");
					Parent->Right_Child= Node1->Left_Child;
					Node1->Left_Child = Parent;
					Parent->Flag = 0;
					Parent = Node1;
				}
				else
				{
					//printf("\n Right to Left Rotation\n");
					Node2 = Node1->Left_Child;
					Node1->Left_Child = Node2->Right_Child;
					Node2->Right_Child = Node1;
					Parent->Right_Child = Node2->Left_Child;
					Node2->Left_Child = Parent;

					if(Node2->Flag == 1)
						Parent->Flag = -1;
					else
						Parent->Flag = 0;
					if(Node2->Flag == -1)
						Node1->Flag = 1;
					else
						Node1->Flag = 0;
					Parent = Node2;
				}

				Parent->Flag = 0;
				*H = F;
			}
		}
	}
	return(Parent);
}

/* Output function */

void Output(struct NODE *Tree,int Level)
{
	int i;
	if (Tree)
	{
		Output(Tree->Right_Child, Level+1);
		printf("\n");
		for (i = 0; i < Level; i++)
			printf("   ");
		printf("%c", Tree->Info);
		Output(Tree->Left_Child, Level+1);
	}
}

/* Balancing Right Heavy */

struct NODE * Balance_Right_Heavy(struct NODE *Parent, int *H)
{
	struct NODE *Node1, *Node2;

	switch(Parent->Flag)
	{
	case -1:
		Parent->Flag = 0;
		break;

	case 0:
		Parent->Flag = 1;
		*H= F;
		break;

	case 1: /* Rebalance */
		Node1 = Parent->Right_Child;
		if(Node1->Flag >= 0)
		{
			//printf("\n Right to Right Rotation\n");
			Parent->Right_Child= Node1->Left_Child;
			Node1->Left_Child = Parent;
			if(Node1->Flag == 0)
			{
				Parent->Flag = 1;
				Node1->Flag = -1;
				*H = F;
			}
			else
			{
				Parent->Flag = Node1->Flag = 0;
			}
			Parent = Node1;
		}
		else
		{
			//printf("\n Right to Left Rotation\n");
			Node2 = Node1->Left_Child;
			Node1->Left_Child = Node2->Right_Child;
			Node2->Right_Child = Node1;
			Parent->Right_Child = Node2->Left_Child;
			Node2->Left_Child = Parent;

			if(Node2->Flag == 1)
				Parent->Flag = -1;
			else
				Parent->Flag = 0;
			if(Node2->Flag == -1)
				Node1->Flag = 1;
			else
				Node1->Flag = 0;
			Parent = Node2;
			Node2->Flag = 0;
		}
	}
	return(Parent);
}

/* Balancing Left Heavy */

struct NODE * Balance_Left_Heavy(struct NODE *Parent, int *H)
{
	struct NODE *Node1, *Node2;

	switch(Parent->Flag)
	{
	case 1:
		Parent->Flag = 0;
		break;

	case 0:
		Parent->Flag = -1;
		*H= F;
		break;

	case -1: /*  Rebalance */
		Node1 = Parent->Left_Child;
		if(Node1->Flag <= 0)
		{
			//printf("\n Left to Left Rotation\n");
			Parent->Left_Child= Node1->Right_Child;
			Node1->Right_Child = Parent;
			if(Node1->Flag == 0)
			{
				Parent->Flag = -1;
				Node1->Flag = 1;
				*H = F;
			}
			else
			{
				Parent->Flag = Node1->Flag = 0;
			}
			Parent = Node1;
		}
		else
		{
			//printf("\n Left to Right Rotation\n");
			Node2 = Node1->Right_Child;
			Node1->Right_Child = Node2->Left_Child;
			Node2->Left_Child = Node1;
			Parent->Left_Child = Node2->Right_Child;
			Node2->Right_Child = Parent;

			if(Node2->Flag == -1)
				Parent->Flag = 1;
			else
				Parent->Flag = 0;

			if(Node2->Flag == 1)
				Node1->Flag = -1;
			else
				Node1->Flag = 0;
			Parent = Node2;
			Node2->Flag = 0;
		}
	}
	return(Parent);
}

/* Replace the node at which key is found with last right key of a left child */

struct NODE * DELETE(struct NODE *R, struct NODE *Temp, int *H)
{
	struct NODE *Dnode = R;
	if( R->Right_Child != NULL)
	{
		R->Right_Child = DELETE(R->Right_Child, Temp, H);
		if(*H)
			R = Balance_Left_Heavy(R, H);
	}
	else
	{
		Dnode = R;
		Temp->Info = R->Info;
		R = R->Left_Child;
		free(Dnode);
		*H = T;
	}
	return(R);
}
/* Delete the key element from the tree */

struct NODE * Delete_Element(struct NODE *Parent, int Info, int *H)
{
	struct NODE *Temp;
	if(!Parent)
	{
		//printf("\n Information does not exist");
		return(Parent);
	}
	else
	{
		if (Info < Parent->Info )
		{
			Parent->Left_Child = Delete_Element(Parent->Left_Child, Info, H);
			if(*H)
				Parent = Balance_Right_Heavy(Parent, H);
		}
		else
			if(Info > Parent->Info)
			{
				Parent->Right_Child = Delete_Element(Parent->Right_Child, Info, H);
				if(*H)
					Parent = Balance_Left_Heavy(Parent, H);
			}
			else
			{
				Temp= Parent;
				if(Temp->Right_Child == NULL)
				{
					Parent = Temp->Left_Child;
					*H = T;
					free(Temp);
				}
				else
					if(Temp->Left_Child == NULL)
					{
						Parent = Temp->Right_Child;
						*H = T;
						free(Temp);
					}
					else
					{
						Temp->Left_Child = DELETE(Temp->Left_Child, Temp, H);
						if(*H)
							Parent = Balance_Right_Heavy(Parent, H);
					}
			}
	}
	return(Parent);
}
