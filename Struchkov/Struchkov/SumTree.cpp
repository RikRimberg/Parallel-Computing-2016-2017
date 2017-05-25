// SumTree.cpp - содержит функции для работы с деревьями

#include "stdafx.h"
#include "SumTree.h"
using namespace std;

// Статус выполнения pthread_create
#define ERROR_CREATE_THREAD -11
#define ERROR_JOIN_THREAD   -12
#define SUCCESS               0

/********************************************************************************
************************ РАБОТА С БИНАРНЫМ ДЕРЕВОМ ******************************
*********************************************************************************/

/* Добавить узел */
tnode* addNode(double v, tnode *tree) 
{
	// Если дерева нет, то формируем корень
	if (tree == NULL) 
	{	
		tree = new tnode;		// память под узел
		tree->value = v;		// значение
		tree->sum = 0;			// сумма дочерних
		tree->left =  NULL;		// ветви инициализируем пустотой
		tree->right = NULL;		
	}
	else if (v < tree->value)	// условие добавление левого потомка
		tree->left = addNode(v,tree->left);
	else						// условие добавление правого потомка
		tree->right = addNode(v,tree->right);
	return(tree);
}

/* Очистить дерево */
void freeTree(tnode *tree) 
{
	if(tree!=NULL) {
		freeTree(tree->left);
		freeTree(tree->right);
		delete tree;
	}
}

/* Вывод дерева (в префиксной форме) */
void printTree(tnode *tree) 
{
	if (tree != NULL)
	{
		cout << tree->value << " " << tree->sum << endl;	//Отображаем корень дерева
		printTree(tree->left);		//Рекурсивная функция для левого поддерева
		printTree(tree->right);		//Рекурсивная функция для правого поддерева
	}
}

/********************************************************************************
************************* ПОСЛЕДОВАТЕЛЬНЫЙ АЛГОРИТМ *****************************
*********************************************************************************/

/* Сумма потомков (последовательно) */
double sum_serial(tnode *tree)
{
	// пока не пустой узел
	if (tree != NULL)
	{		
		double sum_l = 0.0; // сумма левого поддерева
		double sum_r = 0.0; // сумма правого поддерева

		// сумма потомков для левого поддерева		
		if (tree->left != NULL)
		{
			tree->left->sum = sum_serial(tree->left);
			sum_l = tree->left->sum + tree->left->value;
		}

		// сумма потомков для правого поддерева
		if (tree->right != NULL)
		{
			tree->right->sum = sum_serial(tree->right);
			sum_r = tree->right->sum + tree->right->value;
		}

		// вернуть сумму потомков
		return sum_l + sum_r; 
	}
	return 0;
}

/********************************************************************************
**************** ПАРАЛЛЕЛЬНЫЙ АЛГОРИТМ С ИСПОЛЬЗОВАНИЕМ OPENMP ******************
*********************************************************************************/

/* Сумма потомков (параллельно задачами) */
double sum_parallel_task(tnode *tree, int nthreads)
{
	// пока не пустой узел
	if (tree != NULL)
	{		
		double sum_l = 0.0; // сумма левого поддерева
		double sum_r = 0.0; // сумма правого поддерева

		// последовательное выполнение при диспользовании всех выделенных потоков
		if (nthreads <= 1)
			return sum_serial(tree);


		if (tree->left != NULL)
		{
			// каждый рекурсивный вызов - это задача
			#pragma omp task shared(sum_l)
			{
				// в каждом вложенном регионе уменьшаем число выделяемых потоков вдвое
				tree->left->sum = sum_parallel_task(tree->left, nthreads/2);
				sum_l = tree->left->sum + tree->left->value;
			}
		}

		if (tree->right != NULL)
		{
			#pragma omp task shared(sum_r)
			{
				tree->right->sum = sum_parallel_task(tree->right, nthreads - nthreads / 2);
				sum_r = tree->right->sum + tree->right->value;
			}
		}

		// ожидаем завершение дочерних задач
		#pragma omp taskwait
		return sum_l + sum_r;
	}
	return 0;
}

