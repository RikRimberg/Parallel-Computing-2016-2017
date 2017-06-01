// SumTree.cpp - �������� ������� ��� ������ � ���������

#include "stdafx.h"
#include "SumTree.h"
using namespace std;

// ������ ���������� pthread_create
#define ERROR_CREATE_THREAD -11
#define ERROR_JOIN_THREAD   -12
#define SUCCESS               0

/********************************************************************************
************************ ������ � �������� ������� ******************************
*********************************************************************************/

/* �������� ���� */
tnode* addNode(double v, tnode *tree) 
{
	// ���� ������ ���, �� ��������� ������
	if (tree == NULL) 
	{	
		tree = new tnode;		// ������ ��� ����
		tree->value = v;		// ��������
		tree->sum = 0;			// ����� ��������
		tree->left =  NULL;		// ����� �������������� ��������
		tree->right = NULL;		
	}
	else if (v < tree->value)	// ������� ���������� ������ �������
		tree->left = addNode(v,tree->left);
	else						// ������� ���������� ������� �������
		tree->right = addNode(v,tree->right);
	return(tree);
}

/* �������� ������ */
void freeTree(tnode *tree) 
{
	if(tree!=NULL) {
		freeTree(tree->left);
		freeTree(tree->right);
		delete tree;
	}
}

/* ����� ������ (� ���������� �����) */
void printTree(tnode *tree) 
{
	if (tree != NULL)
	{
		cout << tree->value << " " << tree->sum << endl;	//���������� ������ ������
		printTree(tree->left);		//����������� ������� ��� ������ ���������
		printTree(tree->right);		//����������� ������� ��� ������� ���������
	}
}

/********************************************************************************
************************* ���������������� �������� *****************************
*********************************************************************************/

/* ����� �������� (���������������) */
double sum_serial(tnode *tree)
{
	// ���� �� ������ ����
	if (tree != NULL)
	{		
		double sum_l = 0.0; // ����� ������ ���������
		double sum_r = 0.0; // ����� ������� ���������

		// ����� �������� ��� ������ ���������		
		if (tree->left != NULL)
		{
			tree->left->sum = sum_serial(tree->left);
			sum_l = tree->left->sum + tree->left->value;
		}

		// ����� �������� ��� ������� ���������
		if (tree->right != NULL)
		{
			tree->right->sum = sum_serial(tree->right);
			sum_r = tree->right->sum + tree->right->value;
		}

		// ������� ����� ��������
		return sum_l + sum_r; 
	}
	return 0;
}

/********************************************************************************
**************** ������������ �������� � �������������� OPENMP ******************
*********************************************************************************/

/* ����� �������� (����������� ��������) */
double sum_parallel_task(tnode *tree, int nthreads)
{
	// ���� �� ������ ����
	if (tree != NULL)
	{		
		double sum_l = 0.0; // ����� ������ ���������
		double sum_r = 0.0; // ����� ������� ���������

		// ���������������� ���������� ��� �������������� ���� ���������� �������
		if (nthreads <= 1)
			return sum_serial(tree);


		if (tree->left != NULL)
		{
			// ������ ����������� ����� - ��� ������
			#pragma omp task shared(sum_l)
			{
				// � ������ ��������� ������� ��������� ����� ���������� ������� �����
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

		// ������� ���������� �������� �����
		#pragma omp taskwait
		return sum_l + sum_r;
	}
	return 0;
}

/* ����� �������� (����������� ��������) */
double sum_parallel_section(tnode *tree)
{
	// ���� �� ������ ����
	if (tree != NULL)
	{		
		double sum_l = 0.0; // ����� ������ ���������
		double sum_r = 0.0; // ����� ������� ���������
		
		//int a = omp_get_max_active_levels();

		// ��� ���������� ������������ ������� ��������� ������������ ��������
		if(omp_get_active_level() >= omp_get_max_active_levels())
			return sum_serial(tree);
		
		#pragma omp parallel num_threads(2) 
		{
			#pragma omp sections
			{
				#pragma omp section 
				{ 
					// ����� �������� ��� ������ ���������
					if (tree->left != NULL)
					{
						tree->left->sum = sum_parallel_section(tree->left);
						sum_l = tree->left->sum + tree->left->value;
					}
				} 

				#pragma omp section 
				{ 
					// ����� �������� ��� ������� ���������
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
**************** ������������ �������� � �������������� PTHREAD *****************
*********************************************************************************/

/* ����� �������� (����������� pthread) */
void* sum_parallel_pthread(void *args)
{
	// �������������� ����������� ��������� ������
	ptArg *arg = (ptArg *)args; 
	
	// ���� �� ������ ����
	if (arg->tree != NULL)
	{	
		double sum_l = 0.0; // ����� ������ ���������
		double sum_r = 0.0; // ����� ������� ���������	
		
		// ���������������� ���������� ��� ������������ ���� �������
		if (arg->nthreads <= 1)
		{
			arg->sum = sum_serial(arg->tree);
			return 0;
		}

		// ����� ��� ������ ���������
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

		// ����� ��� ������� ���������
		pthread_t thread_r;
		int status_r; // ������ ���������� pthread_create
		int status_addr_r; // ������ pthread_join
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
		
		// �������� ���������� �������
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

	// ����������� ������ ?
	// �� ������� ������ ��������� ������� ����� ������� ?
	return 0;
}


/* �������� �� ��������� 2 (��� ���������� max_active_levels) */
int log2( unsigned int n )
{
	return unsigned int(log( (double)n ) / log( 2.0 ));
}