#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXN 200   // Максимальный размер поля. Выбран с запасом, чтобы избежать динамики.

// Структура для хранения координат клетки
typedef struct {
    int x, y;
} Point;

// Узел очереди BFS: координаты + расстояние от старта
typedef struct {
    Point p;
    int dist;
} Node;

// Смещения для движения по четырём направлениям (вверх/вниз/влево/вправо)
// Используем фиксированный набор направлений, чтобы BFS был детерминированным.
int dx[4] = {1, -1, 0, 0};
int dy[4] = {0, 0, 1, -1};

int main() {
    int id, n;
    Point start, end;

    // Читаем ID поля — используется только для вывода результата
    scanf("%d", &id);

    // Размер поля (матрица n×n)
    scanf("%d", &n);

    // Координаты старта и финиша в формате X:Y
    // Важно: координаты идут в формате (x, y), но в массиве мы храним как [y][x]
    scanf("%d:%d", &start.x, &start.y);
    scanf("%d:%d", &end.x, &end.y);

    char raw[8];
    int maze[MAXN][MAXN];

    // Чтение поля
    // Каждая ячейка может быть:
    //   c/C — холодная стена (граница)
    //   1   — внутренняя стена
    //   0   — свободная клетка
    //   S/E — старт/финиш, но по сути тоже свободные клетки
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            scanf("%s", raw);

            // Приводим все типы стен к единому виду: 1 = стена
            if (raw[0] == 'c' || raw[0] == 'C')
                maze[i][j] = 1;   // внешняя стена
            else if (raw[0] == '1')
                maze[i][j] = 1;   // внутренняя стена
            else
                maze[i][j] = 0;   // свободная клетка
        }
    }

    // Массив посещённых клеток для BFS
    // Нужен, чтобы:
    //   1) не заходить в одну клетку дважды
    //   2) гарантировать корректность BFS (иначе возможны циклы)
    int visited[MAXN][MAXN];
    memset(visited, 0, sizeof(visited));

    // Массив родителей — для восстановления пути
    // parent[y][x] хранит координату клетки, из которой мы пришли в (x, y)
    // Изначально все родители = (-1, -1), что означает "нет родителя"
    Point parent[MAXN][MAXN];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            parent[i][j].x = parent[i][j].y = -1;

    // Очередь BFS
    // Используем статический массив вместо динамики — быстрее и безопаснее
    Node queue[MAXN * MAXN];
    int head = 0, tail = 0;

    // Добавляем стартовую точку в очередь
    queue[tail++] = (Node){start, 0};
    visited[start.y][start.x] = 1;  // старт помечаем как посещённый

    int found = 0;       // флаг: найден ли путь
    int finalDist = -1;  // длина кратчайшего пути

    // BFS — поиск в ширину
    // Гарантирует нахождение кратчайшего пути в невзвешенном графе
    while (head < tail) {
        Node cur = queue[head++];

        // Если достигли финиша — можно завершать
        if (cur.p.x == end.x && cur.p.y == end.y) {
            found = 1;
            finalDist = cur.dist;
            break;
        }

        // Перебираем 4 направления движения
        for (int k = 0; k < 4; k++) {
            int nx = cur.p.x + dx[k];
            int ny = cur.p.y + dy[k];

            // Проверка выхода за границы
            if (nx < 0 || ny < 0 || nx >= n || ny >= n)
                continue;

            // Проверка на стену
            if (maze[ny][nx] == 1)
                continue;

            // Проверка, что клетка ещё не посещена
            if (visited[ny][nx])
                continue;

            // Помечаем клетку как посещённую
            visited[ny][nx] = 1;

            // Запоминаем, откуда пришли — важно для восстановления пути
            parent[ny][nx] = cur.p;

            // Добавляем клетку в очередь BFS
            queue[tail++] = (Node){(Point){nx, ny}, cur.dist + 1};
        }
    }

    // Если путь не найден — выводим сообщение
    if (!found) {
        printf("%d %d:%d %d:%d - NO PATH\n", id, start.x, start.y, end.x, end.y);
        return 0;
    }

    // Восстановление пути
    // Идём от финиша назад по parent[][], пока не дойдём до старта
    Point path[MAXN * MAXN];
    int pathLen = 0;

    Point cur = end;
    while (!(cur.x == start.x && cur.y == start.y)) {
        path[pathLen++] = cur;
        cur = parent[cur.y][cur.x];
    }
    path[pathLen++] = start;  // добавляем старт в конец цепочки

    // Вывод результата
    printf("%d %d:%d %d:%d - %d\n", id, start.x, start.y, end.x, end.y, finalDist);

    // Путь восстановлен в обратном порядке, поэтому выводим с конца
    for (int i = pathLen - 1; i >= 0; i--) {
        printf("%d:%d\n", path[i].x, path[i].y);
    }

    return 0;
}

