#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#define WIDTH 10
#define HEIGHT 10

#define MAX_DISTANCE (WIDTH + HEIGHT + 1)

int grid[WIDTH][HEIGHT];

struct _Node {
	int x, y;
	int f, g, h;
	struct _Node* parent;
	struct _Node* next;
};

typedef struct _Node Node;

struct _List {
	Node* head;
};

typedef struct _List List;

// *********** NODE FUNCTIONS *****************

// Computes manhattan distance
int distance(int x0, int y0, int x1, int y1)
{
	int d = x0 > x1 ? x0 - x1 : x1 - x0;
	d += y0 > y1 ? y0 - y1 : y1 - y0;
	return d;
}

// Create a node and compute heuristics
Node* createNode(int x, int y, int xd, int yd, Node* parent)
{
	Node* node = calloc(1, sizeof(Node));
	node->x = x;
	node->y = y;
	if (parent)
	{
		node->parent = parent;
		node->g = node->parent->g + distance(x, y, parent->x, parent->y);
		node->h = distance(x, y, xd, yd);
		node->f = node->g + node->h;
	}
	
	return node;
}

// Test if a cell x, y  is already in a list
bool cellInList(int x, int y, List* list)
{
	Node* cur = list->head;
	bool result = false;

	while (cur && !result)
		if (cur->x == x && cur->y == y)
			result = true;
		else
			cur = cur->next;

	return result;
}



// *********** LIST FUNCTIONS *****************

// Push a node to a list
void push(List* list, Node* node)
{
	Node* cur = list->head;
	if (!list->head) list->head = node;
	else
	{
		while (cur->next) cur = cur->next;
		cur->next = node;
	}
	node->next = NULL; // Safety
}

// Remove a node from a list
Node* removeFrom(List* list, Node* node)
{
	Node* cur = list->head;
	Node* prev = NULL;

	while (cur && cur != node)
	{
		prev = cur;
		cur = cur->next;
	}
		
	if (cur && cur == node)
	{
		if (prev)
			prev->next = cur->next;
		else
			list->head = cur->next;
		cur->next = NULL;
	}
		

	return cur;
}

// ************ Algorithme A Star *********************
// Trouver les cellules accessibles a partir d'un noeud, les ajouter en liste si elles n'y sont pas deja.
int pushAccessibleToOpen(Node* node, int xd, int yd, List* open, List* closed)
{
	int nbAdded = 0;
	if (node->x - 1 >= 0 && grid[node->x-1][node->y] && !cellInList(node->x - 1, node->y, open) && !cellInList(node->x - 1, node->y, closed))
		{
			Node* newNode = createNode(node->x - 1, node->y, xd, yd, node);
			push(open, newNode);
			nbAdded++;
		}

	if (node->x + 1 < WIDTH && grid[node->x + 1][node->y] && !cellInList(node->x + 1, node->y, open) && !cellInList(node->x + 1, node->y, closed))
	{
		Node* newNode = createNode(node->x + 1, node->y, xd, yd, node);
		push(open, newNode);
		nbAdded++;
	}

	if (node->y - 1>= 0 && grid[node->x][node->y-1] && !cellInList(node->x, node->y - 1, open) && !cellInList(node->x, node->y - 1, closed))
	{
		Node* newNode = createNode(node->x, node->y - 1, xd, yd, node);
		push(open, newNode);
		nbAdded++;
	}

	if (node->y + 1 < HEIGHT && grid[node->x][node->y + 1] && !cellInList(node->x, node->y + 1, open) && !cellInList(node->x, node->y + 1, closed))
	{
		Node* newNode = createNode(node->x, node->y + 1, xd, yd, node);
		push(open, newNode);
		nbAdded++;
	}

	return nbAdded;
 }

// Pour chaque cellule de la liste fermée on cherche les acessibles et on les ajouten en liste ouverte
int findAccessible(int xd, int yd, List* open, List* closed)
{
	int nbAdded = 0;
	Node* cur = closed->head;
	while (cur)
	{
		nbAdded += pushAccessibleToOpen(cur, xd, yd, open, closed);
		cur = cur->next;
	}
		
	return 	nbAdded;
}

// Trouver le node avec le F le plus faible en liste ouverte et le passer en liste fermée
Node* transferBestNode(List* open, List* closed)
{
	Node* cur = open->head;
	int minF = MAX_DISTANCE;
	Node* best = NULL;
	while (cur)
	{
		if (cur->f < minF)
		{
			minF = cur->f;
			best = cur;
		}
		cur = cur->next;
	}
	
	// Remove best from open and push it to closed
	if (best)
	{
		removeFrom(open, best);
		push(closed, best);
		return best;
	}

	return NULL; // Error 
}

// Compute the number of parents
int computeWay(Node* last)
{
	Node* parent = last;
	int parcours = 0;

	while (parent)
	{
		//printf("(%d %d) - ", parent->x, parent->y);
		parcours++;
		parent = parent->parent;
	}

	return parcours - 1; // Parceque le premier ne compte pas
}

// Algorithme A Star
int AStarLength(int xs, int ys, int xd, int yd)
{
	List* open = calloc(1, sizeof(List));
	List* closed = calloc(1, sizeof(List));

	// Etape 0 : ajouter le point de départ
	Node* depart = createNode(xs, ys, xd, yd, NULL);
	Node* last = NULL;
	push(closed, depart);
	int distToTarget = distance(xs, ys, xd, yd);
	int iteration = 0;
	while (distToTarget > 0)
	{
		iteration++;

		// Etape 1 & 2: Pour chaque case de la liste close , on calcul les cases accessibles, on le ajoute en liste ouverte si elle ne sont dans aucune liste
		int nbAdded = findAccessible(xd, yd, open, closed);

		// Etape 3, choisir la case avec F le plus faible dans liste ouverte et la passer en fermé
		last = transferBestNode(open, closed);
		distToTarget = last->h;
		//printf("Iteration %d, added to open list %d, distance to target %d\n", iteration, nbAdded, distToTarget);
	}

	// Now up from last node to first
	int result = computeWay(last);
	return result;
}

// ********** GRID FILE MANAGEMENT ***********************
void ReadGrid(const char* filename)
{
	FILE* gf = fopen(filename, "r");
	if (!gf)
		printf("File %s not found", filename);
	else
	{
		for (int j = 0; j < 10; j++)
			for (int i = 0; i < 10; i++)
				fscanf(gf, "%d", &grid[i][j]);

		fclose(gf);
	}
}

void PrintGrid()
{
	for (int j = 0; j < 10; j++)
	{
		for (int i = 0; i < 10; i++)
			printf("%d ", grid[i][j]);
		printf("\n");
	}
}

// ********** MAIN ***********************
int main(void)
{
	ReadGrid("..\\grid.txt");
	//PrintGrid();

	// Test cases
	FILE* tf = fopen("..\\data.txt", "r");
	if (!tf)
		printf("File data.txt not found\n");
	else
	{
		int nbLines = 0;
		fscanf(tf, "%d", &nbLines);
			for (int i = 0; i < nbLines; i++)
			{
				int xs, ys, xd, yd, r;
				fscanf(tf, "%d %d %d %d %d", &xs, &ys, &xd, &yd, &r);
				int cr = AStarLength(xs, ys, xd, yd);
				if (cr != r)
					printf("ERROR : (%d %d) => (%d %d) should be %d, computed %d\n", xs, ys, xd, yd, r, cr);
				else
					printf("OK : (%d %d) => (%d %d) parcours %d\n", xs, ys, xd, yd, cr);
			}

	}
	return 0;
}