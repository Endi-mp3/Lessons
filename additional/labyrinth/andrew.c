#include <stdio.h>
#include <string.h> 
#include <stdlib.h>

typedef struct {
    int x;
	int y;
} Point;

typedef struct {
    Point p;
    int dist;
} Node;

Point start = {1, 1};
Point end = {2, 5};

int map_original[6][6] = {
	{1, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 0, 1},
	{1, 0, 0, 0, 0, 1},
	{1, 0, 0, 1, 0, 1},
	{1, 1, 1, 1, 1, 1},
};

int map_route [6][6] = {0};
int map_visited[6][6] = {0};




int dx[4] = {-1, 1, 0, 0};
int dy[4] = {0, 0, 1, -1};

int search_route(int **map,  Point start,  Point end)
{
	int exit = 1;

    Point curent = start;
	
	char raw[8];
    int maze[200][200];

	int found = 0;      
    int finalDist = -1;  

   
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            scanf("%s", raw);

          
            if (raw[0] == 'c' || raw[0] == 'C')
                maze[i][j] = 1;   
            else if (raw[0] == '1')
                maze[i][j] = 1;   
            else
                maze[i][j] = 0;   
        }
    }

	Point parent[200][200];
	memset(map_visited, 0, sizeof(map_visited));
    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 6; j++)
            parent[i][j].x = parent[i][j].y = -1;  

	Node queue[200 * 200];
    int head = 0, tail = 0;

	queue[tail++] = (Node){start, 0};
    map_visited[start.y][start.x] = 1;

	int found = 0;       
    int finalDist = -1;

	while(head < tail) {
			Node cur = queue[head++];

			if (cur.p.x == end.x && cur.p.y == end.y) {
            found = 1;
            finalDist = cur.dist;
            break;
        }

			for (int k = 0; k < 4; k++) {
            int nx = cur.p.x + dx[k];
            int ny = cur.p.y + dy[k];

			if (nx < 0 || ny < 0 || nx >= 6 || ny >= 6)
                continue;
		
			if (maze[ny][nx] == 1)
                continue;

			if (map_visited[ny][nx])
                continue;
			
			map_visited[ny][nx] = 1;

			parent[ny][nx] = cur.p;

			queue[tail++] = (Node){(Point){nx, ny}, cur.dist + 1};



			}

	}

	

 
} 

int main()
{
	
		
	int r = search_route(map_original, start, end);
	return ;
}
