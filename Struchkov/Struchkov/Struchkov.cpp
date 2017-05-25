// Struchkov.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "SumTree.h"

using namespace std;

int main()
{	
	system("chcp 1251");						
	system("cls");
	srand(time(NULL));	
	
	// размерность массива определяет число узлов в дереве
	int arr_size = 1000000;						
	printf("Size : \t\t\t%ld\n", arr_size);	
	
	// массив под дерево
	double *arrForTree = new double[arr_size];	

	// заполнение массива случайными числами [0,1)							
	for (int i = 0; i < arr_size; i++)
	{
		arrForTree[i] = (double)rand() / (RAND_MAX + 1.0); 
	}
		

	// Вывод полученного массива
	//for (int i = 0; i < arr_size; i++)		
	//{
	//	cout << arrForTree[i] << endl;
	//}

	// Сохранение сгенерированной последовательности в файл
	//ofstream fout("array_for_tree.txt"); 
	//for (int i = 0; i < arr_size; i++)
	//{
	//	fout << arrForTree[i] << "\n";
	//}	
	//fout.close();


	// Формирование бинарных деревьев под каждый алгоритм
	tnode* rootSerial = NULL;    
	tnode* rootParallelSections = NULL;
	tnode* rootParallelTasks = NULL;
	tnode* rootParallelPthread = NULL;

	for (int i = 0; i < arr_size; i++)
	{
		rootSerial = addNode(arrForTree[i], rootSerial);
		rootParallelSections = addNode(arrForTree[i], rootParallelSections);
		rootParallelTasks = addNode(arrForTree[i], rootParallelTasks);
		rootParallelPthread = addNode(arrForTree[i], rootParallelPthread);
	}

	// временные засечки
	double start_time, end_time;	

	// задаем число потоков для параллельных алгоритмов
	int nthreads = omp_get_num_procs(); // omp_get_num_procs() = 4
	printf("Threads number : \t%i\n\n", nthreads);	
	

	/*
	** Последовательный алгоритм. 
	*/

	// старт таймер
	start_time = omp_get_wtime();	
	// считать последовательно
	rootSerial->sum = sum_serial(rootSerial);	
	// стоп таймер
	end_time = omp_get_wtime();
	// вывод времени
	printf("Serial time: \t\t\t%lf sec.\n", end_time-start_time);	

	
	
	/* 
	** Параллельный алгоритм с использованием секций OpenMP.
	*/
	
	// включить вложенный параллелизм
	omp_set_nested(1);
	// максимальная глубина вложенных областей
	omp_set_max_active_levels(log2(nthreads));

		
	// старт таймер
	start_time = omp_get_wtime();		
	

	// считаем параллельно секциями
	rootParallelSections->sum = sum_parallel_section(rootParallelSections);
	// стоп таймер
	end_time = omp_get_wtime();
	// вывод времени
	printf("Parallel (sections) time: \t%lf sec.\n", end_time-start_time);	
	/// проверка на ошибку в вычислениях
	//printf("Error sum: \t\t\t%lf\n", rootSerial->sum-rootParallelSections->sum);
	

	/* Параллельный алгоритм с использованием задач OpenMP */

	// старт таймер
	start_time = omp_get_wtime();

	// считаем параллельно задачами
	#pragma omp parallel
	{
		#pragma omp single nowait
		rootParallelTasks->sum = sum_parallel_task(rootParallelTasks,nthreads); 
	}
	// стоп таймер
	end_time = omp_get_wtime();
	// вывод времени
	printf("Parallel (tasks) time: \t\t%lf sec.\n", end_time-start_time);


	/*
	** Параллельный алгоритм с использованием Pthread
	*/

	// старт таймер
	start_time = omp_get_wtime();

	// считаем параллельно потоками pthread
	ptArg arg;
	arg.tree = rootParallelPthread;
	arg.nthreads = nthreads;
	arg.sum = 0.0;
	sum_parallel_pthread((void *) &arg);
	rootParallelPthread->sum = arg.sum;

	// стоп таймер
	end_time = omp_get_wtime();
	// вывод времени
	printf("Parallel (pthread) time: \t%lf sec.\n", end_time-start_time);

	//// проверка на ошибку в вычислениях
	//printf("Error sum: \t\t\t%lf\n", rootSerial->sum-rootParallelPthread->sum);
	
	
	//// вывод деревьев
	//cout << "Serial tree:" << endl;
	//printTree(rootSerial); 
	//cout << "Parallel tree:" << endl;
	//printTree(rootParallelTasks);

	system("pause");

	// Освобождение памяти.
	delete[] arrForTree;
	freeTree(rootSerial);
	freeTree(rootParallelSections);
	freeTree(rootParallelTasks);
	freeTree(rootParallelPthread);

	return 0;
}

