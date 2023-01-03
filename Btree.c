#include "stdio.h"
#include "stdlib.h"

#define M 5

typedef struct no {
    int    n; /* Numero de chaves */
    int    keys[M - 1]; /*Array de chaves*/
    struct no *p[M]; /* Array de ponteiros */
} No;
No *root = NULL;

typedef enum StatusDaChave {
  Duplicate,
  SearchFailure,
  Success,
  InsertIt,
  LessKeys,
} StatusDaChave;

void insert(int key);
void DelNo(int x);
void search(int x);
StatusDaChave ins(No *r, int x, int* y, No** u);
int searchPos(int x, int *key_arr, int n);
StatusDaChave del(No *r, int x);
void eatline(void);


int main() {
    int key;
    int choice;
    printf("Creation of B tree for M=%d\n", M);
    do {
        printf("0.Sair\n");
        printf("1.Inserir\n");
        printf("2.Deletar\n");
        printf("3.Buscar\n");
        printf("Escolha uma opcao : ");
        scanf("%d", &choice); eatline();

        switch (choice) {
        case 1:
            printf("Digite uma chave : ");
            scanf("%d", &key); eatline();
            insert(key);
            break;
        case 2:
            printf("Digite uma chave : ");
            scanf("%d", &key); eatline();
            DelNo(key);
            break;
        case 3:
            printf("Digite uma chave : ");
            scanf("%d", &key); eatline();
            search(key);
            break;
        default:
            if(choice != 0){
                printf("Escolha invalida\n");
            }
        }
    }while(choice != 0);
    return 0;
}

void insert(int key) {
        No *newNo;
        int upKey;
        StatusDaChave value;
        value = ins(root, key, &upKey, &newNo);
        if (value == Duplicate)
            printf("A chave ja foi inserida\n");
        if (value == InsertIt) {
            No *uproot = root;
            root = (No*)malloc(sizeof(No));
            root->n = 1;
            root->keys[0] = upKey;
            root->p[0] = uproot;
            root->p[1] = newNo;
        }
}/*Inserscao*/

StatusDaChave ins(No *ptr, int key, int *upKey, No **newNo) {
        No *newPtr, *lastPtr;
        int pos, i, n, splitPos;
        int newKey, lastKey;
        StatusDaChave value;
        if (ptr == NULL) {
            *newNo = NULL;
            *upKey = key;
            return InsertIt;
        }
        n = ptr->n;
        pos = searchPos(key, ptr->keys, n);
        if (pos < n && key == ptr->keys[pos])
            return Duplicate;
        value = ins(ptr->p[pos], key, &newKey, &newPtr);
        if (value != InsertIt)
            return value;
        /*Se as chaves em No for menor que M-1, onde M é a ordem da árvore B*/
        if (n < M - 1) {
            pos = searchPos(newKey, ptr->keys, n);
            /*Deslocando a chave e o ponteiro para a direita para inserir a nova chave*/
            for (i = n; i>pos; i--) {
                ptr->keys[i] = ptr->keys[i - 1];
                ptr->p[i + 1] = ptr->p[i];
            }
            /*A chave é inserida no local exato*/
            ptr->keys[pos] = newKey;
            ptr->p[pos + 1] = newPtr;
            ++ptr->n; /*incrementando o número de chaves em NO*/
            return Success;
        }
         /*Se as chaves em Nos são máximas e a posição de No a ser inserida é a última*/
        if (pos == M - 1) {
            lastKey = newKey;
            lastPtr = newPtr;
        }
        else { /*Se as teclas em No forem máximas e a posição de Não a ser inserida não for a última*/
            lastKey = ptr->keys[M - 2];
            lastPtr = ptr->p[M - 1];
            for (i = M - 2; i>pos; i--) {
                ptr->keys[i] = ptr->keys[i - 1];
                ptr->p[i + 1] = ptr->p[i];
            }
            ptr->keys[pos] = newKey;
            ptr->p[pos + 1] = newPtr;
        }
        splitPos = (M - 1) / 2;
        (*upKey) = ptr->keys[splitPos];

        (*newNo) = (No*)malloc(sizeof(No));/*Direito No após Split*/
        ptr->n = splitPos; /*Nº de chaves para a esquerda de No*/
        (*newNo)->n = M - 1 - splitPos;/*Nº de chaves para a direita de No*/
        for (i = 0; i < (*newNo)->n; i++) {
            (*newNo)->p[i] = ptr->p[i + splitPos + 1];
            if (i < (*newNo)->n - 1)
                (*newNo)->keys[i] = ptr->keys[i + splitPos + 1];
            else
                (*newNo)->keys[i] = lastKey;
        }
        (*newNo)->p[(*newNo)->n] = lastPtr;
        return InsertIt;
}

