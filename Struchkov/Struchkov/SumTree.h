// SumTree.h - заголовочный файл: содержит описание функций для работы с деревьями

/* Структура бинарного дерева*/
struct tnode 
{
	double value;				// числовое значение
	double sum;					// сумма значений дочерних узлов 
	struct tnode *left;			// левый потомок
	struct tnode *right;		// правый потомок
};

/*Аргументы потока pthread*/
struct ptArg {
    struct tnode *tree;
    int nthreads;
    double sum;
};

/* Добавить узел */
tnode* addNode(double v, tnode *tree); 

/* Очистить дерево */
void freeTree(tnode *tree); 

/* Вывод дерева */
void printTree(tnode *tree); 

/* Сумма потомков (последовательно) */
double sum_serial(tnode *tree);

/* Сумма потомков (параллельно задачами) */
double sum_parallel_task(tnode *tree, int nthreads);

/* Сумма потомков (параллельно секциями) */
double sum_parallel_section(tnode *tree);

/* Сумма потомков (параллельно pthred) */
void* sum_parallel_pthread(void *args);

/* Логарифм по основанию 2 */
int log2( unsigned int n );

