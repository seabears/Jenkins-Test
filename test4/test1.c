#include <stdio.h>
#include <string.h>

#define MAX_N 10
#define MAX_M 100

typedef struct {
    int x;
    int y;
}Pos;
typedef struct {
    int x;
    int y;
    int age;
}PreTree;

typedef struct {
    int ages[100];
    int idx;
    int idxDie;
    int food;
}Map;

// 입력
int N, M, K;   // 땅 크기, 이미 심은 나무 수, 몇년후
int A[MAX_N][MAX_N];    // 겨울에 추가하는 양분
PreTree preTree[MAX_M];    // 사전에 심은 나무
//

Map map[MAX_N][MAX_N];  // 양분, 나무들의 나이

void debug() {
    printf("\n----DEBUG---------------\n");
    printf("---------idx------------\n");
    for (int i = 0;i < N;i++) {
        for (int j = 0;j < N;j++) {
            printf("%d ", map[i][j].idx);
        }
        printf("\n");
    }
    printf("----------food----------\n");
    for (int i = 0;i < N;i++) {
        for (int j = 0;j < N;j++) {
            printf("%d ", map[i][j].food);
        }
        printf("\n");
    }
    printf("----------age-----------\n");
    for (int i = 0;i < N;i++) {
        for (int j = 0;j < N;j++) {
            printf("%d ", map[i][j].ages[0]);
        }
        printf("\n");
    }
    printf("------------------------\n");

    printf("%d %d\n", map[1][0].ages[0], map[2][1].ages[0]);

}
void spring() {
    for (int i = 0;i < N;i++) {
        for (int j = 0;j < N;j++) {
            
            //map[i][j].idxDie = 0;

            for (int k = 0;k < map[i][j].idx;k++) {
                map[i][j].food -= map[i][j].ages[k];

                // 양분 못 먹으면 뒷 나무들 다 죽음
                if (map[i][j].food < 0) {
                    map[i][j].food += map[i][j].ages[k];
                    map[i][j].idxDie = map[i][j].idx;
                    map[i][j].idx = k;  // k 전까지 살아있음
                    break;
                }
                else {
                    map[i][j].ages[k] += 1;
                    //map[i][j].idxDie = 0;
                }

            }
        }
    }
}   
void summer() {
    for (int i = 0;i < N;i++) {
        for (int j = 0;j < N;j++) {
            for (int k = map[i][j].idx;k < map[i][j].idxDie;k++) {
                map[i][j].food += map[i][j].ages[k] / 2;
            }
        }
    }
}
void fall() {
    int dx[8] = { 0,-1,-1,-1,0,1,1,1 };
    int dy[8] = { 1,1,0,-1,-1,-1,0,1 };
    
    for (int i = 0;i < N;i++) {
        for (int j = 0;j < N;j++) {
            // 해당 칸에 나무가 있고
            if (map[i][j].idx <= 0) continue;
            
            for (int k = 0;k < map[i][j].idx;k++) {
                // 5의 배수인 나무만
                if (map[i][j].ages[k] % 5 != 0) continue;
                
                for (int m = 0;m < 8;m++) {
                    Pos newTree = { i + dx[m], j + dy[m] };
                    if (newTree.x < 0 || newTree.x >= N || newTree.y < 0 || newTree.y >= N) continue;

                    // 나무 뒤로 밀고
                    map[newTree.x][newTree.y].idx += 1;
                    for (int n = map[newTree.x][newTree.y].idx - 1;n >= 0;n--) {    // idx-2
                        map[newTree.x][newTree.y].ages[n + 1] = map[newTree.x][newTree.y].ages[n];
                    }
                    // 맨 앞에 추가
                    map[newTree.x][newTree.y].ages[0] = 1;
                }
            }
        }
    }
    // 나무 ages 정렬
}
void winter() {
    for (int i = 0;i < N;i++) {
        for (int j = 0;j < N;j++) {
            map[i][j].food += A[i][j];
        }
    }
}

int main() {

    scanf("%d %d %d", &N, &M, &K);
    
    // 겨울에 추가하는 양분
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            scanf("%d", &A[i][j]);
        }
    }
    
    // 심은 나무 정보
    for (int i = 0; i < M; i++) {
        int x = 0, y = 0, z = 0;
        scanf("%d %d %d", &x, &y, &z);
        x -= 1, y -= 1;
        map[x][y].ages[map[x][y].idx++] = z;
    }



    // 기본 양분 5로 초기화
    for (int i = 0;i < N;i++) {
        for (int j = 0;j < N;j++) {
            map[i][j].food = 5;
        }
    }

    int year = 0;
    while (year++ != K) {
        //printf("\n===============year %d\n", year);
        //debug();
        spring();
        //debug();
        summer();
        fall();
        winter();
        //debug();
        //printf("f %d\n", map[0][0].food);
    }

    int sum = 0;
    for (int i = 0;i < N;i++) {
        for (int j = 0;j < N;j++) {
            sum += map[i][j].idx;
        }
    }
    printf("%d\n", sum);

    return 0;
}

/*
땅 NxN
땅에 양분 기본 5

M개의 나무를 땅에
1x1에도 여러개 나무 가능
봄에 나무가 자기 나이만큼 양분 먹고 나이가 1 증가
하나의 칸에 나무가 여러개 -> 어린 나무 부터 양분 섭취
양분 부족하면 즉사

여름에
봄에 죽은 나무 -> 양분으로
죽은 나무의 나이 / 2 => 양분

가을
나이가 5의 배수인 나무가 번식
인접한 8개의 칸에 나이 1 나무 생김

겨울
땅에 양분 추가
A[r][c] -> 입력

구하고 싶은거
K년이 지난 후 땅에 살아있는 나무의 개수



*/