void search(int key) {
        int pos, i, n;
        No *ptr = root;
        // printf("Search path:\n");
        while (ptr) {
            n = ptr->n;
            for (i = 0; i < ptr->n; i++)
                printf(" %d", ptr->keys[i]);
            printf("\n");
            pos = searchPos(key, ptr->keys, n);
            if (pos < n && key == ptr->keys[pos]) {
                printf("Chave %d encontrada\n", key);
                return;
            }
            ptr = ptr->p[pos];
        }
        printf("Chave %d nao encontrada\n", key);
}

int searchPos(int key, int *key_arr, int n) {
    int pos = 0;
    while (pos < n && key > key_arr[pos])
        pos++;
    return pos;
}

void DelNo(int key) {
        No *uproot;
        StatusDaChave value;
        value = del(root, key);
        switch (value) {
        case SearchFailure:
            printf("Chave %d nao encontrada\n", key);
            break;
        case LessKeys:
            uproot = root;
            root = root->p[0];
            free(uproot);
            break;
        default:
            return;
        }
}

StatusDaChave del(No *ptr, int key) {
        int pos, i, pivot, n, min;
        int *key_arr;
        StatusDaChave value;
        No **p, *lptr, *rptr;

        if (ptr == NULL)
            return SearchFailure;
        /*Atribui valores a No*/
        n = ptr->n;
        key_arr = ptr->keys;
        p = ptr->p;
        min = (M - 1) / 2;/*numeor minimo de chaves*/
        //procura pela chave para deletar
        pos = searchPos(key, key_arr, n);
        // p é uma folha
        if (p[0] == NULL) {
            if (pos == n || key < key_arr[pos])
                return SearchFailure;
            /*desloca chaves e ponteiros para a esquerda*/
            for (i = pos + 1; i < n; i++){
                key_arr[i - 1] = key_arr[i];
                p[i] = p[i + 1];
            }
            return --ptr->n >= (ptr == root ? 1 : min) ? Success : LessKeys;
        }

         //se encontrada chave, mas p não é uma folha
        if (pos < n && key == key_arr[pos]) {
            No *qp = p[pos], *qp1;
            int nkey;
            while (1) {
                nkey = qp->n;
                qp1 = qp->p[nkey];
                if (qp1 == NULL)
                        break;
                qp = qp1;
            }
            key_arr[pos] = qp->keys[nkey - 1];
            qp->keys[nkey - 1] = key;
        }
        value = del(p[pos], key);
        if (value != LessKeys)
            return value;

        if (pos > 0 && p[pos - 1]->n > min) {
            pivot = pos - 1; /*pivô para esquerda e direita No*/
            lptr = p[pivot];
            rptr = p[pos];
            /*Atribui valores para a direita No*/
            rptr->p[rptr->n + 1] = rptr->p[rptr->n];
            for (i = rptr->n; i>0; i--) {
                rptr->keys[i] = rptr->keys[i - 1];
                rptr->p[i] = rptr->p[i - 1];
            }
            rptr->n++;
            rptr->keys[0] = key_arr[pivot];
            rptr->p[0] = lptr->p[lptr->n];
            key_arr[pivot] = lptr->keys[--lptr->n];
            return Success;
        }
         //if (posn > min)
        if (pos < n && p[pos + 1]->n > min) {
            pivot = pos; /*pivô para esquerda e direita No*/
            lptr = p[pivot];
            rptr = p[pivot + 1];
            /*Atribui valores para a esquerda No*/
            lptr->keys[lptr->n] = key_arr[pivot];
            lptr->p[lptr->n + 1] = rptr->p[0];
            key_arr[pivot] = rptr->keys[0];
            lptr->n++;
            rptr->n--;
            for (i = 0; i < rptr->n; i++) {
                rptr->keys[i] = rptr->keys[i + 1];
                rptr->p[i] = rptr->p[i + 1];
            }
            rptr->p[rptr->n] = rptr->p[rptr->n + 1];
            return Success;
        }

        if (pos == n)
            pivot = pos - 1;
        else
            pivot = pos;

        lptr = p[pivot];
        rptr = p[pivot + 1];
        /*mesclar no direito com No esquerdo*/
        lptr->keys[lptr->n] = key_arr[pivot];
        lptr->p[lptr->n + 1] = rptr->p[0];
        for (i = 0; i < rptr->n; i++) {
            lptr->keys[lptr->n + 1 + i] = rptr->keys[i];
            lptr->p[lptr->n + 2 + i] = rptr->p[i + 1];
        }
        lptr->n = lptr->n + rptr->n + 1;
        free(rptr); /*Remove No da direita*/
        for (i = pos + 1; i < n; i++) {
            key_arr[i - 1] = key_arr[i];
            p[i] = p[i + 1];
        }
        return --ptr->n >= (ptr == root ? 1 : min) ? Success : LessKeys;
}

void eatline(void) {
    char c;
    while ((c = getchar()) != '\n');
}
