// Struchkov.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"
#include "SumTree.h"

using namespace std;

int main()
{	
	system("chcp 1251");						
	system("cls");
	srand(time(NULL));	
	// ���� ��� ������ �����������
	ofstream fout("results.txt");

	//// ����� ��� ���. ����������
	//ofstream fout1("results_serial.txt");           // ������
	//ofstream fout2("results_openmp_tasks.txt");     // ������
	//ofstream fout3("results_openmp_sections.txt");  // ������
	//ofstream fout4("results_pthreads.txt");         // pthreads
	
	
	// ����������� ������� ���������� ����� ����� � ������
	int arr_size = 1000000;						
	//printf("Size : \t\t\t%ld\n", arr_size);	
	fout << "Size : " << arr_size << endl;
	
	// ������ ��� ������
	double *arrForTree = new double[arr_size];	

	// ���������� ������� ���������� ������� [0,1)							
	for (int i = 0; i < arr_size; i++)
	{
		arrForTree[i] = (double)rand() / (RAND_MAX + 1.0); 
	}
		

	// ����� ����������� �������
	//for (int i = 0; i < arr_size; i++)		
	//{
	//	cout << arrForTree[i] << endl;
	//}

	// ���������� ��������������� ������������������ � ����
	//ofstream fout("array_for_tree.txt"); 
	//for (int i = 0; i < arr_size; i++)
	//{
	//	fout << arrForTree[i] << "\n";
	//}	
	//fout.close();


	// ������������ �������� �������� ��� ������ ��������
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

	// ��������� �������
	double start_time, end_time;	

	// ������ ����� ������� ��� ������������ ����������
	int nthreads = omp_get_num_procs(); // omp_get_num_procs() = 4
	//printf("Threads number : \t%i\n\n", nthreads);	

	fout << "Threads number : " << nthreads << endl << endl;

	

	/* ���������������� ��������.*/

	// ����� ������
	start_time = omp_get_wtime();	
	// ������� ���������������
	rootSerial->sum = sum_serial(rootSerial);	
	// ���� ������
	end_time = omp_get_wtime();
	// ����� �������
	//printf("Serial time: \t\t\t%lf sec.\n", end_time-start_time);	

	fout << "Serial time: " << end_time-start_time << endl;
	//fout1 << end_time-start_time << endl;
	

	
	
	/* ������������ �������� � �������������� ������ OpenMP.*/
	
	// �������� ��������� �����������
	omp_set_nested(1);
	// ������������ ������� ��������� ��������
	omp_set_max_active_levels(log2(nthreads));

		
	// ����� ������
	start_time = omp_get_wtime();
	// ������� ����������� ��������
	rootParallelSections->sum = sum_parallel_section(rootParallelSections);
	// ���� ������
	end_time = omp_get_wtime();
	// ����� �������
	//printf("Parallel (sections) time: \t%lf sec.\n", end_time-start_time);	

	fout << "Parallel (sections) time: " << end_time-start_time << endl;

	/// �������� �� ������ � �����������
	//printf("Error sum: \t\t\t%lf\n", rootSerial->sum-rootParallelSections->sum);
	

	/* ������������ �������� � �������������� ����� OpenMP */

	// ����� ������
	start_time = omp_get_wtime();

	// ������� ����������� ��������
	#pragma omp parallel
	{
		#pragma omp single nowait
		rootParallelTasks->sum = sum_parallel_task(rootParallelTasks,nthreads); 
	}
	// ���� ������
	end_time = omp_get_wtime();
	// ����� �������
	//printf("Parallel (tasks) time: \t\t%lf sec.\n", end_time-start_time);

	fout << "Parallel (tasks) time: " << end_time-start_time << endl;
	//fout2 << end_time-start_time << endl;


	/*
	** ������������ �������� � �������������� Pthread
	*/

	// ����� ������
	start_time = omp_get_wtime();

	// ������� ����������� �������� pthread
	ptArg arg;
	arg.tree = rootParallelPthread;
	arg.nthreads = nthreads;
	arg.sum = 0.0;
	sum_parallel_pthread((void *) &arg);
	rootParallelPthread->sum = arg.sum;

	// ���� ������
	end_time = omp_get_wtime();
	// ����� �������
	//printf("Parallel (pthread) time: \t%lf sec.\n", end_time-start_time);

	fout << "Parallel (pthread) time: " << end_time-start_time << endl;
	//fout4 << end_time-start_time << endl;

	//// �������� �� ������ � �����������
	//printf("Error sum: \t\t\t%lf\n", rootSerial->sum-rootParallelPthread->sum);
	
	
	//// ����� ��������
	//cout << "Serial tree:" << endl;
	//printTree(rootSerial); 
	//cout << "Parallel tree:" << endl;
	//printTree(rootParallelTasks);

	fout.close();
	//system("pause");

	// ������������ ������.
	delete[] arrForTree;
	freeTree(rootSerial);
	freeTree(rootParallelSections);
	freeTree(rootParallelTasks);
	freeTree(rootParallelPthread);

	return 0;
}