/* Сумма потомков (параллельно секциями) */
double sum_parallel_section(tnode *tree)
{
	// пока не пустой узел
	if (tree != NULL)
	{		
		double sum_l = 0.0; // сумма левого поддерева
		double sum_r = 0.0; // сумма правого поддерева
		
		int a = omp_get_max_active_levels();

		// при достижении максимальной глубины вложенных параллельных областей
		if(omp_get_active_level() >= omp_get_max_active_levels())
			return sum_serial(tree);

		#pragma omp parallel num_threads(2) 
		{
			#pragma omp sections
			{
				#pragma omp section 
				{ 
					// сумма потомков для левого поддерева
					if (tree->left != NULL)
					{
						tree->left->sum = sum_parallel_section(tree->left);
						sum_l = tree->left->sum + tree->left->value;
					}
				} 

				#pragma omp section 
				{ 
					// сумма потомков для правого поддерева
					if (tree->right != NULL)
					{
						tree->right->sum = sum_parallel_section(tree->right);
						sum_r = tree->right->sum + tree->right->value;
					}				
				} 
			}
		}
		return sum_l + sum_r;
	}

	return 0;
}


/********************************************************************************
**************** ПАРАЛЛЕЛЬНЫЙ АЛГОРИТМ С ИСПОЛЬЗОВАНИЕМ PTHREAD *****************
*********************************************************************************/

/* Сумма потомков (параллельно pthread) */
void* sum_parallel_pthread(void *args)
{
	// инициализируем специальную структуру данных
	ptArg *arg = (ptArg *)args; 
	
	// пока не пустой узел
	if (arg->tree != NULL)
	{	
		double sum_l = 0.0; // сумма левого поддерева
		double sum_r = 0.0; // сумма правого поддерева	
		
		// последовательное выполнение при расходовании всех потоков
		if (arg->nthreads <= 1)
		{
			arg->sum = sum_serial(arg->tree);
			return 0;
		}

		// поток для левого поддерева
		pthread_t thread_l;
		int status_l;
		int status_addr_l;
		ptArg arg_l;
		arg_l.sum = 0.0;
		if (arg->tree->left != NULL)
		{		
			arg_l.tree = arg->tree->left;
			arg_l.nthreads = arg->nthreads/2;
			status_l = pthread_create(&thread_l, NULL, sum_parallel_pthread, (void*) &arg_l);	
			if (status_l != 0) {
				printf("main error: can't create thread, status = %d\n", status_l);
				exit(ERROR_CREATE_THREAD);
			}
		}

		// поток для правого поддерева
		pthread_t thread_r;
		int status_r; // статус завершения pthread_create
		int status_addr_r; // статус pthread_join
		ptArg arg_r;
		arg_r.sum = 0.0;
		if (arg->tree->right != NULL)
		{		
			arg_r.tree = arg->tree->right;
			arg_r.nthreads =  arg->nthreads - arg->nthreads/2;
			status_r = pthread_create(&thread_r, NULL, sum_parallel_pthread, (void*) &arg_r);	
			if (status_r != 0) {
				printf("main error: can't create thread, status = %d\n", status_r);
				exit(ERROR_CREATE_THREAD);
			}
		}		
		
		// ожидание завершения потоков
		status_l = pthread_join(thread_l, (void**)&status_addr_l);
		if (status_l != SUCCESS) {
			printf("main error: can't join thread, status = %d\n", status_l);
			exit(ERROR_JOIN_THREAD);
		}
		if (arg->tree->left != NULL)
		{
			arg->tree->left->sum = arg_l.sum;
			sum_l = arg->tree->left->sum + arg->tree->left->value;
		}

		status_r = pthread_join(thread_r, (void**)&status_addr_r);
		if (status_r != SUCCESS) {
			printf("main error: can't join thread, status = %d\n", status_r);
			exit(ERROR_JOIN_THREAD);
		}
		if (arg->tree->right != NULL)
		{
			arg->tree->right->sum = arg_r.sum;
			sum_r = arg->tree->right->sum + arg->tree->right->value;
		}
		
		arg->sum = sum_l + sum_r;
	}

	// освобождаем память ?
	// не уверена насчет выделения нужного числа потоков ?
	return 0;
}


/* Логорифм по основанию 2 (для вычисления max_active_levels) */
int log2( unsigned int n )
{
	return unsigned int(log( (double)n ) / log( 2.0 ));
}