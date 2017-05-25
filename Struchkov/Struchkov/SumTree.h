// SumTree.h - ������������ ����: �������� �������� ������� ��� ������ � ���������

/* ��������� ��������� ������*/
struct tnode 
{
	double value;				// �������� ��������
	double sum;					// ����� �������� �������� ����� 
	struct tnode *left;			// ����� �������
	struct tnode *right;		// ������ �������
};

/*��������� ������ pthread*/
struct ptArg {
    struct tnode *tree;
    int nthreads;
    double sum;
};

/* �������� ���� */
tnode* addNode(double v, tnode *tree); 

/* �������� ������ */
void freeTree(tnode *tree); 

/* ����� ������ */
void printTree(tnode *tree); 

/* ����� �������� (���������������) */
double sum_serial(tnode *tree);

/* ����� �������� (����������� ��������) */
double sum_parallel_task(tnode *tree, int nthreads);

/* ����� �������� (����������� ��������) */
double sum_parallel_section(tnode *tree);

/* ����� �������� (����������� pthred) */
void* sum_parallel_pthread(void *args);

/* �������� �� ��������� 2 */
int log2( unsigned int n );

